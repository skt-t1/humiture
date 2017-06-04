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
 * \brief SPI 驱动层实现函数
 * 
 * \internal
 * \par Modification history
 * - 1.01 15-11-20  sky, modified.
 * - 1.01 15-09-28  aii, second implementation.
 * - 1.00 15-07-07  aii, first implementation.
 * \endinternal
 */

/*******************************************************************************
includes
*******************************************************************************/

#include "ametal.h"
#include "am_int.h"
#include "am_gpio.h"
#include "am_lpc82x.h"
#include "am_lpc82x_spi.h"
#include "am_lpc82x_dma.h"
#include "amhw_lpc82x_spi.h"
#include "amhw_lpc82x_clk.h"


/*******************************************************************************
  模块内变量声明
*******************************************************************************/

const uint16_t       const_high = 0xFFFF;     /* SPI接收时MOSI为高电平 */
const uint16_t       const_low  = 0x0000;     /* SPI接收时MOSI为低电平 */

/*******************************************************************************
  SPI 状态和事件定义
*******************************************************************************/

/**
 * SPI 控制器状态
 */

#define __SPI_ST_IDLE               0                   /* 空闲状态 */
#define __SPI_ST_MSG_START          1                   /* 消息开始 */
#define __SPI_ST_TRANS_START        2                   /* 传输开始 */
#define __SPI_ST_M_SEND_DATA        3                   /* 主机发送 */
#define __SPI_ST_M_RECV_DATA        4                   /* 主机接收 */
#define __SPI_ST_DMA_TRANS_DATA     5                   /* DMA 传输 */

/**
 * SPI 控制器事件
 *
 * 共32位，低16位是事件编号，高16位是事件参数
 */

#define __SPI_EVT_NUM_GET(event)    ((event) & 0xFFFF)
#define __SPI_EVT_PAR_GET(event)    ((event >> 16) & 0xFFFF)
#define __SPI_EVT(evt_num, evt_par) (((evt_num) & 0xFFFF) | ((evt_par) << 16))

#define __SPI_EVT_NONE              __SPI_EVT(0, 0)     /* 无事件 */
#define __SPI_EVT_TRANS_LAUNCH      __SPI_EVT(1, 0)     /* 传输就绪 */
#define __SPI_EVT_M_SEND_DATA       __SPI_EVT(2, 0)     /* 发送数据 */
#define __SPI_EVT_M_RECV_DATA       __SPI_EVT(3, 0)     /* 接收数据 */
#define __SPI_EVT_DMA_TRANS_DATA    __SPI_EVT(4, 0)     /* DMA传输数据 */

/*******************************************************************************
  模块内函数声明
*******************************************************************************/
am_local void __spi_default_cs_ha    (am_spi_device_t *p_dev, int state);
am_local void __spi_default_cs_la    (am_spi_device_t *p_dev, int state);
am_local void __spi_default_cs_dummy (am_spi_device_t *p_dev, int state);

am_local void __spi_cs_on  (am_lpc82x_spi_dev_t *p_this, am_spi_device_t *p_dev);
am_local void __spi_cs_off (am_lpc82x_spi_dev_t *p_this, am_spi_device_t *p_dev);

am_local void __spi_write_data (am_lpc82x_spi_dev_t *p_dev);
am_local void __spi_read_data (am_lpc82x_spi_dev_t *p_dev);

am_local void __spi_irq_handler (void *p_arg);
am_local int  __spi_hard_init (am_lpc82x_spi_dev_t *p_this);
am_local int  __spi_config (am_lpc82x_spi_dev_t *p_this);

am_local int  __spi_dma_trans (am_lpc82x_spi_dev_t *p_dev);
am_local void __dma_isr (void *p_arg, int stat);

am_local int  __spi_mst_sm_event (am_lpc82x_spi_dev_t *p_dev, uint32_t event);
/*******************************************************************************
  SPI驱动函数声明
*******************************************************************************/
am_local int __spi_info_get (void *p_arg, am_spi_info_t   *p_info);
am_local int __spi_setup    (void *p_arg, am_spi_device_t *p_dev );
am_local int __spi_msg_start (void              *p_drv,
                              am_spi_device_t   *p_dev,
                              am_spi_message_t  *p_msg);

/**
 * \brief SPI 驱动函数
 */
am_local am_const struct am_spi_drv_funcs __g_spi_drv_funcs = {
    __spi_info_get,
    __spi_setup,
    __spi_msg_start,
};

/******************************************************************************/

/**
 * \brief 默认CS脚控制函数，高电平有效
 */
am_local
void __spi_default_cs_ha (am_spi_device_t *p_dev, int state)
{
    am_gpio_set(p_dev->cs_pin, state ? 1 : 0);
}

/**
 * \brief 默认CS脚控制函数，低电平有效
 */
am_local
void __spi_default_cs_la (am_spi_device_t *p_dev, int state)
{
    am_gpio_set(p_dev->cs_pin, state ? 0 : 1);
}

/**
 * \brief 默认CS脚控制函数，由硬件自行控制
 */
am_local
void __spi_default_cs_dummy (am_spi_device_t *p_dev, int state)
{
    return;
}

/**
 * \brief CS引脚激活
 */
am_local
void __spi_cs_on (am_lpc82x_spi_dev_t *p_this, am_spi_device_t *p_dev)
{
    /* if last device toggled after message */
    if (p_this->p_tgl_dev != NULL) {

        /* last message on defferent device */
        if (p_this->p_tgl_dev != p_dev) {
            p_this->p_tgl_dev->pfunc_cs(p_this->p_tgl_dev, 0);
        }
        p_this->p_tgl_dev = NULL;
    }

    p_dev->pfunc_cs(p_dev, 1);
}

/**
 * \brief CS引脚去活
 */
am_local
void __spi_cs_off (am_lpc82x_spi_dev_t   *p_this,
                   am_spi_device_t  *p_dev)
{
    if (p_this->p_tgl_dev == p_dev) {
        p_this->p_tgl_dev = NULL;
    }

    p_dev->pfunc_cs(p_dev, 0);
}

/******************************************************************************/

am_local
void __spi_write_data (am_lpc82x_spi_dev_t *p_dev)
{
    amhw_lpc82x_spi_t *p_hw_spi = (amhw_lpc82x_spi_t *)(p_dev->p_devinfo->spi_regbase);
    am_spi_transfer_t *p_trans  = p_dev->p_cur_trans;

    if (p_dev == NULL) {
        return ;
    }

    /* tx_buf 有效 */
    if (p_trans->p_txbuf != NULL) {
        if ((p_dev->p_cur_spi_dev->bits_per_word) <= 8) {
            /** \brief 待发送数据的基址+偏移 */
            uint8_t *ptr = (uint8_t *)(p_trans->p_txbuf) + p_dev->data_ptr;
            amhw_lpc82x_spi_txdat(p_hw_spi, *ptr);
        } else {
            uint16_t *ptr = (uint16_t *)((uint8_t *)(p_trans->p_txbuf)
                                                     + p_dev->data_ptr);
            amhw_lpc82x_spi_txdat(p_hw_spi, *ptr);
        }
    /* tx_buf 无效 */
    } else {
        /** \brief 待发送数据无效 直接发0 */
        amhw_lpc82x_spi_txdat(p_hw_spi, 0x0000);
    }

    /** \brief 需接收数据的byte数 */
    p_dev->nbytes_to_recv = p_dev->p_cur_spi_dev->bits_per_word < 9 ? 1 : 2;
    p_dev->p_cur_msg->actual_length += p_dev->nbytes_to_recv;
}

am_local
void __spi_read_data (am_lpc82x_spi_dev_t *p_dev)
{
    amhw_lpc82x_spi_t *p_hw_spi = (amhw_lpc82x_spi_t *)(p_dev->p_devinfo->spi_regbase);
    am_spi_transfer_t *p_trans  = p_dev->p_cur_trans;
    uint8_t *p_buf8  = (uint8_t *)p_trans->p_rxbuf + p_dev->data_ptr;

    if (p_dev == NULL) {
        return ;
    }

    /* rx_buf 有效 */
    if (p_trans->p_rxbuf != NULL && p_dev->nbytes_to_recv) {
        if ((p_dev->p_cur_spi_dev->bits_per_word) <= 8) {
             *p_buf8 = amhw_lpc82x_spi_rxdat_8bit(p_hw_spi);
        } else {
             *(uint16_t *)(p_buf8) = amhw_lpc82x_spi_rxdat_16bit(p_hw_spi);
        }

    /* rx_buf 无效或者不需要接收数据 */
    } else {
        if ((p_dev->p_cur_spi_dev->bits_per_word) <= 8) {
            (void)amhw_lpc82x_spi_rxdat_8bit(p_hw_spi);

        } else {
            (void)amhw_lpc82x_spi_rxdat_16bit(p_hw_spi);
        }
    }

    /* 已经发送或接收的数据byte数 */
    p_dev->data_ptr += p_dev->nbytes_to_recv;
    p_dev->nbytes_to_recv = 0;
}

/******************************************************************************/

/**
 * \brief 添加一条 message 到控制器传输列表末尾
 * \attention 调用此函数必须锁定控制器
 */
am_static_inline
void __spi_msg_in (am_lpc82x_spi_dev_t *p_dev, struct am_spi_message *p_msg)
{
    am_list_add_tail((struct am_list_head *)(&p_msg->ctlrdata),
                                             &(p_dev->msg_list));
}

/**
 * \brief 从控制器传输列表表头取出一条 message
 * \attention 调用此函数必须锁定控制器
 */
am_static_inline
struct am_spi_message *__spi_msg_out (am_lpc82x_spi_dev_t *p_dev)
{
    if (am_list_empty_careful(&(p_dev->msg_list))) {
        return NULL;
    } else {
        struct am_list_head *p_node = p_dev->msg_list.next;
        am_list_del(p_node);
        return am_list_entry(p_node, struct am_spi_message, ctlrdata);
    }
}

/**
 * \brief 从message列表表头取出一条 transfer
 * \attention 调用此函数必须锁定控制器
 */
am_static_inline
struct am_spi_transfer *__spi_trans_out (am_spi_message_t *msg)
{
    if (am_list_empty_careful(&(msg->transfers))) {
        return NULL;
    } else {
        struct am_list_head *p_node = msg->transfers.next;
        am_list_del(p_node);
        return am_list_entry(p_node, struct am_spi_transfer, trans_node);
    }
}

/******************************************************************************/
am_local
int __spi_setup (void *p_arg, am_spi_device_t *p_dev)
{
    am_lpc82x_spi_dev_t *p_this   = (am_lpc82x_spi_dev_t *)p_arg;
    amhw_lpc82x_spi_t   *p_hw_spi = (amhw_lpc82x_spi_t *)(p_this->p_devinfo->spi_regbase);

    uint32_t max_speed, min_speed;

    if (p_dev == NULL) {
        return -AM_EINVAL;
    }

    /* 默认数据为8位，最大不超过16位 */
    if (p_dev->bits_per_word == 0) {
        p_dev->bits_per_word = 8;
    } else if (p_dev->bits_per_word > 16) {
        return -AM_ENOTSUP;
    }

    /* 最大SPI速率不能超过主时钟，最小不能小于主时钟65536分频 */
    max_speed = amhw_lpc82x_clk_periph_freq_get(p_hw_spi);
    min_speed = max_speed / 65536;

    if (p_dev->max_speed_hz > max_speed) {
        p_dev->max_speed_hz = max_speed;
    } else if (p_dev->max_speed_hz < min_speed) {
        return -AM_ENOTSUP;
    }

    /* 无片选函数 */
    if (p_dev->mode & AM_SPI_NO_CS) {
        p_dev->pfunc_cs = __spi_default_cs_dummy;

    /* 有片选函数 */
    }  else {

        /* 不提供则默认片选函数 */
        if (p_dev->pfunc_cs == NULL) {

            /* 片选高电平有效 */
            if (p_dev->mode & AM_SPI_CS_HIGH) {
                am_gpio_pin_cfg(p_dev->cs_pin, AM_GPIO_OUTPUT_INIT_LOW);
                p_dev->pfunc_cs = __spi_default_cs_ha;

            /* 片选低电平有效 */
            } else {
                am_gpio_pin_cfg(p_dev->cs_pin, AM_GPIO_OUTPUT_INIT_HIGH);
                p_dev->pfunc_cs = __spi_default_cs_la;
            }
        }
    }

    /* 解除片选信号 */
    __spi_cs_off(p_this, p_dev);

    return AM_OK;
}


am_local
int __spi_info_get (void *p_arg, am_spi_info_t *p_info)
{
    am_lpc82x_spi_dev_t  *p_this   = (am_lpc82x_spi_dev_t *)p_arg;
    amhw_lpc82x_spi_t    *p_hw_spi = (amhw_lpc82x_spi_t *)(p_this->p_devinfo->spi_regbase);

    if (p_info == NULL) {
        return -AM_EINVAL;
    }

    /* 最大速率等于 PCLK */
    p_info->max_speed = amhw_lpc82x_clk_periph_freq_get(p_hw_spi);
    p_info->min_speed = p_info->max_speed / 65536;
    p_info->features  = AM_SPI_CPHA  |
                        AM_SPI_CPOL  |
                        AM_SPI_NO_CS |
                        AM_SPI_LOOP  |
                        AM_SPI_CS_HIGH;   /* features */
    return AM_OK;
}

/**
 * \brief SPI 硬件初始化
 */
am_local
int __spi_hard_init (am_lpc82x_spi_dev_t *p_this)
{
    amhw_lpc82x_spi_t *p_hw_spi = (amhw_lpc82x_spi_t *)(p_this->p_devinfo->spi_regbase);

    if (p_this == NULL) {
        return -AM_EINVAL;
    }

    /* 等待SPI空闲，方可设置配置寄存器 */
    while ((amhw_lpc82x_spi_stat_get(p_hw_spi) & AMHW_LPC82X_SPI_STAT_IDLE) == 0);
    amhw_lpc82x_spi_cfg_set(p_hw_spi, (AMHW_LPC82X_SPI_CFG_ENABLE |
                                       AMHW_LPC82X_SPI_CFG_MASTER));

    /* 初始化配置SPI */
    return AM_OK;
}

/**
 * \brief SPI 中断句柄定义
 */
am_local
void __spi_irq_handler (void *p_arg)
{
    am_lpc82x_spi_dev_t  *p_dev      = (am_lpc82x_spi_dev_t *)p_arg;
    amhw_lpc82x_spi_t    *p_hw_spi   = (amhw_lpc82x_spi_t *)(p_dev->p_devinfo->spi_regbase);

    uint32_t spi_status = (uint32_t)amhw_lpc82x_spi_intstat_get(p_hw_spi);

    if ((spi_status & AMHW_LPC82X_SPI_STAT_SSA)  ||
        (spi_status & AMHW_LPC82X_SPI_STAT_SSD)) {
        amhw_lpc82x_spi_stat_clr(p_hw_spi, AMHW_LPC82X_SPI_STAT_SSA |
                                           AMHW_LPC82X_SPI_STAT_SSD);
    }

    if ((spi_status & AMHW_LPC82X_SPI_STAT_RXOV)  ||
        (spi_status & AMHW_LPC82X_SPI_STAT_TXUR)) {

        /* 清除错误状态 */
        amhw_lpc82x_spi_stat_clr(p_hw_spi, AMHW_LPC82X_SPI_STAT_RXOV |
                                           AMHW_LPC82X_SPI_STAT_TXUR |
                                           AMHW_LPC82X_SPI_STAT_SSA  |
                                           AMHW_LPC82X_SPI_STAT_SSD);

        /* 禁能所有中断 */
        amhw_lpc82x_spi_int_disable(p_hw_spi, AMHW_LPC82X_SPI_INT_RXRDY |
                                              AMHW_LPC82X_SPI_INT_TXRDY |
                                              AMHW_LPC82X_SPI_INT_RXOV  |
                                              AMHW_LPC82X_SPI_INT_TXUR  |
                                              AMHW_LPC82X_SPI_INT_SSA   |
                                              AMHW_LPC82X_SPI_INT_SSD);
    }

    /* 发送数据 */
    if (spi_status & AMHW_LPC82X_SPI_STAT_TXRDY) {
        /* run the state machine */
        __spi_mst_sm_event(p_dev, __SPI_EVT_M_SEND_DATA);
    }

    /* 接收数据 */
    if (spi_status & AMHW_LPC82X_SPI_STAT_RXRDY) {
        __spi_mst_sm_event(p_dev, __SPI_EVT_M_RECV_DATA);
    }
}

am_local
void __dma_isr (void *p_arg, int stat)
{
    am_lpc82x_spi_dev_t  *p_this   = (am_lpc82x_spi_dev_t *)p_arg;
    amhw_lpc82x_spi_t    *p_hw_spi = (amhw_lpc82x_spi_t *)(p_this->p_devinfo->spi_regbase);

    /* 如果是中断A发生 */
    if (stat == AM_LPC82X_DMA_STAT_INTA)
    {
        /* 记录成功传送字节数 */
        if (p_this->p_cur_trans->p_txbuf != NULL) {
            p_this->p_cur_msg->actual_length += (p_this->p_cur_trans->nbytes)
                            *(p_this->p_cur_spi_dev->bits_per_word < 9 ? 1 : 2);
        }

        /* 等待SPI数据彻底发送结束 */
        while(!(amhw_lpc82x_spi_stat_get(p_hw_spi) & AMHW_LPC82X_SPI_STAT_IDLE));
        /* 传输就绪 */
        __spi_mst_sm_event(p_this, __SPI_EVT_TRANS_LAUNCH);

    } else {
        /* 中断源不匹配 */
    }
}

/**
 * \brief  SPI 使用DMA传输
 */
am_local
int __spi_dma_trans (am_lpc82x_spi_dev_t *p_dev)
{
    am_lpc82x_spi_dev_t           *p_this      = (am_lpc82x_spi_dev_t *)p_dev;
    const am_lpc82x_spi_devinfo_t *p_devinfo   = p_this->p_devinfo;
    amhw_lpc82x_spi_t             *p_hw_spi    = (amhw_lpc82x_spi_t *)(p_devinfo->spi_regbase);
    am_spi_transfer_t             *p_cur_trans = p_dev->p_cur_trans;
    am_lpc82x_dma_controller_t    *p_ctr_tx    = p_this->p_ctr_tx;
    am_lpc82x_dma_controller_t    *p_ctr_rx    = p_this->p_ctr_rx;

    const uint16_t                *p_tx_buf    = NULL; /* 指向用户发送缓冲区指针 */
          uint16_t                *p_rx_buf    = NULL; /* 指向用户接收缓冲区指针 */
          uint16_t                rx_trash     = 0;    /* 丢弃数据的临时接收变量 */
          uint32_t                p_tx_buf_eot = 0;    /* 一帧中的最后一个数据 */

    uint8_t    eot_nbyte    = 0;    /* 发送结束字的数据字节数 */
    uint32_t   dma_flags[3] = {0};  /* DMA发送通道描述符 */

    /* 只接收不发送数据 */
    if (p_cur_trans->p_txbuf == NULL) {
        if (p_cur_trans->flags & AM_SPI_READ_MOSI_HIGH) {
            p_tx_buf = &const_high;                       /* MOSI接收时高电平 */
        } else {
            p_tx_buf = &const_low;                        /* MOSI接收时低电平 */
        }
        p_tx_buf_eot = AM_SBF(p_cur_trans->bits_per_word - 1, 24) |
                       *((uint16_t *)p_tx_buf);

    /* 存在发送数据 */
    } else {
        p_tx_buf = (const uint16_t *)p_cur_trans->p_txbuf; /* MOSI发送用户数据 */
        if (p_cur_trans->bits_per_word > 8) {
            p_tx_buf_eot = AM_SBF(p_cur_trans->bits_per_word - 1, 24) |
                           ((uint16_t *)p_tx_buf)[p_cur_trans->nbytes/2 - 1];
        } else {
            p_tx_buf_eot = AM_SBF(p_cur_trans->bits_per_word - 1, 24) |
                           ((uint8_t *)p_tx_buf)[p_cur_trans->nbytes - 1];
        }
    }

    /* 配置帧结束的最后一个字符是否结束片选信号（硬件片选） */
    p_tx_buf_eot |= AMHW_LPC82X_SPI_TXCTRL_EOT;

    /* 只发送不接收数据 */
    if (p_cur_trans->p_rxbuf == NULL) {
        p_rx_buf = &rx_trash;                           /* 指向变量，丢弃数据 */

    /* 存在接收数据 */
    } else {
    p_rx_buf = p_cur_trans->p_rxbuf;                    /* 指向用户接收缓冲区 */
    }

    /* DMA发送通道配置 */
    dma_flags[0] = AM_LPC82X_DMA_XFER_VALID        |    /* 当前通道描述符有效 */
                   AM_LPC82X_DMA_XFER_RELOAD       |    /* 允许重载通道描述符 */
                   AM_LPC82X_DMA_XFER_SWTRIG       |    /* 软件触发 */
                   AM_LPC82X_DMA_XFER_DSTINC_NOINC;     /* 设置目标地址不递增 */

    /* DMA发送结束字通道配置 */
    dma_flags[1] = AM_LPC82X_DMA_XFER_VALID        |    /* 当前通道描述符有效 */
                   AM_LPC82X_DMA_XFER_SWTRIG       |    /* 软件触发 */
                   AM_LPC82X_DMA_XFER_CLRTRIG      |    /* 清除触发标志 */
                   AM_LPC82X_DMA_XFER_DSTINC_NOINC |    /* 设置目标地址不增加 */
                   AM_LPC82X_DMA_XFER_WIDTH_32BIT  |    /* 数据宽度32位 */
                   AM_LPC82X_DMA_XFER_SRCINC_NOINC;     /* 源地址不递增 */


    /* DMA接收通道配置 */
    dma_flags[2] = AM_LPC82X_DMA_XFER_VALID        |      /* 当前配置描述符有效 */
                   AM_LPC82X_DMA_XFER_SWTRIG       |      /* 软件触发 */
                   AM_LPC82X_DMA_XFER_CLRTRIG      |      /* 清除触发标志 */
                   AM_LPC82X_DMA_XFER_SRCINC_NOINC |      /* 源地址不递增 */
                   AM_LPC82X_DMA_XFER_SETINTA;            /* 使能中断A */

    /* 设置DMA数据位宽 */
    if (p_cur_trans->bits_per_word > 8) {
        dma_flags[0] |= AM_LPC82X_DMA_XFER_WIDTH_16BIT;
        dma_flags[2] |= AM_LPC82X_DMA_XFER_WIDTH_16BIT;
        eot_nbyte     = 2;
    } else {
        dma_flags[0] |= AM_LPC82X_DMA_XFER_WIDTH_8BIT;
        dma_flags[2] |= AM_LPC82X_DMA_XFER_WIDTH_8BIT;
        eot_nbyte     = 1;
    }

    /* 发送缓冲区有效 */
    if (p_cur_trans->p_txbuf != NULL) {
        dma_flags[0] |= AM_LPC82X_DMA_XFER_SRCINC_1X;      /* 源地址递增宽度 */
    } else {
        dma_flags[0] |= AM_LPC82X_DMA_XFER_SRCINC_NOINC;   /* 源地址不递增 */
    }

    /* 接收缓冲区有效 */
    if (p_cur_trans->p_rxbuf != NULL) {
        dma_flags[2] |= AM_LPC82X_DMA_XFER_DSTINC_1X;      /* 目标地址递增宽度 */
    } else {
        dma_flags[2] |= AM_LPC82X_DMA_XFER_DSTINC_NOINC ;  /* 目标地址不递增 */
    }

    /* 当只发生一次传输时 */
    if (((p_cur_trans->nbytes == 1) && (p_cur_trans->bits_per_word < 9)) ||
        ((p_cur_trans->nbytes == 2) && (p_cur_trans->bits_per_word > 8))) {

        /* 建立结束字通道描述符 */
        am_lpc82x_dma_xfer_desc_build(&p_this->g_desc[0],              /* 描述符 */
                                      (uint32_t)&p_tx_buf_eot,         /* 源端BUFF */
                                      (uint32_t)&(p_hw_spi->txdatctl), /* 目标BUFF */
                                       4,                              /* TX字节数 */
                                       dma_flags[1]);                  /* 传输配置 */

    /* 发送多次传输时 */
    } else {

        /* 建立通道描述符 */
        am_lpc82x_dma_xfer_desc_build(&p_this->g_desc[0],              /* 描述符 */
                                      (uint32_t)p_tx_buf,              /* 源端BUFF */
                                      (uint32_t)&(p_hw_spi->txdat),    /* 目标BUFF */
                                       p_cur_trans->nbytes - eot_nbyte,/* TX字节数 */
                                       dma_flags[0]);                  /* 传输配置 */

        /* 建立结束字通道描述符 */
        am_lpc82x_dma_xfer_desc_build(&p_this->g_desc[1],              /* 描述符 */
                                      (uint32_t)&p_tx_buf_eot,         /* 源端BUFF */
                                      (uint32_t)&(p_hw_spi->txdatctl), /* 目标BUFF */
                                       4,                              /* TX字节数 */
                                       dma_flags[1]);                  /* 传输配置 */

        /* DMA连接发送通道 */
        am_lpc82x_dma_xfer_desc_link(&p_this->g_desc[0], &p_this->g_desc[1]);
    }

    /* 建立通道描述符 */
    am_lpc82x_dma_xfer_desc_build(&p_this->g_desc[2],                  /* 描述符 */
                                  (uint32_t)&(p_hw_spi->rxdat),        /* 源端BUFF */
                                  (uint32_t)p_rx_buf,                  /* 目标BUFF */
                                   p_cur_trans->nbytes,                /* TX字节数 */
                                   dma_flags[2]);                      /* 传输配置 */

    /* 置位片选信号 */
    __spi_cs_on(p_this, p_this->p_cur_spi_dev);

    /* 启动DMA传输，马上开始传输 */
    am_lpc82x_dma_xfer_desc_startup(p_ctr_rx,
                                    &p_this->g_desc[2],
                                    __dma_isr,
                                    (void *)p_this);

    /* 启动DMA传输，马上开始传输 */
    am_lpc82x_dma_xfer_desc_startup(p_ctr_tx,
                                    &p_this->g_desc[0],
                                    __dma_isr,
                                    (void *)p_this);
    return AM_OK;
}

am_local
int __spi_config (am_lpc82x_spi_dev_t *p_this)
{
    const am_lpc82x_spi_devinfo_t *p_devinfo = p_this->p_devinfo;
    amhw_lpc82x_spi_t             *p_hw_spi  = (amhw_lpc82x_spi_t *)(p_devinfo->spi_regbase);
    am_spi_transfer_t             *p_trans   = p_this->p_cur_trans;

    uint32_t                       mode_flag = 0;
    uint32_t                       div_val;

    /* 如果为0，使用默认参数值 */
    if (p_trans->bits_per_word == 0) {
        p_trans->bits_per_word = p_this->p_cur_spi_dev->bits_per_word;
    }

    if (p_trans->speed_hz == 0) {
        p_trans->speed_hz = p_this->p_cur_spi_dev->max_speed_hz;
    }

    /* 设置字节数有效性检查 */
    if (p_trans->bits_per_word > 16 || p_trans->bits_per_word < 1) {
        return -AM_EINVAL;
    }

    /* 设置分频值有效性检查 */
    if (p_trans->speed_hz > amhw_lpc82x_clk_periph_freq_get(p_hw_spi) ||
        p_trans->speed_hz < (amhw_lpc82x_clk_periph_freq_get(p_hw_spi) / 65536)) {
        return -AM_EINVAL;
    }

    /* 发送和接收缓冲区有效性检查 */
    if ((p_trans->p_txbuf == NULL) && (p_trans->p_rxbuf == NULL)) {
        return -AM_EINVAL;
    }

    /* 发送字节数检查 */
    if (p_trans->nbytes == 0) {
        return -AM_ELOW;
    }

    /**
     * 配置当前设备模式
     */
    mode_flag = 0;

    if (p_this->p_cur_spi_dev->mode & AM_SPI_CPHA) {
        mode_flag |= AMHW_LPC82X_SPI_CFG_CHANGE;
    }
    if (p_this->p_cur_spi_dev->mode & AM_SPI_CPOL) {
        mode_flag |= AMHW_LPC82X_SPI_CFG_STAT_HIGH;
    }
    if (p_this->p_cur_spi_dev->mode & AM_SPI_CS_HIGH) {
        mode_flag |= AMHW_LPC82X_SPI_CFG_SPOL0_HIGH;
    }
    if (p_this->p_cur_spi_dev->mode & AM_SPI_LSB_FIRST) {
        mode_flag |= AMHW_LPC82X_SPI_CFG_LSB;
    }
    if (p_this->p_cur_spi_dev->mode & AM_SPI_LOOP) {
        mode_flag |= AMHW_LPC82X_SPI_CFG_LOOP;
    }

    /* 设置模式(可在这里设置主从机模式) */
    amhw_lpc82x_spi_cfg_set(p_hw_spi, (mode_flag                  |
                                       AMHW_LPC82X_SPI_CFG_ENABLE |
                                       AMHW_LPC82X_SPI_CFG_MASTER));

    /* 设置单帧数据位数 */
    amhw_lpc82x_spi_data_flen_set (p_hw_spi, p_trans->bits_per_word);

    /* 设置分频值，即SPI速率 */
    div_val = (amhw_lpc82x_clk_periph_freq_get(p_hw_spi) / p_trans->speed_hz);
    amhw_lpc82x_spi_div_set(p_hw_spi, div_val);

    /* 清除状态位 */
    amhw_lpc82x_spi_stat_clr(p_hw_spi, AMHW_LPC82X_SPI_STAT_RXOV |
                                       AMHW_LPC82X_SPI_STAT_TXUR |
                                       AMHW_LPC82X_SPI_STAT_SSA  |
                                       AMHW_LPC82X_SPI_STAT_SSD);

    /* 使能SPI接收发送溢出中断 */
    amhw_lpc82x_spi_int_enable(p_hw_spi, AMHW_LPC82X_SPI_INT_RXOV |
                                         AMHW_LPC82X_SPI_INT_TXUR );

    return AM_OK;
}

/**
 * \brief SPI 传输数据函数
 */
am_local
int __spi_msg_start (void              *p_drv,
                     am_spi_device_t   *p_dev,
                     am_spi_message_t  *p_msg)
{
    am_lpc82x_spi_dev_t *p_this   = (am_lpc82x_spi_dev_t *)p_drv;
    amhw_lpc82x_spi_t   *p_hw_spi = (amhw_lpc82x_spi_t *)(p_this->p_devinfo->spi_regbase);

    int key;

    p_this->p_cur_spi_dev  = p_dev;                         /* 将当前设备参数信息存入 */
    p_this->p_cur_msg      = p_msg;                         /* 将当前设备传输消息存入 */
    p_this->nbytes_to_recv = 0;                             /* 待接收字符数清0 */
    p_this->data_ptr       = 0;                             /* 已接收字符数清0 */
    p_this->trans_mode     = p_this->p_devinfo->trans_mode; /* 加载传输模式 */

    /* 设备有效性检查 */
    if ((p_drv              == NULL) ||
        (p_dev              == NULL) ||
        (p_msg              == NULL) ){
        return -AM_EINVAL;
    }

    /* 清除帧结束位 */
    amhw_lpc82x_spi_txctl_clr(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_EOT);

    key = am_int_cpu_lock();

    /* 当前正在处理消息，只需要将新的消息加入链表即可 */
    if (p_this->busy == TRUE) {
        __spi_msg_in(p_this, p_msg);
        am_int_cpu_unlock(key);
        return AM_OK;
    } else {
        p_this->busy = TRUE;
        __spi_msg_in(p_this, p_msg);
        p_msg->status = -AM_EISCONN; /* 正在排队中 */
        am_int_cpu_unlock(key);

        /* 启动状态机 */
        return __spi_mst_sm_event(p_this, __SPI_EVT_TRANS_LAUNCH);
    }
}

/******************************************************************************/

/*  状态机内部状态切换 */
#define __SPI_NEXT_STATE(s, e) \
    do { \
        p_dev->state = (s); \
        new_event = (e); \
    } while(0)

/**
 * \brief  SPI 使用状态机传输
 */
am_local
int __spi_mst_sm_event (am_lpc82x_spi_dev_t *p_dev, uint32_t event)
{
    amhw_lpc82x_spi_t *p_hw_spi = (amhw_lpc82x_spi_t *)(p_dev->p_devinfo->spi_regbase);

    volatile uint32_t new_event = __SPI_EVT_NONE;

    if (p_dev == NULL) {
        return -AM_EINVAL;
    }

    while (1) {

        if (new_event != __SPI_EVT_NONE) {     /* 检查新事件是否来自内部 */
            event     = new_event;
            new_event  = __SPI_EVT_NONE;
        }

        switch (p_dev->state) {

        case __SPI_ST_IDLE:         /* 控制器处于空闲状态 */
        {
            if (event != __SPI_EVT_TRANS_LAUNCH) {
                return -AM_EINVAL;  /* 空闲状态等待的消息必须是启动传输 */
            }
            /* 切换到消息开始状态，不用break */
        }

        case __SPI_ST_MSG_START:    /* 消息开始 */
        {
            am_spi_message_t  *p_cur_msg   = NULL;

            int key;

            if (event != __SPI_EVT_TRANS_LAUNCH) {
                return -AM_EINVAL;  /* 消息开始状态等待的消息必须是启动传输 */
            }

            key = am_int_cpu_lock();
            p_cur_msg          = __spi_msg_out(p_dev);
            p_dev->p_cur_msg   = p_cur_msg;

            if (p_cur_msg) {
                p_cur_msg->status = -AM_EINPROGRESS;
            } else {
                /* 禁能所有中断 */
                amhw_lpc82x_spi_int_disable(p_hw_spi, AMHW_LPC82X_SPI_INT_RXRDY |
                                                      AMHW_LPC82X_SPI_INT_TXRDY |
                                                      AMHW_LPC82X_SPI_INT_RXOV  |
                                                      AMHW_LPC82X_SPI_INT_TXUR  |
                                                      AMHW_LPC82X_SPI_INT_SSA   |
                                                      AMHW_LPC82X_SPI_INT_SSD   );
                p_dev->busy = FALSE;
            }
            am_int_cpu_unlock(key);

            /* 无需要处理的消息 */
            if (p_cur_msg == NULL) {
                __SPI_NEXT_STATE(__SPI_ST_IDLE, __SPI_EVT_NONE);
                break;
            } else {

                /* 直接进入下一个状态，开始一个传输，此处无需break */
                __SPI_NEXT_STATE(__SPI_ST_TRANS_START, __SPI_EVT_TRANS_LAUNCH);

                event     = new_event;
                new_event = __SPI_EVT_NONE;
            }
        }

        case __SPI_ST_TRANS_START:  /* 传输开始 */
        {
            am_spi_message_t  *p_cur_msg   = p_dev->p_cur_msg;

            if (event != __SPI_EVT_TRANS_LAUNCH) {
                return -AM_EINVAL;  /* 传输开始状态等待的消息必须是启动传输 */
            }

            /* 当前消息传输完成 */
            if (am_list_empty(&(p_cur_msg->transfers))) {

                p_cur_msg->actual_length = 0;

                /* 消息正在处理中 */
                if (p_cur_msg->status == -AM_EINPROGRESS) {
                    p_cur_msg->status = AM_OK;
                }

                /* notify the caller  */
                if (p_cur_msg->pfn_complete != NULL) {
                    p_cur_msg->pfn_complete(p_cur_msg->p_arg);
                }

                /* 片选关闭 */
                __spi_cs_off(p_dev, p_dev->p_cur_spi_dev);

                __SPI_NEXT_STATE(__SPI_ST_MSG_START, __SPI_EVT_TRANS_LAUNCH);

            } else {
                /* 获取到一个传输，正确处理该传输即可 */
                am_spi_transfer_t *p_cur_trans = __spi_trans_out(p_cur_msg);
                p_dev->p_cur_trans             = p_cur_trans;

                /* reset current data pointer */
                p_dev->data_ptr       = 0;
                p_dev->nbytes_to_recv = 0;

                /* 配置SPI传输参数 */
                __spi_config(p_dev);

                /* 判断使用何种传输方式 */
                if (p_dev->trans_mode == AM_LPC82X_SPI_WORK_MODE_INT) {

                    /* 设置位宽 */
                    amhw_lpc82x_spi_data_flen_set(p_hw_spi,
                                        p_dev->p_cur_spi_dev->bits_per_word);
                    /* CS选通 */
                    __spi_cs_on(p_dev, p_dev->p_cur_spi_dev);

                    /* 下一状态是使用中断方式发送数据 */
                    __SPI_NEXT_STATE(__SPI_ST_M_SEND_DATA, __SPI_EVT_M_SEND_DATA);

                } else if (p_dev->trans_mode == AM_LPC82X_SPI_WORK_MODE_DMA) {

                    /* 下一状态是使用DMA发送数据 */
                   __SPI_NEXT_STATE(__SPI_ST_DMA_TRANS_DATA, __SPI_EVT_DMA_TRANS_DATA);

                } else {
                    return -AM_ENOTSUP;
                }
            }
            break;
        }

        case __SPI_ST_M_SEND_DATA:    /* 发送数据 */
        {
            am_spi_message_t  *p_cur_msg   = p_dev->p_cur_msg;
            am_spi_transfer_t *p_cur_trans = p_dev->p_cur_trans;

            if (event != __SPI_EVT_M_SEND_DATA) {
                return -AM_EINVAL;  /* 主机发送状态等待的消息必须是发送数据 */
            }

            /* 没有更多需要传送的数据了 */
            if (p_dev->data_ptr >= p_cur_trans->nbytes) {

                 /* 关闭中断等待发送就绪 */
                 amhw_lpc82x_spi_int_disable(p_hw_spi,AMHW_LPC82X_SPI_INT_TXRDY);

                 /* 回到传输开始状态 */
                 __SPI_NEXT_STATE(__SPI_ST_TRANS_START, __SPI_EVT_TRANS_LAUNCH);

                 break;
            } else {
                if ((amhw_lpc82x_spi_stat_get(p_hw_spi) & AMHW_LPC82X_SPI_STAT_TXRDY) != 0) {

                    /* 最后一个字的传输配置 */
                    if (p_dev->p_cur_spi_dev->bits_per_word <= 8) {
                        if ((p_dev->data_ptr == (p_cur_trans->nbytes - 1))) {
                            amhw_lpc82x_spi_txctl_set(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_EOT);
                        } else {
                            amhw_lpc82x_spi_txctl_clr(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_EOT);
                        }
                    } else {
                        if ((p_dev->data_ptr == (p_cur_trans->nbytes - 2))) {
                            amhw_lpc82x_spi_txctl_set(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_EOT);
                        } else {
                            amhw_lpc82x_spi_txctl_clr(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_EOT);
                        }
                    }

                    /* 不需要接收数据 */
                    if (p_cur_trans->p_rxbuf == NULL) {
                        amhw_lpc82x_spi_txctl_set(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_RXIGNORE);
                    } else {
                        amhw_lpc82x_spi_txctl_clr(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_RXIGNORE);
                    }

                    /* 向从机写数据 */
                    __spi_write_data(p_dev);

                    /* 禁能发送中断 */
                    amhw_lpc82x_spi_int_disable(p_hw_spi,AMHW_LPC82X_SPI_INT_TXRDY);

                    /* 忽略了接收 */
                    if (p_cur_trans->p_rxbuf == NULL) {
                        p_dev->nbytes_to_recv = 0;
                        p_dev->data_ptr +=
                              (p_dev->p_cur_spi_dev->bits_per_word < 9 ? 1 : 2);
                        p_cur_msg->actual_length +=
                              (p_dev->p_cur_spi_dev->bits_per_word < 9 ? 1 : 2);

                        /* 忽略了接收，下一状态还是发送状态 */
                        __SPI_NEXT_STATE(__SPI_ST_M_SEND_DATA, __SPI_EVT_NONE);

                        /* 关闭接收中断 */
                        amhw_lpc82x_spi_int_disable(p_hw_spi,AMHW_LPC82X_SPI_INT_RXRDY);

                        /* 开启发送中断，通过发送完成切换到下一个状态 */
                        amhw_lpc82x_spi_int_enable(p_hw_spi,AMHW_LPC82X_SPI_INT_TXRDY);
                    } else {

                        /* 未忽略接收，下一状态是接收状态 */
                        __SPI_NEXT_STATE(__SPI_ST_M_RECV_DATA, __SPI_EVT_NONE);

                        /* Enable RX interrupt */
                        amhw_lpc82x_spi_int_enable(p_hw_spi,
                                                   AMHW_LPC82X_SPI_INT_RXRDY |
                                                   AMHW_LPC82X_SPI_INT_RXOV  |
                                                   AMHW_LPC82X_SPI_INT_TXUR  );
                    }
                } else {
                    /* 打开中断等待发送就绪 */
                    amhw_lpc82x_spi_int_enable(p_hw_spi,AMHW_LPC82X_SPI_INT_TXRDY);
                }
            }
            break;
        }

        case __SPI_ST_M_RECV_DATA:      /* 接收数据 */
        {
            if (event != __SPI_EVT_M_RECV_DATA) {
                return -AM_EINVAL;      /* 主机接收状态等待的消息必须是接收数据 */
            }

            /* 读取数据 */
            __spi_read_data(p_dev);

            /* 没有更多的数据需要接收了 */
            if (p_dev->nbytes_to_recv == 0) {

                /* 传输剩余数据 */
                __SPI_NEXT_STATE(__SPI_ST_M_SEND_DATA, __SPI_EVT_M_SEND_DATA);

                /* 禁能发送及接收中断 */
                amhw_lpc82x_spi_int_disable(p_hw_spi,AMHW_LPC82X_SPI_INT_RXRDY |
                                                     AMHW_LPC82X_SPI_INT_TXRDY |
                                                     AMHW_LPC82X_SPI_INT_RXOV  |
                                                     AMHW_LPC82X_SPI_INT_TXUR  |
                                                     AMHW_LPC82X_SPI_INT_SSA   |
                                                     AMHW_LPC82X_SPI_INT_SSD  );
            /* 需要接收更多数据 */
            } else {
                /* 这儿什么也不要做 */
            }
            break;
        }

        case __SPI_ST_DMA_TRANS_DATA:    /* DMA发送数据 */
        {
            if (event != __SPI_EVT_DMA_TRANS_DATA) {
                return -AM_EINVAL;  /* 主机发送状态等待的消息必须是发送数据 */
            }

            /* 下一状态还是发送状态 */
            __SPI_NEXT_STATE(__SPI_ST_TRANS_START, __SPI_EVT_NONE);

            /* 使用DMA传输 */
            __spi_dma_trans(p_dev);

            break;
        }

        /*
         * 永远也不该运行到这儿
         */
        default:
            break;
        }

        /* 没有来自内部的信息, 跳出 */
        if (new_event == __SPI_EVT_NONE) {
            break;
        }
    }
    return AM_OK;
}

/******************************************************************************/

/**
 * \brief SPI 初始化
 */
am_spi_handle_t am_lpc82x_spi_init (am_lpc82x_spi_dev_t           *p_dev,
                                    const am_lpc82x_spi_devinfo_t *p_devinfo)
{
    if (NULL == p_devinfo || NULL == p_dev ) {
        return NULL;
    }

    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    p_dev->spi_serve.p_funcs = (struct am_spi_drv_funcs *)&__g_spi_drv_funcs;
    p_dev->spi_serve.p_drv   = p_dev;
    
    p_dev->p_devinfo = p_devinfo;

    p_dev->p_cur_spi_dev    = NULL;
    p_dev->p_tgl_dev        = NULL;
    p_dev->busy             = FALSE;
    p_dev->p_cur_msg        = NULL;
    p_dev->p_cur_trans      = NULL;
    p_dev->data_ptr         = 0;
    p_dev->nbytes_to_recv   = 0;
    p_dev->state            = __SPI_ST_IDLE;     /* 初始化为空闲状态 */

    am_list_head_init(&(p_dev->msg_list));

    if (__spi_hard_init(p_dev) != AM_OK) {
        return NULL;
    }

    am_int_connect(p_dev->p_devinfo->inum,
                   __spi_irq_handler     ,
                  (void *)p_dev         );

    am_int_enable(p_dev->p_devinfo->inum);

    /* 如果使用DMA传输 */
    if (p_devinfo->trans_mode == AM_LPC82X_SPI_WORK_MODE_DMA) {

        /* 配置DMA发送通道 */
        p_dev->p_ctr_tx = am_lpc82x_dma_controller_get(p_devinfo->dma_chan_tx,
                                                       p_devinfo->dma_chan_prio_tx |
                                                       DMA_CHAN_OPT_PERIPH_REQ_EN);
        /* 配置DMA接收通道 */
        p_dev->p_ctr_rx = am_lpc82x_dma_controller_get(p_devinfo->dma_chan_rx,
                                                       p_devinfo->dma_chan_prio_rx |
                                                       DMA_CHAN_OPT_PERIPH_REQ_EN);
    }

    return &(p_dev->spi_serve);
}

/**
 * \brief SPI 去除初始化
 */
void am_lpc82x_spi_deinit (am_spi_handle_t handle)
{
    am_lpc82x_spi_dev_t *p_dev    = (am_lpc82x_spi_dev_t *)handle;
    amhw_lpc82x_spi_t   *p_hw_spi = (amhw_lpc82x_spi_t *)(p_dev->p_devinfo->spi_regbase);

    if (NULL == p_dev) {
        return ;
    }

    p_dev->spi_serve.p_funcs = NULL;
    p_dev->spi_serve.p_drv   = NULL;

    /* 禁能 SPI */
    amhw_lpc82x_spi_disable(p_hw_spi);

    /* 关闭SPI中断号并断开连接 */
    am_int_disable(p_dev->p_devinfo->inum);
    am_int_disconnect(p_dev->p_devinfo->inum,
                      __spi_irq_handler,
                     (void *)p_dev);

    /* 释放DMA控制器 */
    am_lpc82x_dma_controller_release(p_dev->p_ctr_tx);
    am_lpc82x_dma_controller_release(p_dev->p_ctr_rx);

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}
