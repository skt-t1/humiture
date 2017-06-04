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
 * \brief 可嵌套向量中断控制器(NVIC) 硬件操作接口
 *
 * 1. 支持32个中断向量；
 * 2. 4个可编程优先级(带硬件优先级屏蔽功能)；
 * 3. 由ARM异常(SVCall和PendSV)生成软件中断；
 * 4. 支持NMI。
 *
 * \internal
 * \par Modification history
 * - 1.02 15-12-05  cyl, modify the namespace.
 * - 1.01 15-09-23  win, modify amhw_lpc82x_int_set_priority().
 * - 1.00 14-12-04  hbt, first implementation.
 * \endinternal
 */


#ifndef __AMHW_LPC82X_INT_H
#define __AMHW_LPC82X_INT_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * \addtogroup amhw_lpc82x_if_int
 * \copydoc amhw_lpc82x_int.h
 * @{
 */

/**
 * \brief NVIC 寄存器块结构体(兼容LPC5410x的NVIC寄存器块结构体定义)
 */
typedef struct amhw_lpc82x_int {
    __IO uint32_t  iser[2];                    /**< \brief 中断使能寄存器       */
    __I  uint32_t  reserved1[30];              /**< \brief 保留                 */
    __IO uint32_t  icer[2];                    /**< \brief 清除中断使能寄存器   */   
    __I  uint32_t  reserved2[30];              /**< \brief 保留                 */
    __IO uint32_t  ispr[2];                    /**< \brief 中断等待寄存器       */ 
    __I  uint32_t  reserved3[30];              /**< \brief 保留                 */
    __IO uint32_t  icpr[2];                    /**< \brief 清除中断等待寄存器   */  
    __I  uint32_t  reserved4[30];              /**< \brief 保留                 */
    __I  uint32_t  iabr[2];                    /**< \brief 中断有效寄存器       */ 
    __I  uint32_t  reserved5[62];              /**< \brief 保留                 */
    __IO uint32_t  ipr[11];                    /**< \brief 中断优先级寄存器     */
    __I  uint32_t  reserved6[566];             /**< \brief 保留                 */
    __I  uint32_t  icsr;                       /**< \brief 中断控制和状态寄存器 */
    __I  uint32_t  reserved7[126];             /**< \brief 保留                 */
    __O  uint32_t  stir;                       /**< \brief 软件触发中断寄存器   */
} amhw_lpc82x_int_t;

  
/**
 * \brief 获取当前中断号
 * \param[in] p_hw_int : 指向NVIC寄存器块的指针
 * \return 中断号
 */
am_static_inline 
int amhw_inum_get (amhw_lpc82x_int_t *p_hw_int)
{
    return (int)(AM_BITS_GET((p_hw_int->icsr - 16), 0, 8));
}

/**
 * \brief 使能中断
 *
 * \param[in] p_hw_int : 指向NVIC寄存器块的指针
 * \param[in] inum     : 中断号， 参见 
 *                       \sa lpc8xx_inum.h
 *
 * \return 无
 */
am_static_inline 
void amhw_lpc82x_int_enable (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    p_hw_int->iser[(uint32_t)(inum) >> 5] = AM_BIT(((uint32_t)(inum) & 0x1F));
}

/**
 * \brief 禁能中断
 *
 * \param[in] p_hw_int : 指向NVIC寄存器块的指针
 * \param[in] inum     : 中断号， 参见 
 *                       \sa lpc8xx_inum.h
 *
 * \return 无
 */
am_static_inline 
void amhw_lpc82x_int_disable (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    p_hw_int->icer[(uint32_t)(inum) >> 5] = AM_BIT(((uint32_t)(inum) & 0x1F));
}

/**
 * \brief 置位中断等待
 *
 * \param[in] p_hw_int : 指向NVIC寄存器块的指针
 * \param[in] inum     : 中断号， 参见 
 *                       \sa lpc8xx_inum.h
 *
 * \return    无
 */
am_static_inline 
void amhw_lpc82x_int_pending_set (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    p_hw_int->ispr[(uint32_t)(inum) >> 5] = AM_BIT(((uint32_t)(inum) & 0x1F));
}

/**
 * \brief 清除等待中断
 *
 * \param[in] p_hw_int : 指向NVIC寄存器块的指针
 * \param[in] inum     : 中断号， 参见 
 *                       \sa lpc8xx_inum.h
 *
 * \return    无
 */
am_static_inline 
void amhw_lpc82x_int_pending_clr (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    p_hw_int->icpr[(uint32_t)(inum) >> 5] = AM_BIT(((uint32_t)(inum) & 0x1F));
}

/**
 * \brief 获取中断有效状态
 *
 * \param[in] p_hw_int : 指向NVIC寄存器块的指针
 * \param[in] inum     : 中断号， 参见 
 *                       \sa lpc8xx_inum.h
 * 
 * \retval TRUE  : 有效中断
 * \retval FALSE : 无效中断
 */
am_static_inline 
bool_t amhw_lpc82x_int_active_state_get (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    return (AM_BIT_ISSET(p_hw_int->iabr[(uint32_t)(inum) >> 5], 
                         ((uint32_t)(inum) & 0x1F))) ? TRUE : FALSE;
}

/**
 * \brief 设置中断优先级
 *
 * \param[in] p_hw_int : 指向NVIC寄存器块的指针
 * \param[in] inum     : 中断号， 参见 
 *                       \sa lpc8xx_inum.h
 * \param[in] prio     : 中断优先级
 * 
 * \return   无
 */
am_static_inline 
void amhw_lpc82x_int_priority_set (amhw_lpc82x_int_t *p_hw_int,
                                   int                inum, 
                                   uint32_t           prio)
{
    /* num = inum / 4 */
    uint32_t num   = inum >> 2;
    /* value = (inum % 4) * 8 + 6 */
    uint32_t value = ((inum & 0x3) << 3) + 6;

    AM_BIT_CLR_MASK(p_hw_int->ipr[num], AM_SBF(0x3, value));
    p_hw_int->ipr[num] |= AM_SBF((prio & 0x3), value);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AMHW_LPC82X_INT_H */

/* end of file */

