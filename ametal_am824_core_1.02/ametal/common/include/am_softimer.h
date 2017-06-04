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
 * \brief  �����ʱ����׼�ӿ�
 *
 * �����ʱ��ʹ��һ��Ӳ����ʱ����Դ���ṩ��ʱ����
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-07-31  tee,  first implementation.
 * \endinternal
 */

#ifndef __AM_SOFTIMER_H
#define __AM_SOFTIMER_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "am_common.h"
#include "am_list.h"
#include "am_timer.h"


/**
 * \addtogroup am_if_softimer
 * \copydoc am_softimer.h
 * @{
 */

/**
 * \brief �����ʱ���ṹ�壬�����˱�Ҫ����Ϣ
 */
struct am_softimer {
    struct am_list_head node;          /**< \brief �����γ�����ṹ           */
    unsigned int        ticks;         /**< \brief ʣ��ʱ��tickֵ             */
    unsigned int        repeat_ticks;  /**< \brief �������ظ���ʱ��tick��     */
    void (*timeout_callback)( void *); /**< \brief ��ʱʱ�䵽�ص�����         */
    void               *p_arg;         /**< \brief �ص������Ĳ���             */
};

typedef struct am_softimer am_softimer_t;

/**
 * \brief �����ʱ��ģ���ʼ��
 *
 * \param[in] handle : һ����׼Ӳ����ʱ��handle
 * \param[in] chan   : ʹ��Ӳ����ʱ����ͨ����
 * \param[in] freq   : �����������ʱ����ʱ���ܶȣ���Ƶ��Ϊ1000Hz,��ʱ���ܶ�Ϊ1
 *                     ���룬�������������ʱ�Ķ�ʱʱ�侫�Ⱦ�Ϊ1ms��
 * \retval AM_OK     : ģ���ʼ���ɹ�����������ʹ��
 * \retval -AM_EINVAL: ������Ч������handle��freq��ֵ
 */
int am_softimer_module_init (am_timer_handle_t handle, uint8_t chan, unsigned int freq);
    
/**
 * \brief ��ʼ��һ�������ʱ��
 *
 * \param[in] p_timer  : ָ��һ�������ʱ����ָ��
 * \param[in] p_func   : ��ʱʱ�䵽�Ļص�����
 * \param[in] p_arg    : �ص���������
 *
 * \retval AM_OK      : �����ʱ����ʼ���ɹ�
 * \retval -AM_EINVAL : ��Ч����
 * \retval -AM_EPERM  : �����ʱ��ģ�黹δ��ʼ�������������У�����ȷ��ģ����ȷ��ʼ��
 */
int am_softimer_init(am_softimer_t *p_timer, am_pfnvoid_t p_func, void *p_arg);


/**
 * \brief ����һ�������ʱ��
 * \param[in] p_timer : ָ��һ�������ʱ����ָ��
 * \param[in] ms      : ��ʱʱ��(ms)
 * \return ��
 */
void am_softimer_start(am_softimer_t *p_timer, unsigned int ms);

/**
 * \brief ֹͣһ�������ʱ��
 * \param[in] p_timer : ָ��һ�������ʱ����ָ��
 * \return ��
 */
void am_softimer_stop(am_softimer_t *p_timer);

/** 
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_SOFTIMER_H */

/* end of file */
