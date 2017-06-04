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
 * - 1.02 15-12-14  hgo, fix potential bug caused by incorrect p_arg on callback
 * - 1.01 15-12-02  mem,modified.
 * - 1.00 15-07-05  win, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "ametal.h"
#include "am_int.h"
#include "amhw_lpc82x_clk.h"
#include "am_lpc82x_mrt.h"
#include "amhw_lpc82x_mrt.h"

/*******************************************************************************
   函数声明
*******************************************************************************/

/** \brief  获取MRT信息 */
static int __mrt_info_get(void *p_drv, am_timer_info_t *p_info);

/** \brief 获取 MRT 计数器的值 */
static int __mrt_count_get(void *p_drv, uint8_t chan, uint32_t *p_count);

/** \brief 获取MRT roll over 的值 */
static int __mrt_rollover_get(void *p_drv, uint8_t chan, uint32_t *p_rollover);

/** \brief MRT 禁能 */
static int __mrt_disable(void *p_drv, uint8_t chan);

/** \brief MRT 使能 */
static int __mrt_enable(void *p_drv, uint8_t chan, uint32_t max_timer_count);

/** \brief MRT 回调函数设置 */
static int __mrt_callback_set(void    *p_drv,
                              uint8_t  chan,
                              void   (*pfn_callback)(void *),
                              void    *p_arg);

/*******************************************************************************
  全局变量
*******************************************************************************/
                              
/** \brief  MRT 驱动函数 */
static const struct am_timer_drv_funcs __g_mrt_drv_funcs = {
    __mrt_info_get,
    __mrt_count_get,
    __mrt_rollover_get,
    __mrt_disable,
    __mrt_enable,
    __mrt_callback_set,
    NULL,
    NULL,
    NULL,
};

/******************************************************************************/

/** 
 * \brief MRT 中断处理函数
 * 
 * \param[in] p_arg : 中断函数入口参数
 *
 * \return 无
 */
void __mrt_irq_handler (void *p_arg)
{
    uint8_t i;
    
    am_lpc82x_mrt_dev_t *p_dev       = (am_lpc82x_mrt_dev_t *)p_arg;
    amhw_lpc82x_mrt_t   *p_hw_mrt    = (amhw_lpc82x_mrt_t*)p_dev->p_devinfo->mrt_regbase;
    uint8_t             channel_nums = p_dev->p_devinfo->channel_nums;
    
    for (i = 0; i < channel_nums; i++) {
        
        if (amhw_lpc82x_mrt_is_pending(p_hw_mrt, i) == TRUE) {

            amhw_lpc82x_mrt_int_pending_clr(p_hw_mrt, i);

            if (p_dev->callback_info[i].pfn_callback) {
                p_dev->callback_info[i].pfn_callback(p_dev->callback_info[i].p_arg);
            }
        }
    }
}

/**
 * \brief 获取 MRT 信息
 */
static int __mrt_info_get (void *p_drv, am_timer_info_t *p_info)
{
    uint8_t              channel_nums;
    am_lpc82x_mrt_dev_t *p_dev    = NULL;
    amhw_lpc82x_mrt_t   *p_hw_mrt = NULL;
    
    if (p_drv == NULL || p_info == NULL) {
        return -AM_EINVAL;
    }
    
    p_dev        = (am_lpc82x_mrt_dev_t *)p_drv;
    channel_nums = p_dev->p_devinfo->channel_nums;
    p_hw_mrt     = (amhw_lpc82x_mrt_t*)p_dev->p_devinfo->mrt_regbase;

    
    p_info->clk_frequency = amhw_lpc82x_clk_periph_freq_get(p_hw_mrt);
    
    if (p_info->clk_frequency <= AMHW_LPC82X_MRT_INVAL_MAX) {
        p_info->min_frequency = 1;
    } else {
        p_info->min_frequency = p_info->clk_frequency / AMHW_LPC82X_MRT_INVAL_MAX;
        
        /*
         * p_info->clk_frequency / AMHW_LPC82X_MRT_INVAL_MAX, 取不小于他的最小整数
         */
        if (p_info->min_frequency * AMHW_LPC82X_MRT_INVAL_MAX < p_info->clk_frequency) {
            p_info->min_frequency += 1;
        }
    }
    
    p_info->max_frequency = p_info->clk_frequency;
    
    p_info->features = AM_TIMER_SIZE(31)               | /* 31位定时器       */
                       AM_TIMER_CHAN_NUM(channel_nums) | /* 支持4通道        */
                       AM_TIMER_CAN_INTERRUPT          | /* 可以产生中断     */
                       AM_TIMER_INTERMEDIATE_COUNT     | /* 可以读当前计数值 */
                       AM_TIMER_AUTO_RELOAD;             /* 自动重载         */
    
    return AM_OK;
}

/**
 * \brief 获取 MRT 计数器的值
 */
static int __mrt_count_get (void *p_drv, uint8_t chan, uint32_t *p_count)
{
    uint32_t             tmp;
    uint8_t              channel_nums;
    am_lpc82x_mrt_dev_t *p_dev    = NULL;
    amhw_lpc82x_mrt_t   *p_hw_mrt = NULL;

    if (p_drv == NULL || p_count == NULL) {
        return -AM_EINVAL;
    }
    
    p_dev        = (am_lpc82x_mrt_dev_t *)p_drv;
    channel_nums = p_dev->p_devinfo->channel_nums;
    p_hw_mrt     = (amhw_lpc82x_mrt_t*)p_dev->p_devinfo->mrt_regbase;
    
    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }
    
    tmp = amhw_lpc82x_mrt_count_get(p_hw_mrt, chan);
    *p_count = amhw_lpc82x_mrt_intval_get(p_hw_mrt, chan) - tmp;
    
    return AM_OK;
}

/**
 * \brief 获取 MRT roll over 的值
 */
static int __mrt_rollover_get (void *p_drv, uint8_t chan, uint32_t *p_rollover)
{
    uint8_t              channel_nums;
    am_lpc82x_mrt_dev_t *p_dev    = NULL;
    amhw_lpc82x_mrt_t   *p_hw_mrt = NULL;

    if (p_drv == NULL || p_rollover == NULL) {
        return -AM_EINVAL;
    }
    
    p_dev        = (am_lpc82x_mrt_dev_t *)p_drv;
    channel_nums = p_dev->p_devinfo->channel_nums;
    p_hw_mrt     = (amhw_lpc82x_mrt_t*)p_dev->p_devinfo->mrt_regbase;
    
    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }
    
    /* 31位最大值为 AMHW_LPC82X_MRT_INVAL_MAX */
    *p_rollover = AMHW_LPC82X_MRT_INVAL_MAX / 
                       amhw_lpc82x_clk_periph_freq_get(p_hw_mrt);
    
    return AM_OK;
}

/**
 * \brief MRT 禁能
 */
static int __mrt_disable (void *p_drv, uint8_t chan)
{
    uint8_t              channel_nums;
    am_lpc82x_mrt_dev_t *p_dev    = NULL;
    amhw_lpc82x_mrt_t   *p_hw_mrt = NULL;

    if (p_drv == NULL) {
        return -AM_EINVAL;
    }
    
    p_dev        = (am_lpc82x_mrt_dev_t *)p_drv;
    channel_nums = p_dev->p_devinfo->channel_nums;
    p_hw_mrt     = (amhw_lpc82x_mrt_t*)p_dev->p_devinfo->mrt_regbase;
    
    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }
    
    amhw_lpc82x_mrt_intval_set(p_hw_mrt, chan, 0, TRUE);
    
    amhw_lpc82x_mrt_int_pending_clr(p_hw_mrt, chan);
    
    amhw_lpc82x_mrt_int_disable(p_hw_mrt, chan);
    
    return AM_OK;
}

/**
 * \brief MRT 使能
 */
static int __mrt_enable (void *p_drv, uint8_t chan, uint32_t max_timer_count)
{
    uint8_t              channel_nums;
    am_lpc82x_mrt_dev_t *p_dev    = NULL;
    amhw_lpc82x_mrt_t   *p_hw_mrt = NULL;
    
    if (p_drv == NULL) {
        return -AM_EINVAL;
    }
    
    p_dev        = (am_lpc82x_mrt_dev_t *)p_drv;
    channel_nums = p_dev->p_devinfo->channel_nums;
    p_hw_mrt     = (amhw_lpc82x_mrt_t*)p_dev->p_devinfo->mrt_regbase;
    
    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }
    
    amhw_lpc82x_mrt_intval_set(p_hw_mrt, chan, max_timer_count, TRUE);
    
    if (p_dev->callback_info[chan].pfn_callback == NULL) {
        amhw_lpc82x_mrt_int_disable(p_hw_mrt, chan);
    } else {
        amhw_lpc82x_mrt_int_enable(p_hw_mrt, chan);
    }
    
    return AM_OK;
}

/**
 * \brief MRT 回调函数设置
 */
static int __mrt_callback_set (void     *p_drv,
                               uint8_t   chan,
                               void    (*pfn_callback)(void *),
                               void     *p_arg)
{
    uint8_t              channel_nums;
    am_lpc82x_mrt_dev_t *p_dev    = NULL;
    amhw_lpc82x_mrt_t   *p_hw_mrt = NULL;
    
    if (p_drv == NULL) {
        return -AM_EINVAL;
    }
    
    p_dev        = (am_lpc82x_mrt_dev_t *)p_drv;
    channel_nums = p_dev->p_devinfo->channel_nums;
    p_hw_mrt     = (amhw_lpc82x_mrt_t*)p_dev->p_devinfo->mrt_regbase;
    
    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }
    
    /* 关闭中断 */
    if (pfn_callback == NULL) {
        amhw_lpc82x_mrt_int_disable(p_hw_mrt, chan);
        
    } else {
        p_dev->callback_info[chan].p_arg        = p_arg;
        p_dev->callback_info[chan].pfn_callback = pfn_callback;
        
        amhw_lpc82x_mrt_int_enable(p_hw_mrt, chan);
    }
    
    return AM_OK;
}

/**
 * \brief 初始化 MRT 为定时功能
 */
am_timer_handle_t am_lpc82x_mrt_init (am_lpc82x_mrt_dev_t            *p_dev,
                                      const am_lpc82x_mrt_devinfo_t  *p_devinfo)
{
    int     state;
    uint8_t i;
    uint8_t channel_nums;
    
    amhw_lpc82x_mrt_t *p_hw_mrt = NULL;
    
    if (p_dev == NULL || p_devinfo == NULL) {
        return NULL;
    }
    
    channel_nums = p_devinfo->channel_nums;
    p_hw_mrt     = (amhw_lpc82x_mrt_t*)p_devinfo->mrt_regbase;
    
    if (channel_nums > AMHW_LPC82X_MRT_CHAN_NUM) {
        return NULL;
    }
    
    state = am_int_connect(p_devinfo->inum, __mrt_irq_handler, (void *)p_dev);
    if (state != AM_OK) {
        return NULL;
    }
    
    state = am_int_enable(p_devinfo->inum);
    if (state != AM_OK) {
        return NULL;
    }

    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }
    
    for (i = 0; i < channel_nums; i++) {
        
        p_dev->callback_info[i].pfn_callback = NULL;
        p_dev->callback_info[i].p_arg        = NULL;

        amhw_lpc82x_mrt_mode_set(p_hw_mrt, i, AMHW_LPC82X_MRT_MODE_REPEAT);
    }  

    p_dev->p_devinfo          = p_devinfo;
    p_dev->timer_serv.p_funcs = (struct am_timer_drv_funcs *)&__g_mrt_drv_funcs;
    p_dev->timer_serv.p_drv   = p_dev; 

    return &(p_dev->timer_serv);
}

/**
 * \brief 不使用 MRT 时，解初始化 MRT，释放相关资源
 */
void am_lpc82x_mrt_deinit (am_timer_handle_t handle)
{
    uint8_t            i;
    uint8_t            channel_nums;
    amhw_lpc82x_mrt_t *p_hw_mrt = NULL;
     
    am_lpc82x_mrt_dev_t *p_dev = (am_lpc82x_mrt_dev_t *)handle->p_drv;
    
    if (p_dev == NULL) {
        return;
    }
    
    p_dev->timer_serv.p_funcs = NULL;
    p_dev->timer_serv.p_drv   = NULL; 
    
    channel_nums = p_dev->p_devinfo->channel_nums;
    p_hw_mrt     = (amhw_lpc82x_mrt_t*)p_dev->p_devinfo->mrt_regbase;

    
    for (i = 0; i < channel_nums; i++) {
        
        p_dev->callback_info[i].pfn_callback = NULL;
        p_dev->callback_info[i].p_arg        = NULL;
        
        amhw_lpc82x_mrt_intval_set(p_hw_mrt, i, 0, TRUE);

        amhw_lpc82x_mrt_int_pending_clr(p_hw_mrt, i);

        amhw_lpc82x_mrt_int_disable(p_hw_mrt, i);
    }
    
    am_int_disable(p_dev->p_devinfo->inum);
    am_int_disconnect(p_dev->p_devinfo->inum, __mrt_irq_handler, (void *)p_dev);
    
    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

/* end of file */
