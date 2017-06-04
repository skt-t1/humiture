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
 * \brief SCT PWM ������������ PWM ��׼�ӿ�
 *
 * \internal
 * \par Modification history
 * - 1.02 15-11-20  hgo, add param check.
 * - 1.01 15-09-21  win, modify annotation.
 * - 1.00 15-07-14  oce, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "ametal.h"
#include "am_int.h"
#include "am_gpio.h"
#include "am_lpc82x_sct_pwm.h"
#include "amhw_lpc82x_sct.h"
#include "amhw_lpc82x_clk.h"
/*******************************************************************************
* Functions declaration
*******************************************************************************/

/** \brief ���� SCT PWM  */
static int __sct_pwm_config (void          *p_drv,
                             int            chan,
                             unsigned long  duty_ns,
                             unsigned long  period_ns);

/** \brief ʹ�� SCT PWM ��� */
static int __sct_pwm_enable(void *p_drv, int chan);

/** \brief ���� SCT PWM ��� */
static int __sct_pwm_disable(void *p_drv, int chan);

/**
 * \brief SCT PWM ������������
 */
static const struct am_pwm_drv_funcs __g_sct_pwm_drv_funcs = {
    __sct_pwm_config,
    __sct_pwm_enable,
    __sct_pwm_disable,
};

/******************************************************************************/

/**
 * \brief ���� SCT PWM
 */
static int __sct_pwm_config (void          *p_drv,
                             int            chan,
                             unsigned long  duty_ns,
                             unsigned long  period_ns)
{
    am_lpc82x_sct_pwm_dev_t *p_dev    = NULL;
    amhw_lpc82x_sct_t       *p_hw_sct = NULL;
    
    uint32_t clkfreq;
    uint32_t period_c, duty_c;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_pwm_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);

    if ((period_ns == 0) || (duty_ns > 4294967295UL) ||
        (period_ns > 4294967295UL) || (duty_ns > period_ns)) {
        return -AM_EINVAL;
    }

    /**
     * �Ϸ�ͨ������ 0 ~ (channels_num - 1)
     */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }
    
    clkfreq = amhw_lpc82x_clk_periph_freq_get(p_hw_sct);
     
    period_c = (uint64_t)(period_ns) * (clkfreq) / (uint64_t)1000000000;
    duty_c   = (uint64_t)(duty_ns)   * (clkfreq) / (uint64_t)1000000000;
 
    
    /**
     * period ��������Ϊ1
     */
    if (period_c == 0) {
        period_c = 1;
    }
    
    /**
     * ��� SCT �Ƿ�����ֹ״̬, ƥ��Ĵ���0��� period ����
     */
    if (amhw_lpc82x_sct_halt_check(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY) == TRUE ) {

        amhw_lpc82x_sct_mat_val_set(p_hw_sct,
                                    AMHW_LPC82X_SCT_MODE_UNIFY,
                                    AMHW_LPC82X_SCT_MAT(0),
                                    period_c - 1);
    }
    
    /* ����ƥ�� period ��������ֵ*/
    amhw_lpc82x_sct_mat_reload_val_set(p_hw_sct,
                                       AMHW_LPC82X_SCT_MODE_UNIFY,
                                       AMHW_LPC82X_SCT_MAT(0),
                                       period_c - 1);
     
    /**
     * duty ��������Ϊ1
     */
    if (duty_c == 0) {
        duty_c = 1;
    }
    
    /**
     * ƥ��Ĵ��� chan+1 ��� duty ����
     */
    if (amhw_lpc82x_sct_halt_check(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY) == TRUE ) {

        amhw_lpc82x_sct_mat_val_set(p_hw_sct,
                                    AMHW_LPC82X_SCT_MODE_UNIFY,
                                    chan + 1,
                                    duty_c - 1);
    }
    
    /* ����ƥ�� duty ��������ֵ  */
    amhw_lpc82x_sct_mat_reload_val_set(p_hw_sct,
                                AMHW_LPC82X_SCT_MODE_UNIFY,
                                chan + 1,
                                duty_c - 1);

    return AM_OK;
}

/**
 * \brief ʹ�� SCT PWM ���
 */
static int __sct_pwm_enable (void *p_drv, int chan)
{
    am_lpc82x_sct_pwm_dev_t    *p_dev      = NULL;
    am_lpc82x_sct_pwm_ioinfo_t *p_ioinfo   = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct   = NULL;
    uint8_t                     match_num  = AMHW_LPC82X_SCT_MAT(chan+1);
    uint8_t                     event_num  = AMHW_LPC82X_SCT_EVT(chan+1);
    uint8_t                     out_num    = AMHW_LPC82X_SCT_OUT(chan);

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev      = (am_lpc82x_sct_pwm_dev_t *)p_drv;
    p_hw_sct   = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);
    p_ioinfo   = p_dev->p_devinfo->p_ioinfo;
    
    /**
     * �Ϸ���ͨ����  0 ~ (channels_num - 1)
     */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }
    
    /* ����ƥ��/����Ĵ���Ϊƥ��ģʽ */
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY,
                                   match_num,
                                   AMHW_LPC82X_SCT_MATCAP_MATCH);
    
    /* �¼����ʹ��ƥ��Ĵ���ͨ���� */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             event_num,
                             AMHW_LPC82X_SCT_EVT_CTRL_MATCHSEL(match_num) |
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH      |
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD         |
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));
    
    am_gpio_pin_cfg(p_ioinfo[chan].gpio, p_ioinfo[chan].func);
    
    /* ʹ���¼�0���� */
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct, event_num,
                                     AMHW_LPC82X_SCT_STATE(0));
    
    /* �¼�0����Ϊ��� */
    amhw_lpc82x_sct_out_set_enable(p_hw_sct,out_num,
                                   AMHW_LPC82X_SCT_EVT(0));
    
    /* ��� PWM ��� */
    amhw_lpc82x_sct_out_clr_enable(p_hw_sct, out_num, event_num);
    
    /**
     * �� duty == period , ��ͻ�ᷢ������������£�duty�İٷֱ�ӦΪ100%,
     * ���ԣ�Ҫ���ó�ͻ���
     */
    amhw_lpc82x_sct_conflict_res_config(p_hw_sct, out_num,
                                        AMHW_LPC82X_SCT_RES_OUT_SET);

    /* ���ú�����������ڷ��� */
    amhw_lpc82x_sct_out_dir_ctrl(p_hw_sct, out_num,
                                 AMHW_LPC82X_SCT_OUTPUTDIRCTRL_INDEPENDENT);
    
    /* �����ֹ��ֹͣλ������ SCT */
    amhw_lpc82x_sct_ctrl_clr(p_hw_sct,
                             AMHW_LPC82X_SCT_CTRL_STOP_L |
                             AMHW_LPC82X_SCT_CTRL_HALT_L);

    return AM_OK;
}

/**
 * \brief ���� SCT PWM ���
 */
static int __sct_pwm_disable (void *p_drv, int chan)
{
    am_lpc82x_sct_pwm_dev_t    *p_dev      = NULL;
    am_lpc82x_sct_pwm_ioinfo_t *p_ioinfo   = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct   = NULL;
    uint8_t                     event_num  = AMHW_LPC82X_SCT_EVT(chan+1);
    uint8_t                     out_num    = AMHW_LPC82X_SCT_OUT(chan);
    
    int i;


    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev      = (am_lpc82x_sct_pwm_dev_t *)p_drv;
    p_hw_sct   = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);
    p_ioinfo   = p_dev->p_devinfo->p_ioinfo;
    
    /* �Ϸ���ͨ������ 0 ~ (channels_num - 1) */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }

    am_gpio_pin_cfg(p_ioinfo[chan].gpio, p_ioinfo[chan].dfunc);
  
    /* �����¼�0 */
    amhw_lpc82x_sct_evt_state_disable(p_hw_sct, event_num,
                                      AMHW_LPC82X_SCT_STATE(0));
    
    /* �����¼�0 PWM ��� */
    amhw_lpc82x_sct_out_set_disable(p_hw_sct, out_num, AMHW_LPC82X_SCT_EVT(0));
    
    /* �����¼�������  */
    amhw_lpc82x_sct_out_clr_disable(p_hw_sct, out_num, event_num);

    i = p_dev->p_devinfo->channels_num;
    
    for (i = p_dev->p_devinfo->channels_num - 1; i >= 0; i--) {
        if (p_hw_sct->out[i].clr != 0 || p_hw_sct->out[i].set != 0) {
            return AM_OK;
        }
    }
    
    /* ��������ͨ���� PWM ��������� SCT */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);
   
    return AM_OK;
}

/** 
 * \brief ��ʼ�� SCT Ϊ PWM ���
 *
 * \param[in] p_hw_sct : ָ�� SCT �Ĵ����ṹ��ָ��
 *
 * \return ��
 */
void __sct_pwm_init (amhw_lpc82x_sct_t *p_hw_sct)
{
    /* �ر� SCT */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);
    
    /* ���� SCT ƥ��Ĵ���0 Ϊ�Զ�����  */
    amhw_lpc82x_sct_config(p_hw_sct,
                            /* 32Ϊ��ʱ��            */
                           AMHW_LPC82X_SCT_CFG_32BIT_COUNTER  |
                           /* ϵͳʱ��              */
                           AMHW_LPC82X_SCT_CFG_CLKMODE_SYSCLK |
                           /* ƥ��Ĵ���0Ϊ�Զ����� */
                           AMHW_LPC82X_SCT_CFG_AUTOLIMIT_L );
   
    /* ����ƥ��/����Ĵ���Ϊƥ��ģʽ  */
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY,
                                   AMHW_LPC82X_SCT_MAT(0),
                                   AMHW_LPC82X_SCT_MATCAP_MATCH);
                            
    amhw_lpc82x_sct_mat_val_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY,
                                          AMHW_LPC82X_SCT_MAT(0), 0);
    
    /* �����¼�0�Ĵ��� */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             /* �¼�0                */
                             AMHW_LPC82X_SCT_EVT(0),
                             /* ����ƥ��Ĵ���0      */
                             AMHW_LPC82X_SCT_EVT_CTRL_MATCHSEL(0)    |
                             /* ����ʹ��ָ����ƥ��   */
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH |
                             /* ͨ��������ı�״ֵ̬ */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD    |
                             /* ���� STATEV ֵ       */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));

    /* ʹ���¼� 0 ��״̬ 0 */
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0),
                                                 AMHW_LPC82X_SCT_STATE(0));

    /* �����������Ϊ0 */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);
    
    /* ����״ֵ̬Ϊ0 */
    amhw_lpc82x_sct_state_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY,
                                        AMHW_LPC82X_SCT_STATE(0));
    
    /* ����Ԥ��ƵΪ0.ʹ��ϵͳʱ�ӣ���ƵΪ0 */
    amhw_lpc82x_sct_prescale_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY, 0);
}

/**
 * \brief ��ʼ�� SCT Ϊ PWM �������
 */
am_pwm_handle_t am_lpc82x_sct_pwm_init (am_lpc82x_sct_pwm_dev_t     *p_dev,
                                  const am_lpc82x_sct_pwm_devinfo_t *p_devinfo)
{
    amhw_lpc82x_sct_t  *p_hw_sct = NULL;
    
    if (p_dev == NULL || p_devinfo == NULL) {
        return NULL;
    }
    
    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    p_hw_sct                = (amhw_lpc82x_sct_t *)(p_devinfo->sct_regbase);
    p_dev->p_devinfo        = p_devinfo;
    p_dev->pwm_serv.p_funcs = (struct am_pwm_drv_funcs *)&__g_sct_pwm_drv_funcs;
    p_dev->pwm_serv.p_drv   = p_dev;
    
    __sct_pwm_init(p_hw_sct);

    return &(p_dev->pwm_serv);
}
                                     
/**
 * \brief ��ʹ�� SCT PWM �������ʱ�����ʼ�� SCT PWM ������ܣ��ͷ������Դ
 */
void am_lpc82x_sct_pwm_deinit (am_pwm_handle_t handle)
{
    am_lpc82x_sct_pwm_dev_t *p_dev    = (am_lpc82x_sct_pwm_dev_t *)handle;
    amhw_lpc82x_sct_t       *p_hw_sct = NULL;

    if (p_dev == NULL || p_dev->p_devinfo == NULL ) {
        return ;
    }

    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);
 
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);

    p_dev->pwm_serv.p_drv = NULL;

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

/* end of file */
