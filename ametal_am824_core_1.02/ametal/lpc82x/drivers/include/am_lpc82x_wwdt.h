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
 * \brief  WWDT驱动层接口
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-07-07  sss, first implementation.
 * \endinternal
 */
 
#ifndef __AMDR_WWDT_H
#define __AMDR_WWDT_H

#include "ametal.h"
#include "am_wdt.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * \addtogroup am_lpc82x_if_wwdt
 * \copydoc am_lpc82x_wwdt.h
 * @{
 */
 
 
/**
 * \brief WWDT 设备信息结构体
 */
typedef struct am_lpc82x_wwdt_devinfo {
    
    /** \brief WWDT寄存器块基地址 */
    uint32_t  wwdt_regbase;
      
    /** \brief 平台初始化函数 */
    void     (*pfn_plfm_init)(void);

    /** \brief 平台解初始化函数 */
    void     (*pfn_plfm_deinit)(void);
    
} am_lpc82x_wwdt_devinfo_t;

/**
 * \brief WWDT 设备结构体
 */
typedef struct am_lpc82x_wwdt_dev {
    
    /** \brief WWDT提供的标准服务     */
    am_wdt_serv_t              wdt_serv;   
    
    /** \brief 指向WWDT设备的信息指针 */
    const am_lpc82x_wwdt_devinfo_t *p_devinfo;  
} am_lpc82x_wwdt_dev_t;

/** 
 * \brief WWDT 初始化
 * \param[in] p_dev     : 指向WWDT设备
 * \param[in] p_devinfo : 指向WWDT设备信息
 * \return WWDT标准服务句柄，失败则返回NULL
 */
am_wdt_handle_t am_lpc82x_wwdt_init (am_lpc82x_wwdt_dev_t      *p_dev,
                               const am_lpc82x_wwdt_devinfo_t  *p_devinfo);

/**
 * \brief 解除WWDT 设备初始化
 * \param[in] handle : 使用WWDT初始化函数am_lpc82x_wwdt_init()获取到的handle
 * \return    无
 */
void am_lpc82x_wwdt_deinit (am_wdt_handle_t handle);

/**
 * @} 
 */

#ifdef __cplusplus
}
#endif /* __AMHW_WWDT_H */

#endif

/*end of file */

