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
 * \brief UART interrupt mode implementation
 * 
 * \internal
 * \par Modification history
 * - 1.01 14-12-03  jon, add UART interrupt mode
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */
#include "am_uart_int.h"
#include "am_int.h"
#include "am_wait.h"

/*******************************************************************************
  Public functions
*******************************************************************************/ 

/** 
 * \brief The function that to get one char to transmit.
 */
int __usart_int_txchar_get (void *p_arg, char *p_outchar)
{
    am_uart_int_dev_t *p_dev = (am_uart_int_dev_t *)p_arg;
    
    if (p_dev->tx_nbytes != 0) {
        *p_outchar = *(p_dev->p_txbuf++);
        
        p_dev->tx_nbytes--;
        
        if (p_dev->tx_nbytes == 0) {
            /* 发送完成 */
            am_wait_done(p_dev->p_txwait);
        }
        
        return AM_OK;
    }
    
    return -AM_EEMPTY;     /* No data to transmit,return -AM_EEMPTY */
}

/** 
 * \brief the function revived one char.
 */
int __usart_int_rxchar_put (void *p_arg, char inchar)
{
    am_uart_int_dev_t *p_dev = (am_uart_int_dev_t *)p_arg;
    
    if (p_dev->rx_nbytes != 0) {
        
        *(p_dev->p_rxbuf++) = inchar;  /* Received one char */
        
        p_dev->rx_nbytes--;
        
        if (p_dev->rx_nbytes == 0) {
            /* 接收完成 */
            am_wait_done(p_dev->p_rxwait);
        }

        return AM_OK;
    } 
    
    return -AM_EFULL;          /* No data to recive,return -AM_EFULL */
}

/**
 * \brief UART send data(interrupt mode)
 */
int am_uart_int_send (am_uart_int_handle_t   handle,
                      const uint8_t         *p_txbuf,
                      uint32_t               nbytes)
{
    am_uart_int_dev_t *p_dev = (am_uart_int_dev_t *)handle;
    am_wait_t          send_wait;

    am_wait_init(&send_wait);
    
    p_dev->p_txwait    = &send_wait;
    p_dev->p_txbuf   = (uint8_t *)p_txbuf;
    p_dev->tx_nbytes = nbytes;
    
    /* start up transmit */
    am_uart_tx_startup(p_dev->handle);
    
    /* Wait for transmit complete */
    am_wait_on(&send_wait);
    
    return nbytes;
}

/**
 * \brief UART receive data(interrupt mode)
 */
int am_uart_int_receive (am_uart_int_handle_t  handle,
                         uint8_t              *p_rxbuf,
                         uint32_t              nbytes)
{
    am_uart_int_dev_t *p_dev = (am_uart_int_dev_t *)handle;
    am_wait_t          rec_wait;

    am_wait_init(&rec_wait);
    
    p_dev->p_rxwait    = &rec_wait;
    p_dev->p_rxbuf   = p_rxbuf;
    p_dev->rx_nbytes = nbytes;
    
    /* To do.. if not wait(), asynchronous mode. should be install callback */
    
    /* Wait for received complete */
    am_wait_on(&rec_wait);

    return nbytes;
}

/**
 * \brief UART interrupt mode initialize.
 */
am_uart_int_handle_t am_uart_int_init (am_uart_int_dev_t *p_dev,
                                       am_uart_handle_t   handle)
{
    if (handle == NULL) {
        return NULL;
    }
    
    p_dev->handle = handle;
    
    am_uart_ioctl(handle, AM_UART_MODE_SET, (void *)AM_UART_MODE_INT);
    
    am_uart_callback_set(handle,
                         AM_UART_CALLBACK_GET_TX_CHAR,
        (int (*)(void *))__usart_int_txchar_get,
                        (void *)(p_dev));

    am_uart_callback_set(handle,
                        AM_UART_CALLBACK_PUT_RCV_CHAR,
       (int (*)(void *))__usart_int_rxchar_put,
                       (void *)(p_dev));
                                   
    return (am_uart_int_handle_t)(p_dev);
}

/* end of file */
