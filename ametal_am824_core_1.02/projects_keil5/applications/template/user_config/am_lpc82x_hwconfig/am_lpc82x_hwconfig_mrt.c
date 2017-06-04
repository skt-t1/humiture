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
 * \brief LPC82X MRT ���ڶ�ʱ���ܵ��û������ļ�
 * \sa am_lpc82x_hwconfig_mrt.c
 * 
 * \internal
 * \par Modification history
 * - 1.01 15-12-08  mem,modified.
 * - 1.00 15-07-15  win, first implementation.
 * \endinternal
 */
#include "ametal.h"
#include "lpc82x_regbase.h"
#include "amhw_lpc82x_syscon.h"
#include "amhw_lpc82x_clk.h"
#include "am_lpc82x_mrt.h"

/**
 * \addtogroup am_lpc82x_if_hwconfig_src_mrt
 * \copydoc am_lpc82x_hwconfig_mrt.c
 * @{
 */
 
/** \brief MRT ƽ̨��ʼ�� */
static void __lpc82x_mrt_plfm_init (void)
{
    amhw_lpc82x_clk_periph_enable(AMHW_LPC82X_CLK_MRT);
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_MRT);
}

/** \brief MRTƽ̨���ʼ�� */
static void __lpc82x_mrt_plfm_deinit (void)
{
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_MRT);
    amhw_lpc82x_clk_periph_disable(AMHW_LPC82X_CLK_MRT);
}

/** \brief MRT �豸��Ϣ */
static const am_lpc82x_mrt_devinfo_t  __g_mrt_devinfo = {
    
    LPC82X_MRT_BASE,             /**< \brief MRT�Ĵ������ַ       */
    INUM_MRT,                    /**< \brief MRT �жϱ��          */
    4,                           /**< \brief ʹ�õ�ͨ����          */                             
    __lpc82x_mrt_plfm_init,      /**< \brief ƽ̨��ʼ������        */
    __lpc82x_mrt_plfm_deinit,    /**< \brief ƽ̨���ʼ������      */
};

static am_lpc82x_mrt_dev_t  __g_mrt_dev; /**< \brief  MRT �豸���� */

/** \brief MRT ʵ����ʼ������ñ�׼��ʱ�������� */
am_timer_handle_t am_lpc82x_mrt_inst_init (void)
{
    return am_lpc82x_mrt_init(&__g_mrt_dev, &__g_mrt_devinfo);
}

/** \brief MRT ʵ�����ʼ�� */
void am_lpc82x_mrt_inst_deinit (am_timer_handle_t handle)
{
    am_lpc82x_mrt_deinit(handle);
}

/**
 * @}
 */

/* end of file */
