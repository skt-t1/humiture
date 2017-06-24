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
 * \brief UART(使用ring buffer的中断模式)标准接口
 *
 * 该文件函数接口的实现基于UART标准接口，为方便用户使用设计，可作为UART接口函数
 * 的使用范例。
 *
 * \internal
 * \par Modification History
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */

#ifndef __AM_UART_RNGBUF_H
#define __AM_UART_RNGBUF_H


#ifdef __cplusplus
extern "C" {
#endif

#include "am_uart.h"
#include "am_rngbuf.h"
#include "am_wait.h"

/**
 * \addtogroup am_if_uart_rngbuf
 * \copydoc am_uart_rngbuf.h
 * @{
 */


/**
 * \name UART（带缓冲区）的控制指令
 * @{
 */

#define AM_UART_RINGBUF_NREAD     0x0100   /**< \brief 可读数据个数        */
#define AM_UART_RINGBUF_NWRITE    0x0200   /**< \brief 已经写入的数据个数  */
#define AM_UART_RINGBUF_FLUSH     0x0300   /**< \brief 清空读/写缓冲区     */
#define AM_UART_RINGBUF_WFLUSH    0x0400   /**< \brief 清空写缓冲区        */
#define AM_UART_RINGBUF_RFLUSH    0x0500   /**< \brief 清空读缓冲区        */
#define AM_UART_RINGBUF_TIMEOUT   0x0600   /**< \brief 设置等待超时        */

/** 
 * \brief 设置流控模式，软件流控，硬件流控 和 无流控（默认）
 */
#define AM_UART_RINGBUF_FLOWMODE_SET    AM_UART_FLOWMODE_SET
    
/** 
 * \brief 设置流关的阈值（字节数）
 *
 * 对应设置的值由该命令对应的参数设定，当缓冲区空闲字节数小于该值时流关，
 * 未设置时，该值默认为 20% × 缓冲区大小
 */                                               
#define AM_UART_RINGBUF_RX_FLOW_OFF_THR    0x0700
    
/**
 * \brief 设置流开的阈值（字节数）
 *
 * 对应设置的值由该命令对应的参数设定，当缓冲区空闲字节数大于该值时流开，
 * 未设置时，该值默认为 80% × 缓冲区大小
 *
 */
#define AM_UART_RINGBUF_RX_FLOW_ON_THR     0x0800

/** @} */

/**
 * \name 流控制设置选项
 * @{
 */

#define AM_RINGBUF_UART_FLOWCTL_NO  AM_UART_FLOWCTL_NO  /**< \brief 无流控    */
#define AM_RINGBUF_UART_FLOWCTL_HW  AM_UART_FLOWCTL_HW  /**< \brief 硬件流控  */
#define AM_RINGBUF_UART_FLOWCTL_SW  AM_UART_FLOWCTL_SW  /**< \brief 软件流控  */

/** @} */
    
/**
 * \brief UART（带ring buffer的中断模式）设备结构体
 */
typedef struct am_uart_rngbuf_dev {

    /** \brief UART标准服务操作句柄    */
    am_uart_handle_t  handle;

    /** \brief 接收数据环形缓冲区      */
    struct am_rngbuf  rx_rngbuf;

    /** \brief 发送数据环形缓冲区      */
    struct am_rngbuf  tx_rngbuf;
    
    /** \brief 空闲字节数高于该值时流开  */
    uint32_t          xon_threshold;
    
    /** \brief 空闲字节数低于该值时流关  */
    uint32_t          xoff_threshold;
    
    /** \brief 读超时                 */
    uint32_t          timeout_ms;
    
    /** \brief 当前的流控状态 TRUE-打开，FALSE-关闭 */
    bool_t            flow_stat;
    
    /** \brief 用于接收等待           */
    am_wait_t         rx_wait;

} am_uart_rngbuf_dev_t;

/** \brief UART（带ring buffer的中断模式）标准服务操作句柄类型定义 */
typedef struct am_uart_rngbuf_dev * am_uart_rngbuf_handle_t;


/**
 * \brief 初始化UART为带ring buffer的中断模式
 *
 * \param[in] p_dev      : UART（带ring buffer的中断模式）设备结构体指针
 * \param[in] handle     : UART标准服务操作句柄
 * \param[in] p_rxbuf    : 指向接收缓冲区的指针，用于实现接收环形缓冲区
 * \param[in] rxbuf_size : 接收缓冲区的大小（大小必须是2^n）
 * \param[in] p_txbuf    : 指向发送缓冲区的指针，用于实现发送环形缓冲区
 * \param[in] txbuf_size : 发送缓冲区的大小（大小必须是2^n）
 *
 * \return UART（带ring buffer的中断模式）标准服务操作句柄，如果值为NULL，
 *         表明初始化失败
 */
am_uart_rngbuf_handle_t am_uart_rngbuf_init(am_uart_rngbuf_dev_t *p_dev,
                                            am_uart_handle_t      handle,
                                            uint8_t              *p_rxbuf,
                                            uint32_t              rxbuf_size,
                                            uint8_t              *p_txbuf,
                                            uint32_t              txbuf_size);
                                              
                                              
/**
 * \brief UART（带ring buffer的中断模式）串口控制
 *
 *     所有UART控制的命令均支持，如常见的波特率设置、硬件参数设置等，均可直接使用。
 * 值得注意的是，带缓冲区时，串口必须工作在中断模式，不能工作在查询模式。模式设置
 * 指令将被忽略。
 *
 *
 * \param[in] handle  : UART标准服务操作句柄
 * \param[in] request : 控制指令
 *            - AM_UART_RINGBUF_NREAD   : 可读数据个数，参数为uint32_t型指针
 *            - AM_UART_RINGBUF_NWRITE  : 已经写入的数据个数，参数为uint32_t型指针
 *            - AM_UART_RINGBUF_FLUSH   ：清空输入和输出缓冲，无参数
 *            - AM_UART_RINGBUF_WFLUSH  ：清空写缓冲，无参数
 *            - AM_UART_RINGBUF_RFLUSH  : 清空读缓冲，无参数
 *            - AM_UART_RINGBUF_TIMEOUT : 设置读的超时时间(ms)，参数为uint32_t整数,
 *                                        0为关闭超时设置，即为一直等待。
 *            - AM_UART_RINGBUF_FLOWMODE_SET : 设置流控模式，参数可用下列宏：
 *                                               - AM_RINGBUF_UART_FLOWCTL_NO
 *                                               - AM_RINGBUF_UART_FLOWCTL_HW
 *                                               - AM_RINGBUF_UART_FLOWCTL_SW
 *            - AM_UART_RINGBUF_RX_FLOW_OFF_THR ：设置流关的阈值（字节数）
 *            - AM_UART_RINGBUF_RX_FLOW_ON_THR  ：设置流开的阈值（字节数）
 *                                                 
 * \param[in,out] p_arg : 该指令对应的参数
 *
 * \retval AM_OK       : 控制指令执行成功
 * \retval -AM_EIO     : 执行错误
 * \retval -AM_ENOTSUP : 指令不支持
 */
int am_uart_rngbuf_ioctl(am_uart_rngbuf_handle_t   handle,
                         int                       request,
                         void                     *p_arg);

/**
 * \brief UART数据发送（带ring buffer的中断模式）
 *
 * \param[in] handle  : UART（带ring buffer的中断模式）标准服务操作句柄
 * \param[in] p_txbuf : 发送数据缓冲区
 * \param[in] nbytes  : 待发送数据的个数
 * 
 * \retval    >0      ：成功发送数据的个数
 * \retval -AM_EINVAL : 读取失败，参数错误
 * \retval -AM_EIO    : 数据传输出错
 */
int am_uart_rngbuf_send(am_uart_rngbuf_handle_t  handle,
                        const uint8_t           *p_txbuf,
                        uint32_t                 nbytes);

/**
 * \brief UART数据接收（带ring buffer的中断模式）
 *
 * \param[in] handle  : UART（带ring buffer的中断模式）标准服务操作句柄
 * \param[in] p_rxbuf : 接收数据缓冲区
 * \param[in] nbytes  : 待接收数据的个数
 *
 * \retval    >0      ：成功接收数据的个数
 * \retval -AM_EINVAL : 读取失败，参数错误
 * \retval -AM_EIO    : 数据传输出错
 *
 * \note 如设置了超时，则超时时会返回，可以通过返回值判断实际读取到的字节数。
 */
int am_uart_rngbuf_receive(am_uart_rngbuf_handle_t  handle,
                           uint8_t                 *p_rxbuf,
                           uint32_t                 nbytes);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_UART_RINGBUF_H */

/* end of file */
