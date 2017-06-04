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
 * \brief CPU 系统滴答定时器(SYSTICK) 操作接口
 *
 * 1. 简单的24位递减定时器；
 * 2. 使用专用的异常向量号。
 *
 * \internal
 * \par Modification history
 * - 1.00 15-09-22  win, add function amhw_lpc82x_systick_reload_val_get().
 * - 1.00 14-12-01  tee, first implementation.
 * \endinternal
 */

#ifndef __AMHW_LPC82X_SYSTICK_H
#define __AMHW_LPC82X_SYSTICK_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup amhw_lpc82x_if_systick
 * \copydoc amhw_lpc82x_systick.h
 * @{
 */

/**
 * \brief SYSTICK 寄存器块结构体
 */
typedef struct amhw_lpc82x_systick {
    __IO uint32_t ctrl;       /**< \brief SysTick 控制和状态寄存器       */
    __IO uint32_t load;       /**< \brief SysTick 重装载值寄存器         */
    __IO uint32_t val;        /**< \brief SysTick 当前定时器值寄存器     */
    __I  uint32_t calib;      /**< \brief SysTick 校准寄存器             */
} amhw_lpc82x_systick_t;


/**
 * \name Systick控制寄存器的宏定义
 * @{
 */

#define AMHW_LPC82X_SYSTICK_INVAL_MAX                     (0xFFFFFFUL)

/** \brief Systick使能             */
#define AMHW_LPC82X_SYSTICK_CONFIG_ENABLE                 (1 << 0)

/** \brief Systick中断使能         */
#define AMHW_LPC82X_SYSTICK_CONFIG_TICKINT                (1 << 1)

/** \brief Systick时钟源为系统时钟 */
#define AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM          (1 << 2)

/** \brief Systick时钟源的为系统时钟的1/2 */
#define AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM_HALF     (0 << 2)

/** @} */

/**
 * \brief 使能Systick（开始向下计数）
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \return 无
 */
am_static_inline
void amhw_lpc82x_systick_enable (amhw_lpc82x_systick_t *p_hw_systick)
{
    p_hw_systick->ctrl |= AMHW_LPC82X_SYSTICK_CONFIG_ENABLE;
}

/**
 * \brief 禁能Systick（停止向下计数）
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \return 无
 */
am_static_inline
void amhw_lpc82x_systick_disable (amhw_lpc82x_systick_t *p_hw_systick)
{
    p_hw_systick->ctrl &= ~AMHW_LPC82X_SYSTICK_CONFIG_ENABLE;
}

/**
 * \brief 使能Systick中断
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \return 无
 */
am_static_inline
void amhw_lpc82x_systick_int_enable (amhw_lpc82x_systick_t *p_hw_systick)
{
    p_hw_systick->ctrl |= AMHW_LPC82X_SYSTICK_CONFIG_TICKINT;
}

/**
 * \brief 禁能Systick中断
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \return 无
 */
am_static_inline
void amhw_lpc82x_systick_int_disable (amhw_lpc82x_systick_t *p_hw_systick)
{
    p_hw_systick->ctrl &= ~AMHW_LPC82X_SYSTICK_CONFIG_TICKINT;
}

/**
 * \brief 检测Systick计数值是否递减至0
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \retval TRUE  : Systick计数值递减至0
 * \retval FALSE : Systick计数值未递减至0
 */
am_static_inline
bool_t amhw_lpc82x_systick_flag_check (amhw_lpc82x_systick_t *p_hw_systick)
{
    return (bool_t)((p_hw_systick->ctrl & (1 << 16)) != 0);
}

/**
 * \brief 配置Systick
 *
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \param[in] flags        : 可以是下列单个宏值或下列多个宏的或值
 *                            - AMHW_LPC82X_SYSTICK_CONFIG_ENABLE
 *                            - AMHW_LPC82X_SYSTICK_CONFIG_TICKINT
 *                            - AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM
 *                            - AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_MAINCLK_BY_DIV
 * \return 无
 */
am_static_inline
void amhw_lpc82x_systick_config (amhw_lpc82x_systick_t *p_hw_systick,
                                 uint32_t               flags)
{
    p_hw_systick->ctrl = flags;
}

/**
 * \brief 设置Systick的自动重装载值
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \param[in] value        : 自动重装载值(最大为0xFFFFFF)
 * \return 无
 */
am_static_inline
void amhw_lpc82x_systick_reload_val_set (amhw_lpc82x_systick_t *p_hw_systick,
                                         uint32_t               value)
{
    p_hw_systick->load = value & 0xFFFFFF;
}

/**
 * \brief 获取Systick的自动重装载值
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \return 自动装载值
 */
am_static_inline
uint32_t amhw_lpc82x_systick_reload_val_get (amhw_lpc82x_systick_t *p_hw_systick)
{
    return ((p_hw_systick->load) & 0xFFFFFF);
}

/**
 * \brief 设置Systick当前的计数值
 *
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \param[in] value        : Systick当前的计数值
 *
 * \return 无
 *
 * \note 写任何值都将清零Systick当前的计数值，并清除控制寄存器中的COUNTFLAG标志。
 */
am_static_inline
void amhw_lpc82x_systick_val_set (amhw_lpc82x_systick_t *p_hw_systick,
                                  uint32_t               value)
{
    p_hw_systick->val = value & 0xFFFFFF;
}

/**
 * \brief 获取Systick当前的计数值
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \return Systick当前的计数值
 */
am_static_inline
uint32_t amhw_lpc82x_systick_val_get (amhw_lpc82x_systick_t *p_hw_systick)
{
    return (p_hw_systick->val & 0xFFFFFF);
}

/**
 * \brief 获取Systick的校准值
 * \param[in] p_hw_systick : 指向Systick寄存器块的指针
 * \return Systick的校准值
 */
am_static_inline
uint32_t amhw_lpc82x_systick_calib_val_get (amhw_lpc82x_systick_t *p_hw_systick)
{
    return p_hw_systick->calib;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /*__AMHW_LPC82X_SYSTICK_H */

/*end of file */
