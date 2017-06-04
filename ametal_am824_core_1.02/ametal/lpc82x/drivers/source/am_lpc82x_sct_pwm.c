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
 * \brief SCT PWM 驱动，服务于 PWM 标准接口
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

/** \brief 配置 SCT PWM  */
static int __sct_pwm_config (void          *p_drv,
                             int            chan,
                             unsigned long  duty_ns,
                             unsigned long  period_ns);

/** \brief 使能 SCT PWM 输出 */
static int __sct_pwm_enable(void *p_drv, int chan);

/** \brief 禁能 SCT PWM 输出 */
static int __sct_pwm_disable(void *p_drv, int chan);

/**
 * \brief SCT PWM 驱动函数定义
 */
static const struct am_pwm_drv_funcs __g_sct_pwm_drv_funcs = {
    __sct_pwm_config,
    __sct_pwm_enable,
    __sct_pwm_disable,
};

/******************************************************************************/

/**
 * \brief 配置 SCT PWM
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
     * 合法通道号是 0 ~ (channels_num - 1)
     */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }
    
    clkfreq = amhw_lpc82x_clk_periph_freq_get(p_hw_sct);
     
    period_c = (uint64_t)(period_ns) * (clkfreq) / (uint64_t)1000000000;
    duty_c   = (uint64_t)(duty_ns)   * (clkfreq) / (uint64_t)1000000000;
 
    
    /**
     * period 周期至少为1
     */
    if (period_c == 0) {
        period_c = 1;
    }
    
    /**
     * 检查 SCT 是否处于终止状态, 匹配寄存器0存放 period 周期
     */
    if (amhw_lpc82x_sct_halt_check(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY) == TRUE ) {

        amhw_lpc82x_sct_mat_val_set(p_hw_sct,
                                    AMHW_LPC82X_SCT_MODE_UNIFY,
                                    AMHW_LPC82X_SCT_MAT(0),
                                    period_c - 1);
    }
    
    /* 设置匹配 period 周期重载值*/
    amhw_lpc82x_sct_mat_reload_val_set(p_hw_sct,
                                       AMHW_LPC82X_SCT_MODE_UNIFY,
                                       AMHW_LPC82X_SCT_MAT(0),
                                       period_c - 1);
     
    /**
     * duty 周期至少为1
     */
    if (duty_c == 0) {
        duty_c = 1;
    }
    
    /**
     * 匹配寄存器 chan+1 存放 duty 周期
     */
    if (amhw_lpc82x_sct_halt_check(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY) == TRUE ) {

        amhw_lpc82x_sct_mat_val_set(p_hw_sct,
                                    AMHW_LPC82X_SCT_MODE_UNIFY,
                                    chan + 1,
                                    duty_c - 1);
    }
    
    /* 设置匹配 duty 周期重载值  */
    amhw_lpc82x_sct_mat_reload_val_set(p_hw_sct,
                                AMHW_LPC82X_SCT_MODE_UNIFY,
                                chan + 1,
                                duty_c - 1);

    return AM_OK;
}

/**
 * \brief 使能 SCT PWM 输出
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
     * 合法的通道是  0 ~ (channels_num - 1)
     */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }
    
    /* 配置匹配/捕获寄存器为匹配模式 */
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY,
                                   match_num,
                                   AMHW_LPC82X_SCT_MATCAP_MATCH);
    
    /* 事件编号使用匹配寄存器通道号 */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             event_num,
                             AMHW_LPC82X_SCT_EVT_CTRL_MATCHSEL(match_num) |
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH      |
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD         |
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));
    
    am_gpio_pin_cfg(p_ioinfo[chan].gpio, p_ioinfo[chan].func);
    
    /* 使能事件0发生 */
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct, event_num,
                                     AMHW_LPC82X_SCT_STATE(0));
    
    /* 事件0设置为输出 */
    amhw_lpc82x_sct_out_set_enable(p_hw_sct,out_num,
                                   AMHW_LPC82X_SCT_EVT(0));
    
    /* 清除 PWM 输出 */
    amhw_lpc82x_sct_out_clr_enable(p_hw_sct, out_num, event_num);
    
    /**
     * 当 duty == period , 冲突会发生。这种情况下，duty的百分比应为100%,
     * 所以，要配置冲突解决
     */
    amhw_lpc82x_sct_conflict_res_config(p_hw_sct, out_num,
                                        AMHW_LPC82X_SCT_RES_OUT_SET);

    /* 设置和清除不依赖于方向 */
    amhw_lpc82x_sct_out_dir_ctrl(p_hw_sct, out_num,
                                 AMHW_LPC82X_SCT_OUTPUTDIRCTRL_INDEPENDENT);
    
    /* 清除终止和停止位，启动 SCT */
    amhw_lpc82x_sct_ctrl_clr(p_hw_sct,
                             AMHW_LPC82X_SCT_CTRL_STOP_L |
                             AMHW_LPC82X_SCT_CTRL_HALT_L);

    return AM_OK;
}

/**
 * \brief 禁能 SCT PWM 输出
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
    
    /* 合法的通道号是 0 ~ (channels_num - 1) */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }

    am_gpio_pin_cfg(p_ioinfo[chan].gpio, p_ioinfo[chan].dfunc);
  
    /* 禁能事件0 */
    amhw_lpc82x_sct_evt_state_disable(p_hw_sct, event_num,
                                      AMHW_LPC82X_SCT_STATE(0));
    
    /* 禁能事件0 PWM 输出 */
    amhw_lpc82x_sct_out_set_disable(p_hw_sct, out_num, AMHW_LPC82X_SCT_EVT(0));
    
    /* 禁能事件输出清除  */
    amhw_lpc82x_sct_out_clr_disable(p_hw_sct, out_num, event_num);

    i = p_dev->p_devinfo->channels_num;
    
    for (i = p_dev->p_devinfo->channels_num - 1; i >= 0; i--) {
        if (p_hw_sct->out[i].clr != 0 || p_hw_sct->out[i].set != 0) {
            return AM_OK;
        }
    }
    
    /* 禁能所有通道的 PWM 输出，禁能 SCT */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);
   
    return AM_OK;
}

/** 
 * \brief 初始化 SCT 为 PWM 输出
 *
 * \param[in] p_hw_sct : 指向 SCT 寄存器结构体指针
 *
 * \return 无
 */
void __sct_pwm_init (amhw_lpc82x_sct_t *p_hw_sct)
{
    /* 关闭 SCT */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);
    
    /* 配置 SCT 匹配寄存器0 为自动限制  */
    amhw_lpc82x_sct_config(p_hw_sct,
                            /* 32为定时器            */
                           AMHW_LPC82X_SCT_CFG_32BIT_COUNTER  |
                           /* 系统时钟              */
                           AMHW_LPC82X_SCT_CFG_CLKMODE_SYSCLK |
                           /* 匹配寄存器0为自动限制 */
                           AMHW_LPC82X_SCT_CFG_AUTOLIMIT_L );
   
    /* 配置匹配/捕获寄存器为匹配模式  */
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_UNIFY,
                                   AMHW_LPC82X_SCT_MAT(0),
                                   AMHW_LPC82X_SCT_MATCAP_MATCH);
                            
    amhw_lpc82x_sct_mat_val_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY,
                                          AMHW_LPC82X_SCT_MAT(0), 0);
    
    /* 设置事件0寄存器 */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             /* 事件0                */
                             AMHW_LPC82X_SCT_EVT(0),
                             /* 关联匹配寄存器0      */
                             AMHW_LPC82X_SCT_EVT_CTRL_MATCHSEL(0)    |
                             /* 仅仅使用指定的匹配   */
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH |
                             /* 通过加运算改变状态值 */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD    |
                             /* 定义 STATEV 值       */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));

    /* 使能事件 0 的状态 0 */
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0),
                                                 AMHW_LPC82X_SCT_STATE(0));

    /* 将计数器清除为0 */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);
    
    /* 设置状态值为0 */
    amhw_lpc82x_sct_state_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY,
                                        AMHW_LPC82X_SCT_STATE(0));
    
    /* 设置预分频为0.使用系统时钟，分频为0 */
    amhw_lpc82x_sct_prescale_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY, 0);
}

/**
 * \brief 初始化 SCT 为 PWM 输出功能
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
 * \brief 不使用 SCT PWM 输出功能时，解初始化 SCT PWM 输出功能，释放相关资源
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
