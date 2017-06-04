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
 * \brief ZigBeeģ��ZM516X����
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


#define __ZM516X_COM_CONFIG_TAG     "\xAB\xBC\xCD"      /**< \brief ����֡֡ͷ    */
#define __ZM516X_COM_COMM_TAG       "\xDE\xDF\xEF"      /**< \brief ͨѶ֡֡ͷ    */

#define __ZM516X_CMD_BUF_LEN          100               /**< \brief ����泤��    */
#define __ZM516X_RSP_BUF_LEN          100               /**< \brief Ӧ�𻺴泤��    */
#define __ZM516X_CMD_SPACE_TIME       5                 /**< \brief ����֡���ʱ�䣬��λ��ms  */


/** \brief ���ú�ͨѶ����  */
enum __ZM516X_CONFIG_CMD {
    ZM516X_GET_CONFIG     = 0xD1,                       /**< \brief  ��ȡ��������   */
    ZM516X_SET_CHAN       = 0xD2,                       /**< \brief  ����ͨ����   */
    ZM516X_SET_SPEED      = 0xD3,                       /**< \brief  ��������    */
    ZM516X_DISCOVER       = 0xD4,                       /**< \brief  ����   */
    ZM516X_GET_MSG        = 0xD5,                       /**< \brief  ��ȡ��Ϣ����  */
    ZM516X_SET_CONFIG       = 0xD6,                     /**< \brief  �޸�����  */
    ZM516X_RESET          = 0xD9,                       /**< \brief  ��λ    */
    ZM516X_REDEFAULT      = 0xDA,                       /**< \brief  �ָ���������   */
    ZM516X_SLEEP_TIME     = 0xDB,                       /**< \brief  �ն�����ʱ��,��λ��100ms */
    ZM516X_ACTIVE_TIME    = 0xDC,                       /**< \brief  �ն�����ʱ��,��λ��100ms */
    ZM516X_POLL_TIME      = 0xDD,                       /**< \brief  �ն���ѯʱ��,��λ��100ms */
    ZM516X_PWD_ENABLE     = 0xDE,                       /**< \brief  ģ������ʹ��     */
    ZM516X_LOGIN          = 0xDF,                       /**< \brief  ģ���¼    */
};

enum __ZM516X_COM_CMD {
    ZM516X_COM_SET_CHAN   = 0xD1,                       /**< \brief  �޸�ͨ��   */
    ZM516X_COM_SET_DEST   = 0xD2,                       /**< \brief  �޸�Ŀ�ĵ�ַ   */
    ZM516X_COM_DIS_SRC    = 0xD3,                       /**< \brief  ��ͷ��ʾԴ��ַ   */
    ZM516X_SET_GPIO_DIR   = 0xD4,                       /**< \brief  ����GPIO����    */
    ZM516X_GET_GPIO       = 0xD5,                       /**< \brief  ��ȡGPIO��ƽ    */
    ZM516X_SET_GPIO       = 0xD6,                       /**< \brief  ����GPIO��ƽ   */
    ZM516X_GET_ADC        = 0xD7,                       /**< \brief  ��ȡADC��ֵ   */
    ZM516X_COM_SLEEP      = 0xD8,                       /**< \brief  ��������   */
    ZM516X_COM_SET_MODE   = 0xD9,                       /**< \brief  ����ͨѶģʽ   */
    ZM516X_COM_GET_CSQ    = 0xDA,                       /**< \brief  ��ȡ�ź�ǿ��    */
    ZM516X_GET_NEIGHBOUR_TABLE = 0xDB,                  /**< \brief  ��ȡ�ھӱ�      */
    ZM516X_GET_ROUTING_TABLE   = 0xDC,                  /**< \brief  ��ȡ·�ɱ�     */
    ZM516X_GET_DEPTH           = 0xDD,                  /**< \brief  ��ȡ������������    */
    ZM516X_GET_PARENT          = 0xDE,                  /**< \brief  ��ȡ���ؽڵ�ĸ��ڵ�    */
    ZM516X_GET_NETWORK_TABLE   = 0xDF,                  /**< \brief  ��ȡ�����    */
};

/** \brief ���ú�ͨѶӦ��  */
enum __ZM516X_CONFIG_ERR {
    ZM516X_CONFIG_OK           = 0x00,                   /**< \brief  �����ɹ�   */
    ZM516X_ADDRESS_FAUSE       = 0x01,                   /**< \brief  ��ַ����    */
    ZM516X_LENGTH_FAUSE        = 0x02,                   /**< \brief  ֡���ȴ���  */
    ZM516X_CHECK_FAUSE         = 0x03,                   /**< \brief  ֡�ֽ�У�����   */
    ZM516X_WRITE_FAUSE         = 0x04,                   /**< \brief  д����   */
    ZM516X_CONFIG_OTHER_FAUSE  = 0x05,                   /**< \brief  ��������    */
};

enum __ZM516X_COM_ERR {
    ZM516X_COM_OK              = 0x00,                   /**< \brief  �����ɹ�    */
    ZM516X_SEND_DATA_TIMEOUT   = 0xA4,                   /**< \brief  �������ݳ�ʱ  */
    ZM516X_DEVICE_BUSY         = 0xA5,                   /**< \brief  �豸æ   */
    ZM516X_CMD_LEN_FAUSE       = 0xA6,                   /**< \brief  ����ȴ���   */
    ZM516X_COM_OTHER_FAUSE     = 0xA7,                   /**< \brief  ��������   */
};

/**
 * @addtogroup am_zm516x
 * @{
 */

/** \brief ����zigbee����  */
am_local bool_t am_zm516x_send_cmd(am_uart_rngbuf_handle_t ringbuf_handle, uint8_t *p_cmd, uint16_t cmd_len,
                                   uint8_t *p_rsp, uint16_t *p_rsp_len, uint16_t timeout)
{
    uint16_t i;
    int32_t  len = 0, index = 0;
    bool_t   flag = FALSE;

    am_uart_rngbuf_send(ringbuf_handle, p_cmd, cmd_len);

    if (p_rsp == NULL) {            /* Ӧ���޷���ֵ   */
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

    for (i=0; i<10; i++) {            /* Ӧ��֡���Ϊ100ms��Ӧ�����   */
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

/** \brief ����zigbeeģ��֡��У���  */
am_local uint8_t zm_zm516x_checksum(uint8_t *p_data, uint16_t nbytes)
{
    uint16_t i;
    uint8_t  sum = 0;

    for (i=0; i<nbytes; i++) {
        sum += p_data[i];
    }
    return sum;
}

/** \brief ��zigbeeģ�鷢��nbytes���ֽ����� */
size_t am_zm516x_send(am_zm516x_handle_t zm516x_handle, const void *p_buf, size_t nbytes)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;

    return am_uart_rngbuf_send(p_dev->uart_handle, (uint8_t *)p_buf, nbytes);
}

/** \brief ��zigbeeģ�����maxbytes���ֽ����� */
size_t am_zm516x_receive(am_zm516x_handle_t zm516x_handle, void *p_buf, size_t maxbytes)
{
    am_zm516x_dev_t *p_dev = (am_zm516x_dev_t *)zm516x_handle;

    return am_uart_rngbuf_receive(p_dev->uart_handle, (uint8_t *)p_buf, maxbytes);
}

/** \brief ��ȡzigbeeģ���������Ϣ���������D1��*/
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

/** \brief �޸�zigbeeģ���������Ϣ���������D6��*/
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

/** \brief ʹzigbeeģ�鸴λ���������D9�� */
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

/** \brief �ָ�zigbeeģ��������ã��������DA�� */
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

/** \brief ����zigbeeģ���ն�����ʱ�䣨�������DB�����ýӿ�ֻ������ģ����Ч  */
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

/**  \brief ����zigbeeģ���ն�����ʱ�䣨�������DC�����ýӿ�ֻ������ģ����Ч  */
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

/** \brief ����zigbeeģ���ն���ѯʱ�䣨�������DD�����ýӿ�ֻ������ģ����Ч  */
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

/** \brief ����zigbeeģ��ͨ���ţ���ʱ���D1�����ýӿ�ֻ�Ե�Ե�ģ����Ч  */
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

/** \brief ����zigbeeģ��Ŀ���ַ����ʱ���D2�� */
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

/** \brief ����zigbeeģ����յ����ݰ���ͷ�Ƿ���ʾԴ��ַ����ʱ���D3�� */
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

/** \brief ����ָ����ַzigbeeģ���GPIO�������������ʱ���D4�� */
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

/** \brief ��ȡָ����ַzigbeeģ���GPIO����ֵ����ʱ���D5��  */
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

/** \brief ����ָ����ַzigbeeģ���GPIO���ֵ����ʱ���D6�� */
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

/** \brief ��ȡָ����ַzigbeeģ���ADת��ֵ����ʱ���D7�� */
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

/** \brief ����zigbeeģ�����˯��ģʽ����ʱ���D8�� */
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

/** \brief ����zigbeeģ���ͨѶģʽ����ʱ���D9��  */
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

/** \brief ��ȡָ����ַzigbeeģ����ź�ǿ�ȣ���ʱ���DA�� */
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

/** \brief ��ȡzigbeeģ����ھӱ���ʱ���DB�����ýӿ�ֻ������ģ����Ч  */
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

/** \brief ��ȡzigbeeģ���·�ɱ���ʱ���DC�����ýӿ�ֻ������ģ����Ч  */
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

/** \brief ��ȡzigbeeģ�������������е���ȣ���ʱ���DD�����ýӿ�ֻ������ģ����Ч  */
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

/** \brief ��ȡzigbeeģ��ĸ��ڵ㣨��ʱ���DE�����ýӿ�ֻ������ģ����Ч  */
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

/** \brief ��ȡzigbeeģ����������ʱ���DF�����ýӿ�ֻ������ģ����Ч  */
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

/** \brief ZM516Xģ���ʼ��  */
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
