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
 * \brief SCT捕获 驱动实现
 *
 * 1. SCT支持提供如下三种标准服务，本驱动提供的是服务CAP标准服务的驱动。
 *     - 定时
 *     - PMW
 *     - 捕获
 * 2. 资源利用如下所示：
 *    - 捕获通道0用于捕获SCT输入0，与事件0相关联，以产生中断
 *    - 捕获通道1用于捕获SCT输入1，与事件1相关联，以产生中断
 *    - 捕获通道2用于捕获SCT输入2，与事件2相关联，以产生中断
 *    - 捕获通道3用于捕获SCT输入3，与事件3相关联，以产生中断
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
* 函数声明
*******************************************************************************/

/** \brief 配置 SCT 捕获  */
static int __sct_cap_config(void              *p_cookie,
                            int                chan,
                            unsigned int       flags,
                            am_cap_callback_t  pfn_callback,
                            void              *p_arg);

/** \brief 使能 SCT 捕获 */
static int __sct_cap_enable(void *p_drv, int chan);

/** \brief 禁能SCT捕获 */
static int __sct_cap_disable(void *p_drv, int chan);

/** \brief 复位SCT捕获 */
static int __sct_cap_reset(void *p_drv, int chan);

/** \brief SCT捕获计算*/
static int __sct_cap_count_to_time (void         *p_drv,
                                    int           chan,
                                    unsigned int  count1,
                                    unsigned int  count2,
                                    unsigned int *p_time_ns);

/** \brief SCT捕获中断服务函数*/
static void __sct_cap_irq_handler(void *p_arg);

/** \brief SCT 捕获 驱动函数 */
static const struct am_cap_drv_funcs __g_sct_cap_drv_funcs = {
    __sct_cap_config,
    __sct_cap_enable,
    __sct_cap_disable,
    __sct_cap_reset,
    __sct_cap_count_to_time,
};

/******************************************************************************/

/** \brief 配置 SCT 捕获  */
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
    
    /* 有效通道为  0 ~ (channels_num - 1) */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }
    
    /* 支持上升沿 */
    if ((options & AM_CAP_TRIGGER_RISE) == AM_CAP_TRIGGER_RISE) {
        iocond = AMHW_LPC82X_SCT_EVT_CTRL_IOCOND_RISE;
    }

    /* 支持下降沿 */
    if ((options & AM_CAP_TRIGGER_FALL) == AM_CAP_TRIGGER_FALL) {
        iocond = AMHW_LPC82X_SCT_EVT_CTRL_IOCOND_FALL;
    }
    
    if ((options & AM_CAP_TRIGGER_BOTH_EDGES) == AM_CAP_TRIGGER_BOTH_EDGES) {
        return -AM_ENOTSUP; /* 不支持双边沿 */
    }

    /* 根据SCT CAP用户配置文件所设定的引脚、及其引脚功能，进行配置 */
    am_gpio_pin_cfg((p_ioinfo + chan)->gpio, (p_ioinfo + chan)->func);

    /* 设置SCT触发信号源为SCT输入通道chan */
    amhw_lpc82x_inmux_sct_trig_set(AMHW_LPC82X_INMUX, chan,
                                  (amhw_lpc82x_inmux_sct_trig_src_t)\
                                  (AMHW_LPC82X_INMUX_SCTTRIG_SCT_PIN0 + chan));

    /* 将匹配捕捉寄存器配置为捕获功能 */
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   /* 联合模式 使用32位计数器 */
                                   AMHW_LPC82X_SCT_MODE_UNIFY,
                                   /* 使用通道chan */
                                   AMHW_LPC82X_SCT_MAT(chan),
                                   /* 寄存器工作在捕获状态    */
                                   AMHW_LPC82X_SCT_MATCAP_CAPTURE);
    
    /* 使能事件原因 CAPn _L (UNIFY = 0) 寄存器重加载 */
    amhw_lpc82x_sct_cap_ctrl(p_hw_sct,
                             AMHW_LPC82X_SCT_MODE_UNIFY,
                             AMHW_LPC82X_SCT_CAP(chan),
                             AMHW_LPC82X_SCT_EVT(chan),
                             TRUE);
    
    /* 使用事件编号同于通道号。事件只能是IO条件，不能用匹配寄存器 */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             AMHW_LPC82X_SCT_EVT(chan),
                             AMHW_LPC82X_SCT_EVT_CTRL_OUTSEL_INPUT |
                             AMHW_LPC82X_SCT_EVT_CTRL_IOSEL(chan)  |
                             iocond                                |
                             /* 使用指定IO           */
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_IO  |
                             /*  STATEV 值加上 STATE */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD  |
                             /* 加 0，STATE没有改变  */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));
    
    p_dev->callback_info[chan].p_arg         = p_arg;
    p_dev->callback_info[chan].callback_func = pfn_callback;

    return AM_OK;
}

/** \brief 使能 SCT 捕获 */
static int __sct_cap_enable (void *p_drv, int chan)
{
    am_lpc82x_sct_cap_dev_t    *p_dev    ;
    amhw_lpc82x_sct_t          *p_hw_sct ;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_cap_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t       *)(p_dev->p_devinfo->sct_regbase);

    /* 有效通道为  0 ~ (channels_num - 1) */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }

    /* 使能状态0中事件0产生 */
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct,
                                     AMHW_LPC82X_SCT_EVT(chan),
                                     AMHW_LPC82X_SCT_STATE(0));

    /* 清除事件标志 */
    amhw_lpc82x_sct_evt_flag_clr(p_hw_sct, AMHW_LPC82X_SCT_EVT(chan));

    /* 使能事件产生中断 */
    amhw_lpc82x_sct_evt_int_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(chan));

    /* 清零控制寄存器的相应位 */
    amhw_lpc82x_sct_ctrl_clr(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);

    am_int_connect(p_dev->p_devinfo->inum,
                   __sct_cap_irq_handler, (void *)p_dev);
    am_int_enable(p_dev->p_devinfo->inum);
    
    return AM_OK;
}

/** \brief 禁能SCT捕获 */
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

    /* 有效通道为  0 ~ (channels_num - 1) */
    if (chan >= p_dev->p_devinfo->channels_num) {
        return -AM_EINVAL;
    }


    /* 禁能状态0中事件0产生 */
    amhw_lpc82x_sct_evt_state_disable(p_hw_sct, AMHW_LPC82X_SCT_EVT(chan),
                                                AMHW_LPC82X_SCT_STATE(0));
    
    amhw_lpc82x_sct_evt_int_disable(p_hw_sct, AMHW_LPC82X_SCT_EVT(chan));

    i = p_dev->p_devinfo->channels_num;
    
    for (i = p_dev->p_devinfo->channels_num - 1; i >= 0; i--) {
        if (p_hw_sct->event[i].state != 0) {
            return AM_OK;
        }
    }
    
    /* 停止、终止低16位定时器 */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);
    
    return AM_OK;
}

/** \brief 复位SCT捕获 */
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
    
        /* 清零计数器 */
        amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);
    } else {

        /* 置HALT位,终止SCT */
        amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_HALT_L);

        /* 清零计数器 */
        amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);

        amhw_lpc82x_sct_ctrl_clr(p_hw_sct, AMHW_LPC82X_SCT_CTRL_HALT_L);
    }
    
    return AM_OK;
}

/** \brief SCT捕获计算 */
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

    /* 转换计数值为时间 */
    time_ns = (uint64_t)1000000000 * (uint64_t)count_err / (uint64_t)clkfreq ;

    *p_time_ns = time_ns;

    return AM_OK;
}

/** \brief SCT捕获中断服务函数 */
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
            
            /* 清事件 i 标志位 */
            amhw_lpc82x_sct_evt_flag_clr(p_hw_sct, AMHW_LPC82X_SCT_EVT(i));
            
        }
    }
}

/**
 * \brief 初始化SCT以CAP捕获
 *
 * \param[in] p_sct : 指向SCT寄存器块的指针
 *
 * \return 无
 */
void __sct_cap_init (amhw_lpc82x_sct_t *p_hw_sct)
{
    /* 置停止位，SCT停止运行 */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);

    amhw_lpc82x_sct_config(p_hw_sct,
                           AMHW_LPC82X_SCT_CFG_32BIT_COUNTER  |
                           AMHW_LPC82X_SCT_CFG_CLKMODE_SYSCLK |
                           AMHW_LPC82X_SCT_CFG_INSYNC_ALL);

    /* 置CLRCTR位以清计数器为0 */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);
    
    /* 设置当前状态值为0 */
    amhw_lpc82x_sct_state_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY,
                                        AMHW_LPC82X_SCT_STATE(0));

    /* 预分频为0，使用系统时钟 */
    amhw_lpc82x_sct_prescale_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY, 0);
}


/** \brief 初始化SCT捕获 */
am_cap_handle_t am_lpc82x_sct_cap_init (am_lpc82x_sct_cap_dev_t     *p_dev,
                                  const am_lpc82x_sct_cap_devinfo_t *p_devinfo)
{
    amhw_lpc82x_sct_t *p_hw_sct;
    uint8_t    i;
    
    if (p_dev == NULL || p_devinfo == NULL) {
        return NULL;
    }

    /* The SCT 使用的通道数，最大为 4 */
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

    /* 复位SCT计数器和预分频计数器为 0 */
    __sct_cap_init(p_hw_sct);

    return &(p_dev->cap_serv);
}

/** \brief 解初始化SCT捕获功能 */
void am_lpc82x_sct_cap_deinit (am_cap_handle_t handle)
{
    am_lpc82x_sct_cap_dev_t *p_dev     = (am_lpc82x_sct_cap_dev_t *)handle;
    amhw_lpc82x_sct_t       *p_hw_sct  = NULL;

    if (p_dev                      == NULL ||
        p_dev->p_devinfo           == NULL ) {
        return ;
    }

    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);

    /* 终止SCT */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);

    am_int_disable(p_dev->p_devinfo->inum);

    p_dev->cap_serv.p_drv = NULL;

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

/* end of file */
