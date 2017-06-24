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
 * \brief UART(中断模式)标准接口
 *
 * 该文件函数接口的实现基于UART标准接口，为方便用户使用设计，可作为UART接口函数
 * 的使用范例。
 *
 * \internal
 * \par Modification History
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */

#ifndef __AM_UART_INT_H
#define __AM_UART_INT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "am_uart.h"
#include "am_wait.h"

/**
 * \addtogroup am_if_uart_int
 * \copydoc am_uart_int.h
 * @{
 */
    
/**
 * \brief UART（中断模式）设备结构体
 */
typedef struct am_uart_int_dev {
    
    /** \brief UART标准服务操作句柄     */
    am_uart_handle_t   handle;

    /** \brief 指向UART发送缓冲区的指针 */
    uint8_t           *p_txbuf;
    
    /** \brief 发送数据个数             */
    volatile uint32_t  tx_nbytes;
    
    /** \brief 指向UART接收缓冲区的指针 */
    volatile uint8_t  *p_rxbuf;
    
    /** \brief 接收数据个数             */
    volatile uint32_t  rx_nbytes;
    
    /** \brief 用于关闭正在等待发送完成的任务 */
    am_wait_t         *p_txwait;
    
    /** \brief 用于关闭正在等待接收完成的等待任务 */
    am_wait_t         *p_rxwait;
} am_uart_int_dev_t;

/** \brief UART（中断模式）标准服务操作句柄类型定义 */
typedef struct am_uart_int_dev *am_uart_int_handle_t;


/**
 * \brief 初始化UART为中断模式
 *
 * \param[in] p_dev   : UART（中断模式）设备结构体指针
 * \param[in] handle  : UART标准服务操作句柄
 *
 * \return UART（中断模式）标准服务操作句柄，如果值为NULL，表明初始化失败
 */
am_uart_int_handle_t am_uart_int_init(am_uart_int_dev_t *p_dev,
                                      am_uart_handle_t   handle);

/**
 * \brief UART数据发送（中断模式）
 *
 * \param[in] handle  : UART（中断模式）标准服务操作句柄
 * \param[in] p_txbuf : 发送数据缓冲区
 * \param[in] nbytes  : 待发送数据的个数
 * 
 * \return 成功发送数据的个数
 */
int am_uart_int_send(am_uart_int_handle_t  handle,
                     const uint8_t        *p_txbuf,
                     uint32_t              nbytes);
                            

/**
 * \brief UART数据接收（中断模式）
 *
 * \param[in] handle  : UART（中断模式）标准服务操作句柄
 * \param[in] p_rxbuf : 接收数据缓冲区
 * \param[in] nbytes  : 待接收数据的个数
 * 
 * \return 成功接收数据的个数
 */
int am_uart_int_receive(am_uart_int_handle_t  handle,
                        uint8_t              *p_rxbuf,
                        uint32_t              nbytes);

/** 
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_UART_INT_H */

/* end of file */
