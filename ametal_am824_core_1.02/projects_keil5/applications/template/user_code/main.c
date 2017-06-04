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
 * \brief LPC82x 模板工程
 *
 * \internal
 * \par Modification history
 * - 1.00 15-07-13  win, first implementation.
 * \endinternal
 */ 

#include "ametal.h"
#include "am_vdebug.h"
#include "am_led.h"
#include "am_delay.h"
#include "am_lpc82x_inst_init.h"

/**
 * \brief AMetal 应用程序入口
 */
int am_main (void)
{
    
    AM_DBG_INFO("Start up successful!\r\n");

    while (1) {
        am_led_on(0);
        am_mdelay(100);
        am_led_off(0);
        am_mdelay(100);
    }
}

/* end of file */
