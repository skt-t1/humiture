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
 * \brief SCT驱动，服务PWM标准接口
 *
 * 1. SCT支持提供如下三种标准服务，本驱动提供的是服务PWM输出标准服务的驱动。
 *     - 定时；
 *     - PWM输出；
 *     - 捕获。
 * 2. 在本驱动中，SCT相关资源使用如下：
 *    - PWM输出通道0使用SCT输出0，与事件1相关联，使用匹配寄存器1，以产生相应的占空比；
 *    - PWM输出通道1使用SCT输出1，与事件2相关联，使用匹配寄存器2，以产生相应的占空比；
 *    - PWM输出通道2使用SCT输出2，与事件3相关联，使用匹配寄存器3，以产生相应的占空比；
 *    - PWM输出通道3使用SCT输出3，与事件4相关联，使用匹配寄存器4，以产生相应的占空比；
 *    - PWM输出通道4使用SCT输出4，与事件5相关联，使用匹配寄存器5，以产生相应的占空比；
 *    - PWM输出通道5使用SCT输出5，与事件6相关联，使用匹配寄存器6，以产生相应的占空比；
 *    - 驱动内部占用事件0，它使用匹配寄存器0，以产生所有PWM输出通道的共同周期。
 *
 * \note 一个SCT输出的所有PWM共享周期值，也就是说，该SCT输出的所有PWM周期相同，
 *       频率相同
 * \internal
 * \par Modification history
 * - 1.01 15-09-21  win, modify annotation.
 * - 1.00 15-07-09  oce, first implementation.
 * \endinternal
 */


#ifndef __AM_LPC82X_SCT_PWM_H
#define __AM_LPC82X_SCT_PWM_H

#include "ametal.h"
#include "am_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_sct_pwm
 * \copydoc am_lpc82x_sct_pwm.h
 * @{
 */

/**
 * \brief SCT PWM输出功能相关的GPIO信息
 */
typedef struct am_lpc82x_sct_pwm_ioinfo {
    uint32_t gpio;              /**< \brief PWM GPIO */
    uint32_t func;              /**< \brief PWM默认功能 */
    uint32_t dfunc;             /**< \brief PWM禁止状态的功能*/
} am_lpc82x_sct_pwm_ioinfo_t;


/**
 * \brief SCT PWM输出功能相关的设备信息
 */
typedef struct am_lpc82x_sct_pwm_devinfo {
    uint32_t                    sct_regbase;  /**< \brief SCT寄存器块的基地址 */
    uint8_t                     channels_num; /**< \brief 使用的通道数，最大为6  */
    am_lpc82x_sct_pwm_ioinfo_t *p_ioinfo;     /**< \brief 指向输出管脚信息结构体*/

    /** \brief 平台初始化函数，如打开时钟，配置引脚等工作 */
    void     (*pfn_plfm_init)(void);

    /** \brief 平台解初始化函数 */
    void     (*pfn_plfm_deinit)(void);
} am_lpc82x_sct_pwm_devinfo_t;
 
/**
 * \brief SCT PWM输出功能设备结构体
 */
typedef struct am_lpc82x_sct_pwm_dev {

    /** \brief 标准PWM服务           */
    am_pwm_serv_t                      pwm_serv;
    
    /** \brief 指向SCT(PWM输出功能)设备信息常量的指针 */
    const am_lpc82x_sct_pwm_devinfo_t *p_devinfo;

} am_lpc82x_sct_pwm_dev_t;

/**
 * \brief 初始化SCT为PWM输出功能
 *
 * \param[in] p_dev     : 指向SCT(PWM输出功能)设备的指针
 * \param[in] p_devinfo : 指向SCT(PWM输出功能)设备信息常量的指针
 *
 * \return PWM标准服务操作句柄，值为NULL时表明初始化失败
 */
am_pwm_handle_t am_lpc82x_sct_pwm_init(am_lpc82x_sct_pwm_dev_t     *p_dev,
                                 const am_lpc82x_sct_pwm_devinfo_t *p_devinfo);

/**
 * \brief 不使用SCT PWM输出功能时，解初始化SCT PWM输出功能，释放相关资源
 * \param[in] handle :  am_lpc82x_sct_pwm_init() 初始化函数获得的PWM服务句柄
 * \return 无
 */
void am_lpc82x_sct_pwm_deinit(am_pwm_handle_t handle);

/** 
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_SCT_PWM_H */

/* end of file */
