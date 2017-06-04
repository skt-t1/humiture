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
 * \brief LPC82X ADC 用户配置文件。
 * \sa am_lpc82x_hwconfig_adc0.c
 * 
 * \internal
 * \par Modification history
 * - 1.01 15-12-14  hgo, add calibrate func.
 * - 1.00 15-07-15  bob, first implementation.
 * \endinternal
 */
 
#include "ametal.h"
#include "am_lpc82x.h"
#include "am_lpc82x_adc.h"
#include "amhw_lpc82x_syscon.h"
#include "amhw_lpc82x_clk.h"
#include "amhw_lpc82x_adc.h"
/**
 * \addtogroup am_lpc82x_if_hwconfig_src_adc0
 * \copydoc am_lpc82x_hwconfig_adc0.c
 * @{
 */

/** \brief ADC0 平台初始化函数 */
static void __lpc82x_adc0_plfm_init (void)
{    
    amhw_lpc82x_syscon_powerup(AMHW_LPC82X_SYSCON_PD_ADC0);
    amhw_lpc82x_clk_periph_enable(AMHW_LPC82X_CLK_ADC0);

    /* 配置通道，使用通道0和通道1,引脚配置为消极模式INACTIVE  */
    am_gpio_pin_cfg(PIO0_7,  PIO0_7_ADC_0   | PIO0_7_INACTIVE);
    am_gpio_pin_cfg(PIO0_6,  PIO0_6_ADC_1   | PIO0_6_INACTIVE);

//    am_gpio_pin_cfg(PIO0_14, PIO0_14_ADC_2  | PIO0_14_INACTIVE);
//    am_gpio_pin_cfg(PIO0_23, PIO0_23_ADC_3  | PIO0_23_INACTIVE);
//    am_gpio_pin_cfg(PIO0_22, PIO0_22_ADC_4  | PIO0_22_INACTIVE);
//    am_gpio_pin_cfg(PIO0_21, PIO0_21_ADC_5  | PIO0_21_INACTIVE);
//    am_gpio_pin_cfg(PIO0_20, PIO0_20_ADC_6  | PIO0_20_INACTIVE);
    am_gpio_pin_cfg(PIO0_19, PIO0_19_ADC_7  | PIO0_19_INACTIVE);
//    am_gpio_pin_cfg(PIO0_18, PIO0_18_ADC_8  | PIO0_18_INACTIVE);
//    am_gpio_pin_cfg(PIO0_17, PIO0_17_ADC_9  | PIO0_17_INACTIVE);
//    am_gpio_pin_cfg(PIO0_13, PIO0_13_ADC_10 | PIO0_13_INACTIVE);
//    am_gpio_pin_cfg(PIO0_4,  PIO0_4_ADC_11  | PIO0_4_INACTIVE);
    
    /* ADC自动矫正 */
    amhw_lpc82x_adc_calibrate(AMHW_LPC82X_ADC0, 
                              amhw_lpc82x_clk_system_clkrate_get());
}

/** \brief ADC0 平台解初始化 */
static void __lpc82x_adc0_plfm_deinit (void)
{
    amhw_lpc82x_clk_periph_disable(AMHW_LPC82X_CLK_ADC0);
    amhw_lpc82x_syscon_powerdown(AMHW_LPC82X_SYSCON_PD_ADC0);
}

/** \brief ADC0 设备信息 */
static const am_lpc82x_adc_devinfo_t __g_adc0_devinfo = {
    LPC82X_ADC0_BASE,              /**< \brief ADC寄存器块的基地址  */
    2500,                          /**< \brief ADC参考电压，单位：mV  */
    INUM_ADC0_SEQA,                /**< \brief ADC0序列A中断          */
    INUM_ADC0_SEQB,                /**< \brief ADC0序列B中断          */
    INUM_ADC0_THCMP,               /**< \brief ADC0阈值比较和错误中断 */
    INUM_ADC0_OVR,                 /**< \brief ADC0 overrun 中断      */
    AM_LPC82X_ADC_WORK_MODE_DMA,   /**< \brief ADC工作模式            */
    DMA_CHAN_0,                    /**< \brief ADC 选用DMA通道0       */
    
    __lpc82x_adc0_plfm_init,       /**< \brief 平台初始化函数         */
    __lpc82x_adc0_plfm_deinit,     /**< \brief 平台解初始化函数       */
};

/** \brief ADC0设备实例 */
static am_lpc82x_adc_dev_t __g_adc0_dev;

/** \brief ADC0 实例初始化，获得ADC标准服务句柄 */
am_adc_handle_t am_lpc82x_adc0_inst_init (void)
{
    return am_lpc82x_adc_init(&__g_adc0_dev, &__g_adc0_devinfo);
}

/** \brief ADC0 实例解初始化 */
void am_lpc82x_adc0_inst_deinit (am_adc_handle_t handle)
{
    am_lpc82x_adc_deinit(handle);
}

/**
 * @}
 */

/* end of file */
