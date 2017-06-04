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
 * \brief MRT(Multi-Rate timer) 硬件操作接口
 *
 * 1. 31位中断定时器；
 * 2. 4个独立的向下计数通道，可分别设置独立的值；
 * 3. 每个通道支持重复中断模式、一次计时模式和一次计时总线停滞模式。
 *
 * \internal
 * \par Modification history
 * - 1.02 15-12-14  hgo, improve exec efficiency.
 * - 1.01 15-12-02  mem, modified.
 * - 1.00 15-07-01  win, first implementation.
 * \endinternal
 */
 
#ifndef __AMHW_LPC82X_MRT_H
#define __AMHW_LPC82X_MRT_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup amhw_lpc82x_if_mrt
 * \copydoc amhw_lpc82x_mrt.h
 * @{
 */

/**
 * \brief MRT 通道数
 */
#define AMHW_LPC82X_MRT_CHAN_NUM          4
    
/** \brief 通道最大计时值 */
#define AMHW_LPC82X_MRT_INVAL_MAX     (0x7FFFFFFFUL)

/**
 * \brief MRT 通道寄存器块结构
 */
typedef struct amhw_lpc82x_mrt_chan {
    __IO uint32_t intval;                /**< \brief 间隔时间设置寄存器   */
    __I  uint32_t timer;                 /**< \brief 当前通道计数值寄存器 */
    __IO uint32_t ctrl;                  /**< \brief 当前通道控制寄存器   */
    __IO uint32_t stat;                  /**< \brief 当前通道状态寄存器   */
} amhw_lpc82x_mrt_chan_t;

/**
 * \brief MRT 寄存器块结构体
 */
typedef struct amhw_lpc82x_mrt {
    
    /** \brief 通道寄存器组   */
    amhw_lpc82x_mrt_chan_t channel[AMHW_LPC82X_MRT_CHAN_NUM];
    
    /** \brief 保留，不使用   */
    __I  uint32_t   reserved0[45];
    
    /** \brief 通道空闲寄存器 */
    __I  uint32_t   idle_ch;
    
    /** \brief 中断标志寄存器 */    
    __IO uint32_t   irq_flag;     
    
} amhw_lpc82x_mrt_t;

/**
 * \name MRT 通道编号
 * \anchor grp_amhw_lpc82x_mrt_chan
 * @{
 */

#define AMHW_LPC82X_MRT_CHAN_0     0     /**< \brief 通道0                */ 
#define AMHW_LPC82X_MRT_CHAN_1     1     /**< \brief 通道1                */ 
#define AMHW_LPC82X_MRT_CHAN_2     2     /**< \brief 通道2                */ 
#define AMHW_LPC82X_MRT_CHAN_3     3     /**< \brief 通道3                */ 

/** @} */

/**
 * \brief MRT 通道模式定义
 */
typedef enum amhw_lpc82x_mrt_mode{
    AMHW_LPC82X_MRT_MODE_REPEAT = 0,     /**< \brief 重复定时中断模式     */
    AMHW_LPC82X_MRT_MODE_ONESHOT,        /**< \brief 一次定时中断模式     */
    AMHW_LPC82X_MRT_MODE_ONESHOT_STALL   /**< \brief 一次定时总线停滞模式 */
} amhw_lpc82x_mrt_mode_t;                            

/**
 * \brief 设置指定通道的定时值
 * 
 *  该值会被转载进timer(当前通道计数值值寄存器)中，通道将会从 
 *  设置值 - 1 开始递减计数 
 * 
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 * \param[in] value    : 定时值(低31位有效)
 * \param[in] forced   : 是否立即载入定时值
 *      \arg TRUE      : 立即载入定时值
 *      \arg FALSE     : 下个定时器周期再载入定时值
 *
 * \note 以下情况下，无论参数forced为 TRUE 或者 FALSE，定时器均立即载入新的定时值：
 *       - 当通道用于 #AMHW_LPC82X_MRT_MODE_ONESHOT_STALL 模式时，参数forced无效；
 *       - 当通道处于空闲状态下时，参数forced无效。
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_mrt_intval_set (amhw_lpc82x_mrt_t *p_hw_mrt,
                                 uint8_t            chan,
                                 uint32_t           value,
                                 bool_t             forced)
{
    value &= AMHW_LPC82X_MRT_INVAL_MAX;
    value = (forced) ? (value | AM_BIT(31)) : value;
    p_hw_mrt->channel[chan].intval = value;
}

/**
 * \brief 获取指定通道的定时值
 *
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return 通道的定时值(INTVAL)
 */
am_static_inline
uint32_t amhw_lpc82x_mrt_intval_get (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    return (p_hw_mrt->channel[chan].intval & AMHW_LPC82X_MRT_INVAL_MAX);
}

/**
 * \brief 获取指定通道当前计数器的值
 * 
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return 通道当前计数器的值
 */
am_static_inline
uint32_t amhw_lpc82x_mrt_count_get (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    return p_hw_mrt->channel[chan].timer;
}

/**
 * \brief 使能指定通道的中断
 *
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_mrt_int_enable (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    AM_BIT_SET(p_hw_mrt->channel[chan].ctrl, 0);
}

/**
 * \brief 禁能指定通道的中断
 * 
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_mrt_int_disable (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    AM_BIT_CLR(p_hw_mrt->channel[chan].ctrl, 0);
}
    
/**
 * \brief 设置指定通道的定时模式
 * 
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 * \param[in] mode     : 定时器通道模式
 * 
 * \return 无
 */
am_static_inline
void amhw_lpc82x_mrt_mode_set (amhw_lpc82x_mrt_t     *p_hw_mrt, 
                               uint8_t                chan,
                               amhw_lpc82x_mrt_mode_t mode)
{
    AM_BITS_SET(p_hw_mrt->channel[chan].ctrl, 1, 2, mode);
}

/**
 * \brief 获取指定通道的定时模式
 *
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return 通道定时模式
 */
am_static_inline
amhw_lpc82x_mrt_mode_t amhw_lpc82x_mrt_mode_get (amhw_lpc82x_mrt_t *p_hw_mrt, 
                                                 uint8_t chan)
{
    return (amhw_lpc82x_mrt_mode_t)AM_BITS_GET(p_hw_mrt->channel[chan].ctrl, 
                                               1, 
                                               2);
}

/**
 * \brief 判断指定通道的中断是否挂起
 *
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 * 
 * \retval TRUE  : 中断挂起
 * \retval FALSE : 中断未挂起
 */
am_static_inline
bool_t amhw_lpc82x_mrt_is_pending (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    return (AM_BIT_ISSET(p_hw_mrt->channel[chan].stat, 0) ? TRUE : FALSE);
}

/**
 * \brief 消除指定通道的中断挂起状态
 * 
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道编号，AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_mrt_int_pending_clr (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    p_hw_mrt->channel[chan].stat = AM_BIT(0);
}

/**
 * \brief 判断指定通道是否处于空闲状态
 *
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] chan     : 通道ID，使用宏#AMHW_LPC82X_MRT_CH(n)
 *
 * \retval TRUE  : 空闲状态
 * \retval FALSE : 运行状态
 */
am_static_inline
bool_t amhw_lpc82x_mrt_is_idle (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    return (AM_BIT_ISSET(p_hw_mrt->channel[chan].stat, 1) ? FALSE : TRUE);
}

/**
 * \brief 获取当前空闲通道中，通道号最小的通道
 *
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 *
 * \return 最小空闲通道号
 */
am_static_inline
uint8_t amhw_lpc82x_mrt_idle_channel_get (amhw_lpc82x_mrt_t *p_hw_mrt)
{
    return (uint8_t)((p_hw_mrt->idle_ch) >> 4);
}

/**
 * \brief 清除通道的中断挂起状态
 *
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \param[in] masks    : 通道掩码，bit-n对应通道n，为1的位中断挂起状态被清除
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_mrt_int_pending_maks_clr (amhw_lpc82x_mrt_t *p_hw_mrt, 
                                           uint8_t            masks)
{
    p_hw_mrt->irq_flag = masks;
}

/**
 * \brief 获取所有通道的中断状态
 * \param[in] p_hw_mrt : 指向MRT寄存器块的指针
 * \return 中断挂起状态，bit-n对应通道n
 */
am_static_inline
uint8_t amhw_lpc82x_mrt_int_pending_get (amhw_lpc82x_mrt_t *p_hw_mrt)
{
    return p_hw_mrt->irq_flag & 0x0F;
}

/**
 * @} amhw_lpc82x_if_mrt
 */
 
#ifdef __cplusplus
}
#endif

#endif  /* __AMHW_LPC82X_MRT_H */

/* end of file */
