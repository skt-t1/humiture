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
 * \brief  RTC 标准接口
 * 
 * \internal
 * \par Modification History
 * - 1.00 15-07-29  win, modify note.
 * - 1.00 14-11-27  fft, first implementation.
 * \endinternal
 */

#ifndef __AM_RTC_H
#define __AM_RTC_H


#ifdef __cplusplus
extern "C" {
#endif
    
#include "am_common.h"
    
/** 
 * \addtogroup am_if_rtc
 * \copydoc am_rtc.h
 * @{
 */

/**
 * \brief 细分时间, 本结构体与 <time.h> 中的 struct tm 相似, 可使用 <time.h> 中
 * 的辅助函数进行运算 (使用强制转换 (struct tm *)xx )
 */
typedef struct am_tm {
    int tm_sec;        /**< \brief seconds after the minute     - [0, 59]  */
    int tm_min;        /**< \brief minutes after the hour       - [0, 59]  */
    int tm_hour;       /**< \brief hours after midnight         - [0, 23]  */
    int tm_mday;       /**< \brief day of the month             - [1, 31]  */
    int tm_mon;        /**< \brief months since January         - [0, 11]  */
    int tm_year;       /**< \brief years since 1900                        */
    int tm_wday;       /**< \brief days since Sunday            - [0, 6]   */
    int tm_yday;       /**< \brief days since January 1         - [0, 365] */
    int tm_isdst;      /**< \brief Daylight Saving Time flag */
} am_tm_t;
    
/** 
 * \brief RTC驱动函数
 */
struct am_rtc_drv_funcs {
     
    /** \brief 设置当前的细分时间 */
    int (*pfn_rtc_time_set)(void *p_drv, am_tm_t *p_tm);
    
    /** \brief 获取当前的细分时间 */
    int (*pfn_rtc_time_get)(void *p_drv, am_tm_t *p_tm);

};


/** 
 * \brief RTC标准服务
 */
typedef struct am_rtc_serv {

    /** \brief RTC驱动函数 */
    struct am_rtc_drv_funcs *p_funcs; 
    
    /** \brief 驱动函数第一个入口参数 */
    void                    *p_drv;
} am_rtc_serv_t;

/** \brief RTC标准服务操作句柄类型 */
typedef am_rtc_serv_t *am_rtc_handle_t;

/**
 * \brief 设置当前的细分时间
 *
 * \param[in] handle : RTC标准服务操作句柄
 * \param[in] p_tm   : 要设置的时间
 *
 * \return    AM_OK    : 设置成功
 * \retval  -AM_EINVAL : 参数错误
 */
am_static_inline
int am_rtc_time_set (am_rtc_handle_t handle, am_tm_t *p_tm)
{
    return handle->p_funcs->pfn_rtc_time_set(handle->p_drv, p_tm);
}

/**
 * \brief 获取当前的细分时间
 *
 * \param[in]  handle  : RTC标准服务操作句柄
 * \param[out] p_tm    : 读取的时间
 *
 * \return   AM_OK     : 获取时间成功
 * \retval  -AM_EINVAL : 参数错误
 */
am_static_inline
int am_rtc_time_get (am_rtc_handle_t handle, am_tm_t *p_tm)
{
    return handle->p_funcs->pfn_rtc_time_get(handle->p_drv, p_tm);
}
    
/**
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_RTC_H */

/* end of file */
