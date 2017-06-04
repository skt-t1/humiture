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
 * \brief SYSTICK 驱动，服务 Timer 标准接口
 *
 * \note 使用 SYSTICK 服务的 Timer 标准接口，就不能使用标准延时接口
 *       am_mdelay()和am_udelay()
 * \internal
 * \par Modification History
 * - 1.02 15-12-14  hgo, fix potential bug caused by incorrect p_arg on callback
 * - 1.00 15-09-22  win, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "amhw_lpc82x_systick.h"
#include "am_lpc82x_systick.h"
#include "amhw_lpc82x_clk.h"
/*******************************************************************************
  函数声明
*******************************************************************************/

/** \brief  获取 SYSTICK 信息 */
static int __systick_info_get (void *p_drv, am_timer_info_t *p_info);

/** \brief 获取 SYSTICK 计数器的值 */
static int __systick_count_get (void     *p_drv,
                                uint8_t   chan,
                                uint32_t *p_count);

/** \brief 获取 SYSTICK roll over 的值 */
static int __systick_rollover_get (void     *p_drv,
                                   uint8_t   chan,
                                   uint32_t *p_rollover);

/** \brief SYSTICK  禁能 */
static int __systick_disable (void *p_drv, uint8_t chan);

/** \brief SYSTICK  使能 */
static int __systick_enable (void     *p_drv,
                             uint8_t   chan,
                             uint32_t  timer_count);

/** \brief SYSTICK  回调函数设置 */
static int __systick_callback_set (void    *p_drv,
                                   uint8_t  chan,
                                   void   (*pfn_callback)(void *),
                                   void    *p_arg);

/*******************************************************************************
  全局变量
*******************************************************************************/

/**
 * \brief SYSTICK 驱动函数
 */
static const struct am_timer_drv_funcs __g_systick_drv_funcs = {
    __systick_info_get,
    __systick_count_get,
    __systick_rollover_get,
    __systick_disable,
    __systick_enable,
    __systick_callback_set,
    NULL,
    NULL,
    NULL,
};

/**
 * \brief 考虑到 SYSTICK 中断处理函数无法传递参数，使用一个全局变量指针，
 *        保存 SYSTICK 设备结构体地址
 */
static am_lpc82x_systick_dev_t *p_systick_dev;

/**
 * \brief SYSTICK 中断处理函数
 *
 * \return 无
 */
void SysTick_Handler (void)
{
    if (p_systick_dev->pfn_callback != NULL) {
        p_systick_dev->pfn_callback(p_systick_dev->p_arg);
    }
}

/**
 * \brief  获取 SYSTICK 信息
 *
 * \param[in] p_drv  : 指向 SYSTICK 设备结构体的指针
 * \param[in] p_info : 指向 SYSTICK 设备信息结构体的指针
 *
 * \retval  AM_OK     : 获取信息成功
 * \retval -AM_EINVAL : 参数错误
 */
static int __systick_info_get (void *p_drv, am_timer_info_t *p_info)
{
    am_lpc82x_systick_dev_t *p_dev;

    if (p_drv == NULL || p_info == NULL) {
        return -AM_EINVAL;
    }

    p_dev = (am_lpc82x_systick_dev_t *)p_drv;

    /* 获取当前系统时钟频率，systick 使用的是系统时钟 */
    p_info->clk_frequency = amhw_lpc82x_clk_system_clkrate_get();

    if (p_dev->p_devinfo->clk_freq_src ==
                               AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM_HALF) {
        p_info->clk_frequency = p_info->clk_frequency / 2;
    }

    if (p_info->clk_frequency <= AMHW_LPC82X_SYSTICK_INVAL_MAX) {
        p_info->min_frequency = 1;
    } else {
        p_info->min_frequency = p_info->clk_frequency /
                                                  AMHW_LPC82X_SYSTICK_INVAL_MAX;

        if (p_info->min_frequency * AMHW_LPC82X_SYSTICK_INVAL_MAX
                                  < p_info->clk_frequency) {
            p_info->min_frequency += 1;
        }
    }

    p_info->max_frequency = p_info->clk_frequency;
    p_info->features      = AM_TIMER_SIZE(24)           | /* 24位定时器             */
                            AM_TIMER_CHAN_NUM(1)        | /* 定时器通道数           */
                            AM_TIMER_CAN_INTERRUPT      | /* 可以产生中断           */
                            AM_TIMER_AUTO_RELOAD        | /* 支持自动重载           */
                            AM_TIMER_INTERMEDIATE_COUNT;  /* 当前计数器值可读  */

    return AM_OK;
}

/**
 * \brief  获取 SYSTICK 定时器计数值
 *
 * \param[in] p_drv   : 指向 SYSTICK 设备结构体的指针
 * \param[in] chan    : SYSTICK 通道号，SYSTICK 只能使用通道0
 * \param[in] p_count : 指向存放定时器计数值的指针
 *
 * \retval  AM_OK     : 获取信息成功
 * \retval -AM_EINVAL : 参数错误
 */
static int __systick_count_get (void *p_drv, uint8_t chan, uint32_t *p_count)
{
    am_lpc82x_systick_dev_t *p_dev        = NULL;
    amhw_lpc82x_systick_t   *p_hw_systick = NULL;
    uint32_t                 tmp;

    if (p_drv == NULL || p_count == NULL || chan != 0) {
        return -AM_EINVAL;
    }

    p_dev        = (am_lpc82x_systick_dev_t *)p_drv;
    p_hw_systick = (amhw_lpc82x_systick_t *)p_dev->p_devinfo->systick_regbase;

    tmp          = amhw_lpc82x_systick_val_get(p_hw_systick);
    *p_count     = amhw_lpc82x_systick_reload_val_get(p_hw_systick) - tmp;

    return AM_OK;
}

/**
 * \brief 获取 SYSTICK roll over 的值
 *
 * \param[in] p_drv      : 指向 SYSTICK 设备结构体的指针
 * \param[in] chan       : SYSTICK 通道号，SYSTICK 只能使用通道0
 * \param[in] p_rollover : 指向存放翻转值的指针
 *
 * \retval  AM_OK     : 获取信息成功
 * \retval -AM_EINVAL : 参数错误
 */
static int __systick_rollover_get (void     *p_drv,
                                   uint8_t   chan,
                                   uint32_t *p_rollover)
{
    am_lpc82x_systick_dev_t *p_dev;

    if (p_drv == NULL || p_rollover == NULL || chan != 0) {
        return -AM_EINVAL;
    }

    p_dev = (am_lpc82x_systick_dev_t *)p_drv;

    /* 24位最大值为 AMHW_LPC82X_SYSTICK_INVAL_MAX */
    if (p_dev->p_devinfo->clk_freq_src ==
                                AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM_HALF) {

        *p_rollover = AMHW_LPC82X_SYSTICK_INVAL_MAX  /
                                  (amhw_lpc82x_clk_system_clkrate_get() / 2);
    } else {
        *p_rollover = AMHW_LPC82X_SYSTICK_INVAL_MAX  /
                                   amhw_lpc82x_clk_system_clkrate_get();
    }

    return AM_OK;
}

/**
 * \brief SYSTICK 禁能
 *
 * \param[in] p_drv : 指向 SYSTICK 设备结构体的指针
 * \param[in] chan  : SYSTICK 通道号，SYSTICK 只能使用通道0
 *
 * \retval  AM_OK     : 获取信息成功
 * \retval -AM_EINVAL : 参数错误
 */
static int __systick_disable (void *p_drv, uint8_t chan)
{
    am_lpc82x_systick_dev_t *p_dev        = NULL;
    amhw_lpc82x_systick_t   *p_hw_systick = NULL;

    if (p_drv == NULL || chan != 0) {
        return -AM_EINVAL;
    }

    p_dev        = (am_lpc82x_systick_dev_t *)p_drv;
    p_hw_systick = (amhw_lpc82x_systick_t   *)p_dev->p_devinfo->systick_regbase;

    amhw_lpc82x_systick_int_disable(p_hw_systick);
    amhw_lpc82x_systick_disable(p_hw_systick);

    return AM_OK;
}

/**
 * \brief SYSTICK 使能
 *
 * \param[in] p_drv       : 指向 SYSTICK 设备结构体的指针
 * \param[in] chan        : SYSTICK 通道号，SYSTICK 只能使用通道0
 * \param[in] timer_count : 需要设置的定时器计数值
 *
 * \retval  AM_OK     : 获取信息成功
 * \retval -AM_EINVAL : 参数错误
 */
static int __systick_enable (void     *p_drv,
                             uint8_t   chan,
                             uint32_t  timer_count)
{
    am_lpc82x_systick_dev_t *p_dev        = NULL;
    amhw_lpc82x_systick_t   *p_hw_systick = NULL;

    if (p_drv == NULL || chan != 0) {
        return -AM_EINVAL;
    }

    p_dev        = (am_lpc82x_systick_dev_t *)p_drv;
    p_hw_systick = (amhw_lpc82x_systick_t   *)p_dev->p_devinfo->systick_regbase;

    /* 设置重载定时寄存器中的数值 */
    amhw_lpc82x_systick_reload_val_set(p_hw_systick, timer_count);

    /* 清除当前定时计数器中的数值 */
    amhw_lpc82x_systick_val_set(p_hw_systick, 0x0);

    if (p_dev->pfn_callback == NULL) {
        amhw_lpc82x_systick_int_disable(p_hw_systick);
    } else {
        amhw_lpc82x_systick_int_enable(p_hw_systick);
    }

    /* 使能 SYSTICK */
    amhw_lpc82x_systick_enable(p_hw_systick);

    return AM_OK;
}

/**
 * \brief SYSTICK 回调函数设置
 *
 * \param[in] p_drv        : 指向 SYSTICK 设备结构体的指针
 * \param[in] chan         : SYSTICK 通道号，SYSTICK 只能使用通道0
 * \param[in] pfn_callback : 回调函数指针
 * \param[in] p_arg        : 回调函数入口参数
 *
 * \retval  AM_OK     : 获取信息成功
 * \retval -AM_EINVAL : 参数错误
 */
static int __systick_callback_set (void    *p_drv,
                                   uint8_t  chan,
                                   void   (*pfn_callback)(void *),
                                   void    *p_arg)
{
    am_lpc82x_systick_dev_t *p_dev        = NULL;
    amhw_lpc82x_systick_t   *p_hw_systick = NULL;

    if (p_drv == NULL || chan != 0) {
        return -AM_EINVAL;
    }

    p_dev        = (am_lpc82x_systick_dev_t *)p_drv;
    p_hw_systick = (amhw_lpc82x_systick_t   *)p_dev->p_devinfo->systick_regbase;

    if (pfn_callback == NULL) {
        amhw_lpc82x_systick_int_disable(p_hw_systick);
    } else {
        p_dev->p_arg        = p_arg;
        p_dev->pfn_callback = pfn_callback;
        
        amhw_lpc82x_systick_int_enable(p_hw_systick);
    }

    return AM_OK;
}

/**
 * \brief 初始化 SYSTICK 为定时功能
 */
am_timer_handle_t 
am_lpc82x_systick_init (am_lpc82x_systick_dev_t           *p_dev,
                        const am_lpc82x_systick_devinfo_t *p_devinfo)
{
    amhw_lpc82x_systick_t *p_hw_systick = NULL;

    if (p_dev == NULL || p_devinfo == NULL) {
        return NULL;
    }

    p_hw_systick              = (amhw_lpc82x_systick_t *)p_devinfo->systick_regbase;
    p_dev->p_devinfo          = p_devinfo;
    p_dev->timer_serv.p_funcs = (struct am_timer_drv_funcs *)&__g_systick_drv_funcs;
    p_dev->timer_serv.p_drv   = p_dev;
    p_dev->pfn_callback       = NULL;
    p_dev->p_arg              = NULL;

    p_systick_dev             = p_dev;

    /* 设置定时器时钟频率 */
    if ((p_devinfo->clk_freq_src != AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM) &&
        (p_devinfo->clk_freq_src !=
                               AMHW_LPC82X_SYSTICK_CONFIG_CLKSRC_SYSTEM_HALF)) {
        return NULL;
    } else {
        amhw_lpc82x_systick_config(p_hw_systick, p_devinfo->clk_freq_src);
    }

    return &(p_dev->timer_serv);
}

/**
 * \brief 不使用 SYSTICK 时，解初始化 SYSTICK，释放相关资源
 */
void am_lpc82x_systick_deinit (am_timer_handle_t handle)
{
    am_lpc82x_systick_dev_t *p_dev        = NULL;
    amhw_lpc82x_systick_t   *p_hw_systick = NULL;

    if (handle == NULL) {
        return ;
    }

    p_dev = (am_lpc82x_systick_dev_t *)handle;
    
    p_hw_systick = (amhw_lpc82x_systick_t *)p_dev->p_devinfo->systick_regbase;
    p_dev->timer_serv.p_funcs = NULL;
    p_dev->timer_serv.p_drv   = NULL;
    p_dev->pfn_callback       = NULL;
    p_dev->p_arg              = NULL;

    amhw_lpc82x_systick_int_disable(p_hw_systick);
    amhw_lpc82x_systick_disable(p_hw_systick);
}
