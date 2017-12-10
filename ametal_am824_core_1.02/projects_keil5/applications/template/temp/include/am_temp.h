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
 * \brief �¶ȴ������ӿ�
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-01  skt, first implementation.
 * \endinternal
 */
 
#ifndef __AM_TEMP_H
#define __AM_TEMP_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "am_common.h"

/** 
 * \brief �¶ȴ�������Ϣ�ṹ��
 * \attention ��Ҫע����ǣ����������¶ȵ���ֵ����Ϊʵ���¶� * multiple��
 *            ��ˣ�������ӿڻ�ȡ���й��¶�ֵ����multiple���Եõ�ʵ�ʵ��¶�ֵ
 */
typedef struct am_temp_info {
    int freq;           /**< \brief �¶ȴ�������ֵ����Ƶ�� (��λ:Hz) */    
    int multiple;       /**< \brief ��ȡ���¶�ֵ��ʵ���¶�֮��ı���ֵ */
    int temp_min;       /**< \brief �¶Ȳ�������ֵ������¶ȣ�*/
    int temp_max;       /**< \brief �¶Ȳ�������ֵ������¶ȣ�*/
} am_temp_info_t;

/** 
 * \brief ������Ϣ 
 */
struct am_temp_servinfo {
    int id;     /**< \brief �豸ID�� */
};

/**
 * \brief ������
 */
struct am_temp_servopts {
    
    /** \brief �¶ȴ�������Ϣ��ȡ */
    int (*pfn_temp_info_get) (void *p_cookie, am_temp_info_t *p_info);
    
    /** \brief ��ȡ�¶ȴ������¶�ֵ */
    int (*pfn_temp_read) (void *p_cookie, int *p_value);
    
    /** \brief ʹ���¶ȱ������� */
    int (*pfn_temp_alarm_enable) (void *p_cookie, int temp);
    
    /** \brief �����¶ȱ������� */
    int (*pfn_temp_alarm_disable) (void *p_cookie);
    
    /** \brief ���ñ����źŵĻص����� */
    int (*pfn_temp_signal) (void         *p_cookie,
                            am_pfnvoid_t  pfn_alarm, 
                            void         *p_arg);
};

struct am_temp_service {
    
    /** \brief ָ����һ������ */
    struct am_temp_service *p_next;
    
    /** \brief ���������Ϣ */
    const struct am_temp_servinfo *p_info;
    
    /** \brief ������غ��� */
    const struct am_temp_servopts *p_opts;
    
    /** \brief ���÷�����غ���ʱ������ĵ�һ������ */
    void *p_cookie;
};

/**
 * \brief ��ȡ�¶ȴ�������Ϣ
 *
 * \param[in] id     : �豸ID��
 * \param[in] p_info : �����¶ȴ�������Ϣ
 *
 * \retval AM_OK : �ɹ�
 */
int am_temp_info_get (int id, am_temp_info_t *p_info);

/**
 * \brief ��ȡ�¶�
 * 
 * \param[in]  id       : �豸ID��
 * \param[out] p_valude : �����¶�ֵ������Ա������ܵõ�ʵ��ֵ
 *
 * \retval AM_OK : �ɹ�
 */
int am_temp_read (int id, int *p_value);
                       
/**
 * \brief ʹ�ܱ�������
 *
 * \param[in] id   : �豸ID��
 * \param[in] temp : �¶�ֵ
 *
 * \retval AM_OK : �ɹ�
 */
int am_temp_alarm_enable (int id, int temp);

/**
 * \brief ���ܱ�������
 *
 * \param[in] id   : �豸ID��
 * \param[in] temp : �¶�ֵ
 *
 * \retval AM_OK : �ɹ�
 */
int am_temp_alarm_disable (int id);

/**
 * \brief ���ñ����źŵĻص�����
 *
 * \code
 * void __temp_alarm_cb (void *p_arg)
 * {
 *     // ��Ӵ������
 * }
 *
 * // ���ñ����źŴ����� 
 * am_temp_signal(0, __temp_alarm_cb, (void *)0);
 * 
 * // �����¶�70��C
 * am_temp_alarm_enable (0, 70);
 * \endcode
 *
 * \param[in] id        : �豸ID��
 * \param[in] pfn_alarm : �����ص�����
 * \param[in] p_arg     : �ص�������ڲ���
 *
 * \retval AM_OK : �ɹ�
 */
int am_temp_signal (int id, am_pfnvoid_t pfn_alarm, void *p_arg);

#ifdef __cplusplus
}
#endif

#endif /* __AM_TEMP_H */

/* end of file */
