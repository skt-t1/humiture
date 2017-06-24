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
 * \brief  NVIC驱动，服务中断标准接口
 *
 * \internal
 * \par Modification History
 * - 1.00 15-01-29  hbt, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_INT_H
#define __AM_LPC82X_INT_H

#include "ametal.h"
#include "amhw_lpc82x_int.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_int
 * \copydoc am_lpc82x_int.h
 * @{
 */
    

/**
 * \brief 中断服务信息
 */
struct am_lpc82x_int_servinfo {

    /** \brief 起始中断号 */
    int inum_start;

    /** \brief 末尾中断号 */
    int inum_end;
};

/** \brief 中断服务例行信息 */
struct am_lpc82x_int_isr_info {

    /** \brief 触发回调函数 */
    am_pfnvoid_t  pfn_isr;

    /** \brief 回调函数的参数 */
    void         *p_arg;
};


/** \brief 中断设备信息 */
typedef struct am_lpc82x_int_devinfo {
    
    /** \brief 中断服务信息实例 */
    const struct am_lpc82x_int_servinfo int_servinfo;
    
    /** \brief 中断控制器寄存器块基址 */
    uint32_t int_regbase;
    
    /** \brief 总中断数量 */
    int input_cnt;

    /** \brief ISR 信息映射(大小与 input_cnt 一致) */
    uint8_t *p_isrmap;

    /** \brief ISR 信息数量 */
    int isrinfo_cnt;

    /** \brief ISR 信息映射内存(大小与 isrinfo_cnt 一致) */
    struct am_lpc82x_int_isr_info *p_isrinfo;
    
    /** \brief 平台初始化函数 */
    void     (*pfn_plfm_init)(void);

    /** \brief 平台解初始化函数 */
    void     (*pfn_plfm_deinit)(void);
    
} am_lpc82x_int_devinfo_t;

/**
 * \brief 中断设备实例 
 */
typedef struct am_lpc82x_int_dev {
    
    /** \brief 指向中断设备信息的指针 */
    const am_lpc82x_int_devinfo_t *p_devinfo;
    
    /** \brief 参数有效标志 */
    bool_t                         valid_flg;
    
} am_lpc82x_int_dev_t;



/**
 * \brief 中断初始化 
 *
 * \param[in] p_dev     : 指向中断设备的指针 
 * \param[in] p_devinfo : 指向中断设备信息的指针 
 *
 * \retval  AM_OK       : 操作成功 
 * \retval -AM_EINVAL   : 无效参数 
 */
int am_lpc82x_int_init (am_lpc82x_int_dev_t           *p_dev, 
                        const am_lpc82x_int_devinfo_t *p_devinfo);

/**
 * \brief 中断去初始化
 * \return  无 
 */
void am_lpc82x_int_deinit (void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif /* __AM_LPC82X_INT_H */

/* end of file */
