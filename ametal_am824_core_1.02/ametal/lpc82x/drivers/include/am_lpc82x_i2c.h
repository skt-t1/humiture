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
 * \brief I2C����������I2C��׼�ӿ�
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
 * \brief I2C �豸��Ϣ�����ṹ��
 */
typedef struct am_lpc82x_i2c_devinfo {
    
    /** \brief I2C��ʼ�������ٶ�,am_i2c_speed_set() ��׼�ӿڿ��Ըı����� */
    uint32_t  bus_speed;
 
    /** \brief I2C�Ĵ������ַ  */
    uint32_t  i2c_regbase;
    
    /** \brief I2C ���������жϺ�     */
    uint16_t  inum;
    
    /** 
     * \brief ��ʱʱ��ֵ
     *
     * I2C���������ݴ����ڼ䣬���˶�ʱ����ʱ�������κ���Ӧ,����Ϊ��ʱ��
     * ������ǰ���䲢��λ�ڲ��߼� 
     */
    uint32_t  timeout_ms;
    
    /** \brief ƽ̨��ʼ������ */
    void     (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void     (*pfn_plfm_deinit)(void);
    
} am_lpc82x_i2c_devinfo_t;

/**
 * \brief I2C �豸�ṹ��
 */
typedef struct am_lpc82x_i2c_dev {
   
    /** \brief ��׼I2C����           */
    am_i2c_serv_t                  i2c_serv;
    
    /** \brief I2C ��������Ϣ���� */
    struct am_list_head            msg_list;

    /** \brief ָ��I2C����ṹ���ָ��,ͬһʱ��ֻ�ܴ���һ������ */
    am_i2c_transfer_t             *p_cur_trans;
    
    /** \brief ��ǰ���ڴ������Ϣ   */
    am_i2c_message_t              *p_cur_msg;
    
    /** \brief �������ݽ���/���ͼ���   */
    uint32_t                       data_ptr;
    
    /** \brief I2Cʱ�ӣ�ʹ��DIV��Ƶ���ʱ��Ƶ�� */
    uint32_t                       i2c_clkrate;
    
    /** \brief æ��ʶ                 */
    bool_t                         busy;
    
    /** \brief ״̬                   */
    uint8_t                        state;
    
    /** \brief ָ��I2C�豸��Ϣ��ָ��   */
    const am_lpc82x_i2c_devinfo_t *p_devinfo;
    
} am_lpc82x_i2c_dev_t;


/**
 * \brief I2C ��ʼ��
 *
 * \param[in] p_dev     : ָ��I2C�豸�ṹ���ָ��
 * \param[in] p_devinfo : ָ��I2C�豸��Ϣ�ṹ���ָ��
 *
 * \return  I2C��׼���������� 
 */
am_i2c_handle_t am_lpc82x_i2c_init (am_lpc82x_i2c_dev_t           *p_dev,
                                    const am_lpc82x_i2c_devinfo_t *p_devinfo);

/**
 * \brief ���I2C��ʼ��
 * \param[in] handle : ����豸������I2C��׼����������
 * \return ��
 */
void am_lpc82x_i2c_deinit (am_i2c_handle_t handle);
    
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_I2C_H */

/* end of file */
