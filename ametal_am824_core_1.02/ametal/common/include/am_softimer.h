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
 * \brief  软件定时器标准接口
 *
 * 软件定时器使用一个硬件定时器资源，提供定时服务。
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-07-31  tee,  first implementation.
 * \endinternal
 */

#ifndef __AM_SOFTIMER_H
#define __AM_SOFTIMER_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "am_common.h"
#include "am_list.h"
#include "am_timer.h"


/**
 * \addtogroup am_if_softimer
 * \copydoc am_softimer.h
 * @{
 */

/**
 * \brief 软件定时器结构体，包含了必要的信息
 */
struct am_softimer {
    struct am_list_head node;          /**< \brief 用于形成链表结构           */
    unsigned int        ticks;         /**< \brief 剩余时间tick值             */
    unsigned int        repeat_ticks;  /**< \brief 保存下重复定时的tick数     */
    void (*timeout_callback)( void *); /**< \brief 定时时间到回调函数         */
    void               *p_arg;         /**< \brief 回调函数的参数             */
};

typedef struct am_softimer am_softimer_t;

/**
 * \brief 软件定时器模块初始化
 *
 * \param[in] handle : 一个标准硬件定时器handle
 * \param[in] chan   : 使用硬件定时器的通道号
 * \param[in] freq   : 决定了软件定时器的时间密度，如频率为1000Hz,则时间密度为1
 *                     毫秒，后续所有软件定时的定时时间精度就为1ms。
 * \retval AM_OK     : 模块初始化成功，可以正常使用
 * \retval -AM_EINVAL: 参数无效，请检查handle和freq的值
 */
int am_softimer_module_init (am_timer_handle_t handle, uint8_t chan, unsigned int freq);
    
/**
 * \brief 初始化一个软件定时器
 *
 * \param[in] p_timer  : 指向一个软件定时器的指针
 * \param[in] p_func   : 定时时间到的回调函数
 * \param[in] p_arg    : 回调函数参数
 *
 * \retval AM_OK      : 软件定时器初始化成功
 * \retval -AM_EINVAL : 无效参数
 * \retval -AM_EPERM  : 软件定时器模块还未初始化，操作不运行，请先确保模块正确初始化
 */
int am_softimer_init(am_softimer_t *p_timer, am_pfnvoid_t p_func, void *p_arg);


/**
 * \brief 启动一个软件定时器
 * \param[in] p_timer : 指向一个软件定时器的指针
 * \param[in] ms      : 定时时间(ms)
 * \return 无
 */
void am_softimer_start(am_softimer_t *p_timer, unsigned int ms);

/**
 * \brief 停止一个软件定时器
 * \param[in] p_timer : 指向一个软件定时器的指针
 * \return 无
 */
void am_softimer_stop(am_softimer_t *p_timer);

/** 
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_SOFTIMER_H */

/* end of file */
