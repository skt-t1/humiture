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
 * \brief LPC82X 中断用户配置文件
 * \sa am_lpc82x_hwconfig_int.c
 * 
 * \internal
 * \par Modification history
 * - 1.01 15-12-05  cyl, modify the namespace.
 * - 1.00 15-01-29  hbt, first implementation.
 * \endinternal
 */
 
#include "ametal.h"
#include "am_lpc82x.h"
#include "am_lpc82x_int.h"

/**
 * \addtogroup am_lpc82x_if_hwconfig_src_int
 * \copydoc am_lpc82x_hwconfig_int.c
 * @{
 */

/** 
 * \brief ISR 信息数量 
 *        该数量默认定义为MCU所支持的最大向量中断个数，
 *        用户可以根据应用中实际所需要用到的中断个数
 *        来修改该宏值，以达到减少不必要的内存浪费的目的。
 */
#define __ISRINFO_COUNT    INUM_INTERNAL_COUNT

/**
 * \brief 存储用户中断回调信息
 */
static struct am_lpc82x_int_isr_info __int_isr_infor[__ISRINFO_COUNT];

/**
 * \brief 存储中断编号与其对应在__int_isr_infor中断回调信息中
 *        位置的映射，数组大小一般与MCU所支持的最大向量中断个数相等。
 */
static uint8_t __int_isr_map[INUM_INTERNAL_COUNT];
 

/** \brief 中断设备信息 */
static const am_lpc82x_int_devinfo_t __g_int_devinfo =
{
    {
        INUM_INTERNAL_MIN, /**< \brief 中断起始号 */
        INUM_INTERNAL_MAX  /**< \brief 中断末尾号 */
    },                    
                          
    LPC82X_NVIC_BASE,      /**< \brief 中断控制器寄存器块基址                     */
    INUM_INTERNAL_COUNT,   /**< \brief 总中断数量                                */
    __int_isr_map,         /**< \brief ISR 信息映射(大小与 input_cnt 一致)        */
    __ISRINFO_COUNT,       /**< \brief ISR 信息数量                              */
    __int_isr_infor,       /**< \brief ISR 信息映射内存(大小与 isrinfo_cnt 一致)  */
    
    NULL,                  /**< \brief 无需平台初始化 */
    NULL                   /**< \brief 无需平台去初始化 */
};

/** \brief 中断设备实例 */
static am_lpc82x_int_dev_t __g_int_dev;


/**
 * \brief  中断实例初始化，初始化中断驱动
 */
int am_lpc82x_int_inst_init (void)
{
    return am_lpc82x_int_init(&__g_int_dev, &__g_int_devinfo);
}

/** 
 * \brief  中断实例解初始化
 */
void am_lpc82x_int_inst_deinit (void)
{
    am_lpc82x_int_deinit();
}

/**
 * @}
 */

/* end of file */
