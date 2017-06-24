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
 * \brief LPC824 核心板
 *
 * \internal
 * \par Modification history
 * - 1.00 14-12-01  tee, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_prj_config.h"
#include "am_board.h"
#include "am_vdebug.h"
#include "am_lpc82x_clk.h"
#include "am_softimer.h"
#include "am_lpc82x_inst_init.h"
#include "am_lpc82x.h"

/*******************************************************************************
  全局变量
*******************************************************************************/

/** 
 * \brief SCT0_PWM 的handle
 *
 * 当使用蜂鸣器时，默认将使用SCT的OUT1输出PWM波形，因此已经将SCT初始化为PWM功能，
 * 由于SCT可以输出六路PWM，因此可以使用该handle，以便使用其它5路PWM。
 */
am_pwm_handle_t g_sct0_pwm_handle;

/** 
 * \brief MRT 的handle
 *
 * 当使用软件定时器时，默认将使用MRT作为基础定时单元，因此已经将MRT初始化为定时器功能，
 * 由于MRT可以支持4路定时，因此可以使用该 handle，以便使用其它3路 handle 
 */
am_timer_handle_t g_mrt_handle;

/** \brief 如果使能了DELAY，则声明该函数 */
#if (AM_CFG_DELAY_ENABLE == 1)
    extern void am_delay_init (void);
#endif

/*******************************************************************************
  板上LED相关信息
*******************************************************************************/

#if (AM_CFG_LED_ENABLE == 1)

#include "am_led.h"

/** \brief 板上LED的GPIO管脚和是否低电平点亮 */
const am_led_info_t g_led_info[] = {
     {PIO0_20, TRUE},
     {PIO0_21, TRUE},
};

/** \brief 板上LED数量 */
const uint32_t g_led_num = sizeof(g_led_info) / sizeof(g_led_info[0]);

#endif

/*******************************************************************************
  板上无源蜂鸣器相关信息
*******************************************************************************/

#if (AM_CFG_BUZZER_ENABLE == 1)

#include "am_lpc82x_sct_pwm.h"

#define __BUZZER_PWM_PERIOD_NS \
            (1000000000 / AM_CFG_BUZZER_PWM_FREQ)

#define __BUZZER_PWM_DUTY_NS   \
            (10000000 * AM_CFG_BUZZER_PWM_DUTY / AM_CFG_BUZZER_PWM_FREQ)
#endif 

/*******************************************************************************
  公共函数
*******************************************************************************/

/**
 * \brief 板级初始化
 */
void am_board_init (void)
{

/* 
 * 若使能延时函数的使用，则执行初始化，默认实现为使用 Systick 定时器
 * 具体实现可参见 am_delay.c文件，如有其它需求，可以根据实际自行修改。
 */
#if (AM_CFG_DELAY_ENABLE == 1)
    am_delay_init();
#endif

/** 
 * \brief 如果为1，则初始化led的相关功能，板上默认有两个LED
 *
 * ID: 0 --- PIO0.20 （需要短接跳线帽 J9）
 * ID: 1 --- PIO0.21 （需要短接跳线帽 J10）
 */
#if (AM_CFG_LED_ENABLE == 1)
    am_led_init(g_led_info, g_led_num);
#endif

/** 
 * \brief 如果为1，则初始化led的相关功能，板上默认有两个LED
 *
 * 默认使用SCT_OU1 （PIO0_24）输出PWM （需要短接跳线帽 J7）
 */
#if (AM_CFG_BUZZER_ENABLE == 1)
    
    g_sct0_pwm_handle = am_lpc82x_sct0_pwm_inst_init();

    am_buzzer_init(g_sct0_pwm_handle,
                   1,                    /* 使用通道1, SCT_OUT1, PIO0_24 */
                   __BUZZER_PWM_DUTY_NS,
                   __BUZZER_PWM_PERIOD_NS);
#endif
    
#if (AM_CFG_DEBUG_ENABLE == 1)

#if (AM_CFG_DEBUG_UART == 0)

    /* 调试初始化 */
    am_debug_init(am_lpc82x_usart0_inst_init(), 
                  AM_CFG_DEBUG_BAUDRATE);
                  
#endif

#if (AM_CFG_DEBUG_UART == 1)

    /* 调试初始化 */
    am_debug_init(am_lpc82x_usart1_inst_init(), 
                  AM_CFG_DEBUG_BAUDRATE);
                  
#endif

#if (AM_CFG_DEBUG_UART == 2)

    /* 调试初始化 */
    am_debug_init(am_lpc82x_usart2_inst_init(), 
                  AM_CFG_DEBUG_BAUDRATE);
                  
#endif

#endif


#if (AM_CFG_SOFTIMER_ENABLE == 1)
    
    g_mrt_handle = am_lpc82x_mrt_inst_init();

    am_softimer_module_init(g_mrt_handle,
                            AM_CFG_SOFTIMER_CHAN, 
                            1000);
#endif

    //其它内容待添加
}

/* end of file */
