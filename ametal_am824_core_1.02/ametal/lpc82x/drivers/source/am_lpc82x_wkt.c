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
 * \brief WKT����������Timer��׼�ӿ�
 *
 * ������ʵ����WKT�Ķ�ʱ����
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
��������
*******************************************************************************/

/** \brief ��ȡWKT��Ϣ */
static int __wkt_info_get (void *p_drv, am_timer_info_t *p_info);

/** \brief ��ȡ WKT ��������ֵ */
static int __wkt_count_get (void *p_drv, uint8_t chan, uint32_t *p_count);

/** \brief ��ȡWKT roll over ��ֵ */
static int __wkt_rollover_get (void *p_drv, uint8_t chan, uint32_t *p_rollover);

/** \brief WKT ���� */
static int __wkt_disable (void *p_drv, uint8_t chan);

/** \brief WKT ʹ�� */
static int __wkt_enable (void *p_drv, uint8_t chan, uint32_t max_timer_count);

/** \brief WKT �ص��������� */
static int __wkt_callback_set (void    *p_drv,
                               uint8_t  chan,
                               void    (*pfn_callback)(void *),
                               void    *p_arg);

/** \brief WKT ���Ӻ��� */
static int __wkt_connect (void *p_drv);

/*******************************************************************************
  ȫ�ֱ���
*******************************************************************************/

uint32_t __g_count_set;     /**< \brief �趨��WKT��������ֵ */

/** \brief WKT �������� */
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
 * \brief WKT �жϴ�����
 * \param[in] p_arg : �жϺ�����ڲ���
 * \return ��
 */
void __wkt_irq_handler (void *p_arg)
{
    am_lpc82x_wkt_dev_t *p_dev    = (am_lpc82x_wkt_dev_t *)p_arg;
    amhw_lpc82x_wkt_t *p_hw_wkt   = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);

    if (amhw_lpc82x_wkt_alarmflag_get(p_hw_wkt)) {
        amhw_lpc82x_wkt_alarmflag_clear(p_hw_wkt);    /* ����жϱ�־ */

        if (p_dev->callback_info.pfn_callback != NULL) {
            p_dev->callback_info.pfn_callback(p_dev->callback_info.p_arg);
        }
    }
}

/**
 * \brief ��ȡ WKT ��Ϣ
 *
 * \param[in] p_drv  : ָ��WKT�豸�ṹ���ָ��
 * \param[in] p_info : ָ��ʱ���ṹ���ָ��
 *
 * \retval AM_OK : �ɹ�
 */
static int __wkt_info_get (void *p_drv, am_timer_info_t *p_info)
{
    am_lpc82x_wkt_dev_t *p_dev        = (am_lpc82x_wkt_dev_t *)p_drv;
    amhw_lpc82x_wkt_t   *p_hw_wkt     = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);
    uint8_t              channel_nums = p_dev->p_devinfo->channel_nums;

    /* �ж�WKT��ǰʹ�õ�ʱ��Դ */
    if(amhw_lpc82x_wkt_clksel_get(p_hw_wkt) == 1) {         /* �͹���ʱ�� 10kHz*/
        p_info->clk_frequency = AMHW_LPC82X_WKT_LOW_POWER_CLOCK_FREQ;
    } else if (amhw_lpc82x_wkt_clksel_get(p_hw_wkt) == 0) { /* IRCʱ�� 750kHz*/
        p_info->clk_frequency = AMHW_LPC82X_WKT_IRC_CLOCK_FREQ;
    } else {
        p_info->clk_frequency = p_dev->p_devinfo->wkt_ext_clock_freq;
    }

    if (p_info->clk_frequency <= AMHW_LPC82X_WKT_INVAL_MAX) {
        p_info->min_frequency = 1;
    } else {
        p_info->min_frequency = p_info->clk_frequency / AMHW_LPC82X_WKT_INVAL_MAX;

        /* p_info->clk_frequency / AMHW_WKT_INVAL_MAX, ȡ��С��������С���� */
        if (p_info->min_frequency * AMHW_LPC82X_WKT_INVAL_MAX
                                      < p_info->clk_frequency) {
            p_info->min_frequency += 1;
        }
    }

    p_info->max_frequency = p_info->clk_frequency;

    p_info->features = AM_TIMER_SIZE(32)               | /* 32λ��ʱ�� */
                       AM_TIMER_CHAN_NUM(channel_nums) | /* ֧��1ͨ�� */
                       AM_TIMER_CAN_INTERRUPT          | /* ���Բ����ж� */
                       AM_TIMER_INTERMEDIATE_COUNT     | /* ���Զ���ǰ����ֵ */
                       AM_TIMER_CANNOT_DISABLE;          /* ��ʱ�����ܱ�ֹͣ */

    return AM_OK;
}

/**
 * \brief ��ȡ WKT ��������ֵ
 *
 * \param[in] p_drv   : ָ�� WKT �豸�ṹ���ָ��
 * \param[in] chan    : ͨ����
 * \param[in] p_count : ָ�򱣴��������ֵ��ָ��
 *
 * \retval  AM_OK     : ��ȡ�ɹ�
 * \retval -AM_EINVAL : �����Ƿ�
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

    /* �ֲ᲻�����������������������2�� */
    tmp = amhw_lpc82x_wkt_count_get(p_hw_wkt);
    tmp = amhw_lpc82x_wkt_count_get(p_hw_wkt);

    *p_count = __g_count_set - tmp;

    return AM_OK;
}

/**
 * \brief ��ȡ WKT roll over ��ֵ
 *
 * \param[in] p_drv      : ָ�� WKT �豸�ṹ���ָ��
 * \param[in] chan       : WKT ͨ����
 * \param[in] p_rollover : ָ�򱣴� roll over ��ֵ��ָ��
 *
 * \retval  AM_OK     : ��ȡ�ɹ�
 * \retval -AM_EINVAL : �����Ƿ�
 */
static int __wkt_rollover_get (void *p_drv, uint8_t chan, uint32_t *p_rollover)
{
    am_lpc82x_wkt_dev_t *p_dev        = (am_lpc82x_wkt_dev_t *)p_drv;
    amhw_lpc82x_wkt_t   *p_hw_wkt     = (amhw_lpc82x_wkt_t *)(p_dev->p_devinfo->wkt_regbase);
    uint8_t              channel_nums = p_dev->p_devinfo->channel_nums;

    if (chan >= channel_nums) {
        return -AM_EINVAL;
    }

    /* �ж�WKT��ǰʹ�õ�ʱ��Դ */
    if(amhw_lpc82x_wkt_clksel_get(p_hw_wkt) == 1) {        /* �͹���ʱ�� 10kHz */
        *p_rollover = AMHW_LPC82X_WKT_INVAL_MAX / AMHW_LPC82X_WKT_LOW_POWER_CLOCK_FREQ;
    } else if(amhw_lpc82x_wkt_clksel_get(p_hw_wkt) == 0) { /* IRCʱ�� 750kHz */
        *p_rollover = AMHW_LPC82X_WKT_INVAL_MAX / AMHW_LPC82X_WKT_IRC_CLOCK_FREQ;
    } else {
        *p_rollover = AMHW_LPC82X_WKT_INVAL_MAX / p_dev->p_devinfo->wkt_ext_clock_freq;
    }

    return AM_OK;
}

/**
 * \brief WKT ����
 *
 * \param[in] p_drv : ָ�� WKT �豸�ṹ���ָ��
 * \param[in] chan  : WKT ͨ����
 *
 * \retval  AM_OK     : ���ܳɹ�
 * \retval -AM_EINVAL : �����Ƿ�
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
 * \brief WKT ʹ��
 *
 * \param[in] p_drv           : ָ�� WKT �豸�ṹ���ָ��
 * \param[in] chan            : WKT ͨ����
 * \param[in] max_timer_count : ���ʱ������ֵ
 *
 * \retval  AM_OK     : ʹ�ܳɹ�
 * \retval -AM_EINVAL : �����Ƿ�
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
 * \brief WKT �ص���������
 *
 * \param[in] p_drv        : ָ��WKT�豸�ṹ���ָ��
 * \param[in] chan         : WKT ͨ����
 * \param[in] pfn_callback : ����ָ��
 * \param[in] p_arg        : �������
 *
 * \retval  AM_OK     : ���óɹ�
 * \retval -AM_EINVAL : �����Ƿ�
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

    /* ����WKT */
    __wkt_connect(p_dev);

    return AM_OK;
}

/**
 * \brief WKT ���Ӻ���
 *
 * \param[in] p_drv : ָ�� WKT �豸�ṹ���ָ��
 *
 * \retval AM_OK : �ɹ�
 */
static int __wkt_connect (void *p_drv)
{
    am_lpc82x_wkt_dev_t *p_dev = (am_lpc82x_wkt_dev_t *)p_drv;

    am_int_connect(p_dev->p_devinfo->inum, __wkt_irq_handler, (void *)p_dev);
    am_int_enable(p_dev->p_devinfo->inum);

    return AM_OK;
}

/**
 * \brief ��ʼ�� WKT Ϊ��ʱ����
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

    amhw_lpc82x_wkt_alarmflag_clear(p_hw_wkt);    /* ����жϱ�־ */

    if (p_dev->p_devinfo->clksel_str == AMHW_LPC82X_WKT_LOW_POWER_CLOCK) {

        /* ʹ�ܵ͹������� */
        amhw_lpc82x_pmu_lposcen_enable(AMHW_LPC82X_PMU);

        /* ����WKTʱ��ԴΪ�͹������� */
        amhw_lpc82x_wkt_clksel_cfg(p_hw_wkt, AMHW_LPC82X_WKT_LOW_POWER_CLOCK);
    } else if (p_dev->p_devinfo->clksel_str == AMHW_LPC82X_WKT_IRC_CLOCK) {

        /* ����WKTʱ��ԴΪ�ڲ�ʱ�� */
        amhw_lpc82x_wkt_clksel_cfg(p_hw_wkt, AMHW_LPC82X_WKT_IRC_CLOCK);
    } else if (p_dev->p_devinfo->clksel_str == AMHW_LPC82X_WKT_EXT_CLOCK) {

        /* WAKEUPCLKHYS ���ų���ʹ�� */
        amhw_lpc82x_pmu_wakeupclkhys_enable(AMHW_LPC82X_PMU);

        /* WAKECLKPAD����ʹ��(ʹ��PIO0_28�ⲿʱ�ӹ���) */
        amhw_lpc82x_pmu_wakeclkpad_enable(AMHW_LPC82X_PMU);

        /* ����WKTʱ��ԴΪ�ⲿʱ�� */
        amhw_lpc82x_wkt_clksel_cfg(p_hw_wkt, AMHW_LPC82X_WKT_EXT_CLOCK);
    }

    p_dev->callback_info.pfn_callback = NULL;
    p_dev->callback_info.p_arg        = NULL;

    return &(p_dev->timer_serv);
}

/**
 * \brief ��ʹ�� WKT ʱ�����ʼ�� WKT���ͷ������Դ
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
