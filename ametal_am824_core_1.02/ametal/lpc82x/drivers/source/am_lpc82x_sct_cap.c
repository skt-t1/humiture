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
 * \brief SCT���� ����ʵ��
 *
 * 1. SCT֧���ṩ�������ֱ�׼���񣬱������ṩ���Ƿ���CAP��׼�����������
 *     - ��ʱ
 *     - PMW
 *     - ����
 * 2. ��Դ����������ʾ��
 *    - ����ͨ��0���ڲ���SCT����0�����¼�0��������Բ����ж�
 *    - ����ͨ��1���ڲ���SCT����1�����¼�1��������Բ����ж�
 *    - ����ͨ��2���ڲ���SCT����2�����¼�2��������Բ����ж�
 *    - ����ͨ��3���ڲ���SCT����3�����¼�3��������Բ����ж�
 *
 * \internal
 * \par Modification history
 * - 1.03 15-12-14  hgo, fix potential bug caused by incorrect p_arg on callback
 * - 1.02 15-11-20  hgo, add param check.
 * - 1.01 15-09-24  zxl, modified.
 * - 1.00 15-01-14  oce, first implementation.
 * \endinternal
 */
#include "ametal.h"
#include "am_gpio.h"
#include "am_int.h"
#include "am_lpc82x_sct_cap.h"
#include "amhw_lpc82x_sct.h"
#include "amhw_lpc82x_clk.h"
/*******************************************************************************
* ��������
*******************************************************************************/

/** \brief ���� SCT ����  */
static int __sct_cap_config(void              *p_cookie,
                            int                chan,
                            unsigned int       flags,
                            am_cap_callback_t  pfn_callback,
                            void              *p_arg);

/** \brief ʹ�� SCT ���� */
static int __sct_cap_enable(void *p_drv, int chan);

/** \brief ����SCT���� */
static int __sct_cap_disable(void *p_drv, int chan);

/** \brief ��λSCT���� */
static int __sct_cap_reset(void *p_drv, int chan);

/** \brief SCT�������*/
static int __sct_cap_count_to_time (void         *p_drv,
                                    int           chan,
                                    unsigned int  count1,
                                    unsigned int  count2,
                                    unsigned int *p_time_ns);

/** \brief SCT�����жϷ�����*/
static void __sct_cap_irq_handler(void *p_arg);

/** \brief SCT ���� �������� */
static const struct am_cap_drv_funcs __g_sct_cap_drv_funcs = {
    __sct_cap_config,
    __sct_cap_enable,
    __sct_cap_disable,
    __sct_cap_reset,
    __sct_cap_count_to_time,
};

/******************************************************************************/

/** \brief ���� SCT ����  */
static int __sct_cap_config (void              *p_drv,
                             int                chan,
                             unsigned int       options,
                             am_cap_callback_t  pfn_callback,
                             void              *p_arg)
{
    am_lpc82x_sct_cap_dev_t    *p_dev    = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;
    am_lpc82x_sct_cap_ioinfo_t *p_ioinfo = NULL;
    uint32_t                    iocond;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_cap_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);
    p_ioinfo = p_dev->p_devinfo->p_ioinfo;
    
    /* ��Чͨ��Ϊ  0 ~ (channels_num - 1) */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }
    
    /* ֧�������� */
    if ((options & AM_CAP_TRIGGER_RISE) == AM_CAP_TRIGGER_RISE) {
        iocond = AMHW_LPC82X_SCT_EVT_CTRL_IOCOND_RISE;
    }

    /* ֧���½��� */
    if ((options & AM_CAP_TRIGGER_FALL) == AM_CAP_TRIGGER_FALL) {
        iocond = AMHW_LPC82X_SCT_EVT_CTRL_IOCOND_FALL;
    }
    
    if ((options & AM_CAP_TRIGGER_BOTH_EDGES) == AM_CAP_TRIGGER_BOTH_EDGES) {
        return -AM_ENOTSUP; /* ��֧��˫���� */
    }

    /* ����SCT CAP�û������ļ����趨�����š��������Ź��ܣ��������� */
    am_gpio_pin_cfg((p_ioinfo + chan)->gpio, (p_ioinfo + chan)->func);

    /* ����SCT�����ź�ԴΪSCT����ͨ��chan */
    amhw_lpc82x_inmux_sct_trig_set(AMHW_LPC82X_INMUX, chan,
                                  (amhw_lpc82x_inmux_sct_trig_src_t)\
                                  (AMHW_LPC82X_INMUX_SCTTRIG_SCT_PIN0 + chan));

    /* ��ƥ�䲶׽�Ĵ�������Ϊ������ */
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   /* ����ģʽ ʹ��32λ������ */
                                   AMHW_LPC82X_SCT_MODE_UNIFY,
                                   /* ʹ��ͨ��chan */
                                   AMHW_LPC82X_SCT_MAT(chan),
                                   /* �Ĵ��������ڲ���״̬    */
                                   AMHW_LPC82X_SCT_MATCAP_CAPTURE);
    
    /* ʹ���¼�ԭ�� CAPn _L (UNIFY = 0) �Ĵ����ؼ��� */
    amhw_lpc82x_sct_cap_ctrl(p_hw_sct,
                             AMHW_LPC82X_SCT_MODE_UNIFY,
                             AMHW_LPC82X_SCT_CAP(chan),
                             AMHW_LPC82X_SCT_EVT(chan),
                             TRUE);
    
    /* ʹ���¼����ͬ��ͨ���š��¼�ֻ����IO������������ƥ��Ĵ��� */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             AMHW_LPC82X_SCT_EVT(chan),
                             AMHW_LPC82X_SCT_EVT_CTRL_OUTSEL_INPUT |
                             AMHW_LPC82X_SCT_EVT_CTRL_IOSEL(chan)  |
                             iocond                                |
                             /* ʹ��ָ��IO           */
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_IO  |
                             /*  STATEV ֵ���� STATE */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD  |
                             /* �� 0��STATEû�иı�  */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));
    
    p_dev->callback_info[chan].p_arg         = p_arg;
    p_dev->callback_info[chan].callback_func = pfn_callback;

    return AM_OK;
}

/** \brief ʹ�� SCT ���� */
static int __sct_cap_enable (void *p_drv, int chan)
{
    am_lpc82x_sct_cap_dev_t    *p_dev    ;
    amhw_lpc82x_sct_t          *p_hw_sct ;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_cap_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);

    /* ��Чͨ��Ϊ  0 ~ (channels_num - 1) */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }

    /* ʹ��״̬0���¼�0���� */
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct,
                                     AMHW_LPC82X_SCT_EVT(chan),
                                     AMHW_LPC82X_SCT_STATE(0));

    /* ����¼���־ */
    amhw_lpc82x_sct_evt_flag_clr(p_hw_sct, AMHW_LPC82X_SCT_EVT(chan));

    /* ʹ���¼������ж� */
    amhw_lpc82x_sct_evt_int_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(chan));

    /* ������ƼĴ�������Ӧλ */
    amhw_lpc82x_sct_ctrl_clr(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);

    am_int_connect(p_dev->p_devinfo->inum,
                   __sct_cap_irq_handler, (void *)p_dev);
    am_int_enable(p_dev->p_devinfo->inum);
    
    return AM_OK;
}

/** \brief ����SCT���� */
static int __sct_cap_disable (void *p_drv, int chan)
{
    am_lpc82x_sct_cap_dev_t    *p_dev    ;
    amhw_lpc82x_sct_t          *p_hw_sct ;
    int                    i;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_cap_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);

    /* ��Чͨ��Ϊ  0 ~ (channels_num - 1) */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }


    /* ����״̬0���¼�0���� */
    amhw_lpc82x_sct_evt_state_disable(p_hw_sct, AMHW_LPC82X_SCT_EVT(chan),
                                                AMHW_LPC82X_SCT_STATE(0));
    
    amhw_lpc82x_sct_evt_int_disable(p_hw_sct, AMHW_LPC82X_SCT_EVT(chan));

    i = p_dev->p_devinfo->channels_num;
    
    for (i = p_dev->p_devinfo->channels_num - 1; i >= 0; i--) {
        if (p_hw_sct->event[i].state != 0) {
            return AM_OK;
        }
    }
    
    /* ֹͣ����ֹ��16λ��ʱ�� */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);
    
    return AM_OK;
}

/** \brief ��λSCT���� */
static int __sct_cap_reset (void *p_drv, int chan)
{
    am_lpc82x_sct_cap_dev_t *p_dev    ;
    amhw_lpc82x_sct_t         *p_hw_sct ;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_cap_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);
    
    if (amhw_lpc82x_sct_halt_check(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY) == TRUE) {
    
        /* ��������� */
        amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);
    } else {

        /* ��HALTλ,��ֹSCT */
        amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_HALT_L);

        /* ��������� */
        amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);

        amhw_lpc82x_sct_ctrl_clr(p_hw_sct, AMHW_LPC82X_SCT_CTRL_HALT_L);
    }
    
    return AM_OK;
}

/** \brief SCT������� */
static int __sct_cap_count_to_time (void         *p_drv,
                                    int           chan,
                                    unsigned int  count1,
                                    unsigned int  count2,
                                    unsigned int *p_time_ns)
{
    am_lpc82x_sct_cap_dev_t *p_dev    ;
    amhw_lpc82x_sct_t       *p_hw_sct ;
    uint32_t                 clkfreq  ;
    uint32_t                 count_err;
    uint64_t                 time_ns;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_cap_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);
    clkfreq  = amhw_lpc82x_clk_periph_freq_get(p_hw_sct);
    
    count_err = count2 - count1;

    /* ת������ֵΪʱ�� */
    time_ns = (uint64_t)1000000000 * (uint64_t)count_err / (uint64_t)clkfreq ;

    *p_time_ns = time_ns;

    return AM_OK;
}

/** \brief SCT�����жϷ����� */
static void __sct_cap_irq_handler (void *p_arg)
{
    am_lpc82x_sct_cap_dev_t *p_dev     = (am_lpc82x_sct_cap_dev_t *)p_arg;
    amhw_lpc82x_sct_t       *p_hw_sct  = NULL;
    uint8_t                 chans_num  = p_dev->p_devinfo->channels_num;
    
    uint8_t           i;
    am_cap_callback_t callback_func;
    uint32_t          value;

    
    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);

    for (i = 0; i < chans_num; i++) {
        if (amhw_lpc82x_sct_evt_flag_chk(p_hw_sct,
                                         AMHW_LPC82X_SCT_EVT(i)) == TRUE ) {
            
            callback_func = p_dev->callback_info[i].callback_func;
            value = amhw_lpc82x_sct_cap_val_get(p_hw_sct,
                                                AMHW_LPC82X_SCT_MODE_UNIFY, i);
            
            if (callback_func != NULL) {
                callback_func(p_dev->callback_info[i].p_arg, value);
            }
            
            /* ���¼� i ��־λ */
            amhw_lpc82x_sct_evt_flag_clr(p_hw_sct, AMHW_LPC82X_SCT_EVT(i));
            
        }
    }
}

/**
 * \brief ��ʼ��SCT��CAP����
 *
 * \param[in] p_sct : ָ��SCT�Ĵ������ָ��
 *
 * \return ��
 */
void __sct_cap_init (amhw_lpc82x_sct_t *p_hw_sct)
{
    /* ��ֹͣλ��SCTֹͣ���� */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);

    amhw_lpc82x_sct_config(p_hw_sct,
                           AMHW_LPC82X_SCT_CFG_32BIT_COUNTER  |
                           AMHW_LPC82X_SCT_CFG_CLKMODE_SYSCLK |
                           AMHW_LPC82X_SCT_CFG_INSYNC_ALL);

    /* ��CLRCTRλ���������Ϊ0 */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);
    
    /* ���õ�ǰ״ֵ̬Ϊ0 */
    amhw_lpc82x_sct_state_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY,
                                        AMHW_LPC82X_SCT_STATE(0));

    /* Ԥ��ƵΪ0��ʹ��ϵͳʱ�� */
    amhw_lpc82x_sct_prescale_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY, 0);
}


/** \brief ��ʼ��SCT���� */
am_cap_handle_t am_lpc82x_sct_cap_init (am_lpc82x_sct_cap_dev_t     *p_dev,
                                  const am_lpc82x_sct_cap_devinfo_t *p_devinfo)
{
    amhw_lpc82x_sct_t *p_hw_sct;
    uint8_t    i;
    
    if (p_dev == NULL || p_devinfo == NULL) {
        return NULL;
    }

    /* The SCT ʹ�õ�ͨ���������Ϊ 4 */
    if (p_devinfo->channels_num > 4) {
        return NULL;
    }

    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }
    p_hw_sct                = (amhw_lpc82x_sct_t *)(p_devinfo->sct_regbase);
    p_dev->p_devinfo        = p_devinfo;
    p_dev->cap_serv.p_funcs = (struct am_cap_drv_funcs *)&__g_sct_cap_drv_funcs;
    p_dev->cap_serv.p_drv   = p_dev;

    for (i = 0; i < p_devinfo->channels_num; i++) {
        p_dev->callback_info[i].callback_func = NULL;
    }

    /* ��λSCT��������Ԥ��Ƶ������Ϊ 0 */
    __sct_cap_init(p_hw_sct);

    return &(p_dev->cap_serv);
}

/** \brief ���ʼ��SCT������ */
void am_lpc82x_sct_cap_deinit (am_cap_handle_t handle)
{
    am_lpc82x_sct_cap_dev_t *p_dev     = (am_lpc82x_sct_cap_dev_t *)handle;
    amhw_lpc82x_sct_t       *p_hw_sct  = NULL;

    if (p_dev                      == NULL ||
        p_dev->p_devinfo           == NULL ) {
        return ;
    }

    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);

    /* ��ֹSCT */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);

    am_int_disable(p_dev->p_devinfo->inum);

    p_dev->cap_serv.p_drv = NULL;

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

/* end of file */
