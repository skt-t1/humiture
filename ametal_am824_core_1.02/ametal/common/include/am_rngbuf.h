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
 * \brief ͨ�û��λ�����
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-12-09  tee, add implementation from AWorks
 * \endinternal
 */
#ifndef __AM_RNGBUF_H
#define __AM_RNGBUF_H

#include "am_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_if_rngbuf
 * \copydoc am_rngbuf.h
 * @{
 */
 
/**
 * \brief ���λ���������ṹ
 * \note ��Ҫֱ�Ӳ������ṹ�ĳ�Ա
 */
struct am_rngbuf {
    int     in;     /**< \brief ����������д��λ�� */
    int     out;    /**< \brief ���������ݶ���λ�� */
    int     size;   /**< \brief ��������С */
    char   *buf;    /**< \brief ������     */
};

/** \brief ���λ��������� */
typedef struct am_rngbuf *am_rngbuf_t;

/**
 * \brief ��ʼ�����λ�����
 *
 * \param[in] p_rb    : Ҫ��ʼ���Ļ��λ�����
 * \param[in] p_buf   : ���λ�����ʹ�õĻ������ռ�
 * \param[in] size    : ��������С��ϵͳ����һ���ֽڣ�
 *                      ʵ�ʻ��λ��������ô�СΪ size - 1
 *                    
 * \retval  AM_OK     : ��ʼ�����
 * \retval -AM_EINVAL : ��ʼ��ʧ�ܣ�������Ч
 *
 * \warning �ر�ע�⣬�������ռ���ʹ���б�����һ�ֽڿռ䣬���λ�����ʵ�ʿ��ô�СΪ
 *          \b size - \b 1
 */
int am_rngbuf_init (struct am_rngbuf *p_rb, char *p_buf, size_t size);

/**
 * \brief ���һ���ֽڵ����λ�����
 *
 * \param[in] rb   : Ҫ�����Ļ��λ���
 * \param[in] data : Ҫ��ŵ��������������ֽ�
 *
 * \retval 0 : ���ݴ��ʧ�ܣ���������
 * \retval 1 : ���ݳɹ����
 */
int am_rngbuf_putchar (am_rngbuf_t rb, const char data);

/**
 * \brief �ӻ��λ�����ȡ��һ���ֽ�����
 *
 * \param[in] rb     : Ҫ�����Ļ��λ���
 * \param[in] p_data : ��������ֽڵ�ָ��
 *
 * \retval 0 : ����ȡ��ʧ�ܣ���������
 * \retval 1 : ���ݳɹ�ȡ��
 */
int am_rngbuf_getchar (am_rngbuf_t rb, char *p_data);

/**
 * \brief ��������ֽڵ����λ�����
 *
 * \param[in] rb     : Ҫ�����Ļ��λ���
 * \param[in] p_buf  : Ҫ��ŵ����λ����������ݻ���
 * \param[in] nbytes : Ҫ��ŵ����λ����������ݸ���
 *
 * \return �ɹ���ŵ����ݸ���
 */
size_t am_rngbuf_put (am_rngbuf_t rb, const char *p_buf, size_t nbytes);

/**
 * \brief �ӻ��λ�������ȡ����
 *
 * \param[in] rb     : Ҫ�����Ļ��λ���
 * \param[in] p_buf  : ��Ż�ȡ���ݵĻ���
 * \param[in] nbytes : Ҫ��ȡ�����ݸ���
 *
 * \return �ɹ���ȡ�����ݸ���
 */
size_t am_rngbuf_get (am_rngbuf_t rb, char *p_buf, size_t nbytes);

/**
 * \brief ��ջ��λ�����
 *
 * ���������ջ��λ�����������֮ǰ���ڻ����������ݻᶪʧ
 *
 * \param[in] rb : Ҫ�����Ļ��λ���
 *
 * \return ��
 */
void am_rngbuf_flush (am_rngbuf_t rb);

/**
 * \brief ���Ի��λ������Ƿ�Ϊ��
 *
 * \param[in] rb : Ҫ���ԵĻ��λ�����
 *
 * \return ���λ������շ���TRUE, ���򷵻�FALSE
 */
bool_t am_rngbuf_isempty (am_rngbuf_t rb);

/**
 * \brief ���Ի��λ������Ƿ�����
 *
 * \param[in] rb : Ҫ���ԵĻ��λ�����
 *
 * \return ���λ�����������TRUE, ���򷵻�FALSE
 */
bool_t am_rngbuf_isfull (am_rngbuf_t rb);

/**
 * \brief ��ȡ���λ��������пռ��С
 *
 * \param[in] rb : Ҫ�жϵĻ��λ�����
 *
 * \return ���λ��������пռ��С
 */
size_t am_rngbuf_freebytes (am_rngbuf_t rb);

/**
 * \brief ��ȡ���λ������������������ֽڸ���
 *
 * \param[in] rb : Ҫ�жϵĻ��λ�����
 *
 * \return ���λ�����������ֽڸ���
 */
size_t am_rngbuf_nbytes (am_rngbuf_t rb);

/**
 * @} 
 */

#ifdef __cplusplus
}
#endif
 
#endif /* __AM_RINGBUF_H */

/* end of file */
