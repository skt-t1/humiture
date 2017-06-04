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
 * \brief 使用系统滴答时钟(Systick) 实现标准的毫秒级别延时函数和微秒级别延时函数 
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

uint32_t __g_us_ticks;    /**< \brief 当延时1us的计数值   */
uint32_t __g_ms_ticks;    /**< \brief 当延时1ms的计数值   */

uint32_t __g_us_max;      /**< \brief 延时的最大us数      */
uint32_t __g_ms_max;      /**< \brief 延时的最大ms数      */


/**
 * \brief 延时函数初始化
 */
void am_delay_init (void)
{
    uint32_t system_clk;

    system_clk = amhw_lpc82x_clk_system_clkrate_get();
    
    /* 使用系统时钟作为Systick定时器时钟源 */
    amhw_lpc82x_systick_config(AMHW_LPC82X_SYSTICK,
                               AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM );
    
    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);
 
    __g_us_ticks =  system_clk / 1000000;
    __g_ms_ticks =  system_clk / 1000;
    
    __g_us_max   = (uint64_t)0xFFFFFF * (uint64_t)1000000 / system_clk;
    
    __g_ms_max   = __g_us_max / 1000; 
}

/** 
 * \brief 毫秒级别延时函数
 */
void am_mdelay (uint32_t nms)
{
    uint32_t n_max  = nms / __g_ms_max;
    uint32_t ms_rem = nms % __g_ms_max;

    if (n_max > 0) {    
        amhw_lpc82x_systick_reload_val_set(AMHW_LPC82X_SYSTICK,
                                           __g_ms_max * __g_ms_ticks);
       
        amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);  /* 清零计数器值   */
        
        amhw_lpc82x_systick_enable(AMHW_LPC82X_SYSTICK);     /* 开始向下计数   */

        while (n_max > 0) {
            
            /* Wait for time up */
            while (amhw_lpc82x_systick_flag_check(AMHW_LPC82X_SYSTICK) != TRUE);
            n_max--;
        }
        amhw_lpc82x_systick_disable(AMHW_LPC82X_SYSTICK);    /* 停止计数       */
    }  

    amhw_lpc82x_systick_reload_val_set(AMHW_LPC82X_SYSTICK, ms_rem * __g_ms_ticks);
    
    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);     /* 清零计数器值   */
    amhw_lpc82x_systick_enable(AMHW_LPC82X_SYSTICK);         /* 开始向下计数   */
    
    /* 等待延时完成 */
    while (amhw_lpc82x_systick_flag_check(AMHW_LPC82X_SYSTICK) != TRUE);
 
    amhw_lpc82x_systick_disable(AMHW_LPC82X_SYSTICK);        /* 停止计数       */
    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);     /* 清零计数器值   */
}
 
/** 
 * \brief 微秒级别延时函数
 *
 * 当系统时钟为：100MHz, __g_us_max = 0xFFFFFF * 1000000 / 100000000 
 *                                  = 0xFFFFFF / 100 
 *                                  = 167772us
 * 当系统时钟为：12MHz, __g_us_max  = 0xFFFFFF * 1000000 / 12000000 
 *                                  = 0xFFFFFF / 12 
 *                                  = 1398101 us
 */
void am_udelay (uint32_t nus)
{
    /* 根据延时微秒数装载定时值 */
    amhw_lpc82x_systick_reload_val_set(AMHW_LPC82X_SYSTICK, nus * __g_us_ticks);

    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);    /* 清零计数器值   */
    amhw_lpc82x_systick_enable(AMHW_LPC82X_SYSTICK);        /* 开始向下计数   */
   
    /* 等待延时完成 */
    while (amhw_lpc82x_systick_flag_check(AMHW_LPC82X_SYSTICK) != TRUE);
    
    amhw_lpc82x_systick_disable(AMHW_LPC82X_SYSTICK);       /* 停止计数       */
    amhw_lpc82x_systick_val_set(AMHW_LPC82X_SYSTICK, 0);    /* 清零计数器值   */
}
 
/* end of file */
