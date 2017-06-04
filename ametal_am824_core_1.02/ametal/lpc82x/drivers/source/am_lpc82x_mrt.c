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
 * \brief MRT����������Timer��׼�ӿ�
 *
 * ������ʵ����MRT�Ķ�ʱ����
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
   ��������
*******************************************************************************/

/** \brief  ��ȡMRT��Ϣ */
static int __mrt_info_get(void *p_drv, am_timer_info_t *p_info);

/** \brief ��ȡ MRT ��������ֵ */
static int __mrt_count_get(void *p_drv, uint8_t chan, uint32_t *p_count);

/** \brief ��ȡMRT roll over ��ֵ */
static int __mrt_rollover_get(void *p_drv, uint8_t chan, uint32_t *p_rollover);

/** \brief MRT ���� */
static int __mrt_disable(void *p_drv, uint8_t chan);

/** \brief MRT ʹ�� */
static int __mrt_enable(void *p_drv, uint8_t chan, uint32_t max_timer_count);

/** \brief MRT �ص��������� */
static int __mrt_callback_set(void    *p_drv,
                              uint8_t  chan,
                              void   (*pfn_callback)(void *),
                              void    *p_arg);

/*******************************************************************************
  ȫ�ֱ���
*******************************************************************************/
                              
/** \brief  MRT �������� */
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
 * \brief MRT �жϴ�����
 * 
 * \param[in] p_arg : �жϺ�����ڲ���
 *
 * \return ��
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
 * \brief ��ȡ MRT ��Ϣ
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
         * p_info->clk_frequency / AMHW_LPC82X_MRT_INVAL_MAX, ȡ��С��������С����
         */
        if (p_info->min_frequency * AMHW_LPC82X_MRT_INVAL_MAX < p_info->clk_frequency) {
            p_info->min_frequency += 1;
        }
    }
    
    p_info->max_frequency = p_info->clk_frequency;
    
    p_info->features = AM_TIMER_SIZE(31)               | /* 31λ��ʱ��       */
                       AM_TIMER_CHAN_NUM(channel_nums) | /* ֧��4ͨ��        */
                       AM_TIMER_CAN_INTERRUPT          | /* ���Բ����ж�     */
                       AM_TIMER_INTERMEDIATE_COUNT     | /* ���Զ���ǰ����ֵ */
                       AM_TIMER_AUTO_RELOAD;             /* �Զ�����         */
    
    return AM_OK;
}

/**
 * \brief ��ȡ MRT ��������ֵ
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
 * \brief ��ȡ MRT roll over ��ֵ
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
    
    /* 31λ���ֵΪ AMHW_LPC82X_MRT_INVAL_MAX */
    *p_rollover = AMHW_LPC82X_MRT_INVAL_MAX / 
                       amhw_lpc82x_clk_periph_freq_get(p_hw_mrt);
    
    return AM_OK;
}

/**
 * \brief MRT ����
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
 * \brief MRT ʹ��
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
 * \brief MRT �ص���������
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
    
    /* �ر��ж� */
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
 * \brief ��ʼ�� MRT Ϊ��ʱ����
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
 * \brief ��ʹ�� MRT ʱ�����ʼ�� MRT���ͷ������Դ
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
