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
 * \brief  CRC驱动层接口
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-07-09  sss, first implementation.
 * \endinternal
 */
#ifndef __AM_LPC82X_CRC_H
#define __AM_LPC82X_CRC_H

#include "ametal.h"
#include "am_crc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_crc
 * \copydoc am_lpc82x_crc.h
 * @{
 */
 
 
/**
 * \brief CRC设备信息结构体
 */
typedef struct am_lpc82x_crc_devinfo {
    
    /** \brief crc寄存器块基址*/
    uint32_t crc_regbase;
    
    /** \brief 平台初始化函数 */
    void     (*pfn_plfm_init)(void);

    /** \brief 平台解初始化函数 */
    void     (*pfn_plfm_deinit)(void);
    
} am_lpc82x_crc_devinfo_t;

/**
 * \brief CRC设备结构体
 */
typedef struct am_lpc82x_crc_dev {
    
    /** \brief 标准crc服务                */
    am_crc_serv_t                  crc_serv;      
    /** \brief 指向crc设备信息结构体的指针*/    
    const am_lpc82x_crc_devinfo_t *p_devinfo;   
    /** \brief 指向crc模型的指针          */  
    am_crc_pattern_t              *p_pattern;        
    
}am_lpc82x_crc_dev_t;

/**
 * \brief CRC设备初始化
 * \param[in] p_dev     : 指向crc设备的指针
 * \param[in] p_devinfo : 指向crc设备信息结构体指针
 * \return CRC标准服务句柄,为NULL时函数调用失败
 */
am_crc_handle_t am_lpc82x_crc_init (am_lpc82x_crc_dev_t           *p_dev, 
                                    const am_lpc82x_crc_devinfo_t *p_devinfo);

/**
 * \brief CRC设备解初始化
 * \param[in] handle : CRC标准服务操作句柄
 * \return 无
 */
void am_lpc82x_crc_deinit (am_crc_handle_t handle);

/**
 * @} 
 */
 
#ifdef __cplusplus
}
#endif /* __AM_LPC82X_CRC_H */

#endif

/* end of file */

