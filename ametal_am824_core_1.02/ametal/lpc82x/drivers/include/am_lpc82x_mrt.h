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
 * \brief MRT驱动，服务Timer标准接口
 *
 * 该驱动实现了MRT的定时功能
 *
 * \internal
 * \par Modification History
 * - 1.01 15-12-07  mem,modified.
 * - 1.00 15-07-05  win, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_MRT_H
#define __AM_LPC82X_MRT_H

#include "ametal.h"
#include "am_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_mrt
 * \copydoc am_lpc82x_mrt.h
 * @{
 */

/**
 * \brief MRT设备信息结构体
 */
typedef struct am_lpc82x_mrt_devinfo {
    uint32_t   mrt_regbase;            /**< \brief 指向MRT寄存器块的指针    */
    uint8_t    inum;                   /**< \brief MRT中断号                */
    uint8_t    channel_nums;           /**< \brief 支持的通道数量，最大为4  */
    void     (*pfn_plfm_init)(void);   /**< \brief 平台初始化函数           */
    void     (*pfn_plfm_deinit)(void); /**< \brief 平台解初始化函数         */
} am_lpc82x_mrt_devinfo_t;

/**
 * \brief MRT设备
 */
typedef struct am_lpc82x_mrt_dev { 
    am_timer_serv_t   timer_serv;               /**< \brief 标准定时(Timer)服务      */
    const am_lpc82x_mrt_devinfo_t  *p_devinfo;  /**< \brief 指向MRT设备信息常量的指针 */                                                                         
    struct {                                                                    
        void (*pfn_callback)(void *);           /**< \brief 回调函数           */
        void *p_arg;                            /**< \brief 回调函数的用户参数 */
    } callback_info[4];                         /**< \brief 回调函数信息数组   */

} am_lpc82x_mrt_dev_t;

/**
 * \brief 初始化MRT为定时功能
 *
 * \param[in] p_dev     : 指向MRT设备的指针
 * \param[in] p_devinfo : 指向MRT设备信息常量的指针
 *
 * \return Timer标准服务操作句柄，值为NULL时表明初始化失败
 */
am_timer_handle_t am_lpc82x_mrt_init (am_lpc82x_mrt_dev_t           *p_dev,
                                      const am_lpc82x_mrt_devinfo_t *p_devinfo);

/**
 * \brief 不使用MRT时，解初始化MRT，释放相关资源
 *
 * \param[in] handle  : 定时器标准服务操作句柄
 *
 * \return 无
 */
void am_lpc82x_mrt_deinit (am_timer_handle_t handle);
    
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_MRT_H */

/* end of file */
