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
 * \brief I2C驱动，服务I2C标准接口
 *
 * \internal
 * \par Modification History
 * - 1.01 15-12-05  cyl, modify the namespace. 
 * - 1.00 15-07-06  win, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_I2C_H
#define __AM_LPC82X_I2C_H

#include "ametal.h"
#include "am_int.h"
#include "am_i2c.h"
#include "am_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_i2c
 * \copydoc am_lpc82x_i2c.h
 * @{
 */
    
/**
 * \brief I2C 设备信息参数结构体
 */
typedef struct am_lpc82x_i2c_devinfo {
    
    /** \brief I2C初始化总线速度,am_i2c_speed_set() 标准接口可以改变速率 */
    uint32_t  bus_speed;
 
    /** \brief I2C寄存器块基址  */
    uint32_t  i2c_regbase;
    
    /** \brief I2C 控制器的中断号     */
    uint16_t  inum;
    
    /** 
     * \brief 超时时间值
     *
     * I2C总线在数据传输期间，若此段时间内时钟线无任何响应,则视为超时。
     * 结束当前传输并复位内部逻辑 
     */
    uint32_t  timeout_ms;
    
    /** \brief 平台初始化函数 */
    void     (*pfn_plfm_init)(void);

    /** \brief 平台解初始化函数 */
    void     (*pfn_plfm_deinit)(void);
    
} am_lpc82x_i2c_devinfo_t;

/**
 * \brief I2C 设备结构体
 */
typedef struct am_lpc82x_i2c_dev {
   
    /** \brief 标准I2C服务           */
    am_i2c_serv_t                  i2c_serv;
    
    /** \brief I2C 控制器消息队列 */
    struct am_list_head            msg_list;

    /** \brief 指向I2C传输结构体的指针,同一时间只能处理一个传输 */
    am_i2c_transfer_t             *p_cur_trans;
    
    /** \brief 当前正在处理的消息   */
    am_i2c_message_t              *p_cur_msg;
    
    /** \brief 用于数据接收/发送计数   */
    uint32_t                       data_ptr;
    
    /** \brief I2C时钟，使用DIV分频后的时钟频率 */
    uint32_t                       i2c_clkrate;
    
    /** \brief 忙标识                 */
    bool_t                         busy;
    
    /** \brief 状态                   */
    uint8_t                        state;
    
    /** \brief 指向I2C设备信息的指针   */
    const am_lpc82x_i2c_devinfo_t *p_devinfo;
    
} am_lpc82x_i2c_dev_t;


/**
 * \brief I2C 初始化
 *
 * \param[in] p_dev     : 指向I2C设备结构体的指针
 * \param[in] p_devinfo : 指向I2C设备信息结构体的指针
 *
 * \return  I2C标准服务操作句柄 
 */
am_i2c_handle_t am_lpc82x_i2c_init (am_lpc82x_i2c_dev_t           *p_dev,
                                    const am_lpc82x_i2c_devinfo_t *p_devinfo);

/**
 * \brief 解除I2C初始化
 * \param[in] handle : 与从设备关联的I2C标准服务操作句柄
 * \return 无
 */
void am_lpc82x_i2c_deinit (am_i2c_handle_t handle);
    
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_I2C_H */

/* end of file */
