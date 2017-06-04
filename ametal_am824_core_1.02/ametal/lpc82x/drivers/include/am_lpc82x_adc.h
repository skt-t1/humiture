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
 * \brief ADC����������ADC��׼�ӿ�
 *
 * \internal
 * \par Modification History
 * - 1.01 15-12-09  hgo, modified.
 * - 1.00 15-07-15  bob, first implementation.
 * \endinternal
 */
 
#ifndef  __AM_LPC82X_ADC_H
#define  __AM_LPC82X_ADC_H

#include "ametal.h"
#include "am_adc.h"
#include "am_lpc82x_dma.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \addtogroup am_lpc82x_if_adc
 * \copydoc am_lpc82x_adc.h
 * @{
 */

/**
 * \name ADC����ģʽ���ú�
 * 
 * \note ADC�ṩDMA����ģʽ���жϹ���ģʽ
 * \anchor grp_am_lpc82x_adc_work_mode
 * @{
 */

/** \brief DMA����ģʽ */
#define AM_LPC82X_ADC_WORK_MODE_DMA          0

/** \brief �жϹ���ģʽ */
#define AM_LPC82X_ADC_WORK_MODE_INT         -1
    
/** @} */


/**
 * \brief ADC �豸��Ϣ 
 */
typedef struct am_lpc82x_adc_devinfo {
    
    /** \brief ADC�Ĵ�����Ļ���ַ */
    uint32_t   adc_regbase;
    
    /**
     * \brief ADC�ο���ѹ����λ��mV
     *
     * ���� LPC82x, �ο���ѹ Vref = (Vrefp - Vrefn)  Ϊ��������������ʱ
     * �ȶ�����ADC��ȷ����VREFP = VDDA Vrefn ����ֵΪ�� Vss �� Vssa ��
     *
     * \note �òο���ѹ�ɾ���ĵ�·����
     *
     */
    uint32_t   vref;

    /** \brief ADC����A�жϺ�      */
    int16_t    inum_seqa;

    /** \brief ADC����B�жϺ�      */
    int16_t    inum_seqb;

    /** \brief ADC��ֵ�Ƚ��жϺ�     */
    int16_t    inum_thcmp;

    /** \brief ADC��������жϺ�   */
    int16_t    inum_ovr;

    /**
     * \brief ADC����ģʽ
     *
     * �����÷���\ref grp_am_lpc82x_adc_work_mode
     *         ʹ��#AM_LPC82X_ADC_WORK_MODE_DMA��
     *             ����am_adc_start()�ӿ�ʱ������������������desc_num����С�ڵ���2��
     *         ʹ��#AM_LPC82X_ADC_WORK_MODE_INT��
     *             ����am_adc_start()�ӿ�ʱ���������������������ƣ���dma_chan�����ԡ�
     */
    int16_t    work_mode;
    
    /** \brief DMAͨ�� */
    int16_t    dma_chan;
    
    /** \brief ƽ̨��ʼ�����������ʱ�ӣ��������ŵȹ��� */
    void     (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void     (*pfn_plfm_deinit)(void);

} am_lpc82x_adc_devinfo_t;

/**
 * \brief ADC�豸ʵ�� 
 */
typedef struct am_lpc82x_adc_dev {
    
    /** \brief ADC��׼����                    */
    am_adc_serv_t                   adc_serve;

    /** \brief ָ��ADC�豸��Ϣ��ָ��          */
    const am_lpc82x_adc_devinfo_t  *p_devinfo;
    
    /** \brief ����û�����ת����Ļص�����   */
    pfn_adc_seq_complete_t          pfn_callback;

    /** \brief �û������ص������Ļص��������� */
    void                           *p_arg;
    
    /** \brief ��ǰת����ͨ��                 */
    uint32_t                        chan;
    
    /** \brief ��ǰת�������������������׵�ַ */
    am_adc_buf_desc_t              *p_desc;
    
    /** \brief ��ǰת������������������       */
    uint32_t                        desc_num;
    
    /** \brief ת���Ĵ���                     */ 
    uint32_t                        count;
    
    /** \brief ת����־                       */ 
    uint32_t                        flags;

    /** \brief �Ե�ǰ�����������Ѿ������Ĵ��� */
    uint32_t                        conv_cnt;

    /** \brief ����ִ�е�ǰ����������������   */
    uint32_t                        desc_index;

    /** \brief ����������ת����ɵĴ��� */
    uint32_t                        seq_cnt;

    /** \brief ָ��ǰʹ�õ�DMA��������� */
    am_lpc82x_dma_controller_t     *p_dma_ctr;

    /** \brief DMA����������                  */
    __attribute__((aligned(16))) am_lpc82x_dma_xfer_desc_t dma_desc[2];
} am_lpc82x_adc_dev_t;

/**
 * \brief ADC��ʼ�� 
 *
 * Ĭ�ϳ�ʼ������A 
 *
 * \param[in] p_dev     : ָ��ADC�豸��ָ�� 
 * \param[in] p_devinfo : ָ��ADC�豸��Ϣ��ָ�� 
 *
 * \return ADC��׼���������� ���Ϊ NULL��������ʼ��ʧ�� 
 */
am_adc_handle_t am_lpc82x_adc_init (am_lpc82x_adc_dev_t     *p_dev,
                              const am_lpc82x_adc_devinfo_t *p_devinfo);

/**
 * \brief ADCȥ��ʼ�� 
 *
 * Ĭ��ȥ��ʼ������A 
 *
 * \param[in] handle : am_lpc82x_adc_init() ��ʼ��������õ�ADC������
 *
 * \return �� 
 */
void am_lpc82x_adc_deinit (am_adc_handle_t handle);

/**
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /*__AM_LPC82X_ADC_H  */

/* end of file */
