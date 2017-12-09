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
 * \brief 温度传感器LM75驱动
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10  skt, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_vdebug.h"
#include "am_temp_lm75.h"

/* 添加温度传感器设备 */
extern int am_temp_dev_add (struct am_temp_service *p_serv);

/******************************************************************************/
am_local 
int __lm75_temp_info_get (void *p_cookie, am_temp_info_t *p_info)
{
    struct am_temp_lm75_dev *p_dev = p_cookie;
    
    p_info->temp_min = -55;
    p_info->temp_max = 125;
    
    return 0;
}

/******************************************************************************/
am_local 
int __lm75_temp_read (void *p_cookie, int *p_value)
{            
    struct am_temp_lm75_dev *p_dev = p_cookie;
    
    uint8_t            temp_value[2];
    
    am_i2c_read(&p_dev->i2c, 0x00, temp_value, 2);
    
    return 0;
}

/******************************************************************************/
am_local 
int __lm75_temp_alarm_enable (void *p_cookie, int temp)
{
    return 0;
}

/******************************************************************************/
am_local 
int __lm75_temp_alarm_disable (void *p_cookie)
{
    return 0;
}

/******************************************************************************/
am_local 
int __lm75_temp_signal (void         *p_cookie,
                        am_pfnvoid_t  pfn_alarm, 
                        void         *p_arg)
{
    return 0;    
}

/******************************************************************************/
am_local am_const struct am_temp_servopts __g_lm75_servopts = {
    __lm75_temp_info_get,
    __lm75_temp_read,
    __lm75_temp_alarm_enable,
    __lm75_temp_alarm_disable,
    __lm75_temp_signal
};

/**
 * \brief 设备初始化函数
 */
void am_temp_lm75_init (struct am_temp_lm75_dev           *p_dev,
                        const struct am_temp_lm75_devinfo *p_devinfo)
{
    static struct am_temp_service serv;
    am_i2c_handle_t               handle;
    
    if (p_dev == NULL || p_devinfo == NULL) {
        return ;
    }
    
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init(&handle);
    }
    
    if (handle == NULL) {
        AM_DBG_INFO("%s:%s get i2c handle fail!", __FILE__, __LINE__);
        return ;
    }
    
    am_i2c_mkdev(&p_dev->i2c,
                 handle,    
                 p_devinfo->addr, 
                 AM_I2C_ADDR_7BIT | AM_I2C_SUBADDR_1BYTE);
    
    serv.p_opts   = &__g_lm75_servopts;
    serv.p_info   = &p_devinfo->info;
    serv.p_cookie = p_dev;
    am_temp_dev_add(&serv);
}
 
/* end of file */
