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
 * \brief 窗口看门狗硬件层驱动函数
 * 
 * 1. 如果在可编程超时期限内未重新载入，则可在内部复位芯片。
 * 2. 可选的窗口化操作要求在最大和最小超时期限之间发生重新载入，两个时限都可设定。
 * 3. 可选警报中断可在看门狗超时之前的可编程时间生成。
 * 4. 带内部固定预分频器的可编程 24 位定时器。
 * 5. 时间周期可选，从 1,024 个看门狗时钟 (TWDCLK × 256 × 4) 到超过 6,700 万个看门狗
 *    时钟 (TWDCLK × 224 × 4)，取 4 个看门狗时钟的位数。
 * 6.“ 安全 ” 看门狗操作。一旦使能，则要求禁用软件复位或看门狗复位。
 * 7. 喂狗序列不正确会导致看门狗即时事件 （如使能）。
 * 8. 由于对看门狗重载值的保护为选择性，因此只能在 “ 警报中断 ” 时间后更改该值。
 * 9. 指示看门狗复位的标志。
 * 9. 看门狗时钟 (WDCLK) 源是看门狗振荡器。
 * 10. 看门狗定时器可配置为在深度睡眠模式或掉电模式下运行。
 * 11. 调试模式。
 *
 * \internal
 * \par Modification history
 * - 1.01 15-12-14  hgo, del amhw_lpc82x_wwdt_option_clr() .
 * - 1.00 15-07-07  sss, first implementation.
 * \endinternal
 */
#ifndef __AMHW_LPC82X_WWDT_H
#define __AMHW_LPC82X_WWDT_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup amhw_lpc82x_if_wwdt
 * \copydoc amhw_lpc82x_wwdt.h
 * @{
 */
 
/**
 * \brief WWDT寄存器块的结构体
 */
typedef struct amhw_lpc82x_wwdt {
    __IO uint32_t  mod;         /**< \brief 看门狗定时器的模式寄存器*/
    __IO uint32_t  tc;          /**< \brief 喂狗后装入计数器中的值*/
    __O  uint32_t  feed;        /**< \brief 喂狗序列寄存器(0xAA + 0x55)*/
    __I  uint32_t  tv;          /**< \brief 看门狗计数器值，低24位有效*/
    __I  uint32_t  reserved0;   /**< \brief 保留*/
    __IO uint32_t  warnint;     /**< \brief 看门狗中断比较器值*/
    __IO uint32_t  window;      /**< \brief 看门狗窗口比较器值*/
} amhw_lpc82x_wwdt_t;

/**
 * \name 看门狗模式功能位定义
 * \anchor grp_wwdt_mode
 * @{
 */

/** \brief 看门狗模式位掩码*/
#define AMHW_LPC82X_WWDT_WDMOD_BITMASK     ((uint32_t)0x3F)     

/** \brief 看门狗使能位*/
#define AMHW_LPC82X_WWDT_WDMOD_WDEN        AM_BIT(0)           

/** \brief 看门狗复位位*/
#define AMHW_LPC82X_WWDT_WDMOD_WDRESET     AM_BIT(1)            

/** \brief 看门狗超时标志位*/
#define AMHW_LPC82X_WWDT_WDMOD_WDTOF       AM_BIT(2)            

/** \brief 看门狗中断标志位*/
#define AMHW_LPC82X_WWDT_WDMOD_WDINT       AM_BIT(3)            

/** \brief 看门狗更新模式位*/
#define AMHW_LPC82X_WWDT_WDMOD_WDPROTECT   AM_BIT(4)       
     
/** \brief 看门狗锁定时钟源位*/
#define AMHW_LPC82X_WWDT_WDMOD_LOCK        AM_BIT(5)            

/** @} */


/**
 * \brief 初始化WWDT
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_wwdt_init (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    /* 禁止看门狗 */ 
    p_hw_wwdt->mod     = 0;
    p_hw_wwdt->tc      = 0xFF;
    p_hw_wwdt->warnint = 0x0;
    p_hw_wwdt->window  = 0xFFFFFF;
}

/**
 * \brief 设置看门狗的超时时间，准备喂狗
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 * \param[in] timeout   : 设置超时时间的值， 范围0xFF~0xFFFFFF
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_wwdt_timeout_set (amhw_lpc82x_wwdt_t *p_hw_wwdt,
                                   uint32_t            timeout)
{
    p_hw_wwdt->tc = timeout;
}

/**
 * \brief 看门狗喂狗函数
 * 
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 *
 * \return 无
 *
 * \note 避免出错，可以在喂狗操作前，关闭系统中断
 */
am_static_inline
void amhw_lpc82x_wwdt_feed (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    p_hw_wwdt->feed = 0xAA;
    p_hw_wwdt->feed = 0x55;
}

/**
 * \brief 设置看门狗报警的值
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 * \param[in] timeout   : 看门狗超时时间
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_wwdt_warning_set (amhw_lpc82x_wwdt_t *p_hw_wwdt,
                                   uint32_t            timeout)
{
    p_hw_wwdt->warnint = timeout;
}

/**
 * \brief 获取看门狗中断寄存器的值
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 *
 * \return 当前报警中断寄存器的值
 */
am_static_inline
uint32_t amhw_lpc82x_wwdt_warning_get (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    return p_hw_wwdt->warnint;
}

/**
 * \brief 设置看门狗窗口定时器的值
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 * \param[in] timeout   : 窗口定时器的值
 *
 * \return 无
 *
 * \note 计数器值大于window时，喂狗会引起一个看门狗事件
 */
am_static_inline
void amhw_lpc82x_wwdt_window_set (amhw_lpc82x_wwdt_t *p_hw_wwdt, 
                                  uint32_t            timeout)
{
    p_hw_wwdt->window = timeout;
}

/**
 * \brief 获取看门狗定时器窗口的值
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 *
 * \return 当前窗口寄存器的值
 */
am_static_inline
uint32_t amhw_lpc82x_wwdt_window_get (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    return p_hw_wwdt->window;
}

/**
 * \brief 使能看门狗定时器选项
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 * \param[in] options   : 使能看门狗的选项
 *                        #AMHW_LPC82X_WWDT_WDMOD_WDEN    或
 *                        #AMHW_LPC82X_WWDT_WDMOD_WDRESET 或
 *                        #AMHW_LPC82X_WWDT_WDMOD_WDPROTECT 或
 *                        #AMHW_LPC82X_WWDT_WDMOD_LOCK
 *                        这四个宏的或值
 * \return 无
 *
 * \note 使能一个或多个，可将标志位或操作
 */
am_static_inline
void amhw_lpc82x_wwdt_option_set (amhw_lpc82x_wwdt_t *p_hw_wwdt, 
                                  uint32_t            options)
{
    p_hw_wwdt->mod = (p_hw_wwdt->mod & AMHW_LPC82X_WWDT_WDMOD_BITMASK) | 
                      options;
}

/**
 * \brief 使能看门狗
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_wwdt_start (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    amhw_lpc82x_wwdt_option_set(p_hw_wwdt,AMHW_LPC82X_WWDT_WDMOD_WDEN);
    amhw_lpc82x_wwdt_feed(p_hw_wwdt);
}

/**
 * \brief 读取看门狗状态标志位
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 * \param[in] flag      : 标志位，#AMHW_LPC82X_WWDT_WDMOD_WDINT 或
 *                               #AMHW_LPC82X_WWDT_WDMOD_WDTOF
 *
 * \return 标志位
 */
am_static_inline
bool_t amhw_lpc82x_wwdt_status_get (amhw_lpc82x_wwdt_t *p_hw_wwdt, 
                                    uint32_t            flag)
{
    return (bool_t) (p_hw_wwdt->mod & flag);
}

/**
 * \brief 清除看门狗状态标志位
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 * \param[in] flags     : 标志位，#AMHW_LPC82X_WWDT_WDMOD_WDINT 或
 *                               #AMHW_LPC82X_WWDT_WDMOD_WDTOF
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_wwdt_status_clr (amhw_lpc82x_wwdt_t *p_hw_wwdt, 
                                  uint32_t            flags)
{
    if (flags & AMHW_LPC82X_WWDT_WDMOD_WDTOF) {
        p_hw_wwdt->mod &= (~AMHW_LPC82X_WWDT_WDMOD_WDTOF) &
                            AMHW_LPC82X_WWDT_WDMOD_BITMASK;
    }
    
    /* 写1，清除中断标志位 */
    if (flags & AMHW_LPC82X_WWDT_WDMOD_WDINT) {
        p_hw_wwdt->mod = (p_hw_wwdt->mod & AMHW_LPC82X_WWDT_WDMOD_BITMASK) |
                          AMHW_LPC82X_WWDT_WDMOD_WDINT;
    }
}

/**
 * \brief 获取看门狗计数器的值
 *
 * \param[in] p_hw_wwdt : 指向看门狗定时器寄存器块的指针
 *
 * \return 当前看门狗的计数器数值
 */
am_static_inline
uint32_t amhw_lpc82x_wwdt_currentcount_get (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    return p_hw_wwdt->tv;
}

/**
 * @} 
 */
 
#ifdef __cplusplus
}
#endif /* __AMHW_LPC82X_WWDT_H */

#endif

/*end of file */

