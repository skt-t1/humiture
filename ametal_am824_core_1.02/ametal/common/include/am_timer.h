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
 * \brief 定时器标准接口
 *
 * \internal
 * \par Modification History
 * - 1.01 15-12-07  hgo, del am_timer_connect().
 * - 1.00 15-01-05  tee, first implementation.
 * \endinternal
 */

#ifndef __AM_TIMER_H
#define __AM_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_common.h"

/**
 * \addtogroup am_if_timer
 * \copydoc am_timer.h
 * @{
 */

/**
 * \name 定时器特性
 *  
 *  - 低8位代表定时器位数，如32位定时器，则低8位值为32
 *  - 8 ~ 15 位表示定时器通道数
 *  - 其它定时器特性仅占一位，从第16位开始。 AM_TIMER_FEATURE(0) ~ AM_TIMER_FEATURE(15)
 * @{
 */

/** \brief 定时器位数                     */
#define AM_TIMER_SIZE(width)             ((uint32_t)(width & 0x00ffu))

/** \brief 从特性中获取定时器位数         */
#define AM_TIMER_SIZE_GET(features)      (features & 0x00ffu)

/** \brief 定时器通道数                   */
#define AM_TIMER_CHAN_NUM(num)           (((uint32_t)num << 8) & 0xff00u)

/** \brief 从特性中获取定时器通道数       */
#define AM_TIMER_CHAN_NUM_GET(features)  ((features & 0x00ff00u) >> 8)
 
/** \brief 用于其它定时器特性定义         */
#define AM_TIMER_FEATURE(feature_id)     ((uint32_t)(1u << (feature_id + 16)))

/** \brief 定时器可以产生中断             */
#define AM_TIMER_CAN_INTERRUPT             AM_TIMER_FEATURE(0)

/** \brief 定时器可以读取当前计数器的值   */
#define AM_TIMER_INTERMEDIATE_COUNT        AM_TIMER_FEATURE(1)

/** \brief 定时器不能被停止               */
#define AM_TIMER_CANNOT_DISABLE            AM_TIMER_FEATURE(2)

/** \brief 定时器在读取计数值时会停止     */
#define AM_TIMER_STOP_WHILE_READ           AM_TIMER_FEATURE(3)

/** \brief 定时器支持自动重装载计数值     */
#define AM_TIMER_AUTO_RELOAD               AM_TIMER_FEATURE(4)

/** \brief 定时器只有运行，不能改变翻转值 */
#define AM_TIMER_CANNOT_MODIFY_ROLLOVER    AM_TIMER_FEATURE(5)

/** \brief 定时器不能支持最小频率和最大频率之间的任意频率  */
#define AM_TIMER_CANNOT_SUPPORT_ALL_FREQS  AM_TIMER_FEATURE(6)

/** @} */


/** 
 * \brief 定时器信息结构体
 */
typedef struct am_timer_info {
    uint32_t  clk_frequency;   /**< \brief 定时器的输入频率       */
    uint32_t  min_frequency;   /**< \brief 最小工作频率(中断频率) */
    uint32_t  max_frequency;   /**< \brief 最大工作频率(中断频率) */
    uint32_t  features;        /**< \brief 定时器特性             */
} am_timer_info_t;

/** 
 * \brief 定时器驱动函数结构体
 */
struct am_timer_drv_funcs {

    /** \brief 获取定时器信息 */
    int (*pfn_timer_info_get) (void *p_drv, am_timer_info_t *p_info);
    
    /** 
     * \brief 获取当前定时器计数值
     * 返回的计数值应该是假设定时器工作在向上计数模式下的当前计数值
     */
    int (*pfn_timer_count_get) (void     *p_drv,
                                uint8_t   chan,
                                uint32_t *p_count);
    
    /** \brief 获取定时器翻转值 */
    int (*pfn_timer_rollover_get) (void     *p_drv,
                                   uint8_t   chan,
                                   uint32_t *p_rollover);
    
    /** \brief 禁能定时器                         */
    int (*pfn_timer_disable) (void *p_drv, uint8_t chan);
    
    /** \brief 使能定时器，同时设定定时count值    */
    int (*pfn_timer_enable) (void     *p_drv, 
                             uint8_t   chan,
                             uint32_t  max_timer_count);
    
    /** \brief 设置回调函数，每当一个定时周期完成时调用 */
    int (*pfn_timer_callback_set)(void  *p_drv,
                                  uint8_t chan,
                                  void (*pfn_callback)(void *),
                                  void  *p_arg);
                             
    /**
     * \brief 获取64位定时器当前计数值
     * 返回的计数值应该是假设定时器工作在向上计数模式下的当前计数值
     */
    int (*pfn_timer_count_get64) (void      *p_drv,
                                  uint8_t    chan,
                                  uint64_t  *p_count);
                               
    /** \brief 获取64位定时器的翻转值 */
    int (*pfn_timer_rollover_get64) (void     *p_drv,
                                     uint8_t   chan,
                                     uint64_t *p_rollover);
    
    /** \brief 使能64位定时器，同时设定64位定时count值 */
    int (*pfn_timer_enable64) (void     *p_drv, 
                               uint8_t   chan,
                               uint64_t  max_timer_count);
    
};


/** 
 * \brief 定时器服务
 */
typedef struct am_timer_serv {

    /** \brief 定时器驱动函数结构体指针 */
    struct am_timer_drv_funcs *p_funcs; 
    
    /** \brief 用于驱动函数的第一个参数 */
    void                      *p_drv;   
} am_timer_serv_t;

/** \brief 定时器标准服务操作句柄类型定义 */
typedef am_timer_serv_t *am_timer_handle_t;

/** 
 * \brief 获取定时器信息
 * 
 * \param[in]  handle : 定时器标准服务操作句柄
 * \param[out] p_info : 用于获取定时器信息的指针
 *
 * \retval  AM_OK     : 获取信息成功
 * \retval -AM_EINVAL : 获取信息失败, 参数错误
 */
am_static_inline
int am_timer_info_get (am_timer_handle_t handle, am_timer_info_t *p_info)
{
    return handle->p_funcs->pfn_timer_info_get(handle->p_drv,p_info);
}

/** 
 * \brief 获取定时器当前的计数值
 * 
 * 返回的计数值应该是假设定时器工作在向上计数模式下的当前计数值
 *
 * \param[in]  handle  : 定时器标准服务操作句柄
 * \param[in]  chan    : 定时器通道
 * \param[out] p_count : 用于获取定时器当前计数值的指针
 *
 * \retval  AM_OK      : 获取定时器当前计数值成功
 * \retval -AM_EINVAL  : 获取失败, 参数错误
 */
am_static_inline
int am_timer_count_get (am_timer_handle_t handle, 
                        uint8_t           chan, 
                        uint32_t         *p_count)
{
    return handle->p_funcs->pfn_timer_count_get(handle->p_drv,
                                                chan,
                                                p_count);
}

/** 
 * \brief 获取定时器翻转值
 *
 * \param[in]  handle     : 定时器标准服务操作句柄
 * \param[in]  chan       : 定时器通道
 * \param[out] p_rollover : 用于获取定时器翻转值的指针
 *
 * \retval  AM_OK         : 获取定时器翻转值成功
 * \retval -AM_EINVAL     : 获取失败, 参数错误
 */
am_static_inline
int am_timer_rollover_get (am_timer_handle_t  handle, 
                           uint8_t            chan, 
                           uint32_t          *p_rollover)
{
    return handle->p_funcs->pfn_timer_rollover_get(handle->p_drv,
                                                   chan,
                                                   p_rollover);
}

/** 
 * \brief 禁能定时器（停止计时）
 *
 * \param[in] handle  : 定时器标准服务操作句柄
 * \param[in] chan    : 定时器通道
 *
 * \retval  AM_OK     : 禁能定时器成功
 * \retval -AM_EINVAL : 禁能失败, 参数错误
 */
am_static_inline
int am_timer_disable (am_timer_handle_t handle, uint8_t chan)
{
    return handle->p_funcs->pfn_timer_disable(handle->p_drv, chan);
}

/** 
 * \brief 使能定时器，同时设定定时count值 
 *
 * \param[in] handle          : 定时器标准服务操作句柄
 * \param[in] chan            : 定时器通道
 * \param[in] max_timer_count : 定时count值 
 *
 * \retval  AM_OK             : 使能成功
 * \retval -AM_EINVAL         : 失败, 参数错误
 *
 * \note 通常，定时器计数值达到设定的定时count值时，会产生中断。同时，复位计数值
 *       为0，紧接着继续计时。
 */
am_static_inline
int am_timer_enable (am_timer_handle_t  handle, 
                     uint8_t            chan,
                     uint32_t           max_timer_count)
{
     return handle->p_funcs->pfn_timer_enable(handle->p_drv,
                                              chan,
                                              max_timer_count);
}

/** 
 * \brief 设置回调函数，每当一个定时周期完成时调用
 *
 * \param[in] handle       : 定时器标准服务操作句柄
 * \param[in] chan         : 定时器通道
 * \param[in] pfn_callback : 回调函数
 * \param[in] p_arg        : 回调函数的用户参数
 *
 * \retval  AM_OK     : 设置成功
 * \retval -AM_EINVAL : 设置失败, 参数错误
 *
 */
am_static_inline
int am_timer_callback_set (am_timer_handle_t  handle, 
                           uint8_t            chan,
                           void             (*pfn_callback)(void *),
                           void              *p_arg)
{
    return handle->p_funcs->pfn_timer_callback_set(handle->p_drv,
                                                   chan,
                                                   pfn_callback,
                                                   p_arg);
}

/** 
 * \brief 获取33 ~ 64位定时器的当前计数值
 *
 * 返回的计数值应该是假设定时器工作在向上计数模式下的当前计数值
 *
 * \param[in]  handle  : 定时器标准服务操作句柄
 * \param[in]  chan    : 定时器通道
 * \param[out] p_count : 用于获取定时器当前计数值的指针
 *
 * \retval  AM_OK      : 获取到的当前定时器计数值
 * \retval -AM_EINVAL  : 获取失败, 参数错误
 *
 * \note 如果定时器位数在33到64之间，必须调用该函数获取定时器的当前计数值
 */
am_static_inline
int am_timer_count_get64 (am_timer_handle_t  handle, 
                          uint8_t            chan,
                          uint64_t          *p_count)
{
    return handle->p_funcs->pfn_timer_count_get64(handle->p_drv,
                                                  chan,
                                                  p_count);
}
                           
/** 
 * \brief 获取33 ~ 64位定时器的翻转值
 *
 * \param[in]  handle     : 定时器标准服务操作句柄
 * \param[in]  chan       : 定时器通道
 * \param[out] p_rollover : 用于获取定时器翻转值的指针
 *
 * \retval  AM_OK         : 获取翻转值成功
 * \retval -AM_EINVAL     : 获取翻转值失败, 参数错误
 *
 * \note 如果定时器位数在33到64之间，必须调用该函数获取翻转值
 */
am_static_inline
int am_timer_rollover_get64 (am_timer_handle_t  handle, 
                             uint8_t            chan,
                             uint64_t          *p_rollover)
{
    return handle->p_funcs->pfn_timer_rollover_get64(handle->p_drv,
                                                     chan,
                                                     p_rollover);
}

/** 
 * \brief 使能33到64位定时器，并设置定时count值
 *
 * \param[in] handle          : 定时器标准服务操作句柄
 * \param[in] chan            : 定时器通道
 * \param[in] max_timer_count : 定时器count值
 *
 * \retval  AM_OK             : 使能成功
 * \retval -AM_EINVAL         : 使能失败, 参数错误
 *
 * \note 如果定时器位数在33到64之间，必须调用该函数使能定时器
 */
am_static_inline
int am_timer_enable64 (am_timer_handle_t  handle, 
                       uint8_t            chan,
                       uint64_t           max_timer_count)
{
    return handle->p_funcs->pfn_timer_enable64(handle->p_drv,
                                               chan,
                                               max_timer_count);
}

/**
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_TIMER_H */

/* end of file */
