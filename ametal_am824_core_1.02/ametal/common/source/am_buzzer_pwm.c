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
 * \brief Buzzer implementation
 *
 * \internal
 * \par Modification history
 * - 1.00 15-01-15  tee, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_buzzer_pwm.h"
#include "am_delay.h"

/*******************************************************************************
  Global variable
*******************************************************************************/

static int             __g_buzzer_pwm_chan   = 0;
static am_pwm_handle_t __g_buzzer_pwm_handle = NULL;

/*******************************************************************************
  public functions
*******************************************************************************/ 

/**
 * \brief Initialize the Buzzer on the EasyARM-54000 Board
 */ 
void am_buzzer_init (am_pwm_handle_t pwm_handle,
                     int             chan,
                     unsigned int    duty_ns,
                     unsigned int    period_ns)
{
    if (pwm_handle != NULL) {
        __g_buzzer_pwm_chan   = chan;
        __g_buzzer_pwm_handle = pwm_handle;
     
        am_pwm_config(pwm_handle, chan, duty_ns, period_ns);
    }
}

/**
 * \brief The buzzer beep on
 * \return  None
 */ 
void am_buzzer_on (void)
{
    if (__g_buzzer_pwm_handle != NULL) {
        am_pwm_enable(__g_buzzer_pwm_handle, __g_buzzer_pwm_chan);
    }
}

/**
 * \brief The buzzer beep off
 * \return  None
 */ 
void am_buzzer_off (void)
{
    if (__g_buzzer_pwm_handle != NULL) {
        am_pwm_disable(__g_buzzer_pwm_handle, __g_buzzer_pwm_chan);
    }
}

/**
 * \brief The buzzer beep on ms time
 * \param[in] ms : The time to beep on in ms
 * \return  None
 */ 
void am_buzzer_beep (uint32_t ms)
{
    am_buzzer_on();
    am_mdelay(ms);
    
    am_buzzer_off();
}

/* end of file */
