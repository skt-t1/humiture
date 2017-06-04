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
 * \brief ZigBeeģ��ZM516X�����ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� am_zm516x.h
 *
 * \par ʹ��ʾ��
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

/** \brief ZM516Xģ�������� */
typedef struct am_zm516x_dev {
    am_uart_rngbuf_handle_t uart_handle;
} am_zm516x_dev_t;

/** \brief ZM516Xģ���׼�������������Ͷ��� */
typedef struct am_zm516x_dev *am_zm516x_handle_t;

/** \brief ZM516Xģ���������Ϣ */
typedef struct am_zm516x_cfg_info {
    char    dev_name[16];     /**< \brief �豸����      */
    char    dev_pwd[16];      /**< \brief �豸����      */
    uint8_t dev_mode;         /**< \brief ��������      */
    uint8_t chan;             /**< \brief ͨ����        */
    uint8_t panid[2];         /**< \brief PanID         */
    uint8_t my_addr[2];       /**< \brief ���������ַ  */
    uint8_t my_mac[8];        /**< \brief ����MAC��ַ   */
    uint8_t dst_addr[2];      /**< \brief Ŀ�������ַ  */
    uint8_t dst_mac[8];       /**< \brief Ŀ��MAC��ַ   */
    uint8_t data_rate;        /**< \brief RFͨѶ����    */
    uint8_t power_level;      /**< \brief ���书��      */
    uint8_t retry_num;        /**< \brief �����������Դ���           */
    uint8_t tran_timeout;     /**< \brief ������������ʱ����(10ms) */
    uint8_t serial_rate;      /**< \brief ���ڲ����� ֵΪ1��7���ֱ��Ӧ�����ʣ�2400��4800��9600��19200��38400��57600��115200 */
    uint8_t serial_data;      /**< \brief ��������λ ֵΪ 5��8       */
    uint8_t serial_stop;      /**< \brief ����ֹͣλ ֵΪ 1��2       */
    uint8_t serial_parity;    /**< \brief ����У��λ 0Ϊ��У�� 1Ϊ��У�� 2ΪżУ��  */
    uint8_t send_mode;        /**< \brief ����ģʽ 0 ����  1 �㲥   */
} am_zm516x_cfg_info_t;

/**
 * \name ZM516Xģ���ADͨ����
 * @{
 */

#define AM_ZM516X_AD_CHANNEL0       0
#define AM_ZM516X_AD_CHANNEL1       1
#define AM_ZM516X_AD_CHANNEL2       2
#define AM_ZM516X_AD_CHANNEL3       3

/** @} */

/** \brief ZM516Xģ���ͨѶģʽ */
typedef enum am_zm516x_comm_mode {
    AM_ZM516X_COMM_UNICAST = 0,                     /**< \brief ����  */
    AM_ZM516X_COMM_BROADCAST,                      	/**< \brief �㲥  */
} am_zm516x_comm_mode_t;

/** \brief ZM516Xģ���MAC��ַ */
typedef struct am_zm516x_mac_addr {
    uint8_t mac[8];
}am_zm516x_mac_addr_t;

/**
* \brief ZM516Xģ��ĵ�ַ
*
* ��ַ��Ϊ2�ֽڵ������ַ��8�ֽڵ�MAC��ַ��ʹ��addr_size��ַ���������ֵ�ַ���� 
*/
typedef struct am_zm516x_addr {
    uint8_t *p_addr;
    uint8_t  addr_size;
}am_zm516x_addr_t;

/**
 * \brief ��ZM516Xģ�鷢��nbytes���ֽ�����
 *
 * ����ӿ���ZM516Xģ�鷢��nbytes���ֽ����ݡ����豸���ڲ����岻�����������´���
 * ʣ������ݲ��ᱻд�룬����ֵΪ�Ѿ����͵����ݸ���
 *
 * \param[in] zm516x_handle  ZM516Xģ��������
 * \param[in] p_buf          Ҫд��ģ������ݻ�����ָ��
 * \param[in] nbytes   	              Ҫд����ֽڸ���
 *
 * \attention �÷��ͺ���Ϊ�첽����������ZM516X�����������Ǹ��ݴ����ַ�֡���ʱ�����ж�
 * ��������Ľ���������ڵ�����am_zb_send���ͺ�����Ҫ�����ŵ���ZM516X�����������Ҫ
 * ��ʱ�ȴ����ͻ���������ȫ���͵�ʱ�����ZM516X��֡���ʱ��󣬲��ܵ���ZM516X���������
 * ����ZM516X����������ᵱ�������ݷ���ZM516X�����ϣ���ʱ�ȴ�ʱ�佨���ǣ�
 * T(s) = (1/������)*10*�ֽ���+0.05
 *
 * \return �ɹ����͵����ݸ�����С��0Ϊʧ�ܣ�ʧ��ԭ��ɲ鿴 errno
 */
size_t am_zm516x_send(am_zm516x_handle_t zm516x_handle, const void *p_buf, size_t nbytes);

/**
 * \brief ��ZM516Xģ�����maxbytes���ֽ�����
 *
 * - ����ӿڴ�ZM516Xģ��������maxbytes���ֽ�����
 * - ���豸�ɽ������ݸ���Ϊ0������������
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[out] p_buf          ��Ŷ�ȡ���ݵĻ�����ָ��
 * \param[in]  maxbytes       ���Ҫ��ȡ���ֽڸ���
 *
 * \return �ɹ����յ����ݸ�����С��0Ϊʧ�ܣ�ʧ��ԭ��ɲ鿴errno
 */
size_t am_zm516x_receive(am_zm516x_handle_t zm516x_handle, void *p_buf, size_t maxbytes);

/**
 * \brief ��ȡZM516Xģ���������Ϣ���������D1��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[out] p_info         ��ȡZM516Xģ���������Ϣ
 *
 * \return 0  ��ȡ�ɹ�       -1  ��ȡʧ��
 */
int am_zm516x_get_config(am_zm516x_handle_t zm516x_handle, am_zm516x_cfg_info_t *p_info);

/**
 * \brief �޸�ZM516Xģ���������Ϣ���������D6��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  p_info         ZM516Xģ���������Ϣ
 *
 * \return 0  �޸ĳɹ�       -1  �޸�ʧ��
 *
 * \attention apolloƽ̨����ZM516Xģ��Ĵ��ڲ���ֻ֧�ֲ��������ã�����λ�̶�Ϊ8λ��
 * У��Ϊ��У�飬ֹͣλΪ1λ���ڸ���ģ�������λ��У��λ��ֹͣλʱ��Ҫע�⣬�����
 * С�ĸ����ˣ���Ҫ��ģ��ָ�����Ĭ�ϲ��ܲ���
 */
int am_zm516x_set_config(am_zm516x_handle_t zm516x_handle, am_zm516x_cfg_info_t *p_info);

/**
 * \brief ʹZM516Xģ�鸴λ���������D9��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 *
 * \return ��
 */
void am_zm516x_reset(am_zm516x_handle_t zm516x_handle);
 
/**
 * \brief �ָ�ZM516Xģ��������ã��������DA��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 *
 * \return 0  ���óɹ�      -1  ����ʧ��
 */
int am_zm516x_set_default(am_zm516x_handle_t zm516x_handle);

/**
 * \brief ����ZM516Xģ���ն�����ʱ�䣨�������DB�����ýӿ�ֻ������ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  time           �ն�����ʱ�䣬��λΪ100ms
 
 * \return  0  ���óɹ�    -1  ����ʧ��
 */
int am_zm516x_set_sleep_time(am_zm516x_handle_t zm516x_handle, uint16_t time);

/**
 * \brief ����ZM516Xģ���ն�����ʱ�䣨�������DC�����ýӿ�ֻ������ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  time           �ն�����ʱ�䣬��λΪ100ms
 
 * \return 0  ���óɹ�      -1  ����ʧ��
 */
int am_zm516x_set_online_time(am_zm516x_handle_t zm516x_handle, uint16_t time);

/**
 * \brief ����ZM516Xģ���ն���ѯʱ�䣨�������DD�����ýӿ�ֻ������ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  time           �ն���ѯʱ�䣬��λΪ100ms
 
 * \return 0  ���óɹ�       -1  ����ʧ��
 */
int am_zm516x_set_poll_time(am_zm516x_handle_t zm516x_handle, uint16_t time);

/**
 * \brief ����ZM516Xģ��ͨ���ţ���ʱ���D1�����ýӿ�ֻ�Ե�Ե�ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  chan           ͨ����
  
 * \return  0  ���óɹ�      -1  ����ʧ��
 */
int am_zm516x_set_channel(am_zm516x_handle_t zm516x_handle, uint8_t chan);

/**
 * \brief ����ZM516Xģ��Ŀ���ַ����ʱ���D2��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  zb_addr        Ŀ���ַ
  
 * \return  0  ���óɹ�      -1  ����ʧ��
 */
int am_zm516x_set_dest_addr(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr);

/**
 * \brief ����ZM516Xģ����յ����ݰ���ͷ�Ƿ���ʾԴ��ַ����ʱ���D3��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  flag           ��ͷ�Ƿ���ʾԴ��ַ TRUE: ��ʾ��FALSE: ����ʾ
 
 * \return  0  ���óɹ�   -1  ����ʧ��
 */
int am_zm516x_set_display_head(am_zm516x_handle_t zm516x_handle, bool_t flag);

/**
 * \brief ����ָ����ַZM516Xģ���GPIO�������������ʱ���D4��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  zb_addr        Ҫ���õ�ZM516Xģ���Ŀ���ַ
 * \param[in]  dir            �ܽŷ��򣬸��ֽڸ���λ1Ϊ�����0Ϊ���룻bit0��ӦGPIO0��bit7��ӦGPIO7
  
 * \return  0  ���óɹ�      -1  ����ʧ��
 */
int am_zm516x_gpio_dir(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t dir);

/**
 * \brief ��ȡָ����ַZM516Xģ���GPIO����ֵ����ʱ���D5��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  zb_addr        Ҫ��ȡ��ZM516Xģ���Ŀ���ַ
 * \param[out] p_value        ��ȡ���Ĺܽ�����ֵ�����ֽڸ���λ1Ϊ�ߵ�ƽ��0Ϊ�͵�ƽ��bit0��ӦGPIO0��bit7��ӦGPIO7
  
 * \return  0  ��ȡ�ɹ�      -1  ��ȡʧ��
 */
int am_zm516x_gpio_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t *p_value);

/**
 * \brief ����ָ����ַZM516Xģ���GPIO���ֵ����ʱ���D6��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  zb_addr        Ҫ���õ�ZM516Xģ���Ŀ���ַ
 * \param[in]  value          �ܽ����ֵ�����ֽڸ���λ1Ϊ�ߵ�ƽ��0Ϊ�͵�ƽ��bit0��ӦGPIO0��bit7��ӦGPIO7
  
 * \return  0  ���óɹ�      -1  ����ʧ��
 */
int am_zm516x_gpio_set(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t value);

/**
 * \brief ��ȡָ����ַZM516Xģ���ADת��ֵ����ʱ���D7��
 * ��ȡ����ADֵΪAD��ת��ֵ����Ҫ�ο�ZM516Xģ���û��ֲ����ZM516Xģ��AD��λ�����ο���ѹת��Ϊ��ѹֵ
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  zb_addr        Ҫ��ȡ��ZM516Xģ���Ŀ���ַ
 * \param[in]  chan           AD��ͨ����
 * \param[out] p_value        AD��ת��ֵ
  
 * \return  0  ��ȡ�ɹ�      -1  ��ȡʧ��
 */
int am_zm516x_ad_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t chan, uint16_t *p_value);

/**
 * \brief ����ZM516Xģ�����˯��ģʽ����ʱ���D8��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
  
 * \return ��
 */
void am_zm516x_enter_sleep(am_zm516x_handle_t zm516x_handle);

/**
 * \brief ����ZM516Xģ���ͨѶģʽ����ʱ���D9��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  mode           ģ���ͨѶģʽ
  
 * \return  0  ���óɹ�      -1  ����ʧ��
 */
int am_zm516x_mode_set(am_zm516x_handle_t zm516x_handle, am_zm516x_comm_mode_t mode);

/**
 * \brief ��ȡָ����ַZM516Xģ����ź�ǿ�ȣ���ʱ���DA��
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[in]  zb_addr        Ҫ��ȡ�ı���ZM516Xģ����Ŀ���ַ֮����ź�ǿ��
 * \param[out] p_signal       �ź�ǿ��
  
 * \return  0  ��ȡ�ɹ�       -1  ��ȡʧ��
 */
int am_zm516x_sigal_get(am_zm516x_handle_t zm516x_handle, am_zm516x_addr_t zb_addr, uint8_t *p_signal);

/**
 * \brief ��ȡZM516Xģ����ھӱ���ʱ���DB�����ýӿ�ֻ������ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[out] p_mac          �ھӱ��MAC��ַ����
 * \param[in]  maxlen         ����ȡp_mac������
  
 * \return  ��ȡ����Ŀ
 */
int am_zm516x_neighbour_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen);

/**
 * \brief ��ȡZM516Xģ���·�ɱ���ʱ���DC�����ýӿ�ֻ������ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[out] p_mac          ·�ɱ��MAC��ַ����
 * \param[in]  maxlen         ����ȡp_mac������
  
 * \return  ��ȡ����Ŀ
 */
int am_zm516x_route_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen);

/**
 * \brief ��ȡZM516Xģ�������������е���ȣ���ʱ���DD�����ýӿ�ֻ������ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[out] p_deep         ��������
  
 * \return  0  ��ȡ�ɹ�      -1  ��ȡʧ��
 */
int am_zm516x_deep_get(am_zm516x_handle_t zm516x_handle, uint16_t *p_deep);

/**
 * \brief ��ȡZM516Xģ��ĸ��ڵ㣨��ʱ���DE�����ýӿ�ֻ������ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[out] p_mac          ���ڵ��MAC��ַ
  
 * \return  0  ��ȡ�ɹ�      -1  ��ȡʧ��
 */
int am_zm516x_parent_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac);

/**
 * \brief ��ȡZM516Xģ����������ʱ���DF�����ýӿ�ֻ������ģ����Ч
 *
 * \param[in]  zm516x_handle  ZM516Xģ��������
 * \param[out] p_mac          ������MAC��ַ����
 * \param[in]  maxlen         ����ȡp_mac������
  
 * \return  ��ȡ����Ŀ
 */
int am_zm516x_netword_tabel_get(am_zm516x_handle_t zm516x_handle, am_zm516x_mac_addr_t *p_mac, size_t maxlen);

/**
 * \brief ��ʼ��ZM516Xģ������
 *
 * \param[in]  p_dev          ZM516Xģ���豸�ṹ��
 * \param[in]  ringbuf_handle ����ZM516Xģ��Ĵ��ھ��
  
 * \return  zm516x_handle
 */
am_zm516x_handle_t am_zm516x_init (am_zm516x_dev_t *p_dev, am_uart_rngbuf_handle_t ringbuf_handle);

/** @} zm516x_driver */

#ifdef __cplusplus
}
#endif

#endif /* __AM_ZM516X_H */

/* end of file */
 
