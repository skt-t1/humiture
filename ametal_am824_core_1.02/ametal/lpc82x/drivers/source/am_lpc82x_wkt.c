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
 * \brief WKT驱动，服务Timer标准接口
 *
 * 该驱动实现了WKT的定时功能
 *
 * \internal
 * \par Modification History
 * - 1.01 15-12-02  sky, modified.
 * - 1.00 15-09-22  zxl, first implementation.
 * \endinternal
 */

/*******************************************************************************
includes
*******************************************************************************/

#include "am_int.h"
#include "am_lpc82x_wkt.h"
#include "amhw_lpc82x_clk.h"
#include "amhw_lpc82x_pmu.h"

/*******************************************************************************
函数声明
*******************************************************************************/

/** \brief 获取WKT信息 */
static int __wkt_info_get (void *p_drv, am_timer_info_t *p_info);

/** \brief 获取 WKT 计数器的值 */
static int __wkt_count_get (void *p_drv, uint8_t chan, uint32_t *p_count);

/** \brief 获取WKT roll over 的值 */
static int __wkt_rollover_get (void *p_drv, uint8_t chan, uint32_t *p_rollover);

/** \brief WKT 禁能 */
static int __wkt_disable (void *p_drv, uint8_t chan);

/** \brief WKT 使能 */
static int __wkt_enable (void *p_drv, uint8_t chan, uint32_t max_timer_count);

/** \brief WKT 回调函数设置 */
static int __wkt_callback_set (void    *p_drv,
                               uint8_t  chan,
                               void    (*pfn_callback)(void *),
                               void    *p_arg);

/** \brief WKT 连接函数 */
static int __wkt_connect (void *p_drv);

/*******************************************************************************
  全局变量
*******************************************************************************/

uint32_t __g_count_set;     /**< \brief 设定的WKT计数器的值 */

/** \brief WKT 驱动函数 */
static const struct am_timer_drv_funcs __g_wkt_drv_funcs = {
    __wkt_info_get,
    __wkt_count_get,
    __wkt_rollover_get,
    __wkt_disable,
    __wkt_enable,
    __wkt_callback_set,
    NULL,
    NULL,
    NULL,
};

/******************************************************************************/

/**
 * \brief WKT 中断处理函数
 * \param[in] p_arg : 中断函数入口参数
 * \return 无
 */
void __wkt_irq_handler (void *p_arg)
{
    am_lpc82x_wkt_dev_t *p_dev    = (am_lpc82x_wkt_dev_t *)p_arg;
    amhw_lpc82x_wkt_t *p_hw_wkt   = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);

    if (amhw_lpc82x_wkt_alarmflag_get(p_hw_wkt)) {
        amhw_lpc82x_wkt_alarmflag_clear(p_hw_wkt);    /* 清除中断标志 */

        if (p_dev->callback_info.pfn_callback != NULL) {
            p_dev->callback_info.pfn_callback(p_dev->callback_info.p_arg);
        }
    }
}

/**
 * \brief 获取 WKT 信息
 *
 * \param[in] p_drv  : 指向WKT设备结构体的指针
 * \param[in] p_info : 指向定时器结构体的指针
 *
 * \retval AM_OK : 成功
 */
static int __wkt_info_get (void *p_drv, am_timer_info_t *p_info)
{
    am_lpc82x_wkt_dev_t *p_dev        = (am_lpc82x_wkt_dev_t *)p_drv;
    amhw_lpc82x_wkt_t   *p_hw_wkt     = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);
    uint8_t              channel_nums = p_dev->p_devinfo->channel_nums;

    /* 判断WKT当前使用的时钟源 */
    if(amhw_lpc82x_wkt_clksel_get(p_hw_wkt) == 1) {         /* 低功耗时钟 10kHz*/
        p_info->clk_frequency = AMHW_LPC82X_WKT_LOW_POWER_CLOCK_FREQ;
    } else if (amhw_lpc82x_wkt_clksel_get(p_hw_wkt) == 0) { /* IRC时钟 750kHz*/
        p_info->clk_frequency = AMHW_LPC82X_WKT_IRC_CLOCK_FREQ;
    } else {
        p_info->clk_frequency = p_dev->p_devinfo->wkt_ext_clock_freq;
    }

    if (p_info->clk_frequency <= AMHW_LPC82X_WKT_INVAL_MAX) {
        p_info->min_frequency = 1;
    } else {
        p_info->min_frequency = p_info->clk_frequency / AMHW_LPC82X_WKT_INVAL_MAX;

        /* p_info->clk_frequency / AMHW_WKT_INVAL_MAX, 取不小于它的最小整数 */
        if (p_info->min_frequency * AMHW_LPC82X_WKT_INVAL_MAX
                                      < p_info->clk_frequency) {
            p_info->min_frequency += 1;
        }
    }

    p_info->max_frequency = p_info->clk_frequency;

    p_info->features = AM_TIMER_SIZE(32)               | /* 32位定时器 */
                       AM_TIMER_CHAN_NUM(channel_nums) | /* 支持1通道 */
                       AM_TIMER_CAN_INTERRUPT          | /* 可以产生中断 */
                       AM_TIMER_INTERMEDIATE_COUNT     | /* 可以读当前计数值 */
                       AM_TIMER_CANNOT_DISABLE;          /* 定时器不能被停止 */

    return AM_OK;
}

/**
 * \brief 获取 WKT 计数器的值
 *
 * \param[in] p_drv   : 指向 WKT 设备结构体的指针
 * \param[in] chan    : 通道号
 * \param[in] p_count : 指向保存计数器的值的指针
 *
 * \retval  AM_OK     : 获取成功
 * \retval -AM_EINVAL : 参数非法
 */
static int __wkt_count_get (void *p_drv, uint8_t chan, uint32_t *p_count)
{
    am_lpc82x_wkt_dev_t *p_dev        = (am_lpc82x_wkt_dev_t *)p_drv;
    amhw_lpc82x_wkt_t   *p_hw_wkt     = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);
    uint8_t              channel_nums = p_dev->p_devinfo->channel_nums;
    uint32_t             tmp;

    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }

    /* 手册不建议读，如果必须读，连续读2次 */
    tmp = amhw_lpc82x_wkt_count_get(p_hw_wkt);
    tmp = amhw_lpc82x_wkt_count_get(p_hw_wkt);

    *p_count = __g_count_set - tmp;

    return AM_OK;
}

/**
 * \brief 获取 WKT roll over 的值
 *
 * \param[in] p_drv      : 指向 WKT 设备结构体的指针
 * \param[in] chan       : WKT 通道号
 * \param[in] p_rollover : 指向保存 roll over 的值的指针
 *
 * \retval  AM_OK     : 获取成功
 * \retval -AM_EINVAL : 参数非法
 */
static int __wkt_rollover_get (void *p_drv, uint8_t chan, uint32_t *p_rollover)
{
    am_lpc82x_wkt_dev_t *p_dev        = (am_lpc82x_wkt_dev_t *)p_drv;
    amhw_lpc82x_wkt_t   *p_hw_wkt     = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);
    uint8_t              channel_nums = p_dev->p_devinfo->channel_nums;

    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }

    /* 判断WKT当前使用的时钟源 */
    if(amhw_lpc82x_wkt_clksel_get(p_hw_wkt) == 1) {        /* 低功耗时钟 10kHz */
        *p_rollover = AMHW_LPC82X_WKT_INVAL_MAX / AMHW_LPC82X_WKT_LOW_POWER_CLOCK_FREQ;
    } else if(amhw_lpc82x_wkt_clksel_get(p_hw_wkt) == 0) { /* IRC时钟 750kHz */
        *p_rollover = AMHW_LPC82X_WKT_INVAL_MAX / AMHW_LPC82X_WKT_IRC_CLOCK_FREQ;
    } else {
        *p_rollover = AMHW_LPC82X_WKT_INVAL_MAX / p_dev->p_devinfo->wkt_ext_clock_freq;
    }

    return AM_OK;
}

/**
 * \brief WKT 禁能
 *
 * \param[in] p_drv : 指向 WKT 设备结构体的指针
 * \param[in] chan  : WKT 通道号
 *
 * \retval  AM_OK     : 禁能成功
 * \retval -AM_EINVAL : 参数非法
 */
static int __wkt_disable (void *p_drv, uint8_t chan)
{
    am_lpc82x_wkt_dev_t *p_dev        = (am_lpc82x_wkt_dev_t *)p_drv;
    amhw_lpc82x_wkt_t   *p_hw_wkt     = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);
    uint8_t              channel_nums = p_dev->p_devinfo->channel_nums;

    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }

    amhw_lpc82x_wkt_clearctr_clr(p_hw_wkt);

    return AM_OK;
}

/**
 * \brief WKT 使能
 *
 * \param[in] p_drv           : 指向 WKT 设备结构体的指针
 * \param[in] chan            : WKT 通道号
 * \param[in] max_timer_count : 最大定时器计算值
 *
 * \retval  AM_OK     : 使能成功
 * \retval -AM_EINVAL : 参数非法
 */
static int __wkt_enable (void *p_drv, uint8_t chan, uint32_t max_timer_count)
{
    am_lpc82x_wkt_dev_t *p_dev        = (am_lpc82x_wkt_dev_t *)p_drv;
    amhw_lpc82x_wkt_t   *p_hw_wkt     = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);
    uint8_t              channel_nums = p_dev->p_devinfo->channel_nums;

    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }

    amhw_lpc82x_wkt_count_set(p_hw_wkt, max_timer_count);

    __g_count_set = max_timer_count;

    if (p_dev->callback_info.pfn_callback == NULL) {
        return -AM_EINVAL;
    }

    return AM_OK;
}

/**
 * \brief WKT 回调函数设置
 *
 * \param[in] p_drv        : 指向WKT设备结构体的指针
 * \param[in] chan         : WKT 通道号
 * \param[in] pfn_callback : 函数指针
 * \param[in] p_arg        : 函数入参
 *
 * \retval  AM_OK     : 设置成功
 * \retval -AM_EINVAL : 参数非法
 */
static int __wkt_callback_set (void    *p_drv,
                               uint8_t  chan,
                               void    (*pfn_callback)(void *),
                               void    *p_arg)
{
    am_lpc82x_wkt_dev_t *p_dev        = (am_lpc82x_wkt_dev_t *)p_drv;
    uint8_t              channel_nums = p_dev->p_devinfo->channel_nums;

    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }

    if (pfn_callback == NULL) {
        return -AM_EINVAL;
    } else {  
        p_dev->callback_info.p_arg        = p_arg;
        p_dev->callback_info.pfn_callback = pfn_callback;
    }

    /* 连接WKT */
    __wkt_connect(p_dev);

    return AM_OK;
}

/**
 * \brief WKT 连接函数
 *
 * \param[in] p_drv : 指向 WKT 设备结构体的指针
 *
 * \retval AM_OK : 成功
 */
static int __wkt_connect (void *p_drv)
{
    am_lpc82x_wkt_dev_t *p_dev = (am_lpc82x_wkt_dev_t *)p_drv;

    am_int_connect(p_dev->p_devinfo->inum, __wkt_irq_handler, (void *)p_dev);
    am_int_enable(p_dev->p_devinfo->inum);

    return AM_OK;
}

/**
 * \brief 初始化 WKT 为定时功能
 */
am_timer_handle_t am_lpc82x_wkt_init (am_lpc82x_wkt_dev_t            *p_dev,
                                      const am_lpc82x_wkt_devinfo_t  *p_devinfo)
{
    amhw_lpc82x_wkt_t *p_hw_wkt = (amhw_lpc82x_wkt_t *)(p_devinfo->wkt_regbase);

    if (p_dev == NULL || p_devinfo == NULL || p_devinfo->wkt_regbase == NULL) {
        return NULL;
    }

    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    p_dev->p_devinfo          = p_devinfo;
    p_dev->timer_serv.p_funcs = (struct am_timer_drv_funcs *)&__g_wkt_drv_funcs;
    p_dev->timer_serv.p_drv   = p_dev;

    amhw_lpc82x_wkt_alarmflag_clear(p_hw_wkt);    /* 清除中断标志 */

    if (p_dev->p_devinfo->clksel_str == AMHW_LPC82X_WKT_LOW_POWER_CLOCK) {

        /* 使能低功耗振荡器 */
        amhw_lpc82x_pmu_lposcen_enable(AMHW_LPC82X_PMU);

        /* 设置WKT时钟源为低功耗振荡器 */
        amhw_lpc82x_wkt_clksel_cfg(p_hw_wkt, AMHW_LPC82X_WKT_LOW_POWER_CLOCK);
    } else if (p_dev->p_devinfo->clksel_str == AMHW_LPC82X_WKT_IRC_CLOCK) {

        /* 设置WKT时钟源为内部时钟 */
        amhw_lpc82x_wkt_clksel_cfg(p_hw_wkt, AMHW_LPC82X_WKT_IRC_CLOCK);
    } else if (p_dev->p_devinfo->clksel_str == AMHW_LPC82X_WKT_EXT_CLOCK) {

        /* WAKEUPCLKHYS 引脚迟滞使能 */
        amhw_lpc82x_pmu_wakeupclkhys_enable(AMHW_LPC82X_PMU);

        /* WAKECLKPAD引脚使能(使能PIO0_28外部时钟功能) */
        amhw_lpc82x_pmu_wakeclkpad_enable(AMHW_LPC82X_PMU);

        /* 设置WKT时钟源为外部时钟 */
        amhw_lpc82x_wkt_clksel_cfg(p_hw_wkt, AMHW_LPC82X_WKT_EXT_CLOCK);
    }

    p_dev->callback_info.pfn_callback = NULL;
    p_dev->callback_info.p_arg        = NULL;

    return &(p_dev->timer_serv);
}

/**
 * \brief 不使用 WKT 时，解初始化 WKT，释放相关资源
 */
void am_lpc82x_wkt_deinit (am_timer_handle_t handle)
{
    am_lpc82x_wkt_dev_t *p_dev    = (am_lpc82x_wkt_dev_t *)handle;
    amhw_lpc82x_wkt_t   *p_hw_wkt = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);

    p_dev->timer_serv.p_funcs = NULL;
    p_dev->timer_serv.p_drv   = NULL;

    p_dev->callback_info.pfn_callback = NULL;
    p_dev->callback_info.p_arg        = NULL;

    amhw_lpc82x_wkt_clearctr_clr(p_hw_wkt);

    am_int_disable(p_dev->p_devinfo->inum);

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

/* end of file */
