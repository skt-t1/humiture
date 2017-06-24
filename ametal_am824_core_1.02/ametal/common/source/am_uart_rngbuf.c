/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      ametal.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief UART implementation by ring buffer 
 * 
 * \internal
 * \par Modification history
 * - 1.01 15-07-15  bob, add UART flowctrl mode
 * - 1.01 14-12-03  jon, add UART interrupt mode
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */
#include "am_uart_rngbuf.h"
#include "am_rngbuf.h"
#include "am_int.h"
#include "am_wait.h"

 
/******************************************************************************/
 
/** 
 * \brief the function that to get one char to transmit.
 */
int __uart_rngbuf_txchar_get (void *p_arg, char *p_outchar)
{
     am_uart_rngbuf_dev_t *p_dev = (am_uart_rngbuf_dev_t *)p_arg;
  
     am_rngbuf_t            rb    = &(p_dev->tx_rngbuf);
    
    /* Success pop one data from ring buffer */
  
    if (am_rngbuf_getchar(rb, p_outchar) == 1) {
         return AM_OK;
    }

    return -AM_EEMPTY;     /* No data to transmit,return -AM_EEMPTY */ 
}

/** 
 * \brief the function revived one char.
 */
int __uart_rngbuf_rxchar_put (void *p_arg, char inchar)
{
    am_uart_rngbuf_dev_t *p_dev = (am_uart_rngbuf_dev_t *)p_arg;
    am_rngbuf_t            rb    = &(p_dev->rx_rngbuf);

    /* 空闲字节数小于流关阈值，流关 */
    if (am_rngbuf_freebytes(rb) < p_dev->xoff_threshold) {
       
        /* Notify the other party continues to stop send */
        am_uart_ioctl(p_dev->handle, 
                      AM_UART_FLOWSTAT_RX_SET, 
                      (void *)AM_UART_FLOWSTAT_OFF);
        
         p_dev->flow_stat = FALSE;         /* 数据接收被关闭 */
    }
    
    am_wait_done(&p_dev->rx_wait);
    
    if (am_rngbuf_putchar(rb, inchar) == 1) {
        return AM_OK;
    }
 
    return -AM_EFULL;          /* No data to receive,return -AM_EFULL */  
}

/**
 * \brief UART send data.
 */
int am_uart_rngbuf_send (am_uart_rngbuf_handle_t  rngbuf_handle,
                         const uint8_t           *p_txbuf,
                         uint32_t                 nbytes)
{
    
    am_uart_rngbuf_dev_t *p_dev  = (am_uart_rngbuf_dev_t *)rngbuf_handle;
    am_rngbuf_t            rb    = &(p_dev->tx_rngbuf);

    uint32_t rb_ct, write_ct;

    uint32_t len = nbytes;
    
    while (len > 0) {
        if (am_rngbuf_isfull(rb) == TRUE) {
            continue;
        }

        rb_ct    = am_rngbuf_freebytes(rb);

        write_ct = (rb_ct > len) ? len : rb_ct;

        am_rngbuf_put(rb, (const char *)p_txbuf, write_ct);
        
        p_txbuf += write_ct;
        
        len     -= write_ct;
        
        am_uart_tx_startup(p_dev->handle);
    }

    return nbytes;
}

/******************************************************************************/

static void __uart_rngbuf_tx_flush (am_uart_rngbuf_dev_t *p_dev)
{
    int key;
    
    key = am_int_cpu_lock();
    
    am_rngbuf_flush(&p_dev->tx_rngbuf);
    
    am_int_cpu_unlock(key);
}

static void __uart_rngbuf_rx_flush (am_uart_rngbuf_dev_t *p_dev)
{
    int key;
    
    key = am_int_cpu_lock();
    
    am_rngbuf_flush(&p_dev->rx_rngbuf);
    
    am_int_cpu_unlock(key);
}

/******************************************************************************/

int am_uart_rngbuf_ioctl (am_uart_rngbuf_handle_t   handle,
                          int                       request, 
                          void                     *p_arg)
{
    int ret = AM_OK;
    
    am_uart_rngbuf_dev_t *p_dev =  (am_uart_rngbuf_dev_t *)handle;
    
    switch (request) {
        
    case AM_UART_RINGBUF_NREAD :
        *(int *)p_arg = am_rngbuf_nbytes(&p_dev->rx_rngbuf);
        break;
    
    case AM_UART_RINGBUF_NWRITE :
        *(int *)p_arg = am_rngbuf_freebytes(&p_dev->tx_rngbuf);
        break;
    
    case AM_UART_RINGBUF_FLUSH :
        __uart_rngbuf_tx_flush(p_dev);
        __uart_rngbuf_rx_flush(p_dev);
        break;
    
    case AM_UART_RINGBUF_WFLUSH :
        __uart_rngbuf_tx_flush(p_dev);
        break;
    
    case AM_UART_RINGBUF_RFLUSH :
        __uart_rngbuf_rx_flush(p_dev);
        break;
    
    case AM_UART_RINGBUF_TIMEOUT:
        p_dev->timeout_ms   = (int)p_arg;
        break;
    case AM_UART_RINGBUF_RX_FLOW_OFF_THR:
        p_dev->xoff_threshold = (int)p_arg;
        break;

    case AM_UART_RINGBUF_RX_FLOW_ON_THR:
        p_dev->xon_threshold  = (int)p_arg;
        break;
    
    case AM_UART_MODE_SET :                   /* 模式固定为中断模式，不可设置为查询模式 */
        ret = -AM_EINVAL;
        break;

    default :
        ret = am_uart_ioctl(p_dev->handle,request,p_arg);
        break;
    }
 
    return ret;
}



/******************************************************************************/
/**
 * \brief UART receive data
 */
int am_uart_rngbuf_receive (am_uart_rngbuf_handle_t   handle,
                            uint8_t                  *p_rxbuf,
                            uint32_t                  nbytes)
{
    
    am_uart_rngbuf_dev_t *p_dev =  (am_uart_rngbuf_dev_t *)handle;
    am_rngbuf_t            rb    = &(p_dev->rx_rngbuf);
    
    uint32_t rb_ct, read_ct;
    uint32_t len = 0;                                /* 读取的字节数      */

    while (nbytes > 0) {
         
        if (am_rngbuf_isempty(rb) == TRUE) {     /* 如为空，则设置等待 */
            
            if (p_dev->timeout_ms == 0) {           /* 没有超时          */
                am_wait_on(&p_dev->rx_wait);
            } else {
                if (am_wait_on_timeout(&p_dev->rx_wait, 
                                       p_dev->timeout_ms) != AM_OK) {
                    return len;                          /* 返回实际读取的字节数 */
                }
            }
        }

        rb_ct   = am_rngbuf_nbytes(rb);

        read_ct = (rb_ct > nbytes) ? nbytes : rb_ct;

        am_rngbuf_get(rb, (char *)p_rxbuf, read_ct);
        
        p_rxbuf += read_ct;
        nbytes  -= read_ct;
        len     += read_ct;
        
        /* 数据接收关闭，判断是否需要打开 */
        if (p_dev->flow_stat == FALSE) {  
            
            /* 空闲字节数大于流开阈值，流开 */
            if (am_rngbuf_freebytes(rb) > p_dev->xon_threshold) {

                /* Notify the other party continues to send */ 
                am_uart_ioctl(p_dev->handle, 
                              AM_UART_FLOWSTAT_RX_SET, 
                              (void *)AM_UART_FLOWSTAT_ON);
                
                p_dev->flow_stat = TRUE;
            }
        }
    }

    return len;
}

/******************************************************************************/
am_uart_rngbuf_handle_t am_uart_rngbuf_init (am_uart_rngbuf_dev_t *p_dev,
                                            am_uart_handle_t       handle,
                                            uint8_t               *p_rxbuf,
                                            uint32_t               rxbuf_size,
                                            uint8_t               *p_txbuf,
                                            uint32_t               txbuf_size)
{
    if (handle == NULL) {
        return NULL;
    }
    
    p_dev->handle = handle;
    
    if (txbuf_size == 0 || rxbuf_size == 0) {
        return NULL;
    }
    
    /* The size should be 2^n */
    if ((txbuf_size & (txbuf_size - 1)) != 0) {
        return NULL;
    }
    
    if ((rxbuf_size & (rxbuf_size - 1)) != 0) {
        return NULL;
    }
    
    p_dev->flow_stat = TRUE;          /* 初始数据接收是打开的 */

    p_dev->xon_threshold  = rxbuf_size * 80 / 100;
    p_dev->xoff_threshold = rxbuf_size * 20 / 100;
    
    am_wait_init(&p_dev->rx_wait);
    
    p_dev->timeout_ms     = 0;       /* 默认超时时间设置为0，即会一直等待 */
    
    /* Initialize the ring-buffer */
    am_rngbuf_init(&(p_dev->rx_rngbuf), (char *)p_rxbuf, rxbuf_size);
    am_rngbuf_init(&(p_dev->tx_rngbuf), (char *)p_txbuf, txbuf_size);
    
    am_uart_ioctl(handle, AM_UART_MODE_SET, (void *)AM_UART_MODE_INT);
    
    am_uart_callback_set(handle,
                         AM_UART_CALLBACK_GET_TX_CHAR,
        (int (*)(void *))__uart_rngbuf_txchar_get,
                         (void *)(p_dev));

    am_uart_callback_set(handle,
                         AM_UART_CALLBACK_PUT_RCV_CHAR,
        (int (*)(void *))__uart_rngbuf_rxchar_put,
                         (void *)(p_dev));

    return (am_uart_rngbuf_handle_t)(p_dev);
    
}

/* end of file */
