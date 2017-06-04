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
 * \brief SCT����������Timer��׼�ӿ�
 *
 * 1. SCT֧���ṩ�������ֱ�׼���񣬱������ṩ���Ƿ���Timer��׼�����������
 *     - ��ʱ��
 *     - PWM�����
 *     - ����
 * 2. �ڱ������У�SCT�����Դʹ�����£�
 *    - ��32λ��ʱ��ʱ�����¼�0�������ʹ��ƥ��Ĵ���0���Բ�����ʱ�жϣ�����һ��ͨ����
 *    - ��16λ��ʱ��ʱ��ͨ��0���¼�0�������ʹ��ƥ��Ĵ���0���裬�Բ�����ʱ�жϣ�
 *                    ͨ��1���¼�1�������ʹ��ƥ��Ĵ���0���裬�Բ�����ʱ�жϣ�
 *
 * \internal
 * \par Modification history
 * - 1.01 15-09-24  bob, modify annotation.
 * - 1.00 15-07-09  oce, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_SCT_TIMING_H
#define __AM_LPC82X_SCT_TIMING_H

#include "ametal.h"
#include "am_timer.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_sct_timing
 * \copydoc am_lpc82x_sct_timing.h
 * @{
 */

/** \brief SCT������1��32λ��ʱ��ģʽ, �����ṩ1����ʱ��ͨ��    */
#define AM_LPC82X_SCT_TIMING_1_32BIT      1

/** \brief SCT������2��16λ��ʱ��ģʽ, ���ṩ2����ʱ��ͨ��      */
#define AM_LPC82X_SCT_TIMING_2_16BIT      2

/**
 * \brief SCT��ʱ������ص��豸��Ϣ
 */
typedef struct am_lpc82x_sct_timing_devinfo {
    uint32_t sct_regbase;  /**< \brief SCT�Ĵ�����Ļ���ַ */
    uint8_t  flag;         /**< \brief SCT���ñ�־(1-32bit ���� 2-16bit) */
    uint8_t  inum;         /**< \brief SCT�жϺ�                          */

    /** \brief ƽ̨��ʼ�����������ʱ�ӣ��������ŵȹ��� */
    void     (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void     (*pfn_plfm_deinit)(void);
} am_lpc82x_sct_timing_devinfo_t;
    
/**
 * \brief SCT��ʱ�����豸
 */
typedef struct am_lpc82x_sct_timing_dev {

    am_timer_serv_t   timer_serv;     /**< \brief ��׼��ʱ(Timer)����         */

    struct {
        void (*pfn_callback)(void *); /**< \brief �ص�����                    */
        void *p_arg;                  /**< \brief �ص��������û�����          */
    } callback_info[2];               /**< \brief ���֧��2ͨ��(2-16bit��ʱ��)*/
    
    /** \brief ָ��SCT(��ʱ����)�豸��Ϣ������ָ�� */
    const am_lpc82x_sct_timing_devinfo_t  *p_devinfo;

} am_lpc82x_sct_timing_dev_t;

/**
 * \brief ��ʼ��SCTΪ��ʱ����
 *
 * \param[in] p_dev     : ָ��SCT(��ʱ����)�豸��ָ��
 * \param[in] p_devinfo : ָ��SCT(��ʱ����)�豸��Ϣ������ָ��
 *
 * \return Timer��׼������������ֵΪNULLʱ������ʼ��ʧ��
 */
am_timer_handle_t
am_lpc82x_sct_timing_init (am_lpc82x_sct_timing_dev_t     *p_dev,
                     const am_lpc82x_sct_timing_devinfo_t *p_devinfo);

/**
 * \brief ��ʹ��SCT��ʱ����ʱ�����ʼ��SCT��ʱ���ܣ��ͷ������Դ
 *
 * \param[in] handle : am_lpc82x_sct_timing_init() ��ʼ��������õ�Timer������
 *
 * \return ��
 */
void am_lpc82x_sct_timing_deinit (am_timer_handle_t handle);
/** 
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_SCT_TIMING_H */

/* end of file */
