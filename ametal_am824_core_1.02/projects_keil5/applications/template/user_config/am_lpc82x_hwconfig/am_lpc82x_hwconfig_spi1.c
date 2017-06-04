/*******************************************************************************
*                                 AMetal
*                       ---------------------------
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
 * \brief LPC82X SPI �û�����ʵ��
 * \sa am_lpc82x_hwconfig_spi1.c
 * 
 * \internal
 * \par Modification history
 * - 1.02 15-11-25  sky, modified.
 * - 1.01 15-10-10  aii, second implementation.
 * - 1.00 15-07-10  aii, first implementation.
 * \endinternal
 */

#include "am_lpc82x.h"
#include "am_lpc82x_dma.h"
#include "am_lpc82x_spi.h"
#include "amhw_lpc82x_clk.h"

/**
 * \addtogroup am_lpc82x_if_hwconfig_src_spi1
 * \copydoc am_lpc82x_hwconfig_spi1.c
 * @{
 */

/** \brief SPI1 ƽ̨��ʼ�� */
static void __lpc82x_spi1_plfm_init (void)
{
    /* SPI1 �������� */
    am_gpio_pin_cfg(PIO0_6, PIO_FUNC_SPI1_SCK);
    am_gpio_pin_cfg(PIO0_7, PIO_FUNC_SPI1_MOSI);
    am_gpio_pin_cfg(PIO0_8, PIO_FUNC_SPI1_MISO);
    /* CS_Pin ���û�����STD����ʱ���д��룬�˴��������� */

    /* ʹ�� SPI1 ʱ�� */
    amhw_lpc82x_clk_periph_enable(AMHW_LPC82X_CLK_SPI1);
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_SPI1);
}

/** \brief ���SPI1 ƽ̨��ʼ�� */
static void __lpc82x_spi1_plfm_deinit (void)
{
    /* ���� SPI1 ʱ�� */
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_SPI1);
    amhw_lpc82x_clk_periph_disable(AMHW_LPC82X_CLK_SPI1);
}

/**
 * \brief SPI1 �豸��Ϣ
 */
static const struct am_lpc82x_spi_devinfo __g_spi1_devinfo = {
    LPC82X_SPI1_BASE,               /**< \brief SPI1�Ĵ�����Ļ���ַ */
    INUM_SPI1,                      /**< \brief SPI1�жϱ�� */
    AM_LPC82X_SPI_WORK_MODE_DMA,    /**< \brief ʹ��DMA���� */
    DMA_CHAN_SPI1_TX_REQ,           /**< \brief DMA����ͨ�� */
    DMA_CHAN_SPI1_RX_REQ,           /**< \brief DMA����ͨ�� */
    AMHW_LPC82X_DMA_CHAN_PRIO_3,    /**< \brief �������ȼ� */
    AMHW_LPC82X_DMA_CHAN_PRIO_2,    /**< \brief �������ȼ� */

    __lpc82x_spi1_plfm_init,        /**< \brief SPI1ƽ̨��ʼ������ */
    __lpc82x_spi1_plfm_deinit       /**< \brief SPI1ƽ̨���ʼ������ */
};

/** \brief SPI1 �豸ʵ�� */
static am_lpc82x_spi_dev_t __g_spi1_dev;


/** \brief SPI1 ʵ����ʼ�������SPI��׼������ */
am_spi_handle_t am_lpc82x_spi1_inst_init (void)
{
    return am_lpc82x_spi_init(&__g_spi1_dev, &__g_spi1_devinfo);
}

/** \brief SPI1 ʵ�����ʼ�� */
void am_lpc82x_spi1_inst_deinit (am_spi_handle_t handle)
{
    am_lpc82x_spi_deinit(handle);
}


/**
 * @}
 */

/* end of file */
