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
 * \brief SCT����������PWM��׼�ӿ�
 *
 * 1. SCT֧���ṩ�������ֱ�׼���񣬱������ṩ���Ƿ���PWM�����׼�����������
 *     - ��ʱ��
 *     - PWM�����
 *     - ����
 * 2. �ڱ������У�SCT�����Դʹ�����£�
 *    - PWM���ͨ��0ʹ��SCT���0�����¼�1�������ʹ��ƥ��Ĵ���1���Բ�����Ӧ��ռ�ձȣ�
 *    - PWM���ͨ��1ʹ��SCT���1�����¼�2�������ʹ��ƥ��Ĵ���2���Բ�����Ӧ��ռ�ձȣ�
 *    - PWM���ͨ��2ʹ��SCT���2�����¼�3�������ʹ��ƥ��Ĵ���3���Բ�����Ӧ��ռ�ձȣ�
 *    - PWM���ͨ��3ʹ��SCT���3�����¼�4�������ʹ��ƥ��Ĵ���4���Բ�����Ӧ��ռ�ձȣ�
 *    - PWM���ͨ��4ʹ��SCT���4�����¼�5�������ʹ��ƥ��Ĵ���5���Բ�����Ӧ��ռ�ձȣ�
 *    - PWM���ͨ��5ʹ��SCT���5�����¼�6�������ʹ��ƥ��Ĵ���6���Բ�����Ӧ��ռ�ձȣ�
 *    - �����ڲ�ռ���¼�0����ʹ��ƥ��Ĵ���0���Բ�������PWM���ͨ���Ĺ�ͬ���ڡ�
 *
 * \note һ��SCT���������PWM��������ֵ��Ҳ����˵����SCT���������PWM������ͬ��
 *       Ƶ����ͬ
 * \internal
 * \par Modification history
 * - 1.01 15-09-21  win, modify annotation.
 * - 1.00 15-07-09  oce, first implementation.
 * \endinternal
 */


#ifndef __AM_LPC82X_SCT_PWM_H
#define __AM_LPC82X_SCT_PWM_H

#include "ametal.h"
#include "am_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_sct_pwm
 * \copydoc am_lpc82x_sct_pwm.h
 * @{
 */

/**
 * \brief SCT PWM���������ص�GPIO��Ϣ
 */
typedef struct am_lpc82x_sct_pwm_ioinfo {
    uint32_t gpio;              /**< \brief PWM GPIO */
    uint32_t func;              /**< \brief PWMĬ�Ϲ��� */
    uint32_t dfunc;             /**< \brief PWM��ֹ״̬�Ĺ���*/
} am_lpc82x_sct_pwm_ioinfo_t;


/**
 * \brief SCT PWM���������ص��豸��Ϣ
 */
typedef struct am_lpc82x_sct_pwm_devinfo {
    uint32_t                    sct_regbase;  /**< \brief SCT�Ĵ�����Ļ���ַ */
    uint8_t                     channels_num; /**< \brief ʹ�õ�ͨ���������Ϊ6  */
    am_lpc82x_sct_pwm_ioinfo_t *p_ioinfo;     /**< \brief ָ������ܽ���Ϣ�ṹ��*/

    /** \brief ƽ̨��ʼ�����������ʱ�ӣ��������ŵȹ��� */
    void     (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void     (*pfn_plfm_deinit)(void);
} am_lpc82x_sct_pwm_devinfo_t;
 
/**
 * \brief SCT PWM��������豸�ṹ��
 */
typedef struct am_lpc82x_sct_pwm_dev {

    /** \brief ��׼PWM����           */
    am_pwm_serv_t                      pwm_serv;
    
    /** \brief ָ��SCT(PWM�������)�豸��Ϣ������ָ�� */
    const am_lpc82x_sct_pwm_devinfo_t *p_devinfo;

} am_lpc82x_sct_pwm_dev_t;

/**
 * \brief ��ʼ��SCTΪPWM�������
 *
 * \param[in] p_dev     : ָ��SCT(PWM�������)�豸��ָ��
 * \param[in] p_devinfo : ָ��SCT(PWM�������)�豸��Ϣ������ָ��
 *
 * \return PWM��׼������������ֵΪNULLʱ������ʼ��ʧ��
 */
am_pwm_handle_t am_lpc82x_sct_pwm_init(am_lpc82x_sct_pwm_dev_t     *p_dev,
                                 const am_lpc82x_sct_pwm_devinfo_t *p_devinfo);

/**
 * \brief ��ʹ��SCT PWM�������ʱ�����ʼ��SCT PWM������ܣ��ͷ������Դ
 * \param[in] handle :  am_lpc82x_sct_pwm_init() ��ʼ��������õ�PWM������
 * \return ��
 */
void am_lpc82x_sct_pwm_deinit(am_pwm_handle_t handle);

/** 
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_SCT_PWM_H */

/* end of file */
