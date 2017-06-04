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
 * \brief UART(ʹ��ring buffer���ж�ģʽ)��׼�ӿ�
 *
 * ���ļ������ӿڵ�ʵ�ֻ���UART��׼�ӿڣ�Ϊ�����û�ʹ����ƣ�����ΪUART�ӿں���
 * ��ʹ�÷�����
 *
 * \internal
 * \par Modification History
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */

#ifndef __AM_UART_RNGBUF_H
#define __AM_UART_RNGBUF_H


#ifdef __cplusplus
extern "C" {
#endif

#include "am_uart.h"
#include "am_rngbuf.h"
#include "am_wait.h"

/**
 * \addtogroup am_if_uart_rngbuf
 * \copydoc am_uart_rngbuf.h
 * @{
 */


/**
 * \name UART�������������Ŀ���ָ��
 * @{
 */

#define AM_UART_RINGBUF_NREAD     0x0100   /**< \brief �ɶ����ݸ���        */
#define AM_UART_RINGBUF_NWRITE    0x0200   /**< \brief �Ѿ�д������ݸ���  */
#define AM_UART_RINGBUF_FLUSH     0x0300   /**< \brief ��ն�/д������     */
#define AM_UART_RINGBUF_WFLUSH    0x0400   /**< \brief ���д������        */
#define AM_UART_RINGBUF_RFLUSH    0x0500   /**< \brief ��ն�������        */
#define AM_UART_RINGBUF_TIMEOUT   0x0600   /**< \brief ���õȴ���ʱ        */

/** 
 * \brief ��������ģʽ��������أ�Ӳ������ �� �����أ�Ĭ�ϣ�
 */
#define AM_UART_RINGBUF_FLOWMODE_SET    AM_UART_FLOWMODE_SET
    
/** 
 * \brief �������ص���ֵ���ֽ�����
 *
 * ��Ӧ���õ�ֵ�ɸ������Ӧ�Ĳ����趨���������������ֽ���С�ڸ�ֵʱ���أ�
 * δ����ʱ����ֵĬ��Ϊ 20% �� ��������С
 */                                               
#define AM_UART_RINGBUF_RX_FLOW_OFF_THR    0x0700
    
/**
 * \brief ������������ֵ���ֽ�����
 *
 * ��Ӧ���õ�ֵ�ɸ������Ӧ�Ĳ����趨���������������ֽ������ڸ�ֵʱ������
 * δ����ʱ����ֵĬ��Ϊ 80% �� ��������С
 *
 */
#define AM_UART_RINGBUF_RX_FLOW_ON_THR     0x0800

/** @} */

/**
 * \name ����������ѡ��
 * @{
 */

#define AM_RINGBUF_UART_FLOWCTL_NO  AM_UART_FLOWCTL_NO  /**< \brief ������    */
#define AM_RINGBUF_UART_FLOWCTL_HW  AM_UART_FLOWCTL_HW  /**< \brief Ӳ������  */
#define AM_RINGBUF_UART_FLOWCTL_SW  AM_UART_FLOWCTL_SW  /**< \brief �������  */

/** @} */
    
/**
 * \brief UART����ring buffer���ж�ģʽ���豸�ṹ��
 */
typedef struct am_uart_rngbuf_dev {

    /** \brief UART��׼����������    */
    am_uart_handle_t  handle;

    /** \brief �������ݻ��λ�����      */
    struct am_rngbuf  rx_rngbuf;

    /** \brief �������ݻ��λ�����      */
    struct am_rngbuf  tx_rngbuf;
    
    /** \brief �����ֽ������ڸ�ֵʱ����  */
    uint32_t          xon_threshold;
    
    /** \brief �����ֽ������ڸ�ֵʱ����  */
    uint32_t          xoff_threshold;
    
    /** \brief ����ʱ                 */
    uint32_t          timeout_ms;
    
    /** \brief ��ǰ������״̬ TRUE-�򿪣�FALSE-�ر� */
    bool_t            flow_stat;
    
    /** \brief ���ڽ��յȴ�           */
    am_wait_t         rx_wait;

} am_uart_rngbuf_dev_t;

/** \brief UART����ring buffer���ж�ģʽ����׼�������������Ͷ��� */
typedef struct am_uart_rngbuf_dev * am_uart_rngbuf_handle_t;


/**
 * \brief ��ʼ��UARTΪ��ring buffer���ж�ģʽ
 *
 * \param[in] p_dev      : UART����ring buffer���ж�ģʽ���豸�ṹ��ָ��
 * \param[in] handle     : UART��׼����������
 * \param[in] p_rxbuf    : ָ����ջ�������ָ�룬����ʵ�ֽ��ջ��λ�����
 * \param[in] rxbuf_size : ���ջ������Ĵ�С����С������2^n��
 * \param[in] p_txbuf    : ָ���ͻ�������ָ�룬����ʵ�ַ��ͻ��λ�����
 * \param[in] txbuf_size : ���ͻ������Ĵ�С����С������2^n��
 *
 * \return UART����ring buffer���ж�ģʽ����׼���������������ֵΪNULL��
 *         ������ʼ��ʧ��
 */
am_uart_rngbuf_handle_t am_uart_rngbuf_init(am_uart_rngbuf_dev_t *p_dev,
                                            am_uart_handle_t      handle,
                                            uint8_t              *p_rxbuf,
                                            uint32_t              rxbuf_size,
                                            uint8_t              *p_txbuf,
                                            uint32_t              txbuf_size);
                                              
                                              
/**
 * \brief UART����ring buffer���ж�ģʽ�����ڿ���
 *
 *     ����UART���Ƶ������֧�֣��糣���Ĳ��������á�Ӳ���������õȣ�����ֱ��ʹ�á�
 * ֵ��ע����ǣ���������ʱ�����ڱ��빤�����ж�ģʽ�����ܹ����ڲ�ѯģʽ��ģʽ����
 * ָ������ԡ�
 *
 *
 * \param[in] handle  : UART��׼����������
 * \param[in] request : ����ָ��
 *            - AM_UART_RINGBUF_NREAD   : �ɶ����ݸ���������Ϊuint32_t��ָ��
 *            - AM_UART_RINGBUF_NWRITE  : �Ѿ�д������ݸ���������Ϊuint32_t��ָ��
 *            - AM_UART_RINGBUF_FLUSH   ����������������壬�޲���
 *            - AM_UART_RINGBUF_WFLUSH  �����д���壬�޲���
 *            - AM_UART_RINGBUF_RFLUSH  : ��ն����壬�޲���
 *            - AM_UART_RINGBUF_TIMEOUT : ���ö��ĳ�ʱʱ��(ms)������Ϊuint32_t����,
 *                                        0Ϊ�رճ�ʱ���ã���Ϊһֱ�ȴ���
 *            - AM_UART_RINGBUF_FLOWMODE_SET : ��������ģʽ�������������к꣺
 *                                               - AM_RINGBUF_UART_FLOWCTL_NO
 *                                               - AM_RINGBUF_UART_FLOWCTL_HW
 *                                               - AM_RINGBUF_UART_FLOWCTL_SW
 *            - AM_UART_RINGBUF_RX_FLOW_OFF_THR ���������ص���ֵ���ֽ�����
 *            - AM_UART_RINGBUF_RX_FLOW_ON_THR  ��������������ֵ���ֽ�����
 *                                                 
 * \param[in,out] p_arg : ��ָ���Ӧ�Ĳ���
 *
 * \retval AM_OK       : ����ָ��ִ�гɹ�
 * \retval -AM_EIO     : ִ�д���
 * \retval -AM_ENOTSUP : ָ�֧��
 */
int am_uart_rngbuf_ioctl(am_uart_rngbuf_handle_t   handle,
                         int                       request,
                         void                     *p_arg);

/**
 * \brief UART���ݷ��ͣ���ring buffer���ж�ģʽ��
 *
 * \param[in] handle  : UART����ring buffer���ж�ģʽ����׼����������
 * \param[in] p_txbuf : �������ݻ�����
 * \param[in] nbytes  : ���������ݵĸ���
 * 
 * \retval    >0      ���ɹ��������ݵĸ���
 * \retval -AM_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AM_EIO    : ���ݴ������
 */
int am_uart_rngbuf_send(am_uart_rngbuf_handle_t  handle,
                        const uint8_t           *p_txbuf,
                        uint32_t                 nbytes);

/**
 * \brief UART���ݽ��գ���ring buffer���ж�ģʽ��
 *
 * \param[in] handle  : UART����ring buffer���ж�ģʽ����׼����������
 * \param[in] p_rxbuf : �������ݻ�����
 * \param[in] nbytes  : ���������ݵĸ���
 *
 * \retval    >0      ���ɹ��������ݵĸ���
 * \retval -AM_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AM_EIO    : ���ݴ������
 *
 * \note �������˳�ʱ����ʱʱ�᷵�أ�����ͨ������ֵ�ж�ʵ�ʶ�ȡ�����ֽ�����
 */
int am_uart_rngbuf_receive(am_uart_rngbuf_handle_t  handle,
                           uint8_t                 *p_rxbuf,
                           uint32_t                 nbytes);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_UART_RINGBUF_H */

/* end of file */
