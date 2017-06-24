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
 * \brief SCT驱动，服务Timer标准接口
 *
 * 1. SCT支持提供如下三种标准服务，本驱动提供的是服务Timer标准服务的驱动。
 *     - 定时；
 *     - PWM输出；
 *     - 捕获；
 * 2. 在本驱动中，SCT相关资源使用如下：
 *    - 作32位定时器时，与事件0相关联，使用匹配寄存器0，以产生定时中断，仅有一个通道；
 *    - 作16位定时器时，通道0与事件0相关联，使用匹配寄存器0低阻，以产生定时中断；
 *                    通道1与事件1相关联，使用匹配寄存器0高阻，以产生定时中断；
 *
 * \internal
 * \par Modification history
 * - 1.01 15-09-24  bob, modify annotation.
 * - 1.00 15-07-09  oce, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_SCT_TIMING_H
#define __AM_LPC82X_SCT_TIMING_H

#include "ametal.h"
#include "am_timer.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_sct_timing
 * \copydoc am_lpc82x_sct_timing.h
 * @{
 */

/** \brief SCT运行在1个32位定时器模式, 仅能提供1个定时器通道    */
#define AM_LPC82X_SCT_TIMING_1_32BIT      1

/** \brief SCT运行在2个16位定时器模式, 能提供2个定时器通道      */
#define AM_LPC82X_SCT_TIMING_2_16BIT      2

/**
 * \brief SCT定时功能相关的设备信息
 */
typedef struct am_lpc82x_sct_timing_devinfo {
    uint32_t sct_regbase;  /**< \brief SCT寄存器块的基地址 */
    uint8_t  flag;         /**< \brief SCT配置标志(1-32bit 或者 2-16bit) */
    uint8_t  inum;         /**< \brief SCT中断号                          */

    /** \brief 平台初始化函数，如打开时钟，配置引脚等工作 */
    void     (*pfn_plfm_init)(void);

    /** \brief 平台解初始化函数 */
    void     (*pfn_plfm_deinit)(void);
} am_lpc82x_sct_timing_devinfo_t;
    
/**
 * \brief SCT定时功能设备
 */
typedef struct am_lpc82x_sct_timing_dev {

    am_timer_serv_t   timer_serv;     /**< \brief 标准定时(Timer)服务         */

    struct {
        void (*pfn_callback)(void *); /**< \brief 回调函数                    */
        void *p_arg;                  /**< \brief 回调函数的用户参数          */
    } callback_info[2];               /**< \brief 最大支持2通道(2-16bit定时器)*/
    
    /** \brief 指向SCT(定时功能)设备信息常量的指针 */
    const am_lpc82x_sct_timing_devinfo_t  *p_devinfo;

} am_lpc82x_sct_timing_dev_t;

/**
 * \brief 初始化SCT为定时功能
 *
 * \param[in] p_dev     : 指向SCT(定时功能)设备的指针
 * \param[in] p_devinfo : 指向SCT(定时功能)设备信息常量的指针
 *
 * \return Timer标准服务操作句柄，值为NULL时表明初始化失败
 */
am_timer_handle_t
am_lpc82x_sct_timing_init (am_lpc82x_sct_timing_dev_t     *p_dev,
                     const am_lpc82x_sct_timing_devinfo_t *p_devinfo);

/**
 * \brief 不使用SCT定时功能时，解初始化SCT定时功能，释放相关资源
 *
 * \param[in] handle : am_lpc82x_sct_timing_init() 初始化函数获得的Timer服务句柄
 *
 * \return 无
 */
void am_lpc82x_sct_timing_deinit (am_timer_handle_t handle);
/** 
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_SCT_TIMING_H */

/* end of file */
