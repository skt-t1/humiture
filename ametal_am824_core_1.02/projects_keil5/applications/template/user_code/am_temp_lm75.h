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
 
#ifndef __AM_TEMP_LM75_H
#define __AM_TEMP_LM75_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "am_temp.h"
    
/** 
 * \brief 设备信息结构体
 */
typedef struct am_temp_lm75_devinfo {
    
    /** \brief 服务信息 */
    struct am_temp_servinfo info;
    
    /** \brief 设备I2C地址 */
    int addr;
    
    /** \brief 平台初始化函数 */
    void (*pfn_plfm_init) (void);
    
    /** \brief 平台解初始化函数 */
    void (*pfn_plfm_deinit) (void);
    
} am_temp_lm75_devinfo_t;
    
/**
 * \brief 设备结构体
 */
typedef struct am_temp_lm75_dev {
    
    /** \brief 温度传感器服务结构体指针 */
    struct am_temp_service *p_serv;
    
    
} am_temp_lm75_dev_t;
    
/**
 * \brief 设备初始化函数
 */
void am_temp_lm75_init (am_temp_lm75_dev_t           *p_serv,
                        const am_temp_lm75_devinfo_t *p_devinfo);

/**
 * \brief 设备去初始化函数
 */
void am_temp_lm75_deinit (am_temp_lm75_dev_t *p_serv);
    
#ifdef __cplusplus
}
#endif

#endif /* __AM_TEMP_LM75_H */

/* end of file */
