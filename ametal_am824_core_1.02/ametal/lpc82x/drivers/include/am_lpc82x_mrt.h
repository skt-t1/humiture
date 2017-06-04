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
 * \brief MRT����������Timer��׼�ӿ�
 *
 * ������ʵ����MRT�Ķ�ʱ����
 *
 * \internal
 * \par Modification History
 * - 1.01 15-12-07  mem,modified.
 * - 1.00 15-07-05  win, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_MRT_H
#define __AM_LPC82X_MRT_H

#include "ametal.h"
#include "am_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_mrt
 * \copydoc am_lpc82x_mrt.h
 * @{
 */

/**
 * \brief MRT�豸��Ϣ�ṹ��
 */
typedef struct am_lpc82x_mrt_devinfo {
    uint32_t   mrt_regbase;            /**< \brief ָ��MRT�Ĵ������ָ��    */
    uint8_t    inum;                   /**< \brief MRT�жϺ�                */
    uint8_t    channel_nums;           /**< \brief ֧�ֵ�ͨ�����������Ϊ4  */
    void     (*pfn_plfm_init)(void);   /**< \brief ƽ̨��ʼ������           */
    void     (*pfn_plfm_deinit)(void); /**< \brief ƽ̨���ʼ������         */
} am_lpc82x_mrt_devinfo_t;

/**
 * \brief MRT�豸
 */
typedef struct am_lpc82x_mrt_dev { 
    am_timer_serv_t   timer_serv;               /**< \brief ��׼��ʱ(Timer)����      */
    const am_lpc82x_mrt_devinfo_t  *p_devinfo;  /**< \brief ָ��MRT�豸��Ϣ������ָ�� */                                                                         
    struct {                                                                    
        void (*pfn_callback)(void *);           /**< \brief �ص�����           */
        void *p_arg;                            /**< \brief �ص��������û����� */
    } callback_info[4];                         /**< \brief �ص�������Ϣ����   */

} am_lpc82x_mrt_dev_t;

/**
 * \brief ��ʼ��MRTΪ��ʱ����
 *
 * \param[in] p_dev     : ָ��MRT�豸��ָ��
 * \param[in] p_devinfo : ָ��MRT�豸��Ϣ������ָ��
 *
 * \return Timer��׼������������ֵΪNULLʱ������ʼ��ʧ��
 */
am_timer_handle_t am_lpc82x_mrt_init (am_lpc82x_mrt_dev_t           *p_dev,
                                      const am_lpc82x_mrt_devinfo_t *p_devinfo);

/**
 * \brief ��ʹ��MRTʱ�����ʼ��MRT���ͷ������Դ
 *
 * \param[in] handle  : ��ʱ����׼����������
 *
 * \return ��
 */
void am_lpc82x_mrt_deinit (am_timer_handle_t handle);
    
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_MRT_H */

/* end of file */
