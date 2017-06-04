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
 * \brief ZigBee模块ZM516X驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 15-09-06  afm, first implementation.
 * \endinternal
 */
#include "ametal.h"
#include "am_board.h"
#include "am_zm516x.h"
#include "string.h"


#define __ZM516X_COM_CONFIG_TAG     "\xAB\xBC\xCD"      /**< \brief 配置帧帧头    */
#define __ZM516X_COM_COMM_TAG       "\xDE\xDF\xEF"      /**< \brief 通讯帧帧头    */

#define __ZM516X_CMD_BUF_LEN          100               /**< \brief 命令缓存长度    */
#define __ZM516X_RSP_BUF_LEN          100               /**< \brief 应答缓存长度    */
#define __ZM516X_CMD_SPACE_TIME       5                 /**< \brief 命令帧间隔时间，单位：ms  */


/** \brief 配置和通讯命令  */
enum __ZM516X_CONFIG_CMD {
    ZM516X_GET_CONFIG     = 0xD1,                       /**< \brief  读取本地配置   */
    ZM516X_SET_CHAN       = 0xD2,                       /**< \brief  设置通道号   */
    ZM516X_SET_SPEED      = 0xD3,                       /**< \brief  设置速率    */
    ZM516X_DISCOVER       = 0xD4,                       /**< \brief  搜索   */
    ZM516X_GET_MSG        = 0xD5,                       /**< \brief  获取信息命令  */
    ZM516X_SET_CONFIG       = 0xD6,                     /**< \brief  修改配置  */
    ZM516X_RESET          = 0xD9,                       /**< \brief  复位    */
    ZM516X_REDEFAULT      = 0xDA,                       /**< \brief  恢复出厂设置   */
    ZM516X_SLEEP_TIME     = 0xDB,                       /**< \brief  终端休眠时间,单位：100ms */
    ZM516X_ACTIVE_TIME    = 0xDC,                       /**< \brief  终端在线时间,单位：100ms */
    ZM516X_POLL_TIME      = 0xDD,                       /**< \brief  终端轮询时间,单位：100ms */
    ZM516X_PWD_ENABLE     = 0xDE,                       /**< \brief  模块密码使能     */
    ZM516X_LOGIN          = 0xDF,                       /**< \brief  模块登录    */
};

enum __ZM516X_COM_CMD {
    ZM516X_COM_SET_CHAN   = 0xD1,                       /**< \brief  修改通道   */
    ZM516X_COM_SET_DEST   = 0xD2,                       /**< \brief  修改目的地址   */
    ZM516X_COM_DIS_SRC    = 0xD3,                       /**< \brief  包头显示源地址   */
    ZM516X_SET_GPIO_DIR   = 0xD4,                       /**< \brief  设置GPIO方向    */
    ZM516X_GET_GPIO       = 0xD5,                       /**< \brief  获取GPIO电平    */
    ZM516X_SET_GPIO       = 0xD6,                       /**< \brief  设置GPIO电平   */
    ZM516X_GET_ADC        = 0xD7,                       /**< \brief  获取ADC数值   */
    ZM516X_COM_SLEEP      = 0xD8,                       /**< \brief  进入休眠   */
    ZM516X_COM_SET_MODE   = 0xD9,                       /**< \brief  设置通讯模式   */
    ZM516X_COM_GET_CSQ    = 0xDA,                       /**< \brief  获取信号强度    */
    ZM516X_GET_NEIGHBOUR_TABLE = 0xDB,                  /**< \brief  获取邻居表      */
    ZM516X_GET_ROUTING_TABLE   = 0xDC,                  /**< \brief  获取路由表     */
    ZM516X_GET_DEPTH           = 0xDD,                  /**< \brief  获取树形网络的深度    */
    ZM516X_GET_PARENT          = 0xDE,                  /**< \brief  获取本地节点的父节点    */
    ZM516X_GET_NETWORK_TABLE   = 0xDF,                  /**< \brief  获取网络表    */
};

/** \brief 配置和通讯应答  */
enum __ZM516X_CONFIG_ERR {
    ZM516X_CONFIG_OK           = 0x00,                   /**< \brief  操作成功   */
    ZM516X_ADDRESS_FAUSE       = 0x01,                   /**< \brief  地址错误    */
    ZM516X_LENGTH_FAUSE        = 0x02,                   /**< \brief  帧长度错误  */
    ZM516X_CHECK_FAUSE         = 0x03,                   /**< \brief  帧字节校验错误   */
    ZM516X_WRITE_FAUSE         = 0x04,                   /**< \brief  写错误   */
    ZM516X_CONFIG_OTHER_FAUSE  = 0x05,                   /**< \brief  其它错误    */
};

enum __ZM516X_COM_ERR {
    ZM516X_COM_OK              = 0x00,                   /**< \brief  操作成功    */
    ZM516X_SEND_DATA_TIMEOUT   = 0xA4,                   /**< \brief  发送数据超时  */
    ZM516X_DEVICE_BUSY         = 0xA5,                   /**< \brief  设备忙   */
    ZM516X_CMD_LEN_FAUSE       = 0xA6,                   /**< \brief  命令长度错误   */
    ZM516X_COM_OTHER_FAUSE     = 0xA7,                   /**< \brief  其它错误   */
};

/**
 * @addtogroup am_zm516x
 * @{
 */

/** \brief 发送zigbee命令  */
am_local bool_t am_zm516x_send_cmd(am_uart_rngbuf_handle_t ringbuf_handle, uint8_t *p_cmd, uint16_t cmd_len,
                                   uint8_t *p_rsp, uint16_t *p_rsp_len, uint16_t timeout)
{
    uint16_t i;
    int32_t  len = 0, index = 0;
    bool_t   flag = FALSE;

    am_uart_rngbuf_send(ringbuf_handle, p_cmd, cmd_len);

    if (p_rsp == NULL) {            /* 应答无返回值   */
        return TRUE;
    }

    for (i=0; i<timeout; i++) {
        len = am_uart_rngbuf_receive(ringbuf_handle, p_rsp, __ZM516X_RSP_BUF_LEN);
        if (len > 0) {
            index = len;
            flag = TRUE;
            break;
        }
    }
    if (flag == FALSE) {
        return FALSE;
    }

    for (i=0; i<10; i++) {            /* 应答帧间隔为100ms，应答完成   */
        len = am_uart_rngbuf_receive(ringbuf_handle, &p_rsp[index], __ZM516X_RSP_BUF_LEN-index);
        if (len > 0) {
            index += len;
            i = 0;
            continue;
        }
    }

    *p_rsp_len = index;

    return TRUE;
}

/** \brief 计算zigbee模块帧的校验和  */
am_local uint8_t zm_zm516x_checksum(uint8_t *p_data, uint16_t nbytes)
{
    uint16_t i;
    uint8_t  sum = 0;

    for (i=0; i<nbytes; i++) {
        sum += p_data[i];
    }
    return sum;
}

/** \brief 向zigbee模块发送nbytes个字节数据 */
size_t am_zm516x_send(am_zm516x_handle_t zm516x_handle, const void *p_buf, size_t nbytes)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;

    return am_uart_rngbuf_send(p_dev->uart_handle, (uint8_t *)p_buf, nbytes);
}

/** \brief 从zigbee模块接收maxbytes个字节数据 */
size_t am_zm516x_receive(am_zm516x_handle_t zm516x_handle, void *p_buf, size_t maxbytes)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;

    return am_uart_rngbuf_receive(p_dev->uart_handle, (uint8_t *)p_buf, maxbytes);
}

/** \brief 获取zigbee模块的配置信息（永久命令：D1）*/
int am_zm516x_get_config(am_zm516x_handle_t zm516x_handle, am_zm516x_cfg_info_t *p_info)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_CONFIG_TAG, 3);
    cmd_buf[3] = ZM516X_GET_CONFIG;
    cmd_buf[4] = zm_zm516x_checksum(cmd_buf, 4);
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 5, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_CONFIG_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_GET_CONFIG) {
            return -AM_EPERM;
        }
        memcpy(p_info, &rsp_buf[4], sizeof(am_zm516x_cfg_info_t));
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 修改zigbee模块的配置信息（永久命令：D6）*/
int am_zm516x_set_config(am_zm516x_handle_t zm516x_handle, am_zm516x_cfg_info_t *p_info)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    am_zm516x_cfg_info_t zb_info;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    if (am_zm516x_get_config(zm516x_handle, &zb_info) != AM_OK) {
        return -AM_EPERM;
    }

    memcpy(cmd_buf, __ZM516X_COM_CONFIG_TAG, 3);
    cmd_buf[3] = ZM516X_SET_CONFIG;
    cmd_buf[4] = zb_info.my_addr[0];
    cmd_buf[5] = zb_info.my_addr[1];
    memcpy(&cmd_buf[6], p_info, sizeof(am_zm516x_cfg_info_t));
    cmd_buf[6+sizeof(am_zm516x_cfg_info_t)] = zm_zm516x_checksum(cmd_buf, 6+sizeof(am_zm516x_cfg_info_t));
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 7+sizeof(am_zm516x_cfg_info_t), rsp_buf, &rsp_len, 100) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_CONFIG_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_SET_CONFIG) {
            return -AM_EPERM;
        }
        if (rsp_buf[6] != ZM516X_CONFIG_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 使zigbee模块复位（永久命令：D9） */
void am_zm516x_reset(am_zm516x_handle_t zm516x_handle)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    am_zm516x_cfg_info_t zb_info;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint16_t rsp_len;

    if (am_zm516x_get_config(zm516x_handle, &zb_info) != AM_OK) {
        return;
    }

    memcpy(cmd_buf, __ZM516X_COM_CONFIG_TAG, 3);
    cmd_buf[3] = ZM516X_RESET;
    cmd_buf[4] = zb_info.my_addr[0];
    cmd_buf[5] = zb_info.my_addr[1];
    cmd_buf[6] = 0x00;
    cmd_buf[7] = 0x01;
    cmd_buf[8] = zm_zm516x_checksum(cmd_buf, 8);
    am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 9, NULL, &rsp_len, 50);
    am_mdelay(200);
}

/** \brief 恢复zigbee模块出厂设置（永久命令：DA） */
int am_zm516x_set_default(am_zm516x_handle_t zm516x_handle)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    am_zm516x_cfg_info_t zb_info;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    if (am_zm516x_get_config(zm516x_handle, &zb_info) != AM_OK) {
        return -AM_EPERM;
    }

    memcpy(cmd_buf, __ZM516X_COM_CONFIG_TAG, 3);
    cmd_buf[3] = ZM516X_REDEFAULT;
    cmd_buf[4] = zb_info.my_addr[0];
    cmd_buf[5] = zb_info.my_addr[1];
    cmd_buf[6] = 0x00;
    cmd_buf[7] = 0x01;
    cmd_buf[8] = zm_zm516x_checksum(cmd_buf, 8);
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 9, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_CONFIG_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_REDEFAULT) {
            return -AM_EPERM;
        }
        if (rsp_buf[8] != ZM516X_CONFIG_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 设置zigbee模块终端休眠时间（永久命令：DB），该接口只对组网模块有效  */
int am_zm516x_set_sleep_time(am_zm516x_handle_t zm516x_handle, uint16_t time)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_CONFIG_TAG, 3);
    cmd_buf[3] = ZM516X_SLEEP_TIME;
    cmd_buf[4] = 0x01;
    cmd_buf[5] = (time >> 8) & 0x00ff;
    cmd_buf[6] = time & 0x00ff;
    cmd_buf[7] = zm_zm516x_checksum(cmd_buf, 7);
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 8, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_CONFIG_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_SLEEP_TIME) {
            return -AM_EPERM;
        }
        if (rsp_buf[6] != ZM516X_CONFIG_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/**  \brief 设置zigbee模块终端在线时间（永久命令：DC），该接口只对组网模块有效  */
int am_zm516x_set_online_time(am_zm516x_handle_t zm516x_handle, uint16_t time)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_CONFIG_TAG, 3);
    cmd_buf[3] = ZM516X_ACTIVE_TIME;
    cmd_buf[4] = 0x01;
    cmd_buf[5] = (time >> 8) & 0x00ff;
    cmd_buf[6] = time & 0x00ff;
    cmd_buf[7] = zm_zm516x_checksum(cmd_buf, 7);
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 8, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_CONFIG_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_ACTIVE_TIME) {
            return -AM_EPERM;
        }
        if (rsp_buf[6] != ZM516X_CONFIG_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 设置zigbee模块终端轮询时间（永久命令：DD），该接口只对组网模块有效  */
int am_zm516x_set_poll_time(am_zm516x_handle_t zm516x_handle, uint16_t time)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_CONFIG_TAG, 3);
    cmd_buf[3] = ZM516X_POLL_TIME;
    cmd_buf[4] = 0x01;
    cmd_buf[5] = (time >> 8) & 0x00ff;
    cmd_buf[6] = time & 0x00ff;
    cmd_buf[7] = zm_zm516x_checksum(cmd_buf, 7);
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 8, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_CONFIG_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_POLL_TIME) {
            return -AM_EPERM;
        }
        if (rsp_buf[6] != ZM516X_CONFIG_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 设置zigbee模块通道号（临时命令：D1），该接口只对点对点模块有效  */
int am_zm516x_set_channel(am_zm516x_handle_t zm516x_handle, uint8_t chan)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_COM_SET_CHAN;
    cmd_buf[4] = chan;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 5, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_COM_SET_CHAN) {
            return -AM_EPERM;
        }
        if (rsp_buf[4] != ZM516X_COM_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 设置zigbee模块目标地址（临时命令：D2） */
int am_zm516x_set_dest_addr(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_COM_SET_DEST;
    memcpy(&cmd_buf[4], zb_addr.p_addr, zb_addr.addr_size);
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 4+zb_addr.addr_size, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_COM_SET_DEST) {
            return -AM_EPERM;
        }
        if (rsp_buf[4] != ZM516X_COM_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 设置zigbee模块接收的数据包包头是否显示源地址（临时命令：D3） */
int am_zm516x_set_display_head(am_zm516x_handle_t zm516x_handle, bool_t flag)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_COM_DIS_SRC;
    cmd_buf[4] = flag;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 5, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_COM_DIS_SRC) {
            return -AM_EPERM;
        }
        if (rsp_buf[4] != ZM516X_COM_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 设置指定地址zigbee模块的GPIO输入输出方向（临时命令：D4） */
int am_zm516x_gpio_dir(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t dir)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_SET_GPIO_DIR;
    memcpy(&cmd_buf[4], zb_addr.p_addr, zb_addr.addr_size);
    cmd_buf[4+zb_addr.addr_size] = dir;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 5+zb_addr.addr_size, rsp_buf, &rsp_len, 100) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_SET_GPIO_DIR) {
            return -AM_EPERM;
        }
        if (rsp_buf[4+zb_addr.addr_size] != ZM516X_COM_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 读取指定地址zigbee模块的GPIO输入值（临时命令：D5）  */
int am_zm516x_gpio_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t *p_value)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_GET_GPIO;
    memcpy(&cmd_buf[4], zb_addr.p_addr, zb_addr.addr_size);
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 4+zb_addr.addr_size, rsp_buf, &rsp_len, 100) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_GET_GPIO) {
            return -AM_EPERM;
        }
        *p_value = rsp_buf[4+zb_addr.addr_size];
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 设置指定地址zigbee模块的GPIO输出值（临时命令：D6） */
int am_zm516x_gpio_set(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t value)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_SET_GPIO;
    memcpy(&cmd_buf[4], zb_addr.p_addr, zb_addr.addr_size);
    cmd_buf[4+zb_addr.addr_size] = value;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 5+zb_addr.addr_size, rsp_buf, &rsp_len, 100) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_SET_GPIO) {
            return -AM_EPERM;
        }
        if (rsp_buf[4+zb_addr.addr_size] != ZM516X_COM_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 读取指定地址zigbee模块的AD转换值（临时命令：D7） */
int am_zm516x_ad_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t chan, uint16_t *p_value)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_GET_ADC;
    memcpy(&cmd_buf[4], zb_addr.p_addr, zb_addr.addr_size);
    cmd_buf[4+zb_addr.addr_size] = chan;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 5+zb_addr.addr_size, rsp_buf, &rsp_len, 100) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_GET_ADC) {
            return -AM_EPERM;
        }
        *p_value = rsp_buf[4+zb_addr.addr_size] << 8 | rsp_buf[5+zb_addr.addr_size];
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 设置zigbee模块进入睡眠模式（临时命令：D8） */
void am_zm516x_enter_sleep(am_zm516x_handle_t zm516x_handle)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_COM_SLEEP;
    cmd_buf[4] = 0x01;
    am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 5, NULL, &rsp_len, 50);
    am_mdelay(200);
}

/** \brief 设置zigbee模块的通讯模式（临时命令：D9）  */
int am_zm516x_mode_set(am_zm516x_handle_t zm516x_handle, am_zm516x_comm_mode_t mode)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_COM_SET_MODE;
    cmd_buf[4] = mode;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 5, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_COM_SET_MODE) {
            return -AM_EPERM;
        }
        if (rsp_buf[4] != ZM516X_COM_OK) {
            return -AM_EPERM;
        }
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 读取指定地址zigbee模块的信号强度（临时命令：DA） */
int am_zm516x_sigal_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t *p_signal)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_COM_GET_CSQ;
    memcpy(&cmd_buf[4], zb_addr.p_addr, zb_addr.addr_size);
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 4+zb_addr.addr_size, rsp_buf, &rsp_len, 100) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_COM_GET_CSQ) {
            return -AM_EPERM;
        }
        *p_signal = rsp_buf[4+zb_addr.addr_size];
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 读取zigbee模块的邻居表（临时命令：DB），该接口只对组网模块有效  */
int am_zm516x_neighbour_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;
    uint8_t table_len;
    uint8_t i;
    uint8_t *p_tabel;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_GET_NEIGHBOUR_TABLE;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 4, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return 0;
        }
        if (rsp_buf[3] != ZM516X_GET_NEIGHBOUR_TABLE) {
            return 0;
        }
        table_len = rsp_buf[4];
        table_len = table_len > maxlen?maxlen:table_len;
        p_tabel = &rsp_buf[5];
        for (i=0; i<table_len; i++) {
            memcpy(p_mac->mac, p_tabel, 8);
            p_mac++;
            p_tabel += 8;
        }
        return table_len;
    }
    return 0;
}

/** \brief 读取zigbee模块的路由表（临时命令：DC），该接口只对组网模块有效  */
int am_zm516x_route_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;
    uint8_t table_len;
    uint8_t i;
    uint8_t *p_tabel;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_GET_ROUTING_TABLE;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 4, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return 0;
        }
        if (rsp_buf[3] != ZM516X_GET_ROUTING_TABLE) {
            return 0;
        }
        table_len = rsp_buf[4];
        table_len = table_len > maxlen?maxlen:table_len;
        p_tabel = &rsp_buf[5];
        for (i=0; i<table_len; i++) {
            memcpy(p_mac->mac, p_tabel, 8);
            p_mac++;
            p_tabel += 8;
        }
        return table_len;
    }
    return 0;
}

/** \brief 读取zigbee模块在树形网络中的深度（临时命令：DD），该接口只对组网模块有效  */
int am_zm516x_deep_get(am_zm516x_handle_t zm516x_handle, uint16_t *p_deep)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_GET_DEPTH;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 4, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_GET_DEPTH) {
            return -AM_EPERM;
        }
        *p_deep = rsp_buf[4]<<8 | rsp_buf[5];
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 读取zigbee模块的父节点（临时命令：DE），该接口只对组网模块有效  */
int am_zm516x_parent_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_GET_PARENT;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 4, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return -AM_EPERM;
        }
        if (rsp_buf[3] != ZM516X_GET_PARENT) {
            return -AM_EPERM;
        }
        memcpy(p_mac->mac, &rsp_buf[4], 8);
        return AM_OK;
    }
    return -AM_EPERM;
}

/** \brief 读取zigbee模块的网络表（临时命令：DF），该接口只对组网模块有效  */
int am_zm516x_netword_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;
    uint8_t cmd_buf[__ZM516X_CMD_BUF_LEN] = {0};
    uint8_t rsp_buf[__ZM516X_RSP_BUF_LEN] = {0};
    uint16_t rsp_len;
    uint8_t table_len;
    uint8_t i;
    uint8_t *p_tabel;

    memcpy(cmd_buf, __ZM516X_COM_COMM_TAG, 3);
    cmd_buf[3] = ZM516X_GET_NETWORK_TABLE;
    if (am_zm516x_send_cmd(p_dev->uart_handle, cmd_buf, 4, rsp_buf, &rsp_len, 50) == TRUE) {
        if (memcmp(rsp_buf, __ZM516X_COM_COMM_TAG, 3) != 0) {
            return 0;
        }
        if (rsp_buf[3] != ZM516X_GET_NETWORK_TABLE) {
            return 0;
        }
        table_len = rsp_buf[4];
        table_len = table_len > maxlen?maxlen:table_len;
        p_tabel = &rsp_buf[5];
        for (i=0; i<table_len; i++) {
            memcpy(p_mac->mac, p_tabel, 8);
            p_mac++;
            p_tabel += 8;
        }
        return table_len;
    }
    return 0;
}

/** \brief ZM516X模块初始化  */
am_zm516x_handle_t am_zm516x_init (am_zm516x_dev_t *p_dev, am_uart_rngbuf_handle_t ringbuf_handle)
{
    if (p_dev == NULL || ringbuf_handle == NULL) {
        return NULL;
    }

    p_dev->uart_handle = ringbuf_handle;

    am_uart_rngbuf_ioctl(ringbuf_handle, AM_UART_RINGBUF_TIMEOUT, (void *)10);

    return (am_zm516x_handle_t)(p_dev);
}

/** @} */

/* end of file */
