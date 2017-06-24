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
 * \brief UART标准接口
 *
 * \internal
 * \par Modification History
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */

#ifndef __AM_UART_H
#define __AM_UART_H


#ifdef __cplusplus
extern "C" {
#endif

#include "am_common.h"

/**
 * \addtogroup am_if_uart
 * \copydoc am_uart.h
 * @{
 */

/**
 * \name UART模式定义
 * @{
 */

#define AM_UART_MODE_POLL         1    /**< \brief UART 查询模式 */
#define AM_UART_MODE_INT          2    /**< \brief UART 中断模式 */

/** @} */

/**
 * \name UART通用控制指令
 * @{
 */

#define AM_UART_BAUD_SET          1   /**< \brief 设置波特率         */
#define AM_UART_BAUD_GET          2   /**< \brief 获取波特率         */

#define AM_UART_OPTS_SET          3   /**< \brief 设置硬件参数设置   */
#define AM_UART_OPTS_GET          4   /**< \brief 获取硬件参数设置   */

#define AM_UART_MODE_SET          5   /**< \brief 设置模式           */
#define AM_UART_MODE_GET          6   /**< \brief 获取当前模式       */
#define AM_UART_AVAIL_MODES_GET   7   /**< \brief 获取支持的有效模式 */

#define AM_UART_FLOWMODE_SET      8   /**< \brief 设置流控模式       */
#define AM_UART_FLOWSTAT_RX_SET   9   /**< \brief 设置接收器流控状态 */
#define AM_UART_FLOWSTAT_TX_GET   10  /**< \brief 获取发送器流控状态 */
/** @} */

/**
 * \name 硬件参数设置选项，设置硬件参数时，可以是多个选项的或值
 * @{
 */

#define AM_UART_CSIZE    0xc   /**< \brief 位3 ~ 位4为数据宽度                */
#define AM_UART_CS5      0x0   /**< \brief 数据宽度为5位                      */
#define AM_UART_CS6      0x4   /**< \brief 数据宽度为6位                      */
#define AM_UART_CS7      0x8   /**< \brief 数据宽度为7位                      */
#define AM_UART_CS8      0xc   /**< \brief 数据宽度为8位                      */

#define AM_UART_STOPB    0x20  /**< \brief 设置停止位为2位，默认为1位         */
#define AM_UART_PARENB   0x40  /**< \brief 使能奇偶校验，默认奇偶校验是关闭的 */
#define AM_UART_PARODD   0x80  /**< \brief 设置校验为奇校验，默认是偶校验     */

/** @} */

/**
 * \name 流控制设置选项
 * @{
 */

#define AM_UART_FLOWCTL_NO    0xa0    /**< \brief 无流控             */
#define AM_UART_FLOWCTL_HW    0xa1    /**< \brief 硬件流控           */
#define AM_UART_FLOWCTL_SW    0xa2    /**< \brief 软件流控           */

#define AM_UART_FLOWSTAT_ON   0xb0    /**< \brief 开流，继续数据传输 */
#define AM_UART_FLOWSTAT_OFF  0xb1    /**< \brief 关流，停止数据传输 */

/** @} */

/**
 * \name 串行设备回调函数类型编码，用于指定注册何种回调函数
 * @{
 */

#define AM_UART_CALLBACK_GET_TX_CHAR   0 /**< \brief 获取一个发送字符      */
#define AM_UART_CALLBACK_PUT_RCV_CHAR  1 /**< \brief 提交一个接收到的字符  */
#define AM_UART_CALLBACK_ERROR         2 /**< \brief 错误回调函数          */

/** @} */

/**
 * \name 回调函数类型定义
 * @{
 */

/** \brief 获取一个发送字符回调函数类型 */
typedef int  (*pfn_uart_txchar_get_t)(void *p_arg, char *p_char);

/** \brief 提交一个接收到的字符         */
typedef int  (*pfn_uart_rxchar_put_t)(void *p_arg, char  ch);

/** \brief 错误回调函数                 */
typedef int  (*pfn_uart_err_t)(void *p_arg, int code, void *p_data, int size);

/** @} */


/**
 * \brief UART驱动函数结构体
 */
struct am_uart_drv_funcs {

    /**\brief UART控制函数     */
    int (*pfn_uart_ioctl)(void *p_drv,int request, void *p_arg);

    /**\brief 启动UART发送函数 */
    int (*pfn_uart_tx_startup)(void *p_drv);

    /**\brief 设置串口回调函数 */
    int (*pfn_uart_callback_set)(void  *p_drv,
                                 int    callback_type,
                                 int  (*pfn_callback)(void *),
                                 void  *p_callback_arg);

    /**\brief 从串口获取一个字符（查询模式） */
    int (*pfn_uart_poll_getchar)(void *p_drv, char *p_inchar);

    /**\brief 输出一个字符（查询模式）       */
    int (*pfn_uart_poll_putchar)(void *p_drv, char outchar);

};

/**
 * \brief UART服务
 */
typedef struct am_uart_serv {

    /** \brief UART驱动函数结构体指针    */
    struct am_uart_drv_funcs *p_funcs;

    /** \brief 用于驱动函数的第一个参数  */
    void                     *p_drv;
} am_uart_serv_t;

/** \brief UART标准服务操作句柄类型定义  */
typedef am_uart_serv_t *am_uart_handle_t;


/**
 * \brief 串口控制函数
 *
 * \param[in] handle  : UART标准服务操作句柄
 * \param[in] request : 控制指令
 *   - AM_UART_BAUD_SET : 设置波特率, p_arg为 uint32_t类型,值为波特率
 *   - AM_UART_BAUD_GET : 获取波特率, p_arg为 uint32_t指针类型
 *   - AM_UART_OPTS_SET ：设置硬件参数，p_arg 为 uint32_t类型（#AM_UART_CS8）
 *   - AM_UART_OPTS_GET ：获取当前的硬件参数设置，p_arg为 uint32_t指针类型
 *   - AM_UART_MODE_SET : 设置模式, p_arg值为 AM_UART_MODE_POLL或 AM_UART_MODE_INT
 *   - AM_UART_MODE_GET : 获取当前模式, p_arg为 uint32_t指针类型
 *   - AM_UART_AVAIL_MODES_GET : 获取当前可用的模式, p_arg为 uint32_t指针类型
 *   - AM_UART_FLOWMODE_SET    : 设置流控模式, p_arg 为 AM_UART_FLOWCTL_NO
 *                                                       或  AM_UART_FLOWCTL_OFF
 *   - AM_UART_FLOWSTAT_RX_SET : 设置接收器流控状态, p_arg 为AM_UART_FLOWSTAT_ON 
 *                                                       或 AM_UART_FLOWSTAT_OFF
 *   - AM_UART_FLOWSTAT_TX_GET : 获取发送器流控状态, p_arg 为AM_UART_FLOWSTAT_ON 
 *                                                       或 AM_UART_FLOWSTAT_OFF
 *
 * \param[in,out] p_arg : 该指令对应的参数
 *
 * \retval AM_OK       : 控制指令执行成功
 * \retval -AM_EIO     : 执行错误
 * \retval -AM_ENOTSUP : 指令不支持
 */
am_static_inline
int am_uart_ioctl (am_uart_handle_t handle,int request, void *p_arg)
{
    return handle->p_funcs->pfn_uart_ioctl(handle->p_drv,request,p_arg);
}

/**
 * \brief 启动UART中断模式数据传输
 * \param[in] handle : UART标准服务操作句柄
 * \retval AM_OK : 启动成功
 */
am_static_inline
int am_uart_tx_startup (am_uart_handle_t handle)
{
    return handle->p_funcs->pfn_uart_tx_startup(handle->p_drv);
}

/**
 * \brief 设置UART回调函数
 *
 * \param[in] handle         : UART标准服务操作句柄
 * \param[in] callback_type  : 指明设置的何种回调函数
 *            - AM_UART_CALLBACK_GET_TX_CHAR  : 获取一个发送字符函数
 *            - AM_UART_CALLBACK_PUT_RCV_CHAR : 提交一个接收到的字符给应用程序
 *            - AM_UART_CALLBACK_ERROR        : 错误回调函数
 * \param[in] pfn_callback   : 指向回调函数的指针
 * \param[in] p_callback_arg : 回调函数的用户参数
 *
 * \retval  AM_OK      : 回调函数设置成功
 * \retval -AM_EINVAL  : 设置失败，参数错误
 */
am_static_inline
int am_uart_callback_set (am_uart_handle_t  handle,
                          int               callback_type,
                          int             (*pfn_callback)(void *),
                          void             *p_callback_arg)
{
    return handle->p_funcs->pfn_uart_callback_set(handle->p_drv,
                                                  callback_type,
                                                  pfn_callback,
                                                  p_callback_arg);
}

/**
 * \brief UART接收一个数据(查询模式)
 *
 * \param[in]  handle   : UART标准服务操作句柄
 * \param[out] p_inchar : 用于获取数据的指针
 *
 * \retval  AM_OK       : 接收数据成功
 * \retval -AM_EAGAIN   : 接收数据未准备就绪，需重试
 */
am_static_inline
int am_uart_poll_getchar (am_uart_handle_t handle, char *p_inchar)
{
    return handle->p_funcs->pfn_uart_poll_getchar(handle->p_drv, p_inchar);
}

/**
 * \brief UART发送一个数据(查询模式)
 *
 * \param[in] handle  : UART标准服务操作句柄
 * \param[in] outchar : 待发送的数据
 *
 * \retval  AM_OK     : 发送数据成功
 * \retval -AM_EAGAIN : 发送未就绪，需重试
 */
am_static_inline
int am_uart_poll_putchar (am_uart_handle_t handle, char outchar)
{
    return handle->p_funcs->pfn_uart_poll_putchar(handle->p_drv, outchar);
}

/**
 * \brief UART数据发送（查询模式）
 *
 * \param[in] handle  : UART标准服务操作句柄
 * \param[in] p_txbuf : 发送数据缓冲区
 * \param[in] nbytes  : 待发送数据的个数
 *
 * \return 成功发送数据的个数
 */
int am_uart_poll_send(am_uart_handle_t  handle,
                      const uint8_t    *p_txbuf,
                      uint32_t          nbytes);


/**
 * \brief UART数据接收（查询模式）
 *
 * \param[in] handle  : UART标准服务操作句柄
 * \param[in] p_rxbuf : 接收数据缓冲区
 * \param[in] nbytes  : 待接收数据的个数
 *
 * \return 成功接收数据的个数
 */
int am_uart_poll_receive(am_uart_handle_t  handle,
                         uint8_t          *p_rxbuf,
                         uint32_t          nbytes);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_UART_H */

/* end of file */
