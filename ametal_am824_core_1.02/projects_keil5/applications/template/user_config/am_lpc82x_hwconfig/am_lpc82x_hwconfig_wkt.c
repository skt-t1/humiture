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
 * \brief LPC82X WKT 用户配置实现
 * \sa am_lpc82x_hwconfig_wkt.c
 * 
 * \internal
 * \par Modification history
 * - 1.01 15-12-02  sky, modified.
 * - 1.00 15-07-08  zxl, first implementation.
 * \endinternal
 */

#include "am_lpc82x.h"
#include "am_lpc82x_wkt.h"
#include "amhw_lpc82x_clk.h"

/**
 * \addtogroup am_lpc82x_if_hwconfig_src_wkt
 * \copydoc am_lpc82x_hwconfig_wkt.c
 * @{
 */

/** \brief WKT平台初始化 */
static void __lpc82x_wkt_plfm_init (void)
{
    /* 使能WKT时钟 */
    amhw_lpc82x_clk_periph_enable(AMHW_LPC82X_CLK_WKT);

    /* 复位WKT */
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_WKT);
}

/** \brief 解除WKT平台初始化 */
static void __lpc82x_wkt_plfm_deinit (void)
{
    /* 复位WKT */
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_WKT);

    /* 禁能WKT时钟 */
    amhw_lpc82x_clk_periph_disable(AMHW_LPC82X_CLK_WKT);
}

/** \brief WKT设备信息 */
static const am_lpc82x_wkt_devinfo_t __g_wkt_devinfo = {
    LPC82X_WKT_BASE,                /**< \brief WKT寄存器块的基地址 */
    INUM_WKT,                       /**< \brief WKT 中断编号 */
    1,                              /**< \brief 支持1个通道 */
    AMHW_LPC82X_WKT_IRC_CLOCK,      /**< \brief 选择IRC时钟 */
    0,                              /**< \brief 使用外部时钟时的频率设置 */

    __lpc82x_wkt_plfm_init,         /**< \brief WKT平台初始化函数 */
    __lpc82x_wkt_plfm_deinit        /**< \brief WKT平台解初始化函数 */
};

static am_lpc82x_wkt_dev_t __g_wkt_dev;     /**< \brief WKT 设备定义 */


/** \brief WKT 实例初始化，获得Timer标准服务句柄 */
am_timer_handle_t am_lpc82x_wkt_inst_init (void)
{
    return am_lpc82x_wkt_init(&__g_wkt_dev, &__g_wkt_devinfo);
}

/** \brief WKT 实例解初始化 */
void am_lpc82x_wkt_inst_deinit (am_timer_handle_t handle)
{
    am_lpc82x_wkt_deinit(handle);
}

/**
 * @}
 */

/* end of file */
