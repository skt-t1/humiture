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
 * \brief Timing driver implementation for SCT
 *
 * 1. The SCT can provide 3 service as follows,This driver implement
 *    the timing function.
 *     - Timing
 *     - PWM
 *     - CAP
 * 2. In the driver,The SCT resource used as follows:
 *    - The match 0 to save the period time,associate with event 0 to generate
 *      interrupt.
 *
 * \internal
 * \par Modification history
 * - 1.03 15-12-14  hgo, fix potential bug caused by incorrect p_arg on callback
 * - 1.02 15-11-20  hgo, modify macro param and add two funcs.
 * - 1.01 15-09-24  bob, modify prescale settings.
 * - 1.00 15-07-07  oce, first implementation.
 * \endinternal
 */
#include "ametal.h"
#include "am_int.h"
#include "am_gpio.h"
#include "am_lpc82x_sct_timing.h"
#include "amhw_lpc82x_clk.h"
#include "amhw_lpc82x_sct.h"
/*******************************************************************************
  Functions declaration
*******************************************************************************/

/* lpc82x SCT for timing drv functions declaration */
static int __sct_timing_info_get (void *p_drv, am_timer_info_t *p_info);

static int __sct_timing_count_get (void     *p_drv,
                                   uint8_t   chan,
                                   uint32_t *p_count);

static int __sct_timing_rollover_get (void     *p_drv,
                                      uint8_t   chan,
                                      uint32_t *p_rollover);
                                     
static int __sct_timing_disable (void *p_drv, uint8_t chan);

static int __sct_timing_enable (void     *p_drv,
                                uint8_t   chan,
                                uint32_t  max_sct_count);
    
static int __sct_timing_callback_set (void    *p_drv,
                                      uint8_t  chan,
                                      void   (*pfn_callback)(void *),
                                      void    *p_arg);
                                    
static void __sct_irq_handler (void *p_arg);

static int __prescale_and_match_cal (uint32_t count, uint32_t *p_match);
    
/** \brief SCT for timing driver functions */
static const struct am_timer_drv_funcs __g_sct_timing_drv_funcs = {
    __sct_timing_info_get,
    __sct_timing_count_get,
    __sct_timing_rollover_get,
    __sct_timing_disable,
    __sct_timing_enable,
    __sct_timing_callback_set,
    NULL,
    NULL,
    NULL,
};

/******************************************************************************/

/**
 * \brief Calculate 16 bits timer prescale(8-bit) and
 *        match value(16-bit) for a given count
 *
 * \param[in]  count   : number of clocks the 16bits timer count
 * \param[out] p_match : pointer to hold the timer match value
 *
 * \return the 8-bit prescale value calculated
 */
static int __prescale_and_match_cal (uint32_t count, uint32_t *p_match)
{
    uint32_t match, min_remainder, prescale;
    uint32_t result_quot, result_rem;

    if (NULL == p_match) {
        return -AM_EINVAL;
    }
 
    min_remainder = count;
    prescale      = 1;
   *p_match       = 65536;

    /* 
     * the count is less than 65536,
     * the prescale set to 1, and the match equal to count
     */
    if (count <= 65536) {
        prescale = 1;
       *p_match  = count;
        return prescale;
    }
    
    /* check match value one by one until find a best one */
    for (match = 65536; match > 0; match--) {
        result_quot = count / match;
        result_rem  = count % match;
        
        /* 
         * because the width of prescale value is 8-bits,
         * the result_quot maximum
         * value is 256, if beyond 256,the count is can't be reached.
         */
        if (result_quot <= 256) {
            if (result_rem == 0) {
            
                /* find a best prescale and match value */
                prescale = result_quot;
                *p_match = match;
                break;
                
            } else if (result_rem < min_remainder) {
            
                /* find a better prescale and match value */
                min_remainder = result_rem;
                prescale      = result_quot;
               *p_match       = match;
            }
        }
    }

    return prescale;
}

static void __sct_irq_handler (void *p_arg)
{
    am_lpc82x_sct_timing_dev_t *p_dev    = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;

    if (NULL == p_arg) {
        return ;
    }

    p_dev    = (am_lpc82x_sct_timing_dev_t *)p_arg;
    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);

    /* 
     * Because use the event 0 to generate interrupt
     * So,Check if the event 0 interrupt
     */
    if (amhw_lpc82x_sct_evt_flag_chk(p_hw_sct,
                                       AMHW_LPC82X_SCT_EVT(0)) == TRUE ) {
        
        if (p_dev->callback_info[0].pfn_callback != NULL) {
            p_dev->callback_info[0].pfn_callback(p_dev->callback_info[0].p_arg);
        }

        /* Clear the event 0 flag */
        amhw_lpc82x_sct_evt_flag_clr(p_hw_sct, AMHW_LPC82X_SCT_EVT(0));
    }
    
    if (amhw_lpc82x_sct_evt_flag_chk(p_hw_sct,
                                       AMHW_LPC82X_SCT_EVT(1)) == TRUE ) {
        
        if (p_dev->callback_info[1].pfn_callback != NULL) {
            p_dev->callback_info[1].pfn_callback(p_dev->callback_info[1].p_arg);
        }

        /* Clear the event 0 flag */
        amhw_lpc82x_sct_evt_flag_clr(p_hw_sct, AMHW_LPC82X_SCT_EVT(1));
    }
}

/** \brief Get the SCT information */
static int __sct_timing_info_get (void *p_drv, am_timer_info_t *p_info)
{
    am_lpc82x_sct_timing_dev_t *p_dev    = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;

    if ((NULL == p_drv) || (NULL == p_info)) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_timing_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);
    
    /* Get the input frequency */
    p_info->clk_frequency = amhw_lpc82x_clk_periph_freq_get(p_hw_sct);
    p_info->max_frequency = p_info->clk_frequency;
    
    if (p_dev->p_devinfo->flag == AM_LPC82X_SCT_TIMING_1_32BIT) {
        /* The minimum freq is 1Hz */
        p_info->min_frequency = 1;
        
        p_info->features  = AM_TIMER_SIZE(32)           | /* 32-bit SCT */
                            /* support 1 channels      */
                            AM_TIMER_CHAN_NUM(1)        |
                            /* Can generate interrupt  */
                            AM_TIMER_CAN_INTERRUPT      |
                            /* can read now count      */
                            AM_TIMER_INTERMEDIATE_COUNT |
                            /* Auto reload             */
                            AM_TIMER_AUTO_RELOAD;
        
    } else {
        
        /* 8-bit pre-scale,16-bit timer */
        if (p_info->clk_frequency <= 0xFFFFFF) {
            p_info->min_frequency = 1;
        } else {
            p_info->min_frequency = p_info->clk_frequency / 0xFFFFFF;
            
            if (p_info->min_frequency * 0xFFFFFF < p_info->clk_frequency) {
                p_info->min_frequency += 1;
            }
        }
            
        p_info->features  = AM_TIMER_SIZE(16)           | /* 16-bit SCT    */
                            /* support 2 channels     */
                            AM_TIMER_CHAN_NUM(2)        |
                            /* Can generate interrupt */
                            AM_TIMER_CAN_INTERRUPT      |
                            /* can read now count     */
                            AM_TIMER_INTERMEDIATE_COUNT |
                            /* Auto reload            */
                            AM_TIMER_AUTO_RELOAD;
    
    }
    
    return AM_OK;
}

/** \brief Get the SCT Now counter value */
static int __sct_timing_count_get (void     *p_drv,
                                   uint8_t   chan,
                                   uint32_t *p_count)
{
    am_lpc82x_sct_timing_dev_t *p_dev    = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_timing_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);
    
    if (p_dev->p_devinfo->flag == AM_LPC82X_SCT_TIMING_1_32BIT) {
        if (chan != 0) {
            return -AM_EINVAL;
        }

        *p_count = amhw_lpc82x_sct_count_get(p_hw_sct,
                                             AMHW_LPC82X_SCT_MODE_UNIFY);
        
    } else {
        if (chan == 0) {
            *p_count = amhw_lpc82x_sct_count_get(p_hw_sct,
                                             AMHW_LPC82X_SCT_MODE_SEPARATE_L);
        } else if (chan == 1) {
            *p_count = amhw_lpc82x_sct_count_get(p_hw_sct,
                                             AMHW_LPC82X_SCT_MODE_SEPARATE_H);
        }
        
        return -AM_EINVAL;
    }
    return AM_OK;
}

/** \brief The SCT roll over value get */
static int __sct_timing_rollover_get (void     *p_drv,
                                      uint8_t   chan,
                                      uint32_t *p_rollover)
{
    am_lpc82x_sct_timing_dev_t *p_dev    = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_timing_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t          *)(p_dev->p_devinfo->sct_regbase);
    
    if (p_dev->p_devinfo->flag == AM_LPC82X_SCT_TIMING_1_32BIT) {
        if (chan != 0) {
            return -AM_EINVAL;
        }

        *p_rollover = 0xFFFFFFFF / amhw_lpc82x_clk_periph_freq_get(p_hw_sct);
        
    } else {
        if (chan >= 2) {
            return -AM_EINVAL;
        }
        
        *p_rollover = 0xFFFFFF / amhw_lpc82x_clk_periph_freq_get(p_hw_sct);
    }
    
    return AM_OK;
}

/** \brief The SCT disable */
static int __sct_timing_disable (void *p_drv, uint8_t chan)
{
    am_lpc82x_sct_timing_dev_t *p_dev    = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;
    uint8_t                     i;
    
    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_timing_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t          *)(p_dev->p_devinfo->sct_regbase);
    
    if (p_dev->p_devinfo->flag == AM_LPC82X_SCT_TIMING_1_32BIT) {
        if (chan != 0) {
            return -AM_EINVAL;
        }
        /* Set the halt and stop bits,the SCT L counter is stop */
        amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                           AMHW_LPC82X_SCT_CTRL_HALT_L);
        amhw_lpc82x_sct_count_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY, 0);
        
    } else {
        if (chan == 0) {
            /* high group is runing */
            if (amhw_lpc82x_sct_halt_check(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_SEPARATE_H) == FALSE &&
                amhw_lpc82x_sct_stop_check(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_SEPARATE_H) == FALSE) {

                for (i = 0; i < AMHW_LPC82X_SCT_OUT_NUM; i++) {
                     /*
                      * sct output has connected,
                      * needed low group in non-halt state
                      */
                    if (p_hw_sct->out[i].set & AM_BIT(AMHW_LPC82X_SCT_EVT(1))) {
                        /* low group should be set to non-halt */
                        amhw_lpc82x_sct_ctrl_halt_to_stop(p_hw_sct,
                                               AMHW_LPC82X_SCT_MODE_SEPARATE_L);
                        break;
                    }
                }
            }
            if (amhw_lpc82x_sct_stop_check(p_hw_sct,
                                    AMHW_LPC82X_SCT_MODE_SEPARATE_L) == FALSE) {
                /* Set the halt and stop bits,the SCT L counter is stop */
                amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                                   AMHW_LPC82X_SCT_CTRL_HALT_L);
            }
            amhw_lpc82x_sct_count_set(p_hw_sct,
                                      AMHW_LPC82X_SCT_MODE_SEPARATE_L, 0);
            
        } else if (chan == 1) {
            
            /* Set the halt and stop bits,the SCT L counter is stop */
            amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_H |
                                               AMHW_LPC82X_SCT_CTRL_HALT_H);
            amhw_lpc82x_sct_count_set(p_hw_sct,
                                      AMHW_LPC82X_SCT_MODE_SEPARATE_H, 0);
            
        }
        
        return -AM_EINVAL;
    }

    return AM_OK;
}

/** \brief The SCT enable and set the max count value */
static int __sct_timing_enable (void     *p_drv, 
                                uint8_t   chan,
                                uint32_t  max_sct_count)
{
    am_lpc82x_sct_timing_dev_t *p_dev    = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;

    uint8_t  i;
    uint8_t  prescale;
    uint32_t match;

    
    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_timing_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t          *)(p_dev->p_devinfo->sct_regbase);

    if (p_dev->p_devinfo->flag == AM_LPC82X_SCT_TIMING_1_32BIT) {
        if (chan != 0) {
            return -AM_EINVAL;
        }

        /* Before change the count value, halt the counter */
        amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                           AMHW_LPC82X_SCT_CTRL_HALT_L);
        amhw_lpc82x_sct_count_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY, 0);
        
        amhw_lpc82x_sct_mat_val_set(p_hw_sct,
                                    AMHW_LPC82X_SCT_MODE_UNIFY,
                                    AMHW_LPC82X_SCT_MAT(0),
                                    max_sct_count);
        amhw_lpc82x_sct_mat_reload_val_set(p_hw_sct,
                                           AMHW_LPC82X_SCT_MODE_UNIFY,
                                           AMHW_LPC82X_SCT_MAT(0),
                                           max_sct_count);
        
        if (p_dev->callback_info[0].pfn_callback == NULL) {
            amhw_lpc82x_sct_evt_int_disable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0));
        } else {
            amhw_lpc82x_sct_evt_int_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0));
        }
        
        /* start counting */
        amhw_lpc82x_sct_ctrl_clr(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                           AMHW_LPC82X_SCT_CTRL_HALT_L);
        
    } else {

        /* 16bit timer + 8bit prescale support timing 0xFFFFFF counts*/
        if (max_sct_count > 0xFFFFFF || chan >= 2) {
            return -AM_EINVAL;
        }
        
        prescale = __prescale_and_match_cal(max_sct_count, &match);
        
        if (chan == 0) {
            
            /* Before change the count value, halt the counter */
            amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                               AMHW_LPC82X_SCT_CTRL_HALT_L);
            amhw_lpc82x_sct_count_set(p_hw_sct,
                                      AMHW_LPC82X_SCT_MODE_SEPARATE_L, 0);
            
            amhw_lpc82x_sct_prescale_set(p_hw_sct,
                                         AMHW_LPC82X_SCT_MODE_SEPARATE_L,
                                         prescale - 1);
            
            amhw_lpc82x_sct_mat_val_set(p_hw_sct,
                                        AMHW_LPC82X_SCT_MODE_SEPARATE_L,
                                        AMHW_LPC82X_SCT_MAT(0),
                                        match);
            amhw_lpc82x_sct_mat_reload_val_set(p_hw_sct,
                                               AMHW_LPC82X_SCT_MODE_SEPARATE_L,
                                               AMHW_LPC82X_SCT_MAT(0),
                                               match);
            
            if (p_dev->callback_info[0].pfn_callback == NULL) {
                amhw_lpc82x_sct_evt_int_disable(p_hw_sct,
                                                  AMHW_LPC82X_SCT_EVT(0));
            } else {
                amhw_lpc82x_sct_evt_int_enable(p_hw_sct,
                                                 AMHW_LPC82X_SCT_EVT(0));
            }
            
            /* start counting */
            amhw_lpc82x_sct_ctrl_clr(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                               AMHW_LPC82X_SCT_CTRL_HALT_L);
            
        } else if (chan == 1) {
            
            /* Before change the count value, halt the counter */
            amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_H |
                                               AMHW_LPC82X_SCT_CTRL_HALT_H);
            amhw_lpc82x_sct_count_set(p_hw_sct,
                                      AMHW_LPC82X_SCT_MODE_SEPARATE_H, 0);
            
            amhw_lpc82x_sct_prescale_set(p_hw_sct,
                                         AMHW_LPC82X_SCT_MODE_SEPARATE_H,
                                         prescale - 1);
            
            amhw_lpc82x_sct_mat_val_set(p_hw_sct,
                                        AMHW_LPC82X_SCT_MODE_SEPARATE_H,
                                        AMHW_LPC82X_SCT_MAT(0),
                                        match);
            amhw_lpc82x_sct_mat_reload_val_set(p_hw_sct,
                                               AMHW_LPC82X_SCT_MODE_SEPARATE_H,
                                               AMHW_LPC82X_SCT_MAT(0),
                                               match);
            
            for (i = 0; i < AMHW_LPC82X_SCT_OUT_NUM; i++) {
                if (p_hw_sct->out[i].set & AM_BIT(AMHW_LPC82X_SCT_EVT(1))) {
                    /*
                     * sct output has connected,
                     * need low group in non-halt state
                     */
                    if (amhw_lpc82x_sct_halt_check(p_hw_sct,
                                     AMHW_LPC82X_SCT_MODE_SEPARATE_L) == TRUE) {
            
                        /* low group should be set to non-halt */
                        amhw_lpc82x_sct_ctrl_halt_to_stop(p_hw_sct, AMHW_LPC82X_SCT_MODE_SEPARATE_L);
                    }
                    break;
                }
            }
            
            if (p_dev->callback_info[1].pfn_callback == NULL) {
                amhw_lpc82x_sct_evt_int_disable(p_hw_sct,
                                                  AMHW_LPC82X_SCT_EVT(1));
            } else {
                amhw_lpc82x_sct_evt_int_enable(p_hw_sct,
                                                 AMHW_LPC82X_SCT_EVT(1));
            }
            
            /* start counting */
            amhw_lpc82x_sct_ctrl_clr(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_H |
                                               AMHW_LPC82X_SCT_CTRL_HALT_H);
        }
    }

    return AM_OK;
}

/** \brief set the function to be called on every interrupt */
static int __sct_timing_callback_set (void     *p_drv,
                                      uint8_t   chan,
                                      void    (*pfn_callback)(void *),
                                      void     *p_arg)
{
    am_lpc82x_sct_timing_dev_t *p_dev    = NULL;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;

    if (NULL == p_drv) {
        return -AM_EINVAL;
    }

    p_dev    = (am_lpc82x_sct_timing_dev_t *)p_drv;
    p_hw_sct = (amhw_lpc82x_sct_t          *)(p_dev->p_devinfo->sct_regbase);
    
    if (p_dev->p_devinfo->flag == AM_LPC82X_SCT_TIMING_1_32BIT) {
        if (chan != 0) {
            return -AM_EINVAL;
        }
        
        /* needn't interrupt, disable the event 0 interrupt */
        if (pfn_callback == NULL) {
            amhw_lpc82x_sct_evt_int_disable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0));
            
        } else {
            p_dev->callback_info[0].p_arg        = p_arg;
            p_dev->callback_info[0].pfn_callback = pfn_callback;
            
            amhw_lpc82x_sct_evt_int_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0));
        }
    } else {
        if (chan >= 2) {
            return -AM_EINVAL;
        }
        
        /* needn't interrupt, disable the event 0 interrupt */
        if (pfn_callback == NULL) {
            amhw_lpc82x_sct_evt_int_disable(p_hw_sct,
                                              AMHW_LPC82X_SCT_EVT(chan));
            
        } else {
            p_dev->callback_info[chan].p_arg        = p_arg;
            p_dev->callback_info[chan].pfn_callback = pfn_callback;
            
            amhw_lpc82x_sct_evt_int_enable(p_hw_sct,
                                             AMHW_LPC82X_SCT_EVT(chan));
        }
    }
    
    return AM_OK;
}

static void __sct_timing_1_32bit_init (amhw_lpc82x_sct_t *p_hw_sct)
{
    if (NULL == p_hw_sct) {
        return ;
    }

    amhw_lpc82x_sct_config(p_hw_sct,
                           /* the timer is operate as 1 32-bit timer */
                           AMHW_LPC82X_SCT_CFG_32BIT_COUNTER  |
                           /* the SCT clock is system clock          */
                           AMHW_LPC82X_SCT_CFG_CLKMODE_SYSCLK);
    
    /* Configure the match/capture reg 0 operate as match register */
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   /* the SCT mode is UNIFY(1 32-bit counter) */
                                   AMHW_LPC82X_SCT_MODE_UNIFY,
                                   /* select the match/capture 0 to configure */
                                   0,
                                   /* operate as match register               */
                                   AMHW_LPC82X_SCT_MATCAP_MATCH);
    
    /* CLear the match 0 and match reload value */
    amhw_lpc82x_sct_mat_val_set(p_hw_sct,
                                AMHW_LPC82X_SCT_MODE_UNIFY,
                                AMHW_LPC82X_SCT_MAT(0), 0);
    amhw_lpc82x_sct_mat_reload_val_set(p_hw_sct,
                                       AMHW_LPC82X_SCT_MODE_UNIFY,
                                       AMHW_LPC82X_SCT_MAT(0), 0);
    
    /* set the event 0 control */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             /* event 0 */
                             AMHW_LPC82X_SCT_EVT(0),
                             /* the event associated with the match 0 */
                             AMHW_LPC82X_SCT_EVT_CTRL_MATCHSEL(0)    |
                             /* Uses the specified match only.        */
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH |
                             /*  STATEV value is added into STATE     */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD    |
                             /* ADD 0,the state will not change       */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));
                   
    /* set the now state value is 0 */
    amhw_lpc82x_sct_state_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY, 0);
    
    /* enable the event 0 happen in state 0 */
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0),
                                                 AMHW_LPC82X_SCT_STATE(0));
    
    /* enable the event 0 as the counter limit */
    amhw_lpc82x_sct_evt_limit_enable(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY,
                                                 AMHW_LPC82X_SCT_EVT(0));
    
    /* enable the event 0 generate interrupt */
    amhw_lpc82x_sct_evt_int_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0));

    /* Set the CLRCTR bit to clear the counter to 0 */
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);
    
    /* Set the prescaler value to 0. use the system clock,not prescale */
    amhw_lpc82x_sct_prescale_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_UNIFY, 0);
}


/* 
 * The event0 associated with match0 _L 16bits, _L 16bits timer
 * The event1 associated with match0 _H 16bits, _H 16bits timer
 */
static void __sct_timing_2_16bit_init(amhw_lpc82x_sct_t *p_hw_sct)
{
    if (NULL == p_hw_sct) {
        return;
    }

    amhw_lpc82x_sct_config(p_hw_sct,
                           /* the timer is operate as 2 16-bit timer */
                           AMHW_LPC82X_SCT_CFG_16BIT_COUNTER  |
                           /* the SCT clock is system clock */
                           AMHW_LPC82X_SCT_CFG_CLKMODE_SYSCLK);

    /*
     * configure the match/capture reg 0 _L / or _H
     * 16bits operate as match register
     */
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_SEPARATE_L,
                                   AMHW_LPC82X_SCT_MAT(0),
                                   AMHW_LPC82X_SCT_MATCAP_MATCH);
    
    amhw_lpc82x_sct_regmode_config(p_hw_sct,
                                   AMHW_LPC82X_SCT_MODE_SEPARATE_H,
                                   AMHW_LPC82X_SCT_MAT(0),
                                   AMHW_LPC82X_SCT_MATCAP_MATCH);
    
    /* The L timer use the event 0, set the event 1 control */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             AMHW_LPC82X_SCT_EVT(0),
                             AMHW_LPC82X_SCT_EVT_CTRL_HEVENT_L       |
                             /* the event associated with the match 0 */
                             AMHW_LPC82X_SCT_EVT_CTRL_MATCHSEL(0)    |
                             /* Uses the specified match only.        */
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH |
                             /*  STATEV value is added into STATE     */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD    |
                             /* ADD 0,the state will not change       */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));

    /* The H timer use the event 1, set the event 1 control */
    amhw_lpc82x_sct_evt_ctrl(p_hw_sct,
                             AMHW_LPC82X_SCT_EVT(1),
                             AMHW_LPC82X_SCT_EVT_CTRL_HEVENT_H       |
                             /* the event associated with the match 0 */
                             AMHW_LPC82X_SCT_EVT_CTRL_MATCHSEL(0)    |
                             /* Uses the specified match only.        */
                             AMHW_LPC82X_SCT_EVT_CTRL_COMBMODE_MATCH |
                             /*  STATEV value is added into STATE     */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATELD_ADD    |
                             /* ADD 0,the state will not change       */
                             AMHW_LPC82X_SCT_EVT_CTRL_STATEV(0));

    /* set the now state value is 0 */
    amhw_lpc82x_sct_state_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_SEPARATE_L,
                                        AMHW_LPC82X_SCT_STATE(0));
    amhw_lpc82x_sct_state_set(p_hw_sct, AMHW_LPC82X_SCT_MODE_SEPARATE_H,
                                        AMHW_LPC82X_SCT_STATE(0));
    
    /* enable the event happen in state 0 */
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0),
                                                 AMHW_LPC82X_SCT_STATE(0));
    amhw_lpc82x_sct_evt_state_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(1),
                                                 AMHW_LPC82X_SCT_STATE(0));

    /* enable the event as the counter limit */
    amhw_lpc82x_sct_evt_limit_enable(p_hw_sct,
                                       AMHW_LPC82X_SCT_MODE_SEPARATE_L,
                                       AMHW_LPC82X_SCT_EVT(0));
    amhw_lpc82x_sct_evt_limit_enable(p_hw_sct,
                                       AMHW_LPC82X_SCT_MODE_SEPARATE_H,
                                       AMHW_LPC82X_SCT_EVT(1));
    
    /* enable the event generate interrupt */
    amhw_lpc82x_sct_evt_int_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(0));
    amhw_lpc82x_sct_evt_int_enable(p_hw_sct, AMHW_LPC82X_SCT_EVT(1));

    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_H);
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_CLRCTR_L);
}

am_timer_handle_t
am_lpc82x_sct_timing_init (am_lpc82x_sct_timing_dev_t     *p_dev,
                     const am_lpc82x_sct_timing_devinfo_t *p_devinfo)
{
    amhw_lpc82x_sct_t *p_hw_sct = NULL;

    if (NULL == p_dev || NULL == p_devinfo ) {
        return NULL;
    }
    
    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    p_dev->p_devinfo          = p_devinfo;
    p_dev->timer_serv.p_funcs = (struct am_timer_drv_funcs *)
                                 &__g_sct_timing_drv_funcs;
    p_dev->timer_serv.p_drv   = p_dev; 

    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);
    /* Operate as one 32-bit timer */
    if (p_devinfo->flag == AM_LPC82X_SCT_TIMING_1_32BIT) {
        __sct_timing_1_32bit_init(p_hw_sct);
        
        p_dev->callback_info[0].pfn_callback = NULL;
        p_dev->callback_info[0].p_arg        = NULL;
    } else {
        __sct_timing_2_16bit_init(p_hw_sct);
        
        p_dev->callback_info[0].pfn_callback = NULL;
        p_dev->callback_info[0].p_arg        = NULL;
    
        p_dev->callback_info[1].pfn_callback = NULL;
        p_dev->callback_info[1].p_arg        = NULL;
    }
    
    am_int_connect(p_dev->p_devinfo->inum, __sct_irq_handler, (void *)p_dev);
    am_int_enable(p_dev->p_devinfo->inum);

    return &(p_dev->timer_serv);
}

void am_lpc82x_sct_timing_deinit (am_timer_handle_t handle)
{
    am_lpc82x_sct_timing_dev_t *p_dev    = (am_lpc82x_sct_timing_dev_t *)handle;
    amhw_lpc82x_sct_t          *p_hw_sct = NULL;

    if (p_dev == NULL || p_dev->p_devinfo == NULL ) {
        return ;
    }

    p_hw_sct = (amhw_lpc82x_sct_t *)(p_dev->p_devinfo->sct_regbase);
 
    amhw_lpc82x_sct_ctrl_set(p_hw_sct, AMHW_LPC82X_SCT_CTRL_STOP_L |
                                       AMHW_LPC82X_SCT_CTRL_HALT_L);
    
    am_int_disable(p_dev->p_devinfo->inum);

    p_dev->timer_serv.p_drv = NULL;

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}
/* end of file */
