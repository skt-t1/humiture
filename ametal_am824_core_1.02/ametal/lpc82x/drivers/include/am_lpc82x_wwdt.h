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
 * \brief  WWDT������ӿ�
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-07-07  sss, first implementation.
 * \endinternal
 */
 
#ifndef __AMDR_WWDT_H
#define __AMDR_WWDT_H

#include "ametal.h"
#include "am_wdt.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * \addtogroup am_lpc82x_if_wwdt
 * \copydoc am_lpc82x_wwdt.h
 * @{
 */
 
 
/**
 * \brief WWDT �豸��Ϣ�ṹ��
 */
typedef struct am_lpc82x_wwdt_devinfo {
    
    /** \brief WWDT�Ĵ��������ַ */
    uint32_t  wwdt_regbase;
      
    /** \brief ƽ̨��ʼ������ */
    void     (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void     (*pfn_plfm_deinit)(void);
    
} am_lpc82x_wwdt_devinfo_t;

/**
 * \brief WWDT �豸�ṹ��
 */
typedef struct am_lpc82x_wwdt_dev {
    
    /** \brief WWDT�ṩ�ı�׼����     */
    am_wdt_serv_t              wdt_serv;   
    
    /** \brief ָ��WWDT�豸����Ϣָ�� */
    const am_lpc82x_wwdt_devinfo_t *p_devinfo;  
} am_lpc82x_wwdt_dev_t;

/** 
 * \brief WWDT ��ʼ��
 * \param[in] p_dev     : ָ��WWDT�豸
 * \param[in] p_devinfo : ָ��WWDT�豸��Ϣ
 * \return WWDT��׼��������ʧ���򷵻�NULL
 */
am_wdt_handle_t am_lpc82x_wwdt_init (am_lpc82x_wwdt_dev_t      *p_dev,
                               const am_lpc82x_wwdt_devinfo_t  *p_devinfo);

/**
 * \brief ���WWDT �豸��ʼ��
 * \param[in] handle : ʹ��WWDT��ʼ������am_lpc82x_wwdt_init()��ȡ����handle
 * \return    ��
 */
void am_lpc82x_wwdt_deinit (am_wdt_handle_t handle);

/**
 * @} 
 */

#ifdef __cplusplus
}
#endif /* __AMHW_WWDT_H */

#endif

/*end of file */

