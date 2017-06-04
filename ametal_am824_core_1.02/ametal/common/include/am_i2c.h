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
 * \brief I2C��׼�ӿ�
 *
 * \par ��ʾ��
 * \code
 * #include "am_i2c.h"
 *
 * am_i2c_device_t dev;                     // �豸����
 * uint8_t         read_buf[16];            // ��������
 * uint8_t         write_buf[16];           // д������
 *
 * // ���ɴӻ��豸
 * am_i2c_mkdev(&dev,
 *              i2c_handle,
 *              0x50,
 *              AM_I2C_ADDR_7BIT | AM_I2C_SUBADDR_1BYTE);
 *
 * // д�����ݵ��������ӵ�ַ0��ʼ��16���ֽ�
 * am_i2c_write(&dev, 0, &write_buf[0], 16);
 *
 * // �������ӵ�ַ0��ʼ��16���ֽ�
 * am_i2c_read(&dev, 0, &read_buf[0], 16);
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.03 15-12-08  cyl, delete the am_i2c_connect.
 * - 1.02 15-10-27  tee, add the concept of message.
 * - 1.01 15-08-19  tee, modified some interface.
 * - 1.00 14-11-01  jon, first implementation.
 * \endinternal
 */

#ifndef __AM_I2C_H
#define __AM_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_common.h"

/**
 * @addtogroup am_if_i2c
 * @copydoc am_i2c.h
 * @{
 */

/**
 * \name I2C ������Ʊ�ʶ������am_i2c_transfer��flags��Ա
 * @{ 
 * ����I2C����ĵ�ַ���͡���д���͡�������Ƶȱ�־
 *
 * ��Щ��־�ࡰ��֮��Ľ�����ڸ��ṹ���Ա
 * <code>am_i2c_transfer::flags</code>��ֵ��δ��ʽ�����ı�־��ʹ��Ĭ��ֵ��
 * ���磬�������Щ��ֵ���õ���ͬ�Ŀ���Ч����
 * \code
 * am_i2c_transfer_t trans;
 * trans.flags = 0;             // 7-bit�ӻ���ַ(Ĭ��)��д����(Ĭ��)
 * trans.flags = AM_I2C_M_WR;   // 7-bit�ӻ���ַ(Ĭ��)��д����
 * trans.flags = AM_I2C_M_RD;   // 7-bit�ӻ���ַ(Ĭ��)��������
 * trans.flags = AM_I2C_M_10BIT | AM_I2C_M_RD;   // 10-bit�ӻ���ַ��������
 *
 * trans.flags = AM_I2C_M_7BIT |
 *               AM_I2C_M_WR |
 *               AM_I2C_M_IGNORE_NAK; // 7-bit�ӻ���ַ��д������������Ӧ�����
 * \endcode
 *
 * \sa struct am_i2c_transfer
 */

#define AM_I2C_M_7BIT          0x0000u    /**< \brief 7-bits �豸��ַ  */
#define AM_I2C_M_10BIT         0x0001u    /**< \brief 10-bits �豸��ַ */
#define AM_I2C_M_WR            0x0000u    /**< \brief д����           */
#define AM_I2C_M_RD            0x0002u    /**< \brief ������           */
#define AM_I2C_M_NOSTART       0x0004u    /**< \brief ������������     */
#define AM_I2C_M_REV_DIR_ADDR  0x0008u    /**< \brief ��д��־λ��ת   */
#define AM_I2C_M_RECV_LEN      0x0010u    /**< \brief �ݲ�֧��         */

/** \brief ����������Ӧ����� (�����ӻ���ַ��Ӧ���������Ӧ��) */
#define AM_I2C_M_IGNORE_NAK    0x0020u

/** \brief �ڶ������н�������ʱ������Ӧ��    */
#define AM_I2C_M_NO_RD_ACK     0x0040u

/** \brief I2C������Ʊ�־���� */
#define AM_I2C_M_MASK          0x00FFu

/** @} */


/**
 * \name I2C ���豸���Ա�־������ am_i2c_device �� dev_flags ��Ա
 * @{ 
 * ����I2C�ӻ���ַ���͡��������ӵ�ַ���ͺ��������ӵ�ַ��ȵȱ�־
 *
 * ���磬�������Щ��ֵ�����岻ͬ�Ĵ��豸���ԣ�
 * \code
 * am_i2c_device_t dev;
 * dev.flags = 0;                     // 7-bit �ӻ���ַ�����ӵ�ַ
 * dev.flags = AM_I2C_SUBADDR_1BYTE;  // 7-bit �ӻ���ַ��1�ֽ��ӵ�ַ
 * dev.flags = AM_I2C_SUBADDR_2BYTE;  // 7-bit �ӻ���ַ��2�ֽ��ӵ�ַ��
 *                                    // �ӵ�ַ��λ��ַ�ȴ���
 *
 * dev.flags = AM_I2C_SUBADDR_2BYTE |
 *             AM_I2C_SUBADDR_LSB_FIRST; // 7-bit �ӻ���ַ��2�ֽ��ӵ�ַ��
 *                                       // �ӵ�ַ��λ�ֽ��ȴ���
 *
 * \endcode
 *
 * \sa struct am_i2c_device
 */

/** \brief 7λ��ַģʽ(Ĭ��ģʽ) */
#define AM_I2C_ADDR_7BIT         AM_I2C_M_7BIT

/** \brief 10λ��ַģʽ */
#define AM_I2C_ADDR_10BIT        AM_I2C_M_10BIT

/** \brief �����豸����Ӧ�� */
#define AM_I2C_IGNORE_NAK        AM_I2C_M_IGNORE_NAK

/** \brief �������ӵ�ַ��λ�ֽ��ȴ���(Ĭ��)*/
#define AM_I2C_SUBADDR_MSB_FIRST 0x0000u

/** \brief �������ӵ�ַ��λ�ֽ��ȴ��� */
#define AM_I2C_SUBADDR_LSB_FIRST 0x0100u

#define AM_I2C_SUBADDR_NONE      0x0000u  /**< \brief ���ӵ�ַ(Ĭ��)  */
#define AM_I2C_SUBADDR_1BYTE     0x1000u  /**< \brief �ӵ�ַ���1�ֽ� */
#define AM_I2C_SUBADDR_2BYTE     0x2000u  /**< \brief �ӵ�ַ���2�ֽ� */

/** \brief �ӱ�־��������ȡ���ӵ�ַ��� */
#define AM_I2C_SUBADDR_LEN_GET(flags)  (((flags) & 0xF000) >> 12)

/** @} */

struct am_i2c_message;

/**
 * \brief I2C���������ṹ��
 */
struct am_i2c_drv_funcs {
    
    /** \brief ��ʼ��Ϣ���� */
    int (*pfn_i2c_msg_start) (void *p_drv, struct am_i2c_message *p_trans);
    
};

/**
 * \brief I2C��׼����ṹ�� 
 */
typedef struct am_i2c_serv {
    struct am_i2c_drv_funcs   *p_funcs;   /**< \brief I2C��������         */
    void                      *p_drv;     /**< \brief I2C����������ڲ��� */
} am_i2c_serv_t;

/** \brief I2C��׼�������������� */
typedef am_i2c_serv_t *am_i2c_handle_t;

/**
 * \brief I2C���豸�����ṹ�� (�Ƽ�ʹ�� am_i2c_mkdev() ���ñ����ݽṹ) 
 */
typedef struct am_i2c_device {
    am_i2c_handle_t handle;    /**< \brief ����豸������I2C�����׼handle    */
    uint16_t        dev_addr;  /**< \brief �ӻ��豸��ַ��7λ��10λ            */ 
    uint16_t        dev_flags; /**< \brief �ӻ��豸���ԣ�����I2C���豸���Ա�־��*/
} am_i2c_device_t;

/**
 * \brief I2C ����ṹ�� (�Ƽ�ʹ�� am_i2c_mktrans() ���ñ����ݽṹ) 
 */ 
typedef struct am_i2c_transfer {
    uint16_t    addr;         /**< \brief I2C�豸��ַ                       */
    uint16_t    flags;        /**< \brief I2C �����־ ����I2C������Ʊ�־��  */
    uint8_t    *p_buf;        /**< \brief ���ݻ�����                        */
    uint32_t    nbytes;       /**< \brief ���ݸ���                          */
} am_i2c_transfer_t;

/** 
 * \brief I2C ��Ϣ (�Ƽ�ʹ�� am_i2c_mkmsg() ���ñ����ݽṹ) 
 */
typedef struct am_i2c_message {
    am_i2c_transfer_t  *p_transfers;  /**< \brief �����Ϣ�Ĵ���              */
    uint16_t            trans_num;    /**< \brief ������Ĵ������          */
    uint16_t            done_num;     /**< \brief �ɹ�����Ĵ������          */
    am_pfnvoid_t        pfn_complete; /**< \brief ������ɻص�����            */
    void               *p_arg;        /**< \brief ���ݸ� pfn_complete �Ĳ���  */
    int                 status;       /**< \brief ��Ϣ��״̬                  */
    void               *ctlrdata[2];  /**< \brief ������ʹ��                  */
} am_i2c_message_t;


/**
 * \brief I2C�ӻ��豸�����ṹ���������
 *
 * \param[in] p_dev     : ָ��ӻ��豸�����ṹ���ָ��
 * \param[in] handle    : ����豸������I2C��׼����������
 * \param[in] dev_addr  : �ӻ��豸��ַ
 * \param[in] dev_flags : �ӻ��豸���ԣ�����I2C���豸���Ա�־��
 *
 * \return ��
 *
 * \par ����
 * \code
 * #include "am_i2c.h"
 *
 * am_i2c_device_t cat1025; // CAT1025���豸�����ṹ
 *
 * // ��ʼ��CAT1025�豸�����ṹ��λ��I2C0���������豸��ַ0x50��
 * // �Ĵ�����ַΪ1�ֽڿ�
 * am_i2c_mkdev(&cat1025,
 *              i2c_handle,
 *              0x50,
 *              AM_I2C_ADDR_7BIT | AM_I2C_SUBADDR_1BYTE);
 * \endcode
 */
am_static_inline
void am_i2c_mkdev (am_i2c_device_t *p_dev,
                   am_i2c_handle_t  handle,
                   uint16_t         dev_addr,
                   uint16_t         dev_flags)

{
    p_dev->handle     = handle;
    p_dev->dev_addr   = dev_addr;
    p_dev->dev_flags  = dev_flags;
}

/**
 * \brief I2C����ṹ����Ϣ��������
 *
 * \param[in] p_trans : ָ����ṹ���ָ��
 * \param[in] addr    : �����Ĵӻ��豸��ַ
 * \param[in] flags   : ��������еĿ��Ʊ�ʶλ��AM_I2C_M_*�ĺ�ֵ���߶��
 *                      AM_I2C_M_*��Ļ�OR��ֵ (#AM_I2C_M_RD) 
 * \param[in] p_buf   : ���ͻ��߽������ݻ�����
 * \param[in] nbytes  : ��������ݸ���
 *
 * \retval  AM_OK     : ����ṹ������������
 * \retval -AM_EINVAL : ��������
 */
am_static_inline
void am_i2c_mktrans (am_i2c_transfer_t *p_trans, 
                     uint16_t           addr,  
                     uint16_t           flags, 
                     uint8_t           *p_buf, 
                     uint32_t           nbytes)
{
    p_trans->addr   = addr;
    p_trans->flags  = flags;
    p_trans->p_buf  = p_buf; 
    p_trans->nbytes = nbytes;
}

/**
 * \brief ����I2C��Ϣ�ṹ�����
 *
 * \param[in] p_msg          : I2C��Ϣ������ָ��
 * \param[in] p_transfers    : �����Ϣ�ġ�I2C���䡱
 * \param[in] trans_num      : ��I2C���䡱�ĸ���
 * \param[in] pfn_complete   : ��Ϣ������ɻص�����
 * \param[in] p_arg          : ���ݸ��ص������Ĳ���
 *
 * \par ����
 * \code
 * #include "am_i2c.h"
 *
 * am_i2c_message_t  msg;       // ��Ϣ�����ṹ��
 * am_i2c_transfer_t trans[2];  // ���������ṹ
 * uint8_t data0[16]            // ���ݻ����� 0
 * uint8_t data1[16];           // ���ݻ����� 1
 *
 * // ���ô��������ṹ��
 * am_i2c_mktrans(&trans[0],    // ���������ṹ 0
 *                0x50,         // Ҫ�����Ŀ������(�ӻ�)��ַ
 *                AM_I2C_M_7BIT | AM_I2C_M_WR��// 7-bit�ӻ���ַ��д����
 *                1,            // ���ݳ��ȣ�1�ֽ�
 *                &data0[0]);   // ���ݻ����� 0
 *
 * am_i2c_mktrans(&trans[1],    // ���������ṹ 1
 *                0x50,         // Ҫ�����Ŀ������(�ӻ�)��ַ
 *                AM_I2C_M_7BIT | AM_I2C_M_RD��// 7-bit�ӻ���ַ��������
 *                16,           // ���ݳ��ȣ�16�ֽ�
 *                &data1[0]);   // ���ݻ�����1
 *
 * // ������Ϣ�����ṹ��
 * am_i2c_mkmsg(&msg,           // ��Ϣ�����ṹ
 *              &trans[0],      // �����Ϣ�Ĵ���
 *              2,              // �������
 *              my_callback,    // ������ɻص�����
 *              my_arg);        // ������ɻص������Ĳ���
 *
 * \endcode
 */
am_static_inline
void am_i2c_mkmsg (am_i2c_message_t  *p_msg,
                   am_i2c_transfer_t *p_transfers,
                   uint16_t           trans_num,
                   am_pfnvoid_t       pfn_complete,
                   void              *p_arg)
{
    p_msg->p_transfers  = p_transfers;
    p_msg->trans_num    = trans_num;
    p_msg->done_num     = 0;
    p_msg->pfn_complete = pfn_complete;
    p_msg->p_arg        = p_arg;
    p_msg->status       = -AM_ENOTCONN;
}

/**
 * \brief ��ʼ����һ����Ϣ
 *
 *     ���첽�ķ�ʽ������Ϣ������ǰI2C���������У������Ϣ��õ�����ִ�У�
 * �������������У���Ὣ����Ϣ����һ���������Ŷӵȴ���I2C��������˳����
 * �����е���Ϣ����Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status��
 * ��Ϣ�������(�ɹ�����ʱ�����)ʱ���������\a p_msg->pfn_complete ������
 * ����\a p_msg->p_arg��
 *
 * \param[in]     handle : I2C��׼����������
 * \param[in,out] p_msg  : Ҫ�������Ϣ
 *
 * �����ķ���ֵ���£�
 * \retval  AM_OK      : ��Ϣ�Ŷӳɹ����ȴ�����
 * \retval  -AM_EINVAL : ��������
 *
 * �ɹ�����Ĵ��������ӳ��\a p_msg->done_num
 *
 * ��Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status ��
 *
 *      \li  -AM_ENOTCONN    ��Ϣ��δ�Ŷ�
 *      \li  -AM_EISCONN     ��Ϣ�����ŶӴ���
 *      \li  -AM_EINPROGRESS ��Ϣ���ڱ�����
 * ����Ϊ��Ϣ������ϵĽ�� (��ͨ��\a p_msg->done_num ��λ����Ĵ���)
 *      \li  AM_OK           ���д���ɹ�����
 *      \li  -AM_ENOTSUP     ĳ�δ���Ŀ��Ʊ�־��֧��
 *      \li  -AM_ENODEV      ĳ�δ����Ŀ���ַ��Ӧ��
 *      \li  -AM_ENOENT      ĳ�δ��䷢�͵�������Ӧ��
 *      \li  -AM_EIO         ������Ϣʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *      \li  -AM_ECANCELED   �����������������������Ϣ��ȡ�������Ժ�����
 *
 * \par ʾ��
 * \code
 *  #include "am_i2c.h"
 *
 * bool_t g_trans_complete = FALSE;
 *
 * void eeprom_callback (void *p_arg) 
 * {
 *      g_trans_complete = TRUE;
 *
 * }
 *
 * int main ()
 * {
 *     am_i2c_message_t  msg;
 *     am_i2c_transfer_t trans[2];
 *     uint8_t           subaddr;
 *     uint8_t           data[EEPROM_PAGE_SIZE];
 *     int               i;
 *     am_i2c_handle_t   i2c_handle = NULL;
 *
 *     // дEEPROM�ӵ�ַ0x00��ʼ��һ��ҳ (ʱ����ο������ֲ�)
 *
 *     // ��1�����䷢���ӵ�ַ
 *     subaddr = 0x00;
 *     am_i2c_mktrans(&trans[0],
 *                    EEPROM_ADDR,                 // Ŀ��������ַ
 *                    AM_I2C_M_7BIT | AM_I2C_M_WR, // 7-bit������ַ, д����
 *                    &subaddr,                    // �ӵ�ַ
 *                    1);                          // �ӵ�ַ���ȣ�1byte
 *     
 *     // ��2�����䷢������
 *     for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
 *         data[i] = i;
 *     }
 *     am_i2c_mktrans(&trans[1],
 *                    EEPROM_ADDR,                 // Ŀ��������ַ (��������)
 *     
 *                    // д����, ��������ʼ�źż��ӻ���ַ
 *                    AM_I2C_M_WR | AM_I2C_M_NOSTART,
 *     
 *                    &data[0],                    // ���ݻ�����
 *                    EEPROM_PAGE_SIZE);           // ���ݸ���
 *     
 *     // �����������Ϣ
 *     am_i2c_mkmsg(&msg,
 *                  &trans[0],
 *                  2,
 *                  eeprom_callback,
 *                  &sem_sync);
 *     
 *     // ��ʼ�������Ϣ��ʹ��i2c_handle֮ǰ��Ӧ����ʵ��ƽ̨��ȡ���Ȼ�ȡһ��i2c_handle
 *     if (am_i2c_msg_start(i2c_handle, &msg) == AM_OK) {
 *          
 *          while (g_trans_complete == FALSE);
 *         
 *           if (msg.status != AM_OK) {
 *               // ������ִ���
 *           }
 *     }
 *     
 *     while(1) {
 *
 *     }
 * }
 * 
 * \endcode
 */
am_static_inline
int am_i2c_msg_start (am_i2c_handle_t handle, am_i2c_message_t *p_msg)
{
    return handle->p_funcs->pfn_i2c_msg_start(handle->p_drv, p_msg);
}

/**
 * \brief I2Cд����
 *
 * \param[in] p_dev    : ָ��ӻ��豸��Ϣ�Ľṹ���ָ��
 * \param[in] sub_addr : �ӻ��豸�ӵ�ַ
 * \param[in] p_buf    : ָ����/�������ݻ���
 * \param[in] nbytes   : ���ݻ��泤��
 *
 * \return  AM_OK : д�������
 */
int am_i2c_write(am_i2c_device_t *p_dev,
                 uint32_t         sub_addr,
                 const uint8_t   *p_buf, 
                 uint32_t         nbytes);
/**
 * \brief I2C������
 *
 * \param[in] p_dev    : ָ��ӻ��豸��Ϣ�Ľṹ���ָ��
 * \param[in] sub_addr : �ӻ��豸�ӵ�ַ
 * \param[in] p_buf    : ָ����/�������ݻ���
 * \param[in] nbytes   : ���ݻ��泤��
 *
 * \return  AM_OK : ��������� 
 */
int am_i2c_read(am_i2c_device_t *p_dev,
                uint32_t         sub_addr,
                uint8_t         *p_buf, 
                uint32_t         nbytes);
/** 
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_I2C_H */

/* end of file */
