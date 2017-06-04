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
 * \brief  CRC������ӿ�
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-07-09  sss, first implementation.
 * \endinternal
 */
#ifndef __AM_LPC82X_CRC_H
#define __AM_LPC82X_CRC_H

#include "ametal.h"
#include "am_crc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_crc
 * \copydoc am_lpc82x_crc.h
 * @{
 */
 
 
/**
 * \brief CRC�豸��Ϣ�ṹ��
 */
typedef struct am_lpc82x_crc_devinfo {
    
    /** \brief crc�Ĵ������ַ*/
    uint32_t crc_regbase;
    
    /** \brief ƽ̨��ʼ������ */
    void     (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void     (*pfn_plfm_deinit)(void);
    
} am_lpc82x_crc_devinfo_t;

/**
 * \brief CRC�豸�ṹ��
 */
typedef struct am_lpc82x_crc_dev {
    
    /** \brief ��׼crc����                */
    am_crc_serv_t                  crc_serv;      
    /** \brief ָ��crc�豸��Ϣ�ṹ���ָ��*/    
    const am_lpc82x_crc_devinfo_t *p_devinfo;   
    /** \brief ָ��crcģ�͵�ָ��          */  
    am_crc_pattern_t              *p_pattern;        
    
}am_lpc82x_crc_dev_t;

/**
 * \brief CRC�豸��ʼ��
 * \param[in] p_dev     : ָ��crc�豸��ָ��
 * \param[in] p_devinfo : ָ��crc�豸��Ϣ�ṹ��ָ��
 * \return CRC��׼������,ΪNULLʱ��������ʧ��
 */
am_crc_handle_t am_lpc82x_crc_init (am_lpc82x_crc_dev_t           *p_dev, 
                                    const am_lpc82x_crc_devinfo_t *p_devinfo);

/**
 * \brief CRC�豸���ʼ��
 * \param[in] handle : CRC��׼����������
 * \return ��
 */
void am_lpc82x_crc_deinit (am_crc_handle_t handle);

/**
 * @} 
 */
 
#ifdef __cplusplus
}
#endif /* __AM_LPC82X_CRC_H */

#endif

/* end of file */

