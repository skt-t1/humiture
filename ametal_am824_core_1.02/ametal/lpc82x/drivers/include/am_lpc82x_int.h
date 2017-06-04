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
 * \brief  NVIC�����������жϱ�׼�ӿ�
 *
 * \internal
 * \par Modification History
 * - 1.00 15-01-29  hbt, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_INT_H
#define __AM_LPC82X_INT_H

#include "ametal.h"
#include "amhw_lpc82x_int.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_int
 * \copydoc am_lpc82x_int.h
 * @{
 */
    

/**
 * \brief �жϷ�����Ϣ
 */
struct am_lpc82x_int_servinfo {

    /** \brief ��ʼ�жϺ� */
    int inum_start;

    /** \brief ĩβ�жϺ� */
    int inum_end;
};

/** \brief �жϷ���������Ϣ */
struct am_lpc82x_int_isr_info {

    /** \brief �����ص����� */
    am_pfnvoid_t  pfn_isr;

    /** \brief �ص������Ĳ��� */
    void         *p_arg;
};


/** \brief �ж��豸��Ϣ */
typedef struct am_lpc82x_int_devinfo {
    
    /** \brief �жϷ�����Ϣʵ�� */
    const struct am_lpc82x_int_servinfo int_servinfo;
    
    /** \brief �жϿ������Ĵ������ַ */
    uint32_t int_regbase;
    
    /** \brief ���ж����� */
    int input_cnt;

    /** \brief ISR ��Ϣӳ��(��С�� input_cnt һ��) */
    uint8_t *p_isrmap;

    /** \brief ISR ��Ϣ���� */
    int isrinfo_cnt;

    /** \brief ISR ��Ϣӳ���ڴ�(��С�� isrinfo_cnt һ��) */
    struct am_lpc82x_int_isr_info *p_isrinfo;
    
    /** \brief ƽ̨��ʼ������ */
    void     (*pfn_plfm_init)(void);

    /** \brief ƽ̨���ʼ������ */
    void     (*pfn_plfm_deinit)(void);
    
} am_lpc82x_int_devinfo_t;

/**
 * \brief �ж��豸ʵ�� 
 */
typedef struct am_lpc82x_int_dev {
    
    /** \brief ָ���ж��豸��Ϣ��ָ�� */
    const am_lpc82x_int_devinfo_t *p_devinfo;
    
    /** \brief ������Ч��־ */
    bool_t                         valid_flg;
    
} am_lpc82x_int_dev_t;



/**
 * \brief �жϳ�ʼ�� 
 *
 * \param[in] p_dev     : ָ���ж��豸��ָ�� 
 * \param[in] p_devinfo : ָ���ж��豸��Ϣ��ָ�� 
 *
 * \retval  AM_OK       : �����ɹ� 
 * \retval -AM_EINVAL   : ��Ч���� 
 */
int am_lpc82x_int_init (am_lpc82x_int_dev_t           *p_dev, 
                        const am_lpc82x_int_devinfo_t *p_devinfo);

/**
 * \brief �ж�ȥ��ʼ��
 * \return  �� 
 */
void am_lpc82x_int_deinit (void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif /* __AM_LPC82X_INT_H */

/* end of file */
