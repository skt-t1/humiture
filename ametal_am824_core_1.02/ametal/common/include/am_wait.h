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
 * \brief �ȴ�(wait)��׼�ӿ�
 *
 * �ýӿڵ�ʹ�÷�ʽ�ǣ����ȶ���һ���ȴ��źţ�am_wait_t wait;
 * ����Ҫ�ȴ��ĵط�����:am_wait_on(&wait);
 * ����Ҫ����֮ǰ�����ȴ��ĵط����ã�am_wait_done(&wait);
 *
 *
 * \internal
 * \par Modification History
 * - 1.01 15-09-07  tee, add am_wait_on_timeout() interface.
 * - 1.00 15-06-12  tee, first implementation.
 * \endinternal
 */

#ifndef __AM_WAIT_H
#define __AM_WAIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_common.h"
#include "am_types.h"
#include "am_softimer.h"
    
/**
 * \addtogroup am_if_wait
 * \copydoc am_wait.h
 * @{
 */

 /**
  * \brief ����ȴ����ͽṹ�壬Ӧ�ó���Ӧֱ�Ӳ����ýṹ���Ա
  */
typedef struct am_wait {
    
    /** \brief �ȴ�ֵ                  */
    volatile uint8_t  val;
    
    /** \brief ���ڳ�ʱ����������ʱ�� */
    am_softimer_t     timer;
    
    /** \brief ��־��ǰ�ĵȴ�״̬       */
    uint8_t           stat;

} am_wait_t;
    
 
/** 
 * \brief �ȴ��źų�ʼ��
 *
 * ʹ�õȴ�֮ǰ��������ʹ�øú�����ʼ������ʼ��һ�μ���
 *
 * \param[in] p_wait  : ָ��ȴ��źŵ�ָ��
 *
* \retval  AM_OK      : ��ʼ���ɹ�
 * \retval -AM_EINVAL ����ʼ��ʧ�ܣ���������
 */
int am_wait_init(am_wait_t *p_wait);

/** 
 * \brief �����ȴ��¼�
 *
 * �����һֱ�ȴ���ֱ������ am_wait_done() ָ���ȴ����
 * 
 * \param[in] p_wait  : ָ��ȴ��źŵ�ָ��
 *
 * \retval  AM_OK     ���ȴ��ɹ���ɣ��ȴ�������Ӧ���¼�
 * \retval -AM_EINVAL �������ȴ�ʧ�ܣ���������
 */
int am_wait_on(am_wait_t *p_wait);

/** 
 * \brief �����ȴ��¼��������ó�ʱʱ�䣩
 *
 * �����һֱ�ȴ���ֱ������ am_wait_done() ָ���ȴ����
 * 
 * \param[in] p_wait      : ָ��ȴ��źŵ�ָ��
 * \param[in] timeout_ms  : �ȴ���ʱ��ʱ�䣨��λ��ms������Ϊ0����һֱ�ȴ���
 *
 * \retval  AM_OK     ���ȴ��ɹ���ɣ��ȴ�������Ӧ���¼�
 * \retval -AM_ETIME  ���ȴ���ʱ��ָ��ms��ʱ����Ϊ���յ��ȴ�������Ϣ
 * \retval -AM_EINVAL �������ȴ�ʧ�ܣ���������
 */
int am_wait_on_timeout(am_wait_t *p_wait, uint32_t timeout_ms);

/** 
 * \brief �����ȴ��¼�
 *
 *    ���øú�����am_wait_on() �� am_wait_on_timeout() 
 * �����ĵȴ����ɹ��ȴ���ɣ��ȴ�������
 * 
 * \param[in] p_wait  : ָ��ȴ��źŵ�ָ��
 *
 * \retval  AM_OK     ���ɹ��������Ѿ������ĵȴ�
 * \retval -AM_EINVAL �������ȴ�ʧ�ܣ���������
 */
int am_wait_done(am_wait_t *p_wait);
 

/** 
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_WAIT_H */

/* end of file */
