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
 * \brief SCT����������CAP��׼�ӿ�
 *
 * 1. SCT֧���ṩ�������ֱ�׼���񣬱������ṩ���Ƿ���CAP��׼�����������
 *     - ��ʱ��
 *     - PWM�����
 *     - ����
 * 2. �ڱ������У�SCT�����Դʹ�����£�
 *    - ����ͨ��0���ڲ���SCT����0�����¼�0�������ʹ�ò���Ĵ���0���Բ����жϣ�
 *    - ����ͨ��1���ڲ���SCT����1�����¼�1�������ʹ�ò���Ĵ���1���Բ����жϣ�
 *    - ����ͨ��2���ڲ���SCT����2�����¼�2�������ʹ�ò���Ĵ���2���Բ����жϣ�
 *    - ����ͨ��3���ڲ���SCT����3�����¼�3�������ʹ�ò���Ĵ���3���Բ����жϡ�
 *
 * \internal
 * \par Modification history
 * - 1.01 15-09-24  zxl, modified.
 * - 1.00 15-07-07  oce, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_SCT_CAP_H
#define __AM_LPC82X_SCT_CAP_H

#include "ametal.h"
#include "am_cap.h"
#include "amhw_lpc82x_inmux.h"
#include "am_lpc82x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_sct_cap
 * \copydoc am_lpc82x_sct_cap.h
 * @{
 */

/**
 * \brief SCT CAP���빦����ص�GPIO��Ϣ
 */
typedef struct am_lpc82x_sct_cap_ioinfo {
    uint32_t gpio;           /**< \brief GPIO����           */
    uint32_t func;           /**< \brief GPIO����           */
    uint32_t dfunc;          /**< \brief ȡ�����ܺ�Ĭ�Ϲ��� */
} am_lpc82x_sct_cap_ioinfo_t;


/**
 * \brief SCT��������ص��豸��Ϣ
 */
typedef struct am_lpc82x_sct_cap_devinfo {
    uint32_t                     sct_regbase;  /**< \brief SCT�Ĵ�����Ļ���ַ */
    uint8_t                      inum;         /**< \brief SCT��Ӧ���жϺ�     */
    uint8_t                      channels_num; /**< \brief ʹ�õ�ͨ���������Ϊ4*/
    am_lpc82x_sct_cap_ioinfo_t  *p_ioinfo;     /**< \brief CAP���õ��Ĳ�������    */

    /** \brief ƽ̨��ʼ�����������ʱ�ӣ��������ŵȹ��� */
    void     (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void     (*pfn_plfm_deinit)(void);
}am_lpc82x_sct_cap_devinfo_t;

/**
 * \brief SCT�������豸
 */
typedef struct am_lpc82x_sct_cap_dev {
    am_cap_serv_t          cap_serv;       /**< \brief ��׼CAP����                 */

    struct {
        am_cap_callback_t  callback_func;  /**< \brief �ص�����                    */
        void              *p_arg;          /**< \brief �ص��������û�����          */
    } callback_info[4];                    /**< \brief �洢���4ͨ���Ļص�������Ϣ */

    /** \brief ָ��SCT(������)�豸��Ϣ������ָ�� */
    const am_lpc82x_sct_cap_devinfo_t *p_devinfo;

} am_lpc82x_sct_cap_dev_t;

/**
 * \brief ��ʼ��SCTΪ������
 *
 * \param[in] p_dev     : ָ��SCT(������)�豸��ָ��
 * \param[in] p_devinfo : ָ��SCT(������)�豸��Ϣ������ָ��
 *
 * \return CAP��׼������������ֵΪNULLʱ������ʼ��ʧ��
 */
am_cap_handle_t am_lpc82x_sct_cap_init(am_lpc82x_sct_cap_dev_t     *p_dev,
                                 const am_lpc82x_sct_cap_devinfo_t *p_devinfo);


/**
 * \brief ��ʹ��SCT������ʱ�����ʼ��SCT�����ܣ��ͷ������Դ
 *
 * \param[in] handle :  am_lpc82x_sct_cap_init() ��ʼ��������õ�CAP������
 * \return ��
 */
void am_lpc82x_sct_cap_deinit(am_cap_handle_t handle);


#define AM_LPC82X_SCT_CAP_INPUT_CHAN0       0   /**< \brief SCTѡ������Դͨ��0  */
#define AM_LPC82X_SCT_CAP_INPUT_CHAN1       1   /**< \brief SCTѡ������Դͨ��1  */
#define AM_LPC82X_SCT_CAP_INPUT_CHAN2       2   /**< \brief SCTѡ������Դͨ��2  */
#define AM_LPC82X_SCT_CAP_INPUT_CHAN3       3   /**< \brief SCTѡ������Դͨ��3  */

/**
 * \brief �޸�ץȡ��ѡ������Դ
 * \param[in] chan  SCTѡ������Դͨ�� ��ѡ���ֵΪAM_LPC82X_SCT_CAP_INPUT_CHAN*
 * \param[in] input_src   ��������й�����amhw_lpc82x_inmux.h�ļ��й���SCT�Ķ���
 * \return ��
 */
 
 
am_static_inline
void am_lpc82x_sct_cap_set_input(uint8_t                          chan,
                                 amhw_lpc82x_inmux_sct_trig_src_t input_src)
{
    amhw_lpc82x_inmux_sct_trig_set(AMHW_LPC82X_INMUX, chan, input_src);
}


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_SCT_CAP_H */

/* end of file */
