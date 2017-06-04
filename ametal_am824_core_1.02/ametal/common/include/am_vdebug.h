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
 * \brief ���ӻ����Ա�׼�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� am_vdebug.h
 *
 * \par ��ʾ��
 * \code
 *  #include "ametal.h"
 *  #include "am_vdebug.h"
 *
 *  int age = 18;
 *  am_kprintf("I'm %d years old!\n", age);
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 15-01-16  tee, first implementation
 * \endinternal
 */

#ifndef __AM_VDEBUG_H
#define __AM_VDEBUG_H

#include <stdarg.h>
#include <stdlib.h>
#include "am_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_if_vdebug
 * \copydoc am_vdebug.h
 * @{
 */

/**
 * \brief ���������AM_VDEBUG��,�� AM_DBG_INFO() �꽫��������Ϣ������
 *        AM_DBG_INFO()��ʵ��Ϊ��
 */
#ifdef AM_VDEBUG
#define AM_DBG_INFO(...)    (void)am_kprintf(__VA_ARGS__)
#else
#define AM_DBG_INFO(...)
#endif

/**
 * \brief ���Գ�ʼ��
 * \param[in] handle   : ��׼�Ĵ��ڷ���Handle, ������Ϣ������ô���
 * \param[in] baudrate : ʹ�õĲ�����
 * \return ��
 * \note ��ʹ�õ��������Ϣǰ��ȷ�����Դ����ѵ��øú�����ʼ�� 
 */
void am_debug_init (am_uart_handle_t handle, uint32_t baudrate);

/**
 * \brief ��ʽ���ַ��������
 *
 * �ú����� vXXXprintf() �����Ļ����������������� vXXXprintf() �����ڴ˺�����
 *
 * \param[in] fil     :  �������Ϊ f_putc �� f_puts �����ĵڶ�������
 * \param[in] f_putc  :  ����ָ�룬ָ��ĺ����������һ���ַ�
 * \param[in] f_puts  :  ����ָ�룬ָ��ĺ����������һ���ַ���
 * \param[in] str     :  ��ʽ���ַ���
 * \param[in] arp     :  ��ʽ���ַ����Ĳ���
 *
 * \return ʵ��������ַ�����
 */
int am_vfprintf_do (void       *fil,
                    int       (*f_putc) (const char  c, void *fil),
                    int       (*f_puts) (const char *s, void *fil),
                    const char *str,
                    va_list     arp);

/**
 * \brief ��ʽ���ַ�����һ��ָ�����ȵ��ַ�����������
 *
 * \param[in] buf  : �����ʽ���ַ����Ļ�����
 * \param[in] sz   : ��������С
 * \param[in] fmt  : ��ʽ���ַ���
 * \param[in] args : ��ʽ���ַ����Ĳ���
 *
 * \return ʵ�����뻺�����ַ��ĸ���
 */
int am_vsnprintf (char *buf, size_t sz, const char *fmt, va_list args);

/**
 * \brief ��ʽ���ַ�����һ��ָ�����ȵ��ַ�����������
 *
 * \param[in] buf  : �����ʽ���ַ����Ļ�����
 * \param[in] sz   : ��������С
 * \param[in] fmt  : ��ʽ���ַ���
 * \param[in] ...  : ��ʽ���ַ����Ĳ����б�
 *
 * \return ʵ�����뻺�����ַ��ĸ���
 *
 * \par ʾ��
 * \code
 *  #include "ametal.h"
 *  #include "am_vdebug.h"
 *  #include <string.h>
 *
 *  char buf[64];
 *  int age = 18;
 *  am_snprintf(&buf[0], sizeof(buf), "I'm %d years old!\n", age);
 * \endcode
 */
#ifndef AM_SNPRINTF_DEFINED
int am_snprintf (char *buf, size_t sz, const char *fmt, ...);
#endif  /* AM_SNPRINTF_DEFINED */

/**
 * \brief �ں˸�ʽ����ӡ����
 *
 * ����������ʽ���ַ����������ǰ�Ŀ���̨��
 *
 * \param[in] fmt : ��ʽ���ַ���
 * \param[in] ... : ��ʽ���ַ����Ĳ����б�
 *
 * \return ���������̨���ַ�����
 *
 * \par ʾ��
 * \code
 *  #include "ametal.h"
 *  #include "am_vdebug.h"
 *
 *  int age = 18;
 *  am_kprintf("I'm %d years old!\n", age);
 * \endcode
 */
#ifndef AM_KPRINTF_DEFINED
int am_kprintf (const char *fmt, ...);
#endif  /* AM_KPRINTF_DEFINED */

/**
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_VDEBUG_H */

/* end of file */
