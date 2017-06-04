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
 * \brief SPI ������ʵ�ֺ���
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
  ģ���ڱ�������
*******************************************************************************/

const uint16_t       const_high = 0xFFFF;     /* SPI����ʱMOSIΪ�ߵ�ƽ */
const uint16_t       const_low  = 0x0000;     /* SPI����ʱMOSIΪ�͵�ƽ */

/*******************************************************************************
  SPI ״̬���¼�����
*******************************************************************************/

/**
 * SPI ������״̬
 */

#define __SPI_ST_IDLE               0                   /* ����״̬ */
#define __SPI_ST_MSG_START          1                   /* ��Ϣ��ʼ */
#define __SPI_ST_TRANS_START        2                   /* ���俪ʼ */
#define __SPI_ST_M_SEND_DATA        3                   /* �������� */
#define __SPI_ST_M_RECV_DATA        4                   /* �������� */
#define __SPI_ST_DMA_TRANS_DATA     5                   /* DMA ���� */

/**
 * SPI �������¼�
 *
 * ��32λ����16λ���¼���ţ���16λ���¼�����
 */

#define __SPI_EVT_NUM_GET(event)    ((event) & 0xFFFF)
#define __SPI_EVT_PAR_GET(event)    ((event >> 16) & 0xFFFF)
#define __SPI_EVT(evt_num, evt_par) (((evt_num) & 0xFFFF) | ((evt_par) << 16))

#define __SPI_EVT_NONE              __SPI_EVT(0, 0)     /* ���¼� */
#define __SPI_EVT_TRANS_LAUNCH      __SPI_EVT(1, 0)     /* ������� */
#define __SPI_EVT_M_SEND_DATA       __SPI_EVT(2, 0)     /* �������� */
#define __SPI_EVT_M_RECV_DATA       __SPI_EVT(3, 0)     /* �������� */
#define __SPI_EVT_DMA_TRANS_DATA    __SPI_EVT(4, 0)     /* DMA�������� */

/*******************************************************************************
  ģ���ں�������
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
  SPI������������
*******************************************************************************/
am_local int __spi_info_get (void *p_arg, am_spi_info_t   *p_info);
am_local int __spi_setup    (void *p_arg, am_spi_device_t *p_dev );
am_local int __spi_msg_start (void              *p_drv,
                              am_spi_device_t   *p_dev,
                              am_spi_message_t  *p_msg);

/**
 * \brief SPI ��������
 */
am_local am_const struct am_spi_drv_funcs __g_spi_drv_funcs = {
    __spi_info_get,
    __spi_setup,
    __spi_msg_start,
};

/******************************************************************************/

/**
 * \brief Ĭ��CS�ſ��ƺ������ߵ�ƽ��Ч
 */
am_local
void __spi_default_cs_ha (am_spi_device_t *p_dev, int state)
{
    am_gpio_set(p_dev->cs_pin, state ? 1 : 0);
}

/**
 * \brief Ĭ��CS�ſ��ƺ������͵�ƽ��Ч
 */
am_local
void __spi_default_cs_la (am_spi_device_t *p_dev, int state)
{
    am_gpio_set(p_dev->cs_pin, state ? 0 : 1);
}

/**
 * \brief Ĭ��CS�ſ��ƺ�������Ӳ�����п���
 */
am_local
void __spi_default_cs_dummy (am_spi_device_t *p_dev, int state)
{
    return;
}

/**
 * \brief CS���ż���
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
 * \brief CS����ȥ��
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

    /* tx_buf ��Ч */
    if (p_trans->p_txbuf != NULL) {
        if ((p_dev->p_cur_spi_dev->bits_per_word) <= 8) {
            /** \brief ���������ݵĻ�ַ+ƫ�� */
            uint8_t *ptr = (uint8_t *)(p_trans->p_txbuf) + p_dev->data_ptr;
            amhw_lpc82x_spi_txdat(p_hw_spi, *ptr);
        } else {
            uint16_t *ptr = (uint16_t *)((uint8_t *)(p_trans->p_txbuf)
                                                     + p_dev->data_ptr);
            amhw_lpc82x_spi_txdat(p_hw_spi, *ptr);
        }
    /* tx_buf ��Ч */
    } else {
        /** \brief ������������Ч ֱ�ӷ�0 */
        amhw_lpc82x_spi_txdat(p_hw_spi, 0x0000);
    }

    /** \brief ��������ݵ�byte�� */
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

    /* rx_buf ��Ч */
    if (p_trans->p_rxbuf != NULL && p_dev->nbytes_to_recv) {
        if ((p_dev->p_cur_spi_dev->bits_per_word) <= 8) {
             *p_buf8 = amhw_lpc82x_spi_rxdat_8bit(p_hw_spi);
        } else {
             *(uint16_t *)(p_buf8) = amhw_lpc82x_spi_rxdat_16bit(p_hw_spi);
        }

    /* rx_buf ��Ч���߲���Ҫ�������� */
    } else {
        if ((p_dev->p_cur_spi_dev->bits_per_word) <= 8) {
            (void)amhw_lpc82x_spi_rxdat_8bit(p_hw_spi);

        } else {
            (void)amhw_lpc82x_spi_rxdat_16bit(p_hw_spi);
        }
    }

    /* �Ѿ����ͻ���յ�����byte�� */
    p_dev->data_ptr += p_dev->nbytes_to_recv;
    p_dev->nbytes_to_recv = 0;
}

/******************************************************************************/

/**
 * \brief ���һ�� message �������������б�ĩβ
 * \attention ���ô˺�����������������
 */
am_static_inline
void __spi_msg_in (am_lpc82x_spi_dev_t *p_dev, struct am_spi_message *p_msg)
{
    am_list_add_tail((struct am_list_head *)(&p_msg->ctlrdata),
                                             &(p_dev->msg_list));
}

/**
 * \brief �ӿ����������б��ͷȡ��һ�� message
 * \attention ���ô˺�����������������
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
 * \brief ��message�б��ͷȡ��һ�� transfer
 * \attention ���ô˺�����������������
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

    /* Ĭ������Ϊ8λ����󲻳���16λ */
    if (p_dev->bits_per_word == 0) {
        p_dev->bits_per_word = 8;
    } else if (p_dev->bits_per_word > 16) {
        return -AM_ENOTSUP;
    }

    /* ���SPI���ʲ��ܳ�����ʱ�ӣ���С����С����ʱ��65536��Ƶ */
    max_speed = amhw_lpc82x_clk_periph_freq_get(p_hw_spi);
    min_speed = max_speed / 65536;

    if (p_dev->max_speed_hz > max_speed) {
        p_dev->max_speed_hz = max_speed;
    } else if (p_dev->max_speed_hz < min_speed) {
        return -AM_ENOTSUP;
    }

    /* ��Ƭѡ���� */
    if (p_dev->mode & AM_SPI_NO_CS) {
        p_dev->pfunc_cs = __spi_default_cs_dummy;

    /* ��Ƭѡ���� */
    }  else {

        /* ���ṩ��Ĭ��Ƭѡ���� */
        if (p_dev->pfunc_cs == NULL) {

            /* Ƭѡ�ߵ�ƽ��Ч */
            if (p_dev->mode & AM_SPI_CS_HIGH) {
                am_gpio_pin_cfg(p_dev->cs_pin, AM_GPIO_OUTPUT_INIT_LOW);
                p_dev->pfunc_cs = __spi_default_cs_ha;

            /* Ƭѡ�͵�ƽ��Ч */
            } else {
                am_gpio_pin_cfg(p_dev->cs_pin, AM_GPIO_OUTPUT_INIT_HIGH);
                p_dev->pfunc_cs = __spi_default_cs_la;
            }
        }
    }

    /* ���Ƭѡ�ź� */
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

    /* ������ʵ��� PCLK */
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
 * \brief SPI Ӳ����ʼ��
 */
am_local
int __spi_hard_init (am_lpc82x_spi_dev_t *p_this)
{
    amhw_lpc82x_spi_t *p_hw_spi = (amhw_lpc82x_spi_t *)(p_this->p_devinfo->spi_regbase);

    if (p_this == NULL) {
        return -AM_EINVAL;
    }

    /* �ȴ�SPI���У������������üĴ��� */
    while ((amhw_lpc82x_spi_stat_get(p_hw_spi) & AMHW_LPC82X_SPI_STAT_IDLE) == 0);
    amhw_lpc82x_spi_cfg_set(p_hw_spi, (AMHW_LPC82X_SPI_CFG_ENABLE |
                                       AMHW_LPC82X_SPI_CFG_MASTER));

    /* ��ʼ������SPI */
    return AM_OK;
}

/**
 * \brief SPI �жϾ������
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

        /* �������״̬ */
        amhw_lpc82x_spi_stat_clr(p_hw_spi, AMHW_LPC82X_SPI_STAT_RXOV |
                                           AMHW_LPC82X_SPI_STAT_TXUR |
                                           AMHW_LPC82X_SPI_STAT_SSA  |
                                           AMHW_LPC82X_SPI_STAT_SSD);

        /* ���������ж� */
        amhw_lpc82x_spi_int_disable(p_hw_spi, AMHW_LPC82X_SPI_INT_RXRDY |
                                              AMHW_LPC82X_SPI_INT_TXRDY |
                                              AMHW_LPC82X_SPI_INT_RXOV  |
                                              AMHW_LPC82X_SPI_INT_TXUR  |
                                              AMHW_LPC82X_SPI_INT_SSA   |
                                              AMHW_LPC82X_SPI_INT_SSD);
    }

    /* �������� */
    if (spi_status & AMHW_LPC82X_SPI_STAT_TXRDY) {
        /* run the state machine */
        __spi_mst_sm_event(p_dev, __SPI_EVT_M_SEND_DATA);
    }

    /* �������� */
    if (spi_status & AMHW_LPC82X_SPI_STAT_RXRDY) {
        __spi_mst_sm_event(p_dev, __SPI_EVT_M_RECV_DATA);
    }
}

am_local
void __dma_isr (void *p_arg, int stat)
{
    am_lpc82x_spi_dev_t  *p_this   = (am_lpc82x_spi_dev_t *)p_arg;
    amhw_lpc82x_spi_t    *p_hw_spi = (amhw_lpc82x_spi_t *)(p_this->p_devinfo->spi_regbase);

    /* ������ж�A���� */
    if (stat == AM_LPC82X_DMA_STAT_INTA)
    {
        /* ��¼�ɹ������ֽ��� */
        if (p_this->p_cur_trans->p_txbuf != NULL) {
            p_this->p_cur_msg->actual_length += (p_this->p_cur_trans->nbytes)
                            *(p_this->p_cur_spi_dev->bits_per_word < 9 ? 1 : 2);
        }

        /* �ȴ�SPI���ݳ��׷��ͽ��� */
        while(!(amhw_lpc82x_spi_stat_get(p_hw_spi) & AMHW_LPC82X_SPI_STAT_IDLE));
        /* ������� */
        __spi_mst_sm_event(p_this, __SPI_EVT_TRANS_LAUNCH);

    } else {
        /* �ж�Դ��ƥ�� */
    }
}

/**
 * \brief  SPI ʹ��DMA����
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

    const uint16_t                *p_tx_buf    = NULL; /* ָ���û����ͻ�����ָ�� */
          uint16_t                *p_rx_buf    = NULL; /* ָ���û����ջ�����ָ�� */
          uint16_t                rx_trash     = 0;    /* �������ݵ���ʱ���ձ��� */
          uint32_t                p_tx_buf_eot = 0;    /* һ֡�е����һ������ */

    uint8_t    eot_nbyte    = 0;    /* ���ͽ����ֵ������ֽ��� */
    uint32_t   dma_flags[3] = {0};  /* DMA����ͨ�������� */

    /* ֻ���ղ��������� */
    if (p_cur_trans->p_txbuf == NULL) {
        if (p_cur_trans->flags & AM_SPI_READ_MOSI_HIGH) {
            p_tx_buf = &const_high;                       /* MOSI����ʱ�ߵ�ƽ */
        } else {
            p_tx_buf = &const_low;                        /* MOSI����ʱ�͵�ƽ */
        }
        p_tx_buf_eot = AM_SBF(p_cur_trans->bits_per_word - 1, 24) |
                       *((uint16_t *)p_tx_buf);

    /* ���ڷ������� */
    } else {
        p_tx_buf = (const uint16_t *)p_cur_trans->p_txbuf; /* MOSI�����û����� */
        if (p_cur_trans->bits_per_word > 8) {
            p_tx_buf_eot = AM_SBF(p_cur_trans->bits_per_word - 1, 24) |
                           ((uint16_t *)p_tx_buf)[p_cur_trans->nbytes/2 - 1];
        } else {
            p_tx_buf_eot = AM_SBF(p_cur_trans->bits_per_word - 1, 24) |
                           ((uint8_t *)p_tx_buf)[p_cur_trans->nbytes - 1];
        }
    }

    /* ����֡���������һ���ַ��Ƿ����Ƭѡ�źţ�Ӳ��Ƭѡ�� */
    p_tx_buf_eot |= AMHW_LPC82X_SPI_TXCTRL_EOT;

    /* ֻ���Ͳ��������� */
    if (p_cur_trans->p_rxbuf == NULL) {
        p_rx_buf = &rx_trash;                           /* ָ��������������� */

    /* ���ڽ������� */
    } else {
    p_rx_buf = p_cur_trans->p_rxbuf;                    /* ָ���û����ջ����� */
    }

    /* DMA����ͨ������ */
    dma_flags[0] = AM_LPC82X_DMA_XFER_VALID        |    /* ��ǰͨ����������Ч */
                   AM_LPC82X_DMA_XFER_RELOAD       |    /* ��������ͨ�������� */
                   AM_LPC82X_DMA_XFER_SWTRIG       |    /* ������� */
                   AM_LPC82X_DMA_XFER_DSTINC_NOINC;     /* ����Ŀ���ַ������ */

    /* DMA���ͽ�����ͨ������ */
    dma_flags[1] = AM_LPC82X_DMA_XFER_VALID        |    /* ��ǰͨ����������Ч */
                   AM_LPC82X_DMA_XFER_SWTRIG       |    /* ������� */
                   AM_LPC82X_DMA_XFER_CLRTRIG      |    /* ���������־ */
                   AM_LPC82X_DMA_XFER_DSTINC_NOINC |    /* ����Ŀ���ַ������ */
                   AM_LPC82X_DMA_XFER_WIDTH_32BIT  |    /* ���ݿ��32λ */
                   AM_LPC82X_DMA_XFER_SRCINC_NOINC;     /* Դ��ַ������ */


    /* DMA����ͨ������ */
    dma_flags[2] = AM_LPC82X_DMA_XFER_VALID        |      /* ��ǰ������������Ч */
                   AM_LPC82X_DMA_XFER_SWTRIG       |      /* ������� */
                   AM_LPC82X_DMA_XFER_CLRTRIG      |      /* ���������־ */
                   AM_LPC82X_DMA_XFER_SRCINC_NOINC |      /* Դ��ַ������ */
                   AM_LPC82X_DMA_XFER_SETINTA;            /* ʹ���ж�A */

    /* ����DMA����λ�� */
    if (p_cur_trans->bits_per_word > 8) {
        dma_flags[0] |= AM_LPC82X_DMA_XFER_WIDTH_16BIT;
        dma_flags[2] |= AM_LPC82X_DMA_XFER_WIDTH_16BIT;
        eot_nbyte     = 2;
    } else {
        dma_flags[0] |= AM_LPC82X_DMA_XFER_WIDTH_8BIT;
        dma_flags[2] |= AM_LPC82X_DMA_XFER_WIDTH_8BIT;
        eot_nbyte     = 1;
    }

    /* ���ͻ�������Ч */
    if (p_cur_trans->p_txbuf != NULL) {
        dma_flags[0] |= AM_LPC82X_DMA_XFER_SRCINC_1X;      /* Դ��ַ������� */
    } else {
        dma_flags[0] |= AM_LPC82X_DMA_XFER_SRCINC_NOINC;   /* Դ��ַ������ */
    }

    /* ���ջ�������Ч */
    if (p_cur_trans->p_rxbuf != NULL) {
        dma_flags[2] |= AM_LPC82X_DMA_XFER_DSTINC_1X;      /* Ŀ���ַ������� */
    } else {
        dma_flags[2] |= AM_LPC82X_DMA_XFER_DSTINC_NOINC ;  /* Ŀ���ַ������ */
    }

    /* ��ֻ����һ�δ���ʱ */
    if (((p_cur_trans->nbytes == 1) && (p_cur_trans->bits_per_word < 9)) ||
        ((p_cur_trans->nbytes == 2) && (p_cur_trans->bits_per_word > 8))) {

        /* ����������ͨ�������� */
        am_lpc82x_dma_xfer_desc_build(&p_this->g_desc[0],              /* ������ */
                                      (uint32_t)&p_tx_buf_eot,         /* Դ��BUFF */
                                      (uint32_t)&(p_hw_spi->txdatctl), /* Ŀ��BUFF */
                                       4,                              /* TX�ֽ��� */
                                       dma_flags[1]);                  /* �������� */

    /* ���Ͷ�δ���ʱ */
    } else {

        /* ����ͨ�������� */
        am_lpc82x_dma_xfer_desc_build(&p_this->g_desc[0],              /* ������ */
                                      (uint32_t)p_tx_buf,              /* Դ��BUFF */
                                      (uint32_t)&(p_hw_spi->txdat),    /* Ŀ��BUFF */
                                       p_cur_trans->nbytes - eot_nbyte,/* TX�ֽ��� */
                                       dma_flags[0]);                  /* �������� */

        /* ����������ͨ�������� */
        am_lpc82x_dma_xfer_desc_build(&p_this->g_desc[1],              /* ������ */
                                      (uint32_t)&p_tx_buf_eot,         /* Դ��BUFF */
                                      (uint32_t)&(p_hw_spi->txdatctl), /* Ŀ��BUFF */
                                       4,                              /* TX�ֽ��� */
                                       dma_flags[1]);                  /* �������� */

        /* DMA���ӷ���ͨ�� */
        am_lpc82x_dma_xfer_desc_link(&p_this->g_desc[0], &p_this->g_desc[1]);
    }

    /* ����ͨ�������� */
    am_lpc82x_dma_xfer_desc_build(&p_this->g_desc[2],                  /* ������ */
                                  (uint32_t)&(p_hw_spi->rxdat),        /* Դ��BUFF */
                                  (uint32_t)p_rx_buf,                  /* Ŀ��BUFF */
                                   p_cur_trans->nbytes,                /* TX�ֽ��� */
                                   dma_flags[2]);                      /* �������� */

    /* ��λƬѡ�ź� */
    __spi_cs_on(p_this, p_this->p_cur_spi_dev);

    /* ����DMA���䣬���Ͽ�ʼ���� */
    am_lpc82x_dma_xfer_desc_startup(p_ctr_rx,
                                    &p_this->g_desc[2],
                                    __dma_isr,
                                    (void *)p_this);

    /* ����DMA���䣬���Ͽ�ʼ���� */
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

    /* ���Ϊ0��ʹ��Ĭ�ϲ���ֵ */
    if (p_trans->bits_per_word == 0) {
        p_trans->bits_per_word = p_this->p_cur_spi_dev->bits_per_word;
    }

    if (p_trans->speed_hz == 0) {
        p_trans->speed_hz = p_this->p_cur_spi_dev->max_speed_hz;
    }

    /* �����ֽ�����Ч�Լ�� */
    if (p_trans->bits_per_word > 16 || p_trans->bits_per_word < 1) {
        return -AM_EINVAL;
    }

    /* ���÷�Ƶֵ��Ч�Լ�� */
    if (p_trans->speed_hz > amhw_lpc82x_clk_periph_freq_get(p_hw_spi) ||
        p_trans->speed_hz < (amhw_lpc82x_clk_periph_freq_get(p_hw_spi) / 65536)) {
        return -AM_EINVAL;
    }

    /* ���ͺͽ��ջ�������Ч�Լ�� */
    if ((p_trans->p_txbuf == NULL) && (p_trans->p_rxbuf == NULL)) {
        return -AM_EINVAL;
    }

    /* �����ֽ������ */
    if (p_trans->nbytes == 0) {
        return -AM_ELOW;
    }

    /**
     * ���õ�ǰ�豸ģʽ
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

    /* ����ģʽ(���������������ӻ�ģʽ) */
    amhw_lpc82x_spi_cfg_set(p_hw_spi, (mode_flag                  |
                                       AMHW_LPC82X_SPI_CFG_ENABLE |
                                       AMHW_LPC82X_SPI_CFG_MASTER));

    /* ���õ�֡����λ�� */
    amhw_lpc82x_spi_data_flen_set (p_hw_spi, p_trans->bits_per_word);

    /* ���÷�Ƶֵ����SPI���� */
    div_val = (amhw_lpc82x_clk_periph_freq_get(p_hw_spi) / p_trans->speed_hz);
    amhw_lpc82x_spi_div_set(p_hw_spi, div_val);

    /* ���״̬λ */
    amhw_lpc82x_spi_stat_clr(p_hw_spi, AMHW_LPC82X_SPI_STAT_RXOV |
                                       AMHW_LPC82X_SPI_STAT_TXUR |
                                       AMHW_LPC82X_SPI_STAT_SSA  |
                                       AMHW_LPC82X_SPI_STAT_SSD);

    /* ʹ��SPI���շ�������ж� */
    amhw_lpc82x_spi_int_enable(p_hw_spi, AMHW_LPC82X_SPI_INT_RXOV |
                                         AMHW_LPC82X_SPI_INT_TXUR );

    return AM_OK;
}

/**
 * \brief SPI �������ݺ���
 */
am_local
int __spi_msg_start (void              *p_drv,
                     am_spi_device_t   *p_dev,
                     am_spi_message_t  *p_msg)
{
    am_lpc82x_spi_dev_t *p_this   = (am_lpc82x_spi_dev_t *)p_drv;
    amhw_lpc82x_spi_t   *p_hw_spi = (amhw_lpc82x_spi_t *)(p_this->p_devinfo->spi_regbase);

    int key;

    p_this->p_cur_spi_dev  = p_dev;                         /* ����ǰ�豸������Ϣ���� */
    p_this->p_cur_msg      = p_msg;                         /* ����ǰ�豸������Ϣ���� */
    p_this->nbytes_to_recv = 0;                             /* �������ַ�����0 */
    p_this->data_ptr       = 0;                             /* �ѽ����ַ�����0 */
    p_this->trans_mode     = p_this->p_devinfo->trans_mode; /* ���ش���ģʽ */

    /* �豸��Ч�Լ�� */
    if ((p_drv              == NULL) ||
        (p_dev              == NULL) ||
        (p_msg              == NULL) ){
        return -AM_EINVAL;
    }

    /* ���֡����λ */
    amhw_lpc82x_spi_txctl_clr(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_EOT);

    key = am_int_cpu_lock();

    /* ��ǰ���ڴ�����Ϣ��ֻ��Ҫ���µ���Ϣ���������� */
    if (p_this->busy == TRUE) {
        __spi_msg_in(p_this, p_msg);
        am_int_cpu_unlock(key);
        return AM_OK;
    } else {
        p_this->busy = TRUE;
        __spi_msg_in(p_this, p_msg);
        p_msg->status = -AM_EISCONN; /* �����Ŷ��� */
        am_int_cpu_unlock(key);

        /* ����״̬�� */
        return __spi_mst_sm_event(p_this, __SPI_EVT_TRANS_LAUNCH);
    }
}

/******************************************************************************/

/*  ״̬���ڲ�״̬�л� */
#define __SPI_NEXT_STATE(s, e) \
    do { \
        p_dev->state = (s); \
        new_event = (e); \
    } while(0)

/**
 * \brief  SPI ʹ��״̬������
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

        if (new_event != __SPI_EVT_NONE) {     /* ������¼��Ƿ������ڲ� */
            event     = new_event;
            new_event  = __SPI_EVT_NONE;
        }

        switch (p_dev->state) {

        case __SPI_ST_IDLE:         /* ���������ڿ���״̬ */
        {
            if (event != __SPI_EVT_TRANS_LAUNCH) {
                return -AM_EINVAL;  /* ����״̬�ȴ�����Ϣ�������������� */
            }
            /* �л�����Ϣ��ʼ״̬������break */
        }

        case __SPI_ST_MSG_START:    /* ��Ϣ��ʼ */
        {
            am_spi_message_t  *p_cur_msg   = NULL;

            int key;

            if (event != __SPI_EVT_TRANS_LAUNCH) {
                return -AM_EINVAL;  /* ��Ϣ��ʼ״̬�ȴ�����Ϣ�������������� */
            }

            key = am_int_cpu_lock();
            p_cur_msg          = __spi_msg_out(p_dev);
            p_dev->p_cur_msg   = p_cur_msg;

            if (p_cur_msg) {
                p_cur_msg->status = -AM_EINPROGRESS;
            } else {
                /* ���������ж� */
                amhw_lpc82x_spi_int_disable(p_hw_spi, AMHW_LPC82X_SPI_INT_RXRDY |
                                                      AMHW_LPC82X_SPI_INT_TXRDY |
                                                      AMHW_LPC82X_SPI_INT_RXOV  |
                                                      AMHW_LPC82X_SPI_INT_TXUR  |
                                                      AMHW_LPC82X_SPI_INT_SSA   |
                                                      AMHW_LPC82X_SPI_INT_SSD   );
                p_dev->busy = FALSE;
            }
            am_int_cpu_unlock(key);

            /* ����Ҫ�������Ϣ */
            if (p_cur_msg == NULL) {
                __SPI_NEXT_STATE(__SPI_ST_IDLE, __SPI_EVT_NONE);
                break;
            } else {

                /* ֱ�ӽ�����һ��״̬����ʼһ�����䣬�˴�����break */
                __SPI_NEXT_STATE(__SPI_ST_TRANS_START, __SPI_EVT_TRANS_LAUNCH);

                event     = new_event;
                new_event = __SPI_EVT_NONE;
            }
        }

        case __SPI_ST_TRANS_START:  /* ���俪ʼ */
        {
            am_spi_message_t  *p_cur_msg   = p_dev->p_cur_msg;

            if (event != __SPI_EVT_TRANS_LAUNCH) {
                return -AM_EINVAL;  /* ���俪ʼ״̬�ȴ�����Ϣ�������������� */
            }

            /* ��ǰ��Ϣ������� */
            if (am_list_empty(&(p_cur_msg->transfers))) {

                p_cur_msg->actual_length = 0;

                /* ��Ϣ���ڴ����� */
                if (p_cur_msg->status == -AM_EINPROGRESS) {
                    p_cur_msg->status = AM_OK;
                }

                /* notify the caller  */
                if (p_cur_msg->pfn_complete != NULL) {
                    p_cur_msg->pfn_complete(p_cur_msg->p_arg);
                }

                /* Ƭѡ�ر� */
                __spi_cs_off(p_dev, p_dev->p_cur_spi_dev);

                __SPI_NEXT_STATE(__SPI_ST_MSG_START, __SPI_EVT_TRANS_LAUNCH);

            } else {
                /* ��ȡ��һ�����䣬��ȷ����ô��伴�� */
                am_spi_transfer_t *p_cur_trans = __spi_trans_out(p_cur_msg);
                p_dev->p_cur_trans             = p_cur_trans;

                /* reset current data pointer */
                p_dev->data_ptr       = 0;
                p_dev->nbytes_to_recv = 0;

                /* ����SPI������� */
                __spi_config(p_dev);

                /* �ж�ʹ�ú��ִ��䷽ʽ */
                if (p_dev->trans_mode == AM_LPC82X_SPI_WORK_MODE_INT) {

                    /* ����λ�� */
                    amhw_lpc82x_spi_data_flen_set(p_hw_spi,
                                        p_dev->p_cur_spi_dev->bits_per_word);
                    /* CSѡͨ */
                    __spi_cs_on(p_dev, p_dev->p_cur_spi_dev);

                    /* ��һ״̬��ʹ���жϷ�ʽ�������� */
                    __SPI_NEXT_STATE(__SPI_ST_M_SEND_DATA, __SPI_EVT_M_SEND_DATA);

                } else if (p_dev->trans_mode == AM_LPC82X_SPI_WORK_MODE_DMA) {

                    /* ��һ״̬��ʹ��DMA�������� */
                   __SPI_NEXT_STATE(__SPI_ST_DMA_TRANS_DATA, __SPI_EVT_DMA_TRANS_DATA);

                } else {
                    return -AM_ENOTSUP;
                }
            }
            break;
        }

        case __SPI_ST_M_SEND_DATA:    /* �������� */
        {
            am_spi_message_t  *p_cur_msg   = p_dev->p_cur_msg;
            am_spi_transfer_t *p_cur_trans = p_dev->p_cur_trans;

            if (event != __SPI_EVT_M_SEND_DATA) {
                return -AM_EINVAL;  /* ��������״̬�ȴ�����Ϣ�����Ƿ������� */
            }

            /* û�и�����Ҫ���͵������� */
            if (p_dev->data_ptr >= p_cur_trans->nbytes) {

                 /* �ر��жϵȴ����;��� */
                 amhw_lpc82x_spi_int_disable(p_hw_spi,AMHW_LPC82X_SPI_INT_TXRDY);

                 /* �ص����俪ʼ״̬ */
                 __SPI_NEXT_STATE(__SPI_ST_TRANS_START, __SPI_EVT_TRANS_LAUNCH);

                 break;
            } else {
                if ((amhw_lpc82x_spi_stat_get(p_hw_spi) & AMHW_LPC82X_SPI_STAT_TXRDY) != 0) {

                    /* ���һ���ֵĴ������� */
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

                    /* ����Ҫ�������� */
                    if (p_cur_trans->p_rxbuf == NULL) {
                        amhw_lpc82x_spi_txctl_set(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_RXIGNORE);
                    } else {
                        amhw_lpc82x_spi_txctl_clr(p_hw_spi, AMHW_LPC82X_SPI_TXCTRL_RXIGNORE);
                    }

                    /* ��ӻ�д���� */
                    __spi_write_data(p_dev);

                    /* ���ܷ����ж� */
                    amhw_lpc82x_spi_int_disable(p_hw_spi,AMHW_LPC82X_SPI_INT_TXRDY);

                    /* �����˽��� */
                    if (p_cur_trans->p_rxbuf == NULL) {
                        p_dev->nbytes_to_recv = 0;
                        p_dev->data_ptr +=
                              (p_dev->p_cur_spi_dev->bits_per_word < 9 ? 1 : 2);
                        p_cur_msg->actual_length +=
                              (p_dev->p_cur_spi_dev->bits_per_word < 9 ? 1 : 2);

                        /* �����˽��գ���һ״̬���Ƿ���״̬ */
                        __SPI_NEXT_STATE(__SPI_ST_M_SEND_DATA, __SPI_EVT_NONE);

                        /* �رս����ж� */
                        amhw_lpc82x_spi_int_disable(p_hw_spi,AMHW_LPC82X_SPI_INT_RXRDY);

                        /* ���������жϣ�ͨ����������л�����һ��״̬ */
                        amhw_lpc82x_spi_int_enable(p_hw_spi,AMHW_LPC82X_SPI_INT_TXRDY);
                    } else {

                        /* δ���Խ��գ���һ״̬�ǽ���״̬ */
                        __SPI_NEXT_STATE(__SPI_ST_M_RECV_DATA, __SPI_EVT_NONE);

                        /* Enable RX interrupt */
                        amhw_lpc82x_spi_int_enable(p_hw_spi,
                                                   AMHW_LPC82X_SPI_INT_RXRDY |
                                                   AMHW_LPC82X_SPI_INT_RXOV  |
                                                   AMHW_LPC82X_SPI_INT_TXUR  );
                    }
                } else {
                    /* ���жϵȴ����;��� */
                    amhw_lpc82x_spi_int_enable(p_hw_spi,AMHW_LPC82X_SPI_INT_TXRDY);
                }
            }
            break;
        }

        case __SPI_ST_M_RECV_DATA:      /* �������� */
        {
            if (event != __SPI_EVT_M_RECV_DATA) {
                return -AM_EINVAL;      /* ��������״̬�ȴ�����Ϣ�����ǽ������� */
            }

            /* ��ȡ���� */
            __spi_read_data(p_dev);

            /* û�и����������Ҫ������ */
            if (p_dev->nbytes_to_recv == 0) {

                /* ����ʣ������ */
                __SPI_NEXT_STATE(__SPI_ST_M_SEND_DATA, __SPI_EVT_M_SEND_DATA);

                /* ���ܷ��ͼ������ж� */
                amhw_lpc82x_spi_int_disable(p_hw_spi,AMHW_LPC82X_SPI_INT_RXRDY |
                                                     AMHW_LPC82X_SPI_INT_TXRDY |
                                                     AMHW_LPC82X_SPI_INT_RXOV  |
                                                     AMHW_LPC82X_SPI_INT_TXUR  |
                                                     AMHW_LPC82X_SPI_INT_SSA   |
                                                     AMHW_LPC82X_SPI_INT_SSD  );
            /* ��Ҫ���ո������� */
            } else {
                /* ���ʲôҲ��Ҫ�� */
            }
            break;
        }

        case __SPI_ST_DMA_TRANS_DATA:    /* DMA�������� */
        {
            if (event != __SPI_EVT_DMA_TRANS_DATA) {
                return -AM_EINVAL;  /* ��������״̬�ȴ�����Ϣ�����Ƿ������� */
            }

            /* ��һ״̬���Ƿ���״̬ */
            __SPI_NEXT_STATE(__SPI_ST_TRANS_START, __SPI_EVT_NONE);

            /* ʹ��DMA���� */
            __spi_dma_trans(p_dev);

            break;
        }

        /*
         * ��ԶҲ�������е����
         */
        default:
            break;
        }

        /* û�������ڲ�����Ϣ, ���� */
        if (new_event == __SPI_EVT_NONE) {
            break;
        }
    }
    return AM_OK;
}

/******************************************************************************/

/**
 * \brief SPI ��ʼ��
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
    p_dev->state            = __SPI_ST_IDLE;     /* ��ʼ��Ϊ����״̬ */

    am_list_head_init(&(p_dev->msg_list));

    if (__spi_hard_init(p_dev) != AM_OK) {
        return NULL;
    }

    am_int_connect(p_dev->p_devinfo->inum,
                   __spi_irq_handler     ,
                  (void *)p_dev         );

    am_int_enable(p_dev->p_devinfo->inum);

    /* ���ʹ��DMA���� */
    if (p_devinfo->trans_mode == AM_LPC82X_SPI_WORK_MODE_DMA) {

        /* ����DMA����ͨ�� */
        p_dev->p_ctr_tx = am_lpc82x_dma_controller_get(p_devinfo->dma_chan_tx,
                                                       p_devinfo->dma_chan_prio_tx |
                                                       DMA_CHAN_OPT_PERIPH_REQ_EN);
        /* ����DMA����ͨ�� */
        p_dev->p_ctr_rx = am_lpc82x_dma_controller_get(p_devinfo->dma_chan_rx,
                                                       p_devinfo->dma_chan_prio_rx |
                                                       DMA_CHAN_OPT_PERIPH_REQ_EN);
    }

    return &(p_dev->spi_serve);
}

/**
 * \brief SPI ȥ����ʼ��
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

    /* ���� SPI */
    amhw_lpc82x_spi_disable(p_hw_spi);

    /* �ر�SPI�жϺŲ��Ͽ����� */
    am_int_disable(p_dev->p_devinfo->inum);
    am_int_disconnect(p_dev->p_devinfo->inum,
                      __spi_irq_handler,
                     (void *)p_dev);

    /* �ͷ�DMA������ */
    am_lpc82x_dma_controller_release(p_dev->p_ctr_tx);
    am_lpc82x_dma_controller_release(p_dev->p_ctr_rx);

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}
