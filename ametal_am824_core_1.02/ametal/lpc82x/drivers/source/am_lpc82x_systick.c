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
 * \brief SYSTICK ���������� Timer ��׼�ӿ�
 *
 * \note ʹ�� SYSTICK ����� Timer ��׼�ӿڣ��Ͳ���ʹ�ñ�׼��ʱ�ӿ�
 *       am_mdelay()��am_udelay()
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
  ��������
*******************************************************************************/

/** \brief  ��ȡ SYSTICK ��Ϣ */
static int __systick_info_get (void *p_drv, am_timer_info_t *p_info);

/** \brief ��ȡ SYSTICK ��������ֵ */
static int __systick_count_get (void     *p_drv,
                                uint8_t   chan,
                                uint32_t *p_count);

/** \brief ��ȡ SYSTICK roll over ��ֵ */
static int __systick_rollover_get (void     *p_drv,
                                   uint8_t   chan,
                                   uint32_t *p_rollover);

/** \brief SYSTICK  ���� */
static int __systick_disable (void *p_drv, uint8_t chan);

/** \brief SYSTICK  ʹ�� */
static int __systick_enable (void     *p_drv,
                             uint8_t   chan,
                             uint32_t  timer_count);

/** \brief SYSTICK  �ص��������� */
static int __systick_callback_set (void    *p_drv,
                                   uint8_t  chan,
                                   void   (*pfn_callback)(void *),
                                   void    *p_arg);

/*******************************************************************************
  ȫ�ֱ���
*******************************************************************************/

/**
 * \brief SYSTICK ��������
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
 * \brief ���ǵ� SYSTICK �жϴ������޷����ݲ�����ʹ��һ��ȫ�ֱ���ָ�룬
 *        ���� SYSTICK �豸�ṹ���ַ
 */
static am_lpc82x_systick_dev_t *p_systick_dev;

/**
 * \brief SYSTICK �жϴ�����
 *
 * \return ��
 */
void SysTick_Handler (void)
{
    if (p_systick_dev->pfn_callback != NULL) {
        p_systick_dev->pfn_callback(p_systick_dev->p_arg);
    }
}

/**
 * \brief  ��ȡ SYSTICK ��Ϣ
 *
 * \param[in] p_drv  : ָ�� SYSTICK �豸�ṹ���ָ��
 * \param[in] p_info : ָ�� SYSTICK �豸��Ϣ�ṹ���ָ��
 *
 * \retval  AM_OK     : ��ȡ��Ϣ�ɹ�
 * \retval -AM_EINVAL : ��������
 */
static int __systick_info_get (void *p_drv, am_timer_info_t *p_info)
{
    am_lpc82x_systick_dev_t *p_dev;

    if (p_drv == NULL || p_info == NULL) {
        return -AM_EINVAL;
    }

    p_dev = (am_lpc82x_systick_dev_t *)p_drv;

    /* ��ȡ��ǰϵͳʱ��Ƶ�ʣ�systick ʹ�õ���ϵͳʱ�� */
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
    p_info->features      = AM_TIMER_SIZE(24)           | /* 24λ��ʱ��             */
                            AM_TIMER_CHAN_NUM(1)        | /* ��ʱ��ͨ����           */
                            AM_TIMER_CAN_INTERRUPT      | /* ���Բ����ж�           */
                            AM_TIMER_AUTO_RELOAD        | /* ֧���Զ�����           */
                            AM_TIMER_INTERMEDIATE_COUNT;  /* ��ǰ������ֵ�ɶ�  */

    return AM_OK;
}

/**
 * \brief  ��ȡ SYSTICK ��ʱ������ֵ
 *
 * \param[in] p_drv   : ָ�� SYSTICK �豸�ṹ���ָ��
 * \param[in] chan    : SYSTICK ͨ���ţ�SYSTICK ֻ��ʹ��ͨ��0
 * \param[in] p_count : ָ���Ŷ�ʱ������ֵ��ָ��
 *
 * \retval  AM_OK     : ��ȡ��Ϣ�ɹ�
 * \retval -AM_EINVAL : ��������
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
 * \brief ��ȡ SYSTICK roll over ��ֵ
 *
 * \param[in] p_drv      : ָ�� SYSTICK �豸�ṹ���ָ��
 * \param[in] chan       : SYSTICK ͨ���ţ�SYSTICK ֻ��ʹ��ͨ��0
 * \param[in] p_rollover : ָ���ŷ�תֵ��ָ��
 *
 * \retval  AM_OK     : ��ȡ��Ϣ�ɹ�
 * \retval -AM_EINVAL : ��������
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

    /* 24λ���ֵΪ AMHW_LPC82X_SYSTICK_INVAL_MAX */
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
 * \brief SYSTICK ����
 *
 * \param[in] p_drv : ָ�� SYSTICK �豸�ṹ���ָ��
 * \param[in] chan  : SYSTICK ͨ���ţ�SYSTICK ֻ��ʹ��ͨ��0
 *
 * \retval  AM_OK     : ��ȡ��Ϣ�ɹ�
 * \retval -AM_EINVAL : ��������
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
 * \brief SYSTICK ʹ��
 *
 * \param[in] p_drv       : ָ�� SYSTICK �豸�ṹ���ָ��
 * \param[in] chan        : SYSTICK ͨ���ţ�SYSTICK ֻ��ʹ��ͨ��0
 * \param[in] timer_count : ��Ҫ���õĶ�ʱ������ֵ
 *
 * \retval  AM_OK     : ��ȡ��Ϣ�ɹ�
 * \retval -AM_EINVAL : ��������
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

    /* �������ض�ʱ�Ĵ����е���ֵ */
    amhw_lpc82x_systick_reload_val_set(p_hw_systick, timer_count);

    /* �����ǰ��ʱ�������е���ֵ */
    amhw_lpc82x_systick_val_set(p_hw_systick, 0x0);

    if (p_dev->pfn_callback == NULL) {
        amhw_lpc82x_systick_int_disable(p_hw_systick);
    } else {
        amhw_lpc82x_systick_int_enable(p_hw_systick);
    }

    /* ʹ�� SYSTICK */
    amhw_lpc82x_systick_enable(p_hw_systick);

    return AM_OK;
}

/**
 * \brief SYSTICK �ص���������
 *
 * \param[in] p_drv        : ָ�� SYSTICK �豸�ṹ���ָ��
 * \param[in] chan         : SYSTICK ͨ���ţ�SYSTICK ֻ��ʹ��ͨ��0
 * \param[in] pfn_callback : �ص�����ָ��
 * \param[in] p_arg        : �ص�������ڲ���
 *
 * \retval  AM_OK     : ��ȡ��Ϣ�ɹ�
 * \retval -AM_EINVAL : ��������
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
 * \brief ��ʼ�� SYSTICK Ϊ��ʱ����
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

    /* ���ö�ʱ��ʱ��Ƶ�� */
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
 * \brief ��ʹ�� SYSTICK ʱ�����ʼ�� SYSTICK���ͷ������Դ
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
