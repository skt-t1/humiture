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
 * \brief CRC implementation
 * \internal
 * \par Modification history
 * - 1.00 15-07-10  sss, first implementation.
 * \endinternal
 */


#include "ametal.h"
#include "am_lpc82x_crc.h"
#include "amhw_lpc82x_crc.h"

/*******************************************************************************
 * 函数声明
 ******************************************************************************/
static int __crc_init (void *p_cookie, am_crc_pattern_t *p_pattern);
static int __crc_cal (void *p_cookie, const uint8_t *p_data, uint32_t nbytes);
static int __crc_final (void *p_cookie, uint32_t *p_value);

/*
 * \brief 给标准层要提供的服务赋值
 */
static const struct am_crc_drv_funcs __g_crc_drv_funcs = {
    __crc_init,
    __crc_cal,
    __crc_final
 };
 
/******************************************************************************/
static int __crc_init (void *p_drv, am_crc_pattern_t *p_pattern)
{
    am_lpc82x_crc_dev_t *p_dev = (am_lpc82x_crc_dev_t *)p_drv;
    uint8_t                width          = 0;
    uint32_t               poly           = 0;
    uint32_t               crc_mode_flags = 0;
    amhw_lpc82x_crc_poly_t crc_poly       = AMHW_LPC82X_CRC_POLY_CCITT;
    amhw_lpc82x_crc_t     *p_hw_crc       = NULL;
    
    if (p_pattern == NULL || p_dev == NULL) {
        return -AM_EINVAL;
    }
    
    width            = p_pattern->width;
    poly             = p_pattern->poly;
    p_dev->p_pattern = p_pattern;
    
    if ((width != 16 || (poly != 0x1021 && poly != 0x8005)) &&
        (width != 32 || (poly != 0x04C11DB7))) {
             
        return -AM_ENOTSUP;                      /* 不支持*/
    }
    
    p_hw_crc = (amhw_lpc82x_crc_t *)p_dev->p_devinfo->crc_regbase;
        
    if (p_pattern->refin == TRUE) {
        crc_mode_flags |= AMHW_LPC82X_CRC_MODE_WRDATA_BIT_RVS;
    }
        
    if (p_pattern->refout == TRUE) {
        crc_mode_flags |= AMHW_LPC82X_CRC_MODE_SUM_RVS;
    }

    if (width == 16) {
        if (poly == 0x1021) {
            crc_poly = AMHW_LPC82X_CRC_POLY_CCITT;
        } else if (poly == 0x8005) {
            crc_poly = AMHW_LPC82X_CRC_POLY_CRC16;
        }

        if (p_pattern->xorout == 0xFFFF) {
            crc_mode_flags |= AMHW_LPC82X_CRC_MODE_SUM_CMPL;
        }
        
        amhw_lpc82x_crc_mode_set(p_hw_crc, crc_poly, crc_mode_flags);

        amhw_lpc82x_crc_seed_set(p_hw_crc, p_pattern->initvalue);
        
        return AM_OK;
    }
    
    if (width == 32) {
        if (poly == 0x04C11DB7) {
            crc_poly = AMHW_LPC82X_CRC_POLY_CRC32;
        }

        if (p_pattern->xorout == 0xFFFFFFFF) {
            crc_mode_flags |= AMHW_LPC82X_CRC_MODE_SUM_CMPL;
        }
        
        amhw_lpc82x_crc_mode_set(p_hw_crc, crc_poly, crc_mode_flags);

        amhw_lpc82x_crc_seed_set(p_hw_crc, p_pattern->initvalue);
        
        return AM_OK;
    }
    
    return -AM_ENOTSUP;                      /* 不支持*/
}

static int __crc_cal (void *p_drv, const uint8_t *p_data, uint32_t nbytes)
{
    am_lpc82x_crc_dev_t *p_dev = (am_lpc82x_crc_dev_t *)p_drv;
    
    if (p_dev->p_pattern == NULL) {
        return -AM_EINVAL;                /* CRC计算必须在初始化之后 */
    }
    
    amhw_lpc82x_crc_data8_write(
        (amhw_lpc82x_crc_t *)p_dev->p_devinfo->crc_regbase,
         p_data, 
         nbytes);
    
    return AM_OK;
}

static int __crc_final (void *p_drv, uint32_t *p_value)
{
    am_lpc82x_crc_dev_t *p_dev = (am_lpc82x_crc_dev_t *)p_drv;
 
    if (p_dev->p_pattern == NULL) {
        return -AM_EINVAL;                /* CRC没有初始化 */
    }

   *p_value = amhw_lpc82x_crc_sum_get(
        (amhw_lpc82x_crc_t *)p_dev->p_devinfo->crc_regbase);
    
    p_dev->p_pattern = NULL;
    
    return AM_OK;
}

am_crc_handle_t am_lpc82x_crc_init (am_lpc82x_crc_dev_t           *p_dev,
                                    const am_lpc82x_crc_devinfo_t *p_devinfo)
{
    if (p_dev == NULL || p_devinfo == NULL) {
        return NULL;
    }
    
    p_dev->p_devinfo        = p_devinfo;
    p_dev->p_pattern        = NULL;
    
    p_dev->crc_serv.p_funcs = (struct am_crc_drv_funcs *)&__g_crc_drv_funcs;
    p_dev->crc_serv.p_drv   = p_dev; 
    
    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }
 
    return &(p_dev->crc_serv);
}

void am_lpc82x_crc_deinit (am_crc_handle_t handle)
{
    am_lpc82x_crc_dev_t *p_dev = NULL;
    
    if (handle == NULL) {
        return;
    }
    
    p_dev = (am_lpc82x_crc_dev_t *)handle;
    
    p_dev->crc_serv.p_funcs = NULL;
    p_dev->crc_serv.p_drv   = NULL;
    
    p_dev->p_pattern = NULL;
    
    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

/* end of file */
