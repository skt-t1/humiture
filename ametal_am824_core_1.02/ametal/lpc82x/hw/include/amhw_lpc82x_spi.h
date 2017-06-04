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
 * \brief LPC82X SPI 硬件操作接口描述
 * 
 * 1. 支持1-16bits直接传输，较大的数据帧可通过软件处理之后进行传输；
 * 2. 支持主机和从机传输；
 * 3. 数据可以被传输到从机，无需读传进来的数据。这个功能可用于建立SPI内存； 
 * 4. 控制信息可选择性的和数据一同写入。 该功能应用广泛, 包含任意长度数据帧。
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
 * \brief SPI寄存器块结构体
 */
typedef struct amhw_lpc82x_spi {
  __IO uint32_t  cfg;                   /**< \brief SPI 配置寄存器 */
  __IO uint32_t  dly;                   /**< \brief SPI 延迟寄存器 */
  __IO uint32_t  stat;                  /**< \brief SPI 状态寄存器 */
  __IO uint32_t  intenset;              /**< \brief SPI 中断使能读取和设置 */
  __O  uint32_t  intenclr;              /**< \brief SPI 中断使能清零 */
  __I  uint32_t  rxdat;                 /**< \brief SPI 接收数据 */
  __IO uint32_t  txdatctl;              /**< \brief SPI 控制数据传输 */
  __IO uint32_t  txdat;                 /**< \brief SPI 传输数据 */
  __IO uint32_t  txctl;                 /**< \brief SPI 传输控制 */
  __IO uint32_t  div;                   /**< \brief SPI 时钟分频器 */
  __I  uint32_t  intstat;               /**< \brief SPI 中断状态 */
} amhw_lpc82x_spi_t;

/**
 * \name DMA可操作的SPI控制的物理地址
 * @{
 */

/** \brief SPI0 接收数据地址 */
#define AMHW_LPC82X_SPI0_RXDAT_ADDR  (&(AMHW_LPC82X_SPI0->rxdat))

/** \brief SPI0 发送数据地址 */
#define AMHW_LPC82X_SPI0_TXDAT_ADDR  (&(AMHW_LPC82X_SPI0->txdat))

/** \brief SPI1 接收数据地址 */
#define AMHW_LPC82X_SPI1_RXDAT_ADDR  (&(AMHW_LPC82X_SPI1->rxdat))

/** \brief SPI1 发送数据地址 */
#define AMHW_LPC82X_SPI1_TXDAT_ADDR  (&(AMHW_LPC82X_SPI1->txdat))

/** @} */


/**
 * \name 用于设置多个SPI配置信息
 * @{
 */

#define AMHW_LPC82X_SPI_CFG_ENABLE      AM_BIT(0)  /**< \brief SPI设备使能 */
#define AMHW_LPC82X_SPI_CFG_MASTER      AM_BIT(2)  /**< \brief SPI作为主机 */
#define AMHW_LPC82X_SPI_CFG_LSB         AM_BIT(3)  /**< \brief SPI低位在前 */
#define AMHW_LPC82X_SPI_CFG_CHANGE      AM_BIT(4)  /**< \brief 时钟首翻变数据 */
#define AMHW_LPC82X_SPI_CFG_STAT_HIGH   AM_BIT(5)  /**< \brief 时钟静态高电平 */
#define AMHW_LPC82X_SPI_CFG_LOOP        AM_BIT(7)  /**< \brief SPI数据回环 */
#define AMHW_LPC82X_SPI_CFG_SPOL0_HIGH  AM_BIT(8)  /**< \brief SSEL0高电平有效 */
#define AMHW_LPC82X_SPI_CFG_SPOL1_HIGH  AM_BIT(9)  /**< \brief SSEL1高电平有效 */
#define AMHW_LPC82X_SPI_CFG_SPOL2_HIGH  AM_BIT(10) /**< \brief SSEL2高电平有效 */
#define AMHW_LPC82X_SPI_CFG_SPOL3_HIGH  AM_BIT(11) /**< \brief SSEL3高电平有效 */

/** @} */


/**
 * \brief 使能SPI
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_enable (amhw_lpc82x_spi_t *p_hw_spi)
{
    p_hw_spi->cfg |= AMHW_LPC82X_SPI_CFG_ENABLE;
}

/**
 * \brief 禁能SPI
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_disable (amhw_lpc82x_spi_t *p_hw_spi)
{
    p_hw_spi->cfg &= ~AMHW_LPC82X_SPI_CFG_ENABLE;
}

/**
 * \brief 用于设置SPI的多个配置参数
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] flags    : 配置参数
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_cfg_set (amhw_lpc82x_spi_t *p_hw_spi, uint32_t flags)
{
    p_hw_spi->cfg = flags;
}

/**
 * \brief 获取SPI配置信息
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return 返回SPI配置信息
 */
am_static_inline
uint16_t amhw_lpc82x_spi_cfg_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->cfg & 0x0FBD);
}

/**
 * \brief PRE_DELAY设置函数
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] delay    : 控制SSEL置未和数据帧起点之间的时间
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_pre_delay (amhw_lpc82x_spi_t *p_hw_spi, uint8_t delay)
{
    p_hw_spi->dly = (p_hw_spi->dly & ~(0xf)) | (delay & 0xf);
}

/**
 * \brief POST_DELAY设置函数
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] delay    : 控制数据帧末尾和SSEL解除位之间的时间
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_post_delay (amhw_lpc82x_spi_t *p_hw_spi, uint8_t delay)
{
    p_hw_spi->dly = (p_hw_spi->dly & ~AM_SBF(0xf, 4)) | AM_SBF(delay & 0xf, 4);
}

/**
 * \brief FRAME_DELAY设置函数
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] delay    : 控制相邻数据帧之间的最短时间
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_frame_delay (amhw_lpc82x_spi_t *p_hw_spi, uint8_t delay)
{
    p_hw_spi->dly = (p_hw_spi->dly & ~AM_SBF(0xf, 8)) | AM_SBF(delay & 0xf, 8);
}

/**
 * \brief TRANSFER_DELAY设置函数
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] delay    : 控制两次传输之间SSEL解除置位的最短时间
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_trans_delay (amhw_lpc82x_spi_t *p_hw_spi, uint8_t delay)
{
    p_hw_spi->dly = (p_hw_spi->dly & ~AM_SBF(0xf, 12)) | AM_SBF(delay & 0xf, 12);
}

/** 
 * \name SPI状态
 * @{
 */

/** \brief 接收器就绪标志(RO) */
#define AMHW_LPC82X_SPI_STAT_RXRDY     AM_BIT(0)

/** \brief 发送器就绪标志(RO) */
#define AMHW_LPC82X_SPI_STAT_TXRDY     AM_BIT(1)

/** \brief 接收器溢出中断(W1) */
#define AMHW_LPC82X_SPI_STAT_RXOV      AM_BIT(2)

/** \brief 发送器欠载中断(W1) */
#define AMHW_LPC82X_SPI_STAT_TXUR      AM_BIT(3)

/** \brief 从机选择置位(W1) */
#define AMHW_LPC82X_SPI_STAT_SSA       AM_BIT(4)

/** \brief 从机选择解除置位(W1) */
#define AMHW_LPC82X_SPI_STAT_SSD       AM_BIT(5)

/** \brief 从机选择解除置位(RO) */
#define AMHW_LPC82X_SPI_STAT_STALLED   AM_BIT(6)

/** \brief 结束传输控制位(W1/RO) */
#define AMHW_LPC82X_SPI_STAT_ENDTRANS  AM_BIT(7)

/** \brief 设备空闲标志位(RO) */
#define AMHW_LPC82X_SPI_STAT_IDLE      AM_BIT(8)

/** @} */

/**
 * \brief 清除一个或多个SPI状态位
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] stats    : SPI状态信息标志位
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_stat_clr (amhw_lpc82x_spi_t *p_hw_spi, uint16_t stats)
{
    p_hw_spi->stat = (stats & 0x1ff);
}

/**
 * \brief 读取SPI状态位
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return SPI状态标志位信息
 */
am_static_inline
uint16_t amhw_lpc82x_spi_stat_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->stat & 0x1ff);
}

/** 
 * \name 用于SPI中断
 * @{
 */

#define AMHW_LPC82X_SPI_INT_RXRDY  AM_BIT(0)   /**< \brief 接收器就绪中断 */
#define AMHW_LPC82X_SPI_INT_TXRDY  AM_BIT(1)   /**< \brief 发送器就绪中断 */
#define AMHW_LPC82X_SPI_INT_RXOV   AM_BIT(2)   /**< \brief 接收器溢出中断 */
#define AMHW_LPC82X_SPI_INT_TXUR   AM_BIT(3)   /**< \brief 发送器欠载中断 */
#define AMHW_LPC82X_SPI_INT_SSA    AM_BIT(4)   /**< \brief 从机选择置位中断 */
#define AMHW_LPC82X_SPI_INT_SSD    AM_BIT(5)   /**< \brief 从机选择解除中断 */

/** @} */


/**
 * \brief 使能SPI一个或多个中断
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] flags    : SPI状态信息标志位
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_int_enable (amhw_lpc82x_spi_t *p_hw_spi, uint8_t flags)
{
    p_hw_spi->intenset |= (flags & 0x3f);
}

/**
 * \brief 禁能SPI一个或多个中断
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] flags    : SPI状态信息标志位
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_int_disable (amhw_lpc82x_spi_t *p_hw_spi, uint8_t flags)
{
    p_hw_spi->intenclr |= (flags & 0x3f);
}

/**
 * \brief 读取SPI中断状态标志
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return SPI中断状态
 */
am_static_inline
uint8_t amhw_lpc82x_spi_intstat_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->intstat & 0x3f);
}

/**
 * \brief 读取SPI接收到的数据（8bit）
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return 返回接收到的数据（8bit）
 */
am_static_inline
uint8_t amhw_lpc82x_spi_rxdat_8bit (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->rxdat & 0xff);
}

/**
 * \brief 读取SPI接收到的数据（16bit）
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return 返回接收到的数据（16bit）
 */
am_static_inline
uint16_t amhw_lpc82x_spi_rxdat_16bit (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->rxdat & 0xffff);
}

/**
 * \brief 读取SPI接收到的数据（16bit，带RXSSELN和SOT标志位）
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return 返回接收到的数据（16bit，且带RXSSELN和SOT标志位）
 *
 * \note 0~15 位：为接收到的数据位
 *       16~19位：为接收数据的从机选择(RXSSELN)
 *       第20 位：为传送开始标志(SOT)
 *       其他位 ：为保留位恒为零
 */
am_static_inline
uint32_t amhw_lpc82x_spi_rxdat_with_flags (amhw_lpc82x_spi_t *p_hw_spi)
{
    return (p_hw_spi->rxdat & 0x1fffff);
}

/**
 * \name SPI发送参数
 * @{
 */

#define AMHW_LPC82X_SPI_TXCTRL_SSELN0   AM_BIT(16)  /**< \brief 不置位SSEL0引脚 */
#define AMHW_LPC82X_SPI_TXCTRL_SSELN1   AM_BIT(17)  /**< \brief 不置位SSEL1引脚 */
#define AMHW_LPC82X_SPI_TXCTRL_SSELN2   AM_BIT(18)  /**< \brief 不置位SSEL2引脚 */
#define AMHW_LPC82X_SPI_TXCTRL_SSELN3   AM_BIT(19)  /**< \brief 不置位SSEL3引脚 */
#define AMHW_LPC82X_SPI_TXCTRL_EOT      AM_BIT(20)  /**< \brief 传输结束标志 */
#define AMHW_LPC82X_SPI_TXCTRL_EOF      AM_BIT(21)  /**< \brief 非帧结束数据 */
#define AMHW_LPC82X_SPI_TXCTRL_RXIGNORE AM_BIT(22)  /**< \brief 忽略接收到的数据 */

/** @} */


/**
 * \brief SPI发送数据（带控制指令）
 * 
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] data     : 欲发送的数据
 * \param[in] flags    : 发送时的参数（多个参数可“或”运算）
 * \param[in] flen     : 数据帧长度位数（1~16位）
 *
 * \return 无
 *
 * \note 参数flags 可以通过“或”运算一同传递（共4个参数），如果4个参数中存在某1个
 *       参数没有“或”上，则默认该参数为零。如果所有参数都要设置为0，可直接赋值0
 *       到该参数上。该函数可用于起始帧的发送和结束帧的发送，单纯的数据发送建议使用
 *       amhw_lpc82x_spi_txdat()，该函数不会影响发送的其他参数，仅会改变发送器中
 *       的数据。单纯的数据发送参数的改变，建议使用amhw_lpc82x_spi_txctl_set()和
 *       amhw_lpc82x_spi_txctl_clr()使能或禁能，这两个函数只会改变发送过程的参数。
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
 * \brief SPI发送数据
 * 
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] data     : 欲发送的数据
 *
 * \return 无
 *
 * \note 该函数可发送字段在4~16位的数据
 */
am_static_inline
void amhw_lpc82x_spi_txdat (amhw_lpc82x_spi_t *p_hw_spi, uint16_t data)
{
    p_hw_spi->txdat = (data & 0xffff);
}

/**
 * \brief 置位SPI发送参数（一个或多个）
 * 
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] flags    : SPI发送参数
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_txctl_set (amhw_lpc82x_spi_t *p_hw_spi, uint32_t flags)
{
    p_hw_spi->txctl |= flags & 0xf7f0000;
}

/**
 * \brief 解除置位SPI发送参数（一个或多个）
 * 
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] flags    : SPI发送参数
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_txctl_clr (amhw_lpc82x_spi_t *p_hw_spi, uint32_t flags)
{
    p_hw_spi->txctl &= ~(flags & 0xf7f0000);
}

/**
 * \brief 设置SPI发送数据长度
 * 
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] flen     : SPI发送数据长度
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_data_flen_set (amhw_lpc82x_spi_t *p_hw_spi, uint8_t flen)
{
    p_hw_spi->txctl = (p_hw_spi->txctl & ~AM_SBF(0xf, 24)) 
                                       |  AM_SBF(((flen - 1) & 0xf), 24);
}

/**
 * \brief 读取SPI一帧数据的位数
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return 一帧数据的位数
 */
am_static_inline
uint8_t amhw_lpc82x_spi_data_flen_get (amhw_lpc82x_spi_t *p_hw_spi)
{
    return AM_BITS_GET(p_hw_spi->txctl, 24, 4) + 1;
}

/**
 * \brief 设置SPI分频值
 *
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \param[in] div      : 分频值
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_spi_div_set (amhw_lpc82x_spi_t *p_hw_spi, uint16_t div)
{
    p_hw_spi->div = div - 1;
}

/**
 * \brief 读取SPI分频值
 * \param[in] p_hw_spi : 指向SPI寄存器块的指针
 * \return SPI分频值
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
