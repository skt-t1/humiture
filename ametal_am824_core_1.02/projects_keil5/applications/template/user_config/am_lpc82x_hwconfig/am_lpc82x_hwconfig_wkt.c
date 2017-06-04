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
 * \brief LPC82X WKT �û�����ʵ��
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

/** \brief WKTƽ̨��ʼ�� */
static void __lpc82x_wkt_plfm_init (void)
{
    /* ʹ��WKTʱ�� */
    amhw_lpc82x_clk_periph_enable(AMHW_LPC82X_CLK_WKT);

    /* ��λWKT */
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_WKT);
}

/** \brief ���WKTƽ̨��ʼ�� */
static void __lpc82x_wkt_plfm_deinit (void)
{
    /* ��λWKT */
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_WKT);

    /* ����WKTʱ�� */
    amhw_lpc82x_clk_periph_disable(AMHW_LPC82X_CLK_WKT);
}

/** \brief WKT�豸��Ϣ */
static const am_lpc82x_wkt_devinfo_t __g_wkt_devinfo = {
    LPC82X_WKT_BASE,                /**< \brief WKT�Ĵ�����Ļ���ַ */
    INUM_WKT,                       /**< \brief WKT �жϱ�� */
    1,                              /**< \brief ֧��1��ͨ�� */
    AMHW_LPC82X_WKT_IRC_CLOCK,      /**< \brief ѡ��IRCʱ�� */
    0,                              /**< \brief ʹ���ⲿʱ��ʱ��Ƶ������ */

    __lpc82x_wkt_plfm_init,         /**< \brief WKTƽ̨��ʼ������ */
    __lpc82x_wkt_plfm_deinit        /**< \brief WKTƽ̨���ʼ������ */
};

static am_lpc82x_wkt_dev_t __g_wkt_dev;     /**< \brief WKT �豸���� */


/** \brief WKT ʵ����ʼ�������Timer��׼������ */
am_timer_handle_t am_lpc82x_wkt_inst_init (void)
{
    return am_lpc82x_wkt_init(&__g_wkt_dev, &__g_wkt_devinfo);
}

/** \brief WKT ʵ�����ʼ�� */
void am_lpc82x_wkt_inst_deinit (am_timer_handle_t handle)
{
    am_lpc82x_wkt_deinit(handle);
}

/**
 * @}
 */

/* end of file */
