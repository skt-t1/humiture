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
 * \brief ʹ��ϵͳ�δ�ʱ��(Systick) ʵ�ֱ�׼�ĺ��뼶����ʱ������΢�뼶����ʱ���� 
 *
 * \internal
 * \par Modification history
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */
#include "ametal.h"
#include "am_delay.h"
#include "amhw_lpc82x_systick.h"

#include "am_lpc82x.h"
#include "amhw_lpc82x_clk.h"

uint32_t __g_us_ticks;    /**< \brief ����ʱ1us�ļ���ֵ   */
uint32_t __g_ms_ticks;    /**< \brief ����ʱ1ms�ļ���ֵ   */

uint32_t __g_us_max;      /**< \brief ��ʱ�����us��      */
uint32_t __g_ms_max;      /**< \brief ��ʱ�����ms��      */


/**
 * \brief ��ʱ������ʼ��
 */
void am_delay_init (void)
{
    uint32_t system_clk;

    system_clk = amhw_lpc82x_clk_system_clkrate_get();
    
    /* ʹ��ϵͳʱ����ΪSystick��ʱ��ʱ��Դ */
    amhw_lpc82x_systick_config(AMHW_LPC82X_SYSTICK,
                               AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM );
    
    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);
 
    __g_us_ticks =  system_clk / 1000000;
    __g_ms_ticks =  system_clk / 1000;
    
    __g_us_max   = (uint64_t)0xFFFFFF * (uint64_t)1000000 / system_clk;
    
    __g_ms_max   = __g_us_max / 1000; 
}

/** 
 * \brief ���뼶����ʱ����
 */
void am_mdelay (uint32_t nms)
{
    uint32_t n_max  = nms / __g_ms_max;
    uint32_t ms_rem = nms % __g_ms_max;

    if (n_max > 0) {    
        amhw_lpc82x_systick_reload_val_set(AMHW_LPC82X_SYSTICK,
                                           __g_ms_max * __g_ms_ticks);
       
        amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);  /* ���������ֵ   */
        
        amhw_lpc82x_systick_enable(AMHW_LPC82X_SYSTICK);     /* ��ʼ���¼���   */

        while (n_max > 0) {
            
            /* Wait for time up */
            while (amhw_lpc82x_systick_flag_check(AMHW_LPC82X_SYSTICK) != TRUE);
            n_max--;
        }
        amhw_lpc82x_systick_disable(AMHW_LPC82X_SYSTICK);    /* ֹͣ����       */
    }  

    amhw_lpc82x_systick_reload_val_set(AMHW_LPC82X_SYSTICK, ms_rem * __g_ms_ticks);
    
    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);     /* ���������ֵ   */
    amhw_lpc82x_systick_enable(AMHW_LPC82X_SYSTICK);         /* ��ʼ���¼���   */
    
    /* �ȴ���ʱ��� */
    while (amhw_lpc82x_systick_flag_check(AMHW_LPC82X_SYSTICK) != TRUE);
 
    amhw_lpc82x_systick_disable(AMHW_LPC82X_SYSTICK);        /* ֹͣ����       */
    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);     /* ���������ֵ   */
}
 
/** 
 * \brief ΢�뼶����ʱ����
 *
 * ��ϵͳʱ��Ϊ��100MHz, __g_us_max = 0xFFFFFF * 1000000 / 100000000 
 *                                  = 0xFFFFFF / 100 
 *                                  = 167772us
 * ��ϵͳʱ��Ϊ��12MHz, __g_us_max  = 0xFFFFFF * 1000000 / 12000000 
 *                                  = 0xFFFFFF / 12 
 *                                  = 1398101 us
 */
void am_udelay (uint32_t nus)
{
    /* ������ʱ΢����װ�ض�ʱֵ */
    amhw_lpc82x_systick_reload_val_set(AMHW_LPC82X_SYSTICK, nus * __g_us_ticks);

    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);    /* ���������ֵ   */
    amhw_lpc82x_systick_enable(AMHW_LPC82X_SYSTICK);        /* ��ʼ���¼���   */
   
    /* �ȴ���ʱ��� */
    while (amhw_lpc82x_systick_flag_check(AMHW_LPC82X_SYSTICK) != TRUE);
    
    amhw_lpc82x_systick_disable(AMHW_LPC82X_SYSTICK);       /* ֹͣ����       */
    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);    /* ���������ֵ   */
}
 
/* end of file */
