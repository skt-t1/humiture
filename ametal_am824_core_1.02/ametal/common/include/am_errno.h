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
 * \brief �������ض���ͷ�ļ�
 *
 * \internal
 * \par Modification history
 * - 1.00 14-11-01  zen, first implementation.
 * \endinternal
 */

#ifndef __AM_ERRNO_H
#define __AM_ERRNO_H
 
#ifdef __cplusplus
extern "C" {
#endif


/**
 * \addtogroup am_if_errno
 * \copydoc am_errno.h
 * @{
 */

/**
 * \name POSIX����ֵ
 * @{
 */
#define AM_EPERM           1       /**< \brief ����������          */
#define AM_ENOENT          2       /**< \brief �ļ���Ŀ¼������    */
#define AM_ESRCH           3       /**< \brief ���̲�����          */
#define AM_EINTR           4       /**< \brief ���ñ��ж�          */
#define AM_EIO             5       /**< \brief I/O ����            */
#define AM_ENXIO           6       /**< \brief �豸���ַ������    */
#define AM_E2BIG           7       /**< \brief �����б�̫��        */
#define AM_ENOEXEC         8       /**< \brief ��ִ���ļ���ʽ����  */
#define AM_EBADF           9       /**< \brief �ļ���������      */
#define AM_ECHILD          10      /**< \brief û���ӽ���          */
#define AM_EAGAIN          11      /**< \brief ��Դ�����ã�������  */
#define AM_ENOMEM          12      /**< \brief �ռ䣨�ڴ棩����    */
#define AM_EACCES          13      /**< \brief Ȩ�޲���            */
#define AM_EFAULT          14      /**< \brief ��ַ����            */
#define AM_ENOTEMPTY       15      /**< \brief Ŀ¼�ǿ�            */
#define AM_EBUSY           16      /**< \brief �豸����Դæ        */
#define AM_EEXIST          17      /**< \brief �ļ��Ѿ�����        */
#define AM_EXDEV           18      /**< \brief ���豸����          */
#define AM_ENODEV          19      /**< \brief �豸������          */
#define AM_ENOTDIR         20      /**< \brief ����Ŀ¼            */
#define AM_EISDIR          21      /**< \brief ��Ŀ¼              */
#define AM_EINVAL          22      /**< \brief ��Ч����            */
#define AM_ENFILE          23      /**< \brief ϵͳ���ļ�̫�࣬������������ */
#define AM_EMFILE          24      /**< \brief �򿪵��ļ�̫��      */
#define AM_ENOTTY          25      /**< \brief �����ʵ�I/O���Ʋ��� */
#define AM_ENAMETOOLONG    26      /**< \brief �ļ���̫��          */
#define AM_EFBIG           27      /**< \brief �ļ�̫��            */
#define AM_ENOSPC          28      /**< \brief �豸ʣ��ռ䲻��    */
#define AM_ESPIPE          29      /**< \brief ��Ч��������Invalid seek�� */
#define AM_EROFS           30      /**< \brief �ļ�ϵͳֻ��        */
#define AM_EMLINK          31      /**< \brief ����̫��            */
#define AM_EPIPE           32      /**< \brief �𻵵Ĺܵ�          */
#define AM_EDEADLK         33      /**< \brief ��Դ��������        */
#define AM_ENOLCK          34      /**< \brief �޿��ã����У�����  */
#define AM_ENOTSUP         35      /**< \brief ��֧��              */
#define AM_EMSGSIZE        36      /**< \brief ��Ϣ̫��            */

/** @} */

/**
 * \name ANSI����ֵ
 * @{
 */

#define AM_EDOM            37      /**< \brief ��ѧ��������������Χ */
#define AM_ERANGE          38      /**< \brief ��ѧ����������̫�� */
#define AM_EILSEQ          39      /**< \brief �Ƿ����ֽ�˳��       */
 
/** @} */

/**
 * \name ��������������
 * @{
 */

#define AM_EDESTADDRREQ    40      /**< \brief ��ҪĿ���ַ         */
#define AM_EPROTOTYPE      41      /**< \brief socketЭ�����ʹ���   */
#define AM_ENOPROTOOPT     42      /**< \brief Э�鲻����           */
#define AM_EPROTONOSUPPORT 43      /**< \brief Э�鲻֧��           */
#define AM_ESOCKTNOSUPPORT 44      /**< \brief Socket���Ͳ�֧��     */
#define AM_EOPNOTSUPP      45      /**< \brief socket��֧�ָò���   */
#define AM_EPFNOSUPPORT    46      /**< \brief Э���岻֧��         */
#define AM_EAFNOSUPPORT    47      /**< \brief ��ַ�ز�֧��         */
#define AM_EADDRINUSE      48      /**< \brief ��ַ�Ѿ���ռ��       */
#define AM_EADDRNOTAVAIL   49      /**< \brief ��ַ������           */
#define AM_ENOTSOCK        50      /**< \brief ������������socket */

/** @} */

/**
 * \name ��ѡ��ʵ�ֵĴ���ֵ
 * @{
 */

#define AM_ENETUNREACH     51        /**< \brief ���粻�ɴ�         */
#define AM_ENETRESET       52        /**< \brief �����ж�������     */
#define AM_ECONNABORTED    53        /**< \brief �����ж�           */
#define AM_ECONNRESET      54        /**< \brief ���Ӹ�λ           */
#define AM_ENOBUFS         55        /**< \brief ����ռ䲻��       */
#define AM_EISCONN         56        /**< \brief Socket�Ѿ�����     */
#define AM_ENOTCONN        57        /**< \brief Socketû������     */
#define AM_ESHUTDOWN       58        /**< \brief Socket�Ѿ��رգ����ܷ������� */
#define AM_ETOOMANYREFS    59        /**< \brief ����̫�࣬�޷�ƴ�� */
#define AM_ETIMEDOUT       60        /**< \brief ���ӳ�ʱ           */
#define AM_ECONNREFUSED    61        /**< \brief ���ӱ��ܾ�         */
#define AM_ENETDOWN        62        /**< \brief �����Ѿ�ֹͣ       */
#define AM_ETXTBSY         63        /**< \brief �ı��ļ�æ         */
#define AM_ELOOP           64        /**< \brief �������Ӽ���̫��   */
#define AM_EHOSTUNREACH    65        /**< \brief �������ɴ�         */
#define AM_ENOTBLK         66        /**< \brief �ǿ��豸           */
#define AM_EHOSTDOWN       67        /**< \brief �����Ѿ��ر�       */

/** @} */

/**
 * \name ���������ж�I/O����ֵ
 * @{
 */

#define AM_EINPROGRESS     68         /**< \brief �������ڽ����� */
#define AM_EALREADY        69         /**< \brief ��������ʹ���� */

/* 70 */

#define AM_EWOULDBLOCK     AM_EAGAIN  /**< \brief ������������ͬEAGAIN�� */
#define AM_ENOSYS          71         /**< \brief ��֧�ֵĹ��ܣ�����δʵ�֣�*/
 
/** @} */

/**
 * \name �첽I/O����ֵ
 * @{
 */
#define AM_ECANCELED       72         /**< \brief �����Ѿ�ȡ�� */

/* 73 */
/** @} */

/**
 * \name ����ش���ֵ
 * @{
 */
#define AM_ENOSR           74        /**< \brief û������Դ */
#define AM_ENOSTR          75        /**< \brief �������豸 */
#define AM_EPROTO          76        /**< \brief Э����� */
#define AM_EBADMSG         77        /**< \brief �𻵵���Ϣ */
#define AM_ENODATA         78        /**< \brief ���������� */
#define AM_ETIME           79        /**< \brief ��ioctl()��ʱ */
#define AM_ENOMSG          80        /**< \brief û���������Ϣ���� */
#define AM_EUCLEAN         81        /**< \brief Structure��Ҫ���� */
/** @} */                            

/**
* \name �����Զ������ֵ
 * @{
 */
 
#define AM_EFULL           100     /**< \brief ��               */ 
#define AM_EEMPTY          101     /**< \brief ��               */
#define AM_ERXOV           102     /**< \brief �������         */
#define AM_ETXUR           103     /**< \brief ����Underrun���� */
#define AM_ESSA            104     /**< \brief �ӻ�����         */
#define AM_ESSD            105     /**< \brief �ӻ��������     */
#define AM_EHIGH           106     /**< \brief ֵ����           */
#define AM_ELOW            107     /**< \brief ֵ����           */

/** @} */

/** \brief �û��Զ��������ʼֵ */
#define AM_ERRNO_USER_START 2000

/** \brief �û��Զ������ֵ */
#define AM_ERRNO_USER(x)    (AM_ERRNO_USER_START + (x))

/** @} */

/**
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_ERRNO_H */

/* end of file */
