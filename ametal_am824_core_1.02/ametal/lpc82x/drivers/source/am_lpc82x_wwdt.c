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
 * \brief WWDT������ӿ�
 * \internal
 * \par Modification history
 * - 1.00 15-07-10  sss, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_lpc82x_wwdt.h"
#include "amhw_lpc82x_clk.h"
#include "amhw_lpc82x_int.h"
#include "amhw_lpc82x_wwdt.h"

/****************************************************************************
 * Functions Declaration
 ***************************************************************************/
 
static int __wwdt_feed (void *p_drv);
static int __wwdt_enable (void *p_drv, uint32_t timeout);
static int __wwdt_info_get (void *p_drv, am_wdt_info_t *p_info);
 
/** \brief ���Ź���׼�������ṹ�� */
static const struct am_wdt_drv_funcs __g_wwdt_drv_funcs = {
    __wwdt_info_get,
    __wwdt_enable,
    __wwdt_feed
};
/** \brief ��ȡ���Ź���Ϣ */
static int __wwdt_info_get (void *p_drv, am_wdt_info_t *p_info)
{
    am_lpc82x_wwdt_dev_t *p_dev     = (am_lpc82x_wwdt_dev_t *)p_drv;
    amhw_lpc82x_wwdt_t   *p_hw_wwdt = 
        (amhw_lpc82x_wwdt_t *)p_dev->p_devinfo->wwdt_regbase;
    
    uint32_t         wdt_freq  = 0;
    
    wdt_freq  = amhw_lpc82x_clk_periph_freq_get(p_hw_wwdt) / 4;
    
    p_info->min_timeout_ms = (0xFF + 1) * 1000 / wdt_freq;
    p_info->max_timeout_ms = (uint64_t)(0xFFFFFF + 1) * 1000 /
                             (uint64_t)wdt_freq ;
    
    return AM_OK;
}

/** \brief ι������ */
static int __wwdt_feed (void *p_drv)
{
    am_lpc82x_wwdt_dev_t *p_dev     = (am_lpc82x_wwdt_dev_t *)p_drv;
    amhw_lpc82x_wwdt_t   *p_hw_wwdt = 
        (amhw_lpc82x_wwdt_t *)p_dev->p_devinfo->wwdt_regbase;
    
    amhw_lpc82x_wwdt_feed(p_hw_wwdt);

    return AM_OK;
}

/** \brief ���ó�ʱʱ�䲢�������Ź� */
static int __wwdt_enable (void *p_drv, uint32_t timeout_ms)
{
    am_lpc82x_wwdt_dev_t *p_dev     = (am_lpc82x_wwdt_dev_t *)p_drv;
    amhw_lpc82x_wwdt_t   *p_hw_wwdt = 
        (amhw_lpc82x_wwdt_t *)p_dev->p_devinfo->wwdt_regbase;
    
    uint32_t         wdt_freq;
    uint32_t         ticks;
    
    wdt_freq  = amhw_lpc82x_clk_periph_freq_get(p_hw_wwdt) / 4;
    
    ticks = (uint64_t) (timeout_ms) * wdt_freq / 1000;
    
    if (ticks <= 0xFF) {
        ticks = 0xFF;
    } else if (ticks >= 0xFFFFFF) {
        ticks = 0xFFFFFF;
    }
    
    amhw_lpc82x_wwdt_timeout_set(p_hw_wwdt, ticks);
    
    /* ��鸴λ�Ƿ����ɿ��Ź���λ��ɵ� */
    if (amhw_lpc82x_wwdt_status_get(p_hw_wwdt, AMHW_LPC82X_WWDT_WDMOD_WDTOF)) {
        amhw_lpc82x_wwdt_status_clr(p_hw_wwdt, AMHW_LPC82X_WWDT_WDMOD_WDTOF);
    }
 
    /* ����WWDT�ڳ�ʱ��λ */
    amhw_lpc82x_wwdt_option_set(p_hw_wwdt, AMHW_LPC82X_WWDT_WDMOD_LOCK | 
                                           AMHW_LPC82X_WWDT_WDMOD_WDRESET);
    
    /* ����жϱ�־�͸�λ��־ */
    amhw_lpc82x_wwdt_status_clr(p_hw_wwdt, AMHW_LPC82X_WWDT_WDMOD_WDTOF | 
                                           AMHW_LPC82X_WWDT_WDMOD_WDINT);
    
    /* �������Ź� */
    amhw_lpc82x_wwdt_start(p_hw_wwdt);
    
    return AM_OK;
}

/** \brief ��ʼ��WWDT����ȡ��׼������ */
am_wdt_handle_t am_lpc82x_wwdt_init (am_lpc82x_wwdt_dev_t           *p_dev,
                               const am_lpc82x_wwdt_devinfo_t       *p_devinfo)
{
    amhw_lpc82x_wwdt_t *p_hw_wwdt = NULL;

    
    if (p_devinfo == NULL) {
        return NULL;
    }
    
    p_dev->p_devinfo        = p_devinfo;
    p_dev->wdt_serv.p_funcs = (struct am_wdt_drv_funcs *)&__g_wwdt_drv_funcs;
    p_dev->wdt_serv.p_drv   = p_dev;
    
    p_hw_wwdt = (amhw_lpc82x_wwdt_t *)p_dev->p_devinfo->wwdt_regbase;

    /* ���Ź�ƽ̨��ʼ�� */
    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    amhw_lpc82x_wwdt_init(p_hw_wwdt);
    
    /* ��鸴λ�Ƿ��ɿ��Ź���ɵ� */
    if (amhw_lpc82x_wwdt_status_get(p_hw_wwdt, AMHW_LPC82X_WWDT_WDMOD_WDTOF)) {
        amhw_lpc82x_wwdt_status_clr(p_hw_wwdt, AMHW_LPC82X_WWDT_WDMOD_WDTOF);
    }
    
    return &(p_dev->wdt_serv);
}

/** \brief WWDT���ʼ�� */
void am_lpc82x_wwdt_deinit (am_wdt_handle_t handle)
{
    am_lpc82x_wwdt_dev_t *p_dev = (am_lpc82x_wwdt_dev_t *)handle;

    if (NULL == p_dev) {
        return ;
    }

    p_dev->p_devinfo = NULL;
    
    p_dev->wdt_serv.p_funcs = NULL;
    p_dev->wdt_serv.p_drv   = NULL;

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

/* end of file */
