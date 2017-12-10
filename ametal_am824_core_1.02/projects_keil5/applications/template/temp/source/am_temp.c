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
 * \brief 温度传感器接口实现
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-01  skt, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_temp.h"

/** \brief 服务结构体链表头结点 */
static struct am_temp_service *__gp_temp_serv = NULL;

/** \brief 添加服务节点 */
int am_temp_dev_add (struct am_temp_service *p_serv)
{
    if (p_serv == NULL) {
        return -AM_EINVAL;
    }
    
    p_serv->p_next = __gp_temp_serv;
    __gp_temp_serv = p_serv;
    
    return AM_OK;
}

/** \brief 根据设备ID号获取服务结构体指针 */
static struct am_temp_service *__temp_service_get (int id)
{
    struct am_temp_service *p_serv = __gp_temp_serv;
    
    while (p_serv != NULL) {
        if (p_serv->p_info->id == id) {
            break;
        }
        p_serv = p_serv->p_next;
    }
    
    return p_serv;
}

/******************************************************************************/
int am_temp_info_get (int id, am_temp_info_t *p_info)
{
    struct am_temp_service *p_serv = __temp_service_get(id);
    
    int ret = -AM_ENOTSUP;
    
    if (p_serv == NULL) {
        return -AM_EINVAL;
    }
    
    if (p_serv->p_opts->pfn_temp_info_get) {
        ret = p_serv->p_opts->pfn_temp_info_get(p_serv->p_cookie, p_info);
    }
    
    return ret;
}

/******************************************************************************/
int am_temp_read (int id, int *p_value)
{
    struct am_temp_service *p_serv = __temp_service_get(id);
    
    int ret = -AM_ENOTSUP;
    
    if (p_serv == NULL) {
        return -AM_EINVAL;
    }
    
    if (p_serv->p_opts->pfn_temp_read) {
        ret = p_serv->p_opts->pfn_temp_read(p_serv->p_cookie, p_value);
    }
    
    return ret;
}
                       
/******************************************************************************/
int am_temp_alarm_enable (int id, int temp)
{
    struct am_temp_service *p_serv = __temp_service_get(id);
    
    int ret = -AM_ENOTSUP;
    
    if (p_serv == NULL) {
        return -AM_EINVAL;
    }
    
    if (p_serv->p_opts->pfn_temp_alarm_enable) {
        ret = p_serv->p_opts->pfn_temp_alarm_enable(p_serv->p_cookie, temp);
    }
    
    return ret;
}

/******************************************************************************/
int am_temp_alarm_disable (int id)
{
    struct am_temp_service *p_serv = __temp_service_get(id);
    
    int ret = -AM_ENOTSUP;
    
    if (p_serv == NULL) {
        return -AM_EINVAL;
    }
    
    if (p_serv->p_opts->pfn_temp_alarm_disable) {
        ret = p_serv->p_opts->pfn_temp_alarm_disable(p_serv->p_cookie);
    }
    
    return ret;
}

/******************************************************************************/
int am_temp_signal (int id, am_pfnvoid_t pfn_alarm, void *p_arg)
{
    struct am_temp_service *p_serv = __temp_service_get(id);
    
    int ret = -AM_ENOTSUP;
    
    if (p_serv == NULL) {
        return -AM_EINVAL;
    }
    
    if (p_serv->p_opts->pfn_temp_signal) {
        ret = p_serv->p_opts->pfn_temp_signal(p_serv->p_cookie,
                                              pfn_alarm,
                                              p_arg);
    }
    
    return ret;
}
 
/* end of file */
