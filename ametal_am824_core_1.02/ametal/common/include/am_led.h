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
 * \brief  LED标准接口
 * 
 * \internal
 * \par Modification history
 * - 1.01 14-12-01  tee, add the LED to the standard interface
 * - 1.00 14-11-25  fft, first implementation.
 * \endinternal
 */

#ifndef __AM_LED_H
#define __AM_LED_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "am_common.h"


/**
 * \addtogroup am_if_led
 * \copydoc am_led.h
 * @{
 */


/**
 * \brief LED信息
 */
typedef struct am_led_info {
    uint32_t pin;          /**< \brief LED占用的GPIO引脚     */
    bool_t   active_low;   /**< \brief LED亮时引脚是否低电平 */
} am_led_info_t;

/**
 * \brief LED初始化
 * \param[in] p_led_info : 指向LED信息的指针
 * \param[in] led_num    : LED数量
 * \return 无
 */
void am_led_init(const am_led_info_t *p_led_info, const uint32_t led_num);
    
/**
 * \brief 设置板级LED的状态
 *
 * \param[in] led_id : LED的ID
 * \param[in] state  : TRUE : LED亮
 *                     FALSE: LED灭
 *
 * \return 无
 */
void am_led_set(uint8_t led_id, bool_t state);

/**
 * \brief 点亮LED
 * \param[in] led_id : LED的ID
 * \return 无
 */
void am_led_on(uint8_t led_id);

/**
 * \brief 熄灭LED
 * \param[in] led_id : LED的ID
 * \return 无
 */
void am_led_off(uint8_t led_id);

/**
 * \brief 获取指定LED的状态
 * \param[in] led_id : LED的ID
 * 
 * \retval TRUE  : LED亮
 * \retval FALSE : LED灭
 */
bool_t am_led_get(uint8_t led_id);

/**
 * \brief 翻转指定LED的状态
 * \param[in] led_id : LED的ID
 * \return 无
 */
void am_led_toggle(uint8_t led_id);


/** 
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LED_H */

/* end of file */
