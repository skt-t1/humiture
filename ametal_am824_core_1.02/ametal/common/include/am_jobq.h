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
 * \brief �������
 *
 *      ���Խ���������������ŵ�һ�������У��ں��ʵ�ʱ���ٿ�ʼ������Щ����
 *  �������ж��¼��Ĵ����С�ͨ������£��жϺ���Ӧ�þ����ܿ�Ľ���������Ӧ��
 *  ��������ʽ��䣨����ʱ����ѯ�ȴ��ȣ�����ʱ���Ϳ��Խ��ж���Ҫ�����������
 *  ��ŵ���������У�Ȼ���ڷ��жϻ����£��ٵ���������д��������������е�
 *  ����
 *      �ر�أ�������������������ʱ������ָ�����ȼ������ȼ��ߵ����񽫻�����
 *  ��������������д�����ʱ����ֵ��ע����ǣ������ֻ�����ȴ������ȼ��ߵ�
 *  ���񣬲����ܴ�����ȼ��͵�����
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ�:
 * \code
 * #include "am_jobq.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 15-09-17  tee, first implementation
 * \endinternal
 */

#ifndef __AM_JOBQ_H
#define __AM_JOBQ_H

/**
 * \addtogroup am_if_jobq
 * \copydoc am_jobq.h
 * @{
 */
#include "am_list.h"
#include <string.h>


/**
 * \brief ����������нṹ��
 */
typedef struct am_jobq_queue {
    
    /** \brief ÿ�����ȼ���Ӧһ������ͷ    */
    struct am_list_head  *p_heads;
    
    /** \brief ������е����ȼ���Ŀ��������ȼ���ĿΪ1024   */
    unsigned int          pri_num;
    
     /** \brief ÿλ��ʾһ���飬Ϊ 1 ��ʾ������������       */
    unsigned int          bitmap_grp;
    
    /** \brief һ��������32������λΪ 1 ��ʾ�����ȼ������� */
    unsigned int         *p_bitmap_job;
 
    /** \brief ������е�һЩ״̬��־      */
    uint32_t              flags;
} am_jobq_queue_t;

/**
 * \brief �����׼������еı�׼���������þ�����ɳ�ʼ��������� 
 */
typedef struct am_jobq_queue *am_jobq_handle_t;

/** 
 * \brief ��������ṹ�壬Ӧ�ó���Ӧֱ�Ӳ����ṹ���Ա
 *
 * ʹ�ø����Ͷ���һ��job��Ӧ����am_jobq_mkjob()�����������еĸ�����Ա
 */
typedef struct am_jobq_job {
    am_pfnvoid_t        func;   /**< \brief �������Ӧ�Ĵ����� */
    void               *p_arg;  /**< \brief ��������Ӧ�Ĳ���   */
    uint16_t            pri;    /**< \brief ���ȼ�               */
    uint16_t            flags;  /**< \brief һЩ��־             */
    struct am_list_head node;   /**< \brief ����ڵ�             */
} am_jobq_job_t;

/**
 * \brief ��ʼ��һ���������
 *
 * \param[in] p_jobq_queue : ָ��������е�ָ��
 * \param[in] pri_num      : �������ʹ�õ����ȼ���Ŀ
 *                             - ʵ���������Ч���ȼ�Ϊ��0 ~ priority_num - 1
                               - ֵԽ�����ȼ�Խ�ͣ�0Ϊ������ȼ�
 * \param[in] p_heads      : �ڴ���䣬��С����Ϊ�� priority_num 
 * \param[in] p_bitmap_job : �ڴ���䣬��С����Ϊ�� priority_num / 32 ����ȡ����
 *                           ����(priority_num + 31) / 32
 *
 * \return ������еĲ�������������ʼ��ʧ�ܣ���ֵΪNULL
 *
 * \par ����
 * \code
 *
 *  #define MY_QJOB_NUM_PRI  64    // ���ȼ���Ŀ64
 *
 *  am_jobq_queue_t      myqjob_queue;
 *  struct am_list_head  myqjob_head[MY_QJOB_NUM_PRI];
 *  unsigned int         myqjob_pri_active[(MY_QJOB_NUM_PRI + 31) / 32];
 *
 *  am_jobq_queue_init(&myqjob_queue,
 *                     MY_QJOB_NUM_PRI,
 *                     myqjob_head,
 *                     myqjob_pri_active);
 * \endcode
 *
 * \note ���ڸú�����Ҫ�ڴ���䣬�������Ϥ���򲻽���ֱ��ʹ�øú���������ʹ�ø�
 *       Ϊ�򵥵ĺ�ͬ���õ�һ��handle����ʹ�� AM_JOBQ_QUEUE_DECL()����������С�
 *       ��ʹ�� AM_JOBQ_QUEUE_INIT() ��ʼ��������С�
 *
 * \sa AM_JOBQ_QUEUE_DECL(), AM_JOBQ_QUEUE_INIT()
 */
am_jobq_handle_t am_jobq_queue_init (am_jobq_queue_t     *p_jobq_queue,
                                     unsigned int         pri_num,
                                     struct am_list_head *p_heads,
                                     unsigned int        *p_bitmap_job);

/**
 * \brief ��ʼ��һ������
 *
 * �ڽ�һ��������뵽����֮ǰ��ȷ��ʹ�øú�����һ�������ʼ��
 *
 * \param[in] p_job : ָ�������ָ��
 * \param[in] func  : �����Ӧ�Ĵ�����
 * \param[in] p_arg : ��������Ӧ���û�����
 * \param[in] pri   : ��������ȼ�
 *
 * \retval AM_OK      ����ʼ���ɹ�
 * \retval -AM_EINVAL : ��������
 *
 * \note ʹ�øú���ǰ��������ʹ�� am_jobq_job_t ����һ��job����ʹ�øú�����ʼ��
 *       ���еĸ�����Ա����������ʵ�ʴ������ڶ��д���ʱ����˶���ʱ���붨��Ϊ
 *       ȫ�ֱ�����̬������
 */
void am_jobq_job_init (am_jobq_job_t  *p_job,
                       am_pfnvoid_t    func,
                       void           *p_arg,
                       uint16_t        pri);
 
/**
 * \brief ��һ��������ӵ�������
 *
 * \param[in] handle  : ������еı�׼������
 * \param[in] p_job   : ָ�������ָ��
 *
 * \retval AM_OK      ������ɹ�
 * \retval -AM_EINVAL : ��������
 * \retval -AM_EBUSY  : ������ǰ�����Ѿ��ڶ����У��޷��ظ����
 */
int am_jobq_post (am_jobq_handle_t handle, am_jobq_job_t *p_job);

/**
 * \brief ����һ��������У�������������Ϻ󷵻�
 *
 * һ�����񱻴�����Զ��˳����У����Ա��ٴμ��롣����
 * ����δ������ʱ���޷��ٴμ��������
 *
 * \param[in] handle  : ������еı�׼������
 *
 * \retval AM_OK      ���������������������
 * \retval -AM_EINVAL : ��������
 * \retval -AM_EBUSY  : ������ǰ����������ڴ�����
 */
int am_jobq_process (am_jobq_handle_t handle);

/**
 * \brief �����������ʵ������ָ������ʹ�õ�������ȼ�
 *
 * \param[in] queue_name   : ������
 * \param[in] priority_num : �������ʹ�õ����ȼ���Ŀ
 *                             - ʵ���������Ч���ȼ�Ϊ��0 ~ priority_num - 1;
 *                             - ֵԽ�����ȼ�Խ�ͣ�0Ϊ������ȼ���
 *
 * \note �ö�����Ϊ�ڴ���䣬���ڶ�����Ҫ�����ڳ�ʼ��ʱ�ʹ���ʱ����Ч�������Ҫ
 *       ����Ϊȫ�ֵģ����ֻ��ģ����ʹ�ã�����ʹ�� AM_JOBQ_QUEUE_DECL_STATIC()
 *       ����Ϊ��̬�ġ�
 */
#define AM_JOBQ_QUEUE_DECL(queue_name, priority_num)                        \
            struct __jobqinfo_##queue_name {                                \
                am_jobq_queue_t      jobq_queue;                            \
                struct am_list_head  pri_heads[priority_num];               \
                unsigned int         bitmap_job[(priority_num + 31) >> 5];  \
            } queue_name;

/**
 * \brief �����������ʵ������̬��
 *
 * \param[in] queue_name   : ������
 * \param[in] priority_num : �������ʹ�õ����ȼ���Ŀ
 *                             - ʵ���������Ч���ȼ�Ϊ��0 ~ priority_num - 1;
 *                             - ֵԽ�����ȼ�Խ�ͣ�0Ϊ������ȼ���
 */
#define AM_JOBQ_QUEUE_DECL_STATIC(queue_name, priority_num)            \
            static AM_JOBQ_QUEUE_DECL(queue_name, priority_num)
/**
 * \brief ��ʼ���������
 *
 * \param[in] queue_name : ����������ƣ���AM_JOBQ_QUEUE_DECL_STATIC()��
 *                          AM_JOBQ_QUEUE_DECL()�����������С�
 *
 * \return ������еĲ�������������ʼ��ʧ�ܣ���ֵΪNULL
 */
#define AM_JOBQ_QUEUE_INIT(queue_name)                                         \
            am_jobq_queue_init(                                                \
             &(queue_name.jobq_queue),                                         \
               sizeof(queue_name.pri_heads) / sizeof(queue_name.pri_heads[0]), \
               queue_name.pri_heads,                                           \
               queue_name.bitmap_job)

/** @}  */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AM_JOBQ_H */

/* end of file */

