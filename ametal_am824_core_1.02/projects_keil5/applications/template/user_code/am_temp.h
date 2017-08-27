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
 * \brief 温度传感器接口
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-01  skt, first implementation.
 * \endinternal
 */
 
#ifndef __AM_TEMP_H
#define __AM_TEMP_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "am_common.h"

/** 
 * \brief 温度传感器信息结构体
 * \attention 需要注意的是，下面描述温度的数值，皆为实际温度 * multiple，
 *            因此，将下面接口获取的有关温度值除以multiple可以得到实际的温度值
 */
typedef struct am_temp_info {
    int freq;           /**< \brief 温度传感器数值更新频率 (单位:Hz) */    
    int multiple;       /**< \brief 获取的温度值与实际温度之间的倍数值 */
    int temp_min;       /**< \brief 温度测量下行值（最低温度）*/
    int temp_max;       /**< \brief 温度测试上行值（最高温度）*/
} am_temp_info_t;

/** 
 * \brief 服务信息 
 */
struct am_temp_servinfo {
    int id;     /**< \brief 设备ID号 */
};

/**
 * \brief 服务函数
 */
struct am_temp_servopts {
    
    /** \brief 温度传感器信息获取 */
    int (*pfn_temp_info_get) (void *p_cookie, am_temp_info_t *p_info);
    
    /** \brief 获取温度传感器温度值 */
    int (*pfn_temp_read) (void *p_cookie, int *p_value);
    
    /** \brief 使能温度报警功能 */
    int (*pfn_temp_alarm_enable) (void *p_cookie, int temp);
    
    /** \brief 禁能温度报警功能 */
    int (*pfn_temp_alarm_disable) (void *p_cookie);
    
    /** \brief 设置报警信号的回调函数 */
    int (*pfn_temp_signal) (void         *p_cookie,
                            am_pfnvoid_t  pfn_alarm, 
                            void         *p_arg);
};

struct am_temp_service {
    
    /** \brief 指向下一个服务 */
    struct am_temp_service *p_next;
    
    /** \brief 服务相关信息 */
    struct am_temp_servinfo *p_info;
    
    /** \brief 服务相关函数 */
    struct am_temp_servopts *p_opts;
    
    /** \brief 调用服务相关函数时，传入的第一个参数 */
    void *p_cookie;
};

/**
 * \brief 获取温度传感器信息
 *
 * \param[in] id     : 设备ID号
 * \param[in] p_info : 返回温度传感器信息
 *
 * \retval AM_OK : 成功
 */
int am_temp_info_get (int id, am_temp_info_t *p_info);

/**
 * \brief 获取温度
 * 
 * \param[in]  id       : 设备ID号
 * \param[out] p_valude : 返回温度值，需除以倍数才能得到实际值
 *
 * \retval AM_OK : 成功
 */
int am_temp_read (int id, int *p_value);
                       
/**
 * \brief 使能报警功能
 *
 * \param[in] id   : 设备ID号
 * \param[in] temp : 温度值
 *
 * \retval AM_OK : 成功
 */
int am_temp_alarm_enable (int id, int temp);

/**
 * \brief 禁能报警功能
 *
 * \param[in] id   : 设备ID号
 * \param[in] temp : 温度值
 *
 * \retval AM_OK : 成功
 */
int am_temp_alarm_disable (int id);

/**
 * \brief 设置报警信号的回调函数
 *
 * \param[in] id        : 设备ID号
 * \param[in] pfn_alarm : 报警回调函数
 * \param[in] p_arg     : 回调函数入口参数
 *
 * \retval AM_OK : 成功
 */
int am_temp_signal (int id, am_pfnvoid_t pfn_alarm, void *p_arg);

#ifdef __cplusplus
}
#endif

#endif /* __AM_TEMP_H */

/* end of file */
