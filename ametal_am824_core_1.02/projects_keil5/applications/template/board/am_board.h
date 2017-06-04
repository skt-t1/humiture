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
 * \brief AM824-Core ���İ�
 *
 * \internal
 * \par Modification history
 * - 1.00 14-12-01  tee, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_prj_config.h"

#include "am_delay.h"
#include "am_led.h"
#include "am_buzzer_pwm.h"
#include "am_timer.h"

/** \brief LED ����ID�� */
#define LED0          0
#define LED1          1

#if (AM_CFG_BUZZER_ENABLE == 1)

/** 
 * \brief SCT0_PWM ��handle
 *
 * ��ʹ�÷�����ʱ��Ĭ�Ͻ�ʹ��SCT��OUT1���PWM���Σ�����Ѿ���SCT��ʼ��ΪPWM���ܣ�
 * ����SCT���������·PWM����˿���ʹ�ø�handle���Ա�ʹ������5·PWM��
 */
extern am_pwm_handle_t g_sct0_pwm_handle;

#endif

#if (AM_CFG_SOFTIMER_ENABLE == 1)

/** 
 * \brief MRT ��handle
 *
 * ��ʹ�������ʱ��ʱ��Ĭ�Ͻ�ʹ��MRT��Ϊ������ʱ��Ԫ������Ѿ���MRT��ʼ��Ϊ��ʱ�����ܣ�
 * ����MRT����֧��4·��ʱ����˿���ʹ�ø� handle���Ա�ʹ������3· handle 
 */
extern am_timer_handle_t g_mrt_handle;

#endif

/**
 * \brief �弶��ʼ��(EasyARM-54000M4)
 * \return ��
 * \note �ú������ʼ��ϵͳʱ�ӡ�GPIO���жϡ���������LED����ʱ����
 */
void am_board_init(void);
    
/* end of file */
