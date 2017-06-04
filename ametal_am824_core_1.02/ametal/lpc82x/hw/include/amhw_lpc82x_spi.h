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
 * \brief LPC82X SPI Ӳ�������ӿ�����
 * 
 * 1. ֧��1-16bitsֱ�Ӵ��䣬�ϴ������֡��ͨ���������֮����д��䣻
 * 2. ֧�������ʹӻ����䣻
 * 3. ���ݿ��Ա����䵽�ӻ�������������������ݡ�������ܿ����ڽ���SPI�ڴ棻 
 * 4. ������Ϣ��ѡ���Եĺ�����һͬд�롣 �ù���Ӧ�ù㷺, �������ⳤ������֡��
 *
 * \internal
 * \par Modification history
 * - 1.01 15-11-23  sky, modified.
 * - 1.00 15-07-06  aii, first implementation.
 * \endinternal
 */
 
#ifndef __AMHW_LPC82X_SPI_H
#define __AMHW_LPC82X_SPI_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup amhw_lpc82x_if_spi
 * \copydoc amhw_lpc82x_spi.h
 * @{
 */

/**
 * \brief SPI�Ĵ�����ṹ��
 */
typedef struct amhw_lpc82x_spi {
  __IO uint32_t  cfg;                   /**< \brief SPI ���üĴ��� */
  __IO uint32_t  dly;                   /**< \brief SPI �ӳټĴ��� */
  __IO uint32_t  stat;                  /**< \brief SPI ״̬�Ĵ��� */
  __IO uint32_t  intenset;              /**< \brief SPI �ж�ʹ�ܶ�ȡ������ */
  __O  uint32_t  intenclr;              /**< \brief SPI �ж�ʹ������ */
  __I  uint32_t  rxdat;                 /**< \brief SPI �������� */
  __IO uint32_t  txdatctl;              /**< \brief SPI �������ݴ��� */
  __IO uint32_t  txdat;                 /**< \brief SPI �������� */
  __IO uint32_t  txctl;                 /**< \brief SPI ������� */
  __IO uint32_t  div;                   /**< \brief SPI ʱ�ӷ�Ƶ�� */
  __I  uint32_t  intstat;               /**< \brief SPI �ж�״̬ */
} amhw_lpc82x_spi_t;

/**
 * \name DMA�ɲ�����SPI���Ƶ������ַ
 * @{
 */

/** \brief SPI0 �������ݵ�ַ */
#define AMHW_LPC82X_SPI0_RXDAT_ADDR  (&(AMHW_LPC82X_SPI0->rxdat))

/** \brief SPI0 �������ݵ�ַ */
#define AMHW_LPC82X_SPI0_TXDAT_ADDR  (&(AMHW_LPC82X_SPI0->txdat))

/** \brief SPI1 �������ݵ�ַ */
#define AMHW_LPC82X_SPI1_RXDAT_ADDR  (&(AMHW_LPC82X_SPI1->rxdat))

/** \brief SPI1 �������ݵ�ַ */
#define AMHW_LPC82X_SPI1_TXDAT_ADDR  (&(AMHW_LPC82X_SPI1->txdat))

/** @} */


/**
 * \name �������ö��SPI������Ϣ
 * @{
 */

#define AMHW_LPC82X_SPI_CFG_ENABLE      AM_BIT(0)  /**< \brief SPI�豸ʹ�� */
#define AMHW_LPC82X_SPI_CFG_MASTER      AM_BIT(2)  /**< \brief SPI��Ϊ���� */
#define AMHW_LPC82X_SPI_CFG_LSB         AM_BIT(3)  /**< \brief SPI��λ��ǰ */
#define AMHW_LPC82X_SPI_CFG_CHANGE      AM_BIT(4)  /**< \brief ʱ���׷������� */
#define AMHW_LPC82X_SPI_CFG_STAT_HIGH   AM_BIT(5)  /**< \brief ʱ�Ӿ�̬�ߵ�ƽ */
#define AMHW_LPC82X_SPI_CFG_LOOP        AM_BIT(7)  /**< \brief SPI���ݻػ� */
#define AMHW_LPC82X_SPI_CFG_SPOL0_HIGH  AM_BIT(8)  /**< \brief SSEL0�ߵ�ƽ��Ч */
#define AMHW_LPC82X_SPI_CFG_SPOL1_HIGH  AM_BIT(9)  /**< \brief SSEL1�ߵ�ƽ��Ч */
#define AMHW_LPC82X_SPI_CFG_SPOL2_HIGH  AM_BIT(10) /**< \brief SSEL2�ߵ�ƽ��Ч */
#define AMHW_LPC82X_SPI_CFG_SPOL3_HIGH  AM_BIT(11) /**< \brief SSEL3�ߵ�ƽ��Ч */

/** @} */


/**
 * \brief ʹ��SPI
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_enable (amhw_lpc82x_spi_t *p_hw_spi)
{
    p_hw_spi->cfg |= AMHW_LPC82X_SPI_CFG_ENABLE;
}

/**
 * \brief ����SPI
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_disable (amhw_lpc82x_spi_t *p_hw_spi)
{
    p_hw_spi->cfg &= ~AMHW_LPC82X_SPI_CFG_ENABLE;
}

/**
 * \brief ��������SPI�Ķ�����ò���
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] flags    : ���ò���
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_cfg_set (amhw_lpc82x_spi_t *p_hw_spi, uint32_t flags)
{
    p_hw_spi->cfg = flags;
}

/**
 * \brief ��ȡSPI������Ϣ
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return ����SPI������Ϣ
 */
am_static_inline
uint16_t amhw_lpc82x_spi_cfg_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->cfg & 0x0FBD);
}

/**
 * \brief PRE_DELAY���ú���
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] delay    : ����SSEL��δ������֡���֮���ʱ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_pre_delay (amhw_lpc82x_spi_t *p_hw_spi, uint8_t delay)
{
    p_hw_spi->dly = (p_hw_spi->dly & ~(0xf)) | (delay & 0xf);
}

/**
 * \brief POST_DELAY���ú���
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] delay    : ��������֡ĩβ��SSEL���λ֮���ʱ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_post_delay (amhw_lpc82x_spi_t *p_hw_spi, uint8_t delay)
{
    p_hw_spi->dly = (p_hw_spi->dly & ~AM_SBF(0xf, 4)) | AM_SBF(delay & 0xf, 4);
}

/**
 * \brief FRAME_DELAY���ú���
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] delay    : ������������֮֡������ʱ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_frame_delay (amhw_lpc82x_spi_t *p_hw_spi, uint8_t delay)
{
    p_hw_spi->dly = (p_hw_spi->dly & ~AM_SBF(0xf, 8)) | AM_SBF(delay & 0xf, 8);
}

/**
 * \brief TRANSFER_DELAY���ú���
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] delay    : �������δ���֮��SSEL�����λ�����ʱ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_trans_delay (amhw_lpc82x_spi_t *p_hw_spi, uint8_t delay)
{
    p_hw_spi->dly = (p_hw_spi->dly & ~AM_SBF(0xf, 12)) | AM_SBF(delay & 0xf, 12);
}

/** 
 * \name SPI״̬
 * @{
 */

/** \brief ������������־(RO) */
#define AMHW_LPC82X_SPI_STAT_RXRDY     AM_BIT(0)

/** \brief ������������־(RO) */
#define AMHW_LPC82X_SPI_STAT_TXRDY     AM_BIT(1)

/** \brief ����������ж�(W1) */
#define AMHW_LPC82X_SPI_STAT_RXOV      AM_BIT(2)

/** \brief ������Ƿ���ж�(W1) */
#define AMHW_LPC82X_SPI_STAT_TXUR      AM_BIT(3)

/** \brief �ӻ�ѡ����λ(W1) */
#define AMHW_LPC82X_SPI_STAT_SSA       AM_BIT(4)

/** \brief �ӻ�ѡ������λ(W1) */
#define AMHW_LPC82X_SPI_STAT_SSD       AM_BIT(5)

/** \brief �ӻ�ѡ������λ(RO) */
#define AMHW_LPC82X_SPI_STAT_STALLED   AM_BIT(6)

/** \brief �����������λ(W1/RO) */
#define AMHW_LPC82X_SPI_STAT_ENDTRANS  AM_BIT(7)

/** \brief �豸���б�־λ(RO) */
#define AMHW_LPC82X_SPI_STAT_IDLE      AM_BIT(8)

/** @} */

/**
 * \brief ���һ������SPI״̬λ
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] stats    : SPI״̬��Ϣ��־λ
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_stat_clr (amhw_lpc82x_spi_t *p_hw_spi, uint16_t stats)
{
    p_hw_spi->stat = (stats & 0x1ff);
}

/**
 * \brief ��ȡSPI״̬λ
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return SPI״̬��־λ��Ϣ
 */
am_static_inline
uint16_t amhw_lpc82x_spi_stat_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->stat & 0x1ff);
}

/** 
 * \name ����SPI�ж�
 * @{
 */

#define AMHW_LPC82X_SPI_INT_RXRDY  AM_BIT(0)   /**< \brief �����������ж� */
#define AMHW_LPC82X_SPI_INT_TXRDY  AM_BIT(1)   /**< \brief �����������ж� */
#define AMHW_LPC82X_SPI_INT_RXOV   AM_BIT(2)   /**< \brief ����������ж� */
#define AMHW_LPC82X_SPI_INT_TXUR   AM_BIT(3)   /**< \brief ������Ƿ���ж� */
#define AMHW_LPC82X_SPI_INT_SSA    AM_BIT(4)   /**< \brief �ӻ�ѡ����λ�ж� */
#define AMHW_LPC82X_SPI_INT_SSD    AM_BIT(5)   /**< \brief �ӻ�ѡ�����ж� */

/** @} */


/**
 * \brief ʹ��SPIһ�������ж�
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] flags    : SPI״̬��Ϣ��־λ
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_int_enable (amhw_lpc82x_spi_t *p_hw_spi, uint8_t flags)
{
    p_hw_spi->intenset |= (flags & 0x3f);
}

/**
 * \brief ����SPIһ�������ж�
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] flags    : SPI״̬��Ϣ��־λ
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_int_disable (amhw_lpc82x_spi_t *p_hw_spi, uint8_t flags)
{
    p_hw_spi->intenclr |= (flags & 0x3f);
}

/**
 * \brief ��ȡSPI�ж�״̬��־
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return SPI�ж�״̬
 */
am_static_inline
uint8_t amhw_lpc82x_spi_intstat_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->intstat & 0x3f);
}

/**
 * \brief ��ȡSPI���յ������ݣ�8bit��
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return ���ؽ��յ������ݣ�8bit��
 */
am_static_inline
uint8_t amhw_lpc82x_spi_rxdat_8bit (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->rxdat & 0xff);
}

/**
 * \brief ��ȡSPI���յ������ݣ�16bit��
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return ���ؽ��յ������ݣ�16bit��
 */
am_static_inline
uint16_t amhw_lpc82x_spi_rxdat_16bit (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->rxdat & 0xffff);
}

/**
 * \brief ��ȡSPI���յ������ݣ�16bit����RXSSELN��SOT��־λ��
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return ���ؽ��յ������ݣ�16bit���Ҵ�RXSSELN��SOT��־λ��
 *
 * \note 0~15 λ��Ϊ���յ�������λ
 *       16~19λ��Ϊ�������ݵĴӻ�ѡ��(RXSSELN)
 *       ��20 λ��Ϊ���Ϳ�ʼ��־(SOT)
 *       ����λ ��Ϊ����λ��Ϊ��
 */
am_static_inline
uint32_t amhw_lpc82x_spi_rxdat_with_flags (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->rxdat & 0x1fffff);
}

/**
 * \name SPI���Ͳ���
 * @{
 */

#define AMHW_LPC82X_SPI_TXCTRL_SSELN0   AM_BIT(16)  /**< \brief ����λSSEL0���� */
#define AMHW_LPC82X_SPI_TXCTRL_SSELN1   AM_BIT(17)  /**< \brief ����λSSEL1���� */
#define AMHW_LPC82X_SPI_TXCTRL_SSELN2   AM_BIT(18)  /**< \brief ����λSSEL2���� */
#define AMHW_LPC82X_SPI_TXCTRL_SSELN3   AM_BIT(19)  /**< \brief ����λSSEL3���� */
#define AMHW_LPC82X_SPI_TXCTRL_EOT      AM_BIT(20)  /**< \brief ���������־ */
#define AMHW_LPC82X_SPI_TXCTRL_EOF      AM_BIT(21)  /**< \brief ��֡�������� */
#define AMHW_LPC82X_SPI_TXCTRL_RXIGNORE AM_BIT(22)  /**< \brief ���Խ��յ������� */

/** @} */


/**
 * \brief SPI�������ݣ�������ָ�
 * 
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] data     : �����͵�����
 * \param[in] flags    : ����ʱ�Ĳ�������������ɡ������㣩
 * \param[in] flen     : ����֡����λ����1~16λ��
 *
 * \return ��
 *
 * \note ����flags ����ͨ����������һͬ���ݣ���4�������������4�������д���ĳ1��
 *       ����û�С����ϣ���Ĭ�ϸò���Ϊ�㡣������в�����Ҫ����Ϊ0����ֱ�Ӹ�ֵ0
 *       ���ò����ϡ��ú�����������ʼ֡�ķ��ͺͽ���֡�ķ��ͣ����������ݷ��ͽ���ʹ��
 *       amhw_lpc82x_spi_txdat()���ú�������Ӱ�췢�͵���������������ı䷢������
 *       �����ݡ����������ݷ��Ͳ����ĸı䣬����ʹ��amhw_lpc82x_spi_txctl_set()��
 *       amhw_lpc82x_spi_txctl_clr()ʹ�ܻ���ܣ�����������ֻ��ı䷢�͹��̵Ĳ�����
 */
am_static_inline
void amhw_lpc82x_spi_txdatctl (amhw_lpc82x_spi_t *p_hw_spi,
                               uint16_t           data,
                               uint32_t           flags,
                               uint8_t            flen)
{
    p_hw_spi->txdatctl = data | flags | AM_SBF(((flen - 1) & 0xf), 24);
}

/**
 * \brief SPI��������
 * 
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] data     : �����͵�����
 *
 * \return ��
 *
 * \note �ú����ɷ����ֶ���4~16λ������
 */
am_static_inline
void amhw_lpc82x_spi_txdat (amhw_lpc82x_spi_t *p_hw_spi, uint16_t data)
{
    p_hw_spi->txdat = (data & 0xffff);
}

/**
 * \brief ��λSPI���Ͳ�����һ��������
 * 
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] flags    : SPI���Ͳ���
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_txctl_set (amhw_lpc82x_spi_t *p_hw_spi, uint32_t flags)
{
    p_hw_spi->txctl |= flags & 0xf7f0000;
}

/**
 * \brief �����λSPI���Ͳ�����һ��������
 * 
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] flags    : SPI���Ͳ���
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_txctl_clr (amhw_lpc82x_spi_t *p_hw_spi, uint32_t flags)
{
    p_hw_spi->txctl &= ~(flags & 0xf7f0000);
}

/**
 * \brief ����SPI�������ݳ���
 * 
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] flen     : SPI�������ݳ���
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_data_flen_set (amhw_lpc82x_spi_t *p_hw_spi, uint8_t flen)
{
    p_hw_spi->txctl = (p_hw_spi->txctl & ~AM_SBF(0xf, 24)) 
                                       |  AM_SBF(((flen - 1) & 0xf), 24);
}

/**
 * \brief ��ȡSPIһ֡���ݵ�λ��
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return һ֡���ݵ�λ��
 */
am_static_inline
uint8_t amhw_lpc82x_spi_data_flen_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return AM_BITS_GET(p_hw_spi->txctl, 24, 4) + 1;
}

/**
 * \brief ����SPI��Ƶֵ
 *
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \param[in] div      : ��Ƶֵ
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_spi_div_set (amhw_lpc82x_spi_t *p_hw_spi, uint16_t div)
{
    p_hw_spi->div = div - 1;
}

/**
 * \brief ��ȡSPI��Ƶֵ
 * \param[in] p_hw_spi : ָ��SPI�Ĵ������ָ��
 * \return SPI��Ƶֵ
 */
am_static_inline
uint16_t amhw_lpc82x_spi_div_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return p_hw_spi->div + 1;
}

/** 
 * @} amhw_lpc82x_if_spi
 */

#ifdef __cplusplus
}
#endif

#endif /* __AMHW_LPC82X_SPI_H */

/* end of file */
