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
 * \brief LPC824 ���İ�
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
  ȫ�ֱ���
*******************************************************************************/

/** 
 * \brief SCT0_PWM ��handle
 *
 * ��ʹ�÷�����ʱ��Ĭ�Ͻ�ʹ��SCT��OUT1���PWM���Σ�����Ѿ���SCT��ʼ��ΪPWM���ܣ�
 * ����SCT���������·PWM����˿���ʹ�ø�handle���Ա�ʹ������5·PWM��
 */
am_pwm_handle_t g_sct0_pwm_handle;

/** 
 * \brief MRT ��handle
 *
 * ��ʹ�������ʱ��ʱ��Ĭ�Ͻ�ʹ��MRT��Ϊ������ʱ��Ԫ������Ѿ���MRT��ʼ��Ϊ��ʱ�����ܣ�
 * ����MRT����֧��4·��ʱ����˿���ʹ�ø� handle���Ա�ʹ������3· handle 
 */
am_timer_handle_t g_mrt_handle;

/** \brief ���ʹ����DELAY���������ú��� */
#if (AM_CFG_DELAY_ENABLE == 1)
    extern void am_delay_init (void);
#endif

/*******************************************************************************
  ����LED�����Ϣ
*******************************************************************************/

#if (AM_CFG_LED_ENABLE == 1)

#include "am_led.h"

/** \brief ����LED��GPIO�ܽź��Ƿ�͵�ƽ���� */
const am_led_info_t g_led_info[] = {
     {PIO0_20, TRUE},
     {PIO0_21, TRUE},
};

/** \brief ����LED���� */
const uint32_t g_led_num = sizeof(g_led_info) / sizeof(g_led_info[0]);

#endif

/*******************************************************************************
  ������Դ�����������Ϣ
*******************************************************************************/

#if (AM_CFG_BUZZER_ENABLE == 1)

#include "am_lpc82x_sct_pwm.h"

#define __BUZZER_PWM_PERIOD_NS \
            (1000000000 / AM_CFG_BUZZER_PWM_FREQ)

#define __BUZZER_PWM_DUTY_NS   \
            (10000000 * AM_CFG_BUZZER_PWM_DUTY / AM_CFG_BUZZER_PWM_FREQ)
#endif 

/*******************************************************************************
  ��������
*******************************************************************************/

/**
 * \brief �弶��ʼ��
 */
void am_board_init (void)
{

/* 
 * ��ʹ����ʱ������ʹ�ã���ִ�г�ʼ����Ĭ��ʵ��Ϊʹ�� Systick ��ʱ��
 * ����ʵ�ֿɲμ� am_delay.c�ļ��������������󣬿��Ը���ʵ�������޸ġ�
 */
#if (AM_CFG_DELAY_ENABLE == 1)
    am_delay_init();
#endif

/** 
 * \brief ���Ϊ1�����ʼ��led����ع��ܣ�����Ĭ��������LED
 *
 * ID: 0 --- PIO0.20 ����Ҫ�̽�����ñ J9��
 * ID: 1 --- PIO0.21 ����Ҫ�̽�����ñ J10��
 */
#if (AM_CFG_LED_ENABLE == 1)
    am_led_init(g_led_info, g_led_num);
#endif

/** 
 * \brief ���Ϊ1�����ʼ��led����ع��ܣ�����Ĭ��������LED
 *
 * Ĭ��ʹ��SCT_OU1 ��PIO0_24�����PWM ����Ҫ�̽�����ñ J7��
 */
#if (AM_CFG_BUZZER_ENABLE == 1)
    
    g_sct0_pwm_handle = am_lpc82x_sct0_pwm_inst_init();

    am_buzzer_init(g_sct0_pwm_handle,
                   1,                    /* ʹ��ͨ��1, SCT_OUT1, PIO0_24 */
                   __BUZZER_PWM_DUTY_NS,
                   __BUZZER_PWM_PERIOD_NS);
#endif
    
#if (AM_CFG_DEBUG_ENABLE == 1)

#if (AM_CFG_DEBUG_UART == 0)

    /* ���Գ�ʼ�� */
    am_debug_init(am_lpc82x_usart0_inst_init(), 
                  AM_CFG_DEBUG_BAUDRATE);
                  
#endif

#if (AM_CFG_DEBUG_UART == 1)

    /* ���Գ�ʼ�� */
    am_debug_init(am_lpc82x_usart1_inst_init(), 
                  AM_CFG_DEBUG_BAUDRATE);
                  
#endif

#if (AM_CFG_DEBUG_UART == 2)

    /* ���Գ�ʼ�� */
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

    //�������ݴ����
}

/* end of file */
