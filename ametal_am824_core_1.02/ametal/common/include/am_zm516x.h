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
 * \brief ZigBee模块ZM516X操作接口
 *
 * 使用本服务需要包含头文件 am_zm516x.h
 *
 * \par 使用示例
 * \code
 * #include "am_zm516x.h"
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 15-09-06  afm, first implementation
 * \endinternal
 */
 
#ifndef __AM_ZM516X_H
#define __AM_ZM516X_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup zm516x_driver
 * \copydoc am_zm516x.h
 * @{
 */
 
#include "am_common.h"
#include "am_uart_rngbuf.h"

/** \brief ZM516X模块操作句柄 */
typedef struct am_zm516x_dev {
    am_uart_rngbuf_handle_t uart_handle;
} am_zm516x_dev_t;

/** \brief ZM516X模块标准服务操作句柄类型定义 */
typedef struct am_zm516x_dev *am_zm516x_handle_t;

/** \brief ZM516X模块的配置信息 */
typedef struct am_zm516x_cfg_info {
    char    dev_name[16];     /**< \brief 设备名称      */
    char    dev_pwd[16];      /**< \brief 设备密码      */
    uint8_t dev_mode;         /**< \brief 工作类型      */
    uint8_t chan;             /**< \brief 通道号        */
    uint8_t panid[2];         /**< \brief PanID         */
    uint8_t my_addr[2];       /**< \brief 本地网络地址  */
    uint8_t my_mac[8];        /**< \brief 本地MAC地址   */
    uint8_t dst_addr[2];      /**< \brief 目标网络地址  */
    uint8_t dst_mac[8];       /**< \brief 目标MAC地址   */
    uint8_t data_rate;        /**< \brief RF通讯速率    */
    uint8_t power_level;      /**< \brief 发射功率      */
    uint8_t retry_num;        /**< \brief 发送数据重试次数           */
    uint8_t tran_timeout;     /**< \brief 发送数据重试时间间隔(10ms) */
    uint8_t serial_rate;      /**< \brief 串口波特率 值为1～7，分别对应波特率：2400、4800、9600、19200、38400、57600、115200 */
    uint8_t serial_data;      /**< \brief 串口数据位 值为 5～8       */
    uint8_t serial_stop;      /**< \brief 串口停止位 值为 1～2       */
    uint8_t serial_parity;    /**< \brief 串口校验位 0为无校验 1为奇校验 2为偶校验  */
    uint8_t send_mode;        /**< \brief 发送模式 0 单播  1 广播   */
} am_zm516x_cfg_info_t;

/**
 * \name ZM516X模块的AD通道号
 * @{
 */

#define AM_ZM516X_AD_CHANNEL0       0
#define AM_ZM516X_AD_CHANNEL1       1
#define AM_ZM516X_AD_CHANNEL2       2
#define AM_ZM516X_AD_CHANNEL3       3

/** @} */

/** \brief ZM516X模块的通讯模式 */
typedef enum am_zm516x_comm_mode {
    AM_ZM516X_COMM_UNICAST = 0,                     /**< \brief 单播  */
    AM_ZM516X_COMM_BROADCAST,                      	/**< \brief 广播  */
} am_zm516x_comm_mode_t;

/** \brief ZM516X模块的MAC地址 */
typedef struct am_zm516x_mac_addr {
    uint8_t mac[8];
}am_zm516x_mac_addr_t;

/**
* \brief ZM516X模块的地址
*
* 地址分为2字节的网络地址和8字节的MAC地址，使用addr_size地址长度来区分地址类型 
*/
typedef struct am_zm516x_addr {
    uint8_t *p_addr;
    uint8_t  addr_size;
}am_zm516x_addr_t;

/**
 * \brief 向ZM516X模块发送nbytes个字节数据
 *
 * 这个接口向ZM516X模块发送nbytes个字节数据。若设备的内部缓冲不够，将做如下处理：
 * 剩余的数据不会被写入，返回值为已经发送的数据个数
 *
 * \param[in] zm516x_handle  ZM516X模块操作句柄
 * \param[in] p_buf          要写入模块的数据缓冲区指针
 * \param[in] nbytes   	              要写入的字节个数
 *
 * \attention 该发送函数为异步操作函数，ZM516X的配置命令是根据串口字符帧间隔时间来判断
 * 配置命令的结束，如果在调用了am_zb_send发送函数后，要紧接着调用ZM516X的配置命令，需要
 * 延时等待发送缓存数据完全发送的时间加上ZM516X的帧间隔时间后，才能调用ZM516X的配置命令，
 * 否则ZM516X的配置命令会当作是数据发到ZM516X网络上，延时等待时间建议是：
 * T(s) = (1/波特率)*10*字节数+0.05
 *
 * \return 成功发送的数据个数，小于0为失败，失败原因可查看 errno
 */
size_t am_zm516x_send(am_zm516x_handle_t zm516x_handle, const void *p_buf, size_t nbytes);

/**
 * \brief 从ZM516X模块接收maxbytes个字节数据
 *
 * - 这个接口从ZM516X模块接收最多maxbytes个字节数据
 * - 若设备可接收数据个数为0，将立即返回
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[out] p_buf          存放读取数据的缓冲区指针
 * \param[in]  maxbytes       最多要读取的字节个数
 *
 * \return 成功接收的数据个数，小于0为失败，失败原因可查看errno
 */
size_t am_zm516x_receive(am_zm516x_handle_t zm516x_handle, void *p_buf, size_t maxbytes);

/**
 * \brief 获取ZM516X模块的配置信息（永久命令：D1）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[out] p_info         读取ZM516X模块的配置信息
 *
 * \return 0  读取成功       -1  读取失败
 */
int am_zm516x_get_config(am_zm516x_handle_t zm516x_handle, am_zm516x_cfg_info_t *p_info);

/**
 * \brief 修改ZM516X模块的配置信息（永久命令：D6）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  p_info         ZM516X模块的配置信息
 *
 * \return 0  修改成功       -1  修改失败
 *
 * \attention apollo平台操作ZM516X模块的串口参数只支持波特率配置，数据位固定为8位，
 * 校验为无校验，停止位为1位，在更改模块的数据位，校验位，停止位时需要注意，如果不
 * 小心更改了，需要把模块恢复出厂默认才能操作
 */
int am_zm516x_set_config(am_zm516x_handle_t zm516x_handle, am_zm516x_cfg_info_t *p_info);

/**
 * \brief 使ZM516X模块复位（永久命令：D9）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 *
 * \return 无
 */
void am_zm516x_reset(am_zm516x_handle_t zm516x_handle);
 
/**
 * \brief 恢复ZM516X模块出厂设置（永久命令：DA）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 *
 * \return 0  设置成功      -1  设置失败
 */
int am_zm516x_set_default(am_zm516x_handle_t zm516x_handle);

/**
 * \brief 设置ZM516X模块终端休眠时间（永久命令：DB），该接口只对组网模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  time           终端休眠时间，单位为100ms
 
 * \return  0  设置成功    -1  设置失败
 */
int am_zm516x_set_sleep_time(am_zm516x_handle_t zm516x_handle, uint16_t time);

/**
 * \brief 设置ZM516X模块终端在线时间（永久命令：DC），该接口只对组网模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  time           终端在线时间，单位为100ms
 
 * \return 0  设置成功      -1  设置失败
 */
int am_zm516x_set_online_time(am_zm516x_handle_t zm516x_handle, uint16_t time);

/**
 * \brief 设置ZM516X模块终端轮询时间（永久命令：DD），该接口只对组网模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  time           终端轮询时间，单位为100ms
 
 * \return 0  设置成功       -1  设置失败
 */
int am_zm516x_set_poll_time(am_zm516x_handle_t zm516x_handle, uint16_t time);

/**
 * \brief 设置ZM516X模块通道号（临时命令：D1），该接口只对点对点模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  chan           通道号
  
 * \return  0  设置成功      -1  设置失败
 */
int am_zm516x_set_channel(am_zm516x_handle_t zm516x_handle, uint8_t chan);

/**
 * \brief 设置ZM516X模块目标地址（临时命令：D2）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  zb_addr        目标地址
  
 * \return  0  设置成功      -1  设置失败
 */
int am_zm516x_set_dest_addr(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr);

/**
 * \brief 设置ZM516X模块接收的数据包包头是否显示源地址（临时命令：D3）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  flag           包头是否显示源地址 TRUE: 显示，FALSE: 不显示
 
 * \return  0  设置成功   -1  设置失败
 */
int am_zm516x_set_display_head(am_zm516x_handle_t zm516x_handle, bool_t flag);

/**
 * \brief 设置指定地址ZM516X模块的GPIO输入输出方向（临时命令：D4）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  zb_addr        要设置的ZM516X模块的目标地址
 * \param[in]  dir            管脚方向，该字节各个位1为输出，0为输入；bit0对应GPIO0，bit7对应GPIO7
  
 * \return  0  设置成功      -1  设置失败
 */
int am_zm516x_gpio_dir(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t dir);

/**
 * \brief 读取指定地址ZM516X模块的GPIO输入值（临时命令：D5）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  zb_addr        要读取的ZM516X模块的目标地址
 * \param[out] p_value        读取到的管脚输入值，该字节各个位1为高电平，0为低电平；bit0对应GPIO0，bit7对应GPIO7
  
 * \return  0  读取成功      -1  读取失败
 */
int am_zm516x_gpio_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t *p_value);

/**
 * \brief 设置指定地址ZM516X模块的GPIO输出值（临时命令：D6）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  zb_addr        要设置的ZM516X模块的目标地址
 * \param[in]  value          管脚输出值，该字节各个位1为高电平，0为低电平；bit0对应GPIO0，bit7对应GPIO7
  
 * \return  0  设置成功      -1  设置失败
 */
int am_zm516x_gpio_set(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t value);

/**
 * \brief 读取指定地址ZM516X模块的AD转换值（临时命令：D7）
 * 读取到的AD值为AD的转换值，需要参考ZM516X模块用户手册根据ZM516X模块AD的位数及参考电压转换为电压值
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  zb_addr        要读取的ZM516X模块的目标地址
 * \param[in]  chan           AD的通道号
 * \param[out] p_value        AD的转换值
  
 * \return  0  读取成功      -1  读取失败
 */
int am_zm516x_ad_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t chan, uint16_t *p_value);

/**
 * \brief 设置ZM516X模块进入睡眠模式（临时命令：D8）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
  
 * \return 无
 */
void am_zm516x_enter_sleep(am_zm516x_handle_t zm516x_handle);

/**
 * \brief 设置ZM516X模块的通讯模式（临时命令：D9）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  mode           模块的通讯模式
  
 * \return  0  设置成功      -1  设置失败
 */
int am_zm516x_mode_set(am_zm516x_handle_t zm516x_handle, am_zm516x_comm_mode_t mode);

/**
 * \brief 读取指定地址ZM516X模块的信号强度（临时命令：DA）
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[in]  zb_addr        要读取的本机ZM516X模块与目标地址之间的信号强度
 * \param[out] p_signal       信号强度
  
 * \return  0  读取成功       -1  读取失败
 */
int am_zm516x_sigal_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t *p_signal);

/**
 * \brief 读取ZM516X模块的邻居表（临时命令：DB），该接口只对组网模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[out] p_mac          邻居表的MAC地址缓存
 * \param[in]  maxlen         最多读取p_mac的条数
  
 * \return  读取的数目
 */
int am_zm516x_neighbour_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen);

/**
 * \brief 读取ZM516X模块的路由表（临时命令：DC），该接口只对组网模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[out] p_mac          路由表的MAC地址缓存
 * \param[in]  maxlen         最多读取p_mac的条数
  
 * \return  读取的数目
 */
int am_zm516x_route_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen);

/**
 * \brief 读取ZM516X模块在树形网络中的深度（临时命令：DD），该接口只对组网模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[out] p_deep         网络的深度
  
 * \return  0  读取成功      -1  读取失败
 */
int am_zm516x_deep_get(am_zm516x_handle_t zm516x_handle, uint16_t *p_deep);

/**
 * \brief 读取ZM516X模块的父节点（临时命令：DE），该接口只对组网模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[out] p_mac          父节点的MAC地址
  
 * \return  0  读取成功      -1  读取失败
 */
int am_zm516x_parent_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac);

/**
 * \brief 读取ZM516X模块的网络表（临时命令：DF），该接口只对组网模块有效
 *
 * \param[in]  zm516x_handle  ZM516X模块操作句柄
 * \param[out] p_mac          网络表的MAC地址缓存
 * \param[in]  maxlen         最多读取p_mac的条数
  
 * \return  读取的数目
 */
int am_zm516x_netword_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen);

/**
 * \brief 初始化ZM516X模块驱动
 *
 * \param[in]  p_dev          ZM516X模块设备结构体
 * \param[in]  ringbuf_handle 操作ZM516X模块的串口句柄
  
 * \return  zm516x_handle
 */
am_zm516x_handle_t am_zm516x_init (am_zm516x_dev_t *p_dev, am_uart_rngbuf_handle_t ringbuf_handle);

/** @} zm516x_driver */

#ifdef __cplusplus
}
#endif

#endif /* __AM_ZM516X_H */

/* end of file */
 
