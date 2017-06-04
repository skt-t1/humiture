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
 * \brief ADC implementation
 * 
 * \internal
 * \par Modification history
 * - 1.01 15-11-26  hgo, modify data align.
 * - 1.00 15-04-27  tee, first implementation.
 * \endinternal
 */
#include "am_adc.h"
#include "am_wait.h"

/*******************************************************************************/

volatile static int  g_adc_satus = 0;

/* adc转换回调函数 */
static void adc_convert_cb (void *p_arg, int state)
{
    am_wait_t *p_wait = (am_wait_t *)p_arg;
    
    g_adc_satus = state;
    
    am_wait_done(p_wait);
}

/*******************************************************************************/

int am_adc_read (am_adc_handle_t  handle, 
                 int              chan, 
                 am_adc_val_t    *p_val,
                 uint32_t         length)
{
    
    am_adc_buf_desc_t desc;
    am_wait_t  wait;
    
    am_wait_init(&wait);

    am_adc_mkbufdesc(&desc, p_val, length, adc_convert_cb,(void *) &wait);
 
    am_adc_start(handle, chan, &desc, 1, 1, AM_ADC_DATA_ALIGN_RIGHT, NULL, NULL);
    
    am_wait_on(&wait);
    
    return g_adc_satus;
}

/*******************************************************************************/

int am_adc_read_mv (am_adc_handle_t  handle, 
                    int              chan, 
                    am_adc_val_t    *p_mv,
                    uint32_t         length)
{
    int stat = am_adc_read(handle,chan,p_mv,length);
 
    uint32_t adc_bits;
    uint32_t ref_mv;
    uint64_t temp;
    int i;

    if (stat != AM_OK) {
        return stat;
    }
    
    adc_bits = am_adc_bits_get(handle,chan);
    ref_mv   = am_adc_vref_get(handle,chan);
    
    for (i = 0; i < length; i++) {
        temp    = (uint64_t)ref_mv * p_mv[i]; 
        p_mv[i] = temp / ((1 << adc_bits) - 1);
    }
    
    return g_adc_satus;
}

/*******************************************************************************/
int am_adc_val_mv(am_adc_handle_t  handle, 
                  int              chan, 
                  am_adc_val_t    *p_val,
                  uint32_t         length,
                  am_adc_val_t    *p_mv)
{
    int i;
    
    uint32_t adc_bits;
    uint32_t ref_mv;
    uint64_t temp;

    adc_bits = am_adc_bits_get(handle,chan);
    ref_mv   = am_adc_vref_get(handle,chan);
    
    for (i = 0; i < length; i++) {
        
        temp    = (uint64_t)ref_mv * p_val[i];
        p_mv[i] = temp / ((1 << adc_bits) - 1);
    }
    
    
    return AM_OK;
}

/* end of file */
