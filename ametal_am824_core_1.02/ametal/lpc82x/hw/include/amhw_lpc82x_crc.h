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
 * \brief ѭ������У��(CRC) �����ӿ�
 *
 * 1. ֧��3��CRC���ɶ���ʽ�� CRC-CCITT, CRC-16, and CRC-32
 *    - CRC-CCITT : x16 + x12 + x5 + 1   (0x1021);
 *    - CRC-16    : x16 + x15 + x2 + 1   (0x8005);
 *    - CRC-32    : x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 +
 *                  x5 + x4 + x2+ x + 1  (0x04C11DB7);
 * 2. ֧���������ݺ�CRCУ��͵�λ�����1's����;
 * 3. �ɱ��������������;
 * 4. ֧���������ݿ�ȵ���������: 8λ��16λ��32λ
 *    - 8 λ����д��: ����1������;
 *    - 16λ����д��: ����2������;
 *    - 32λ����д��: ����4�����ڡ�
 *
 * \internal
 * \par Modification history
 * - 1.00 15-7-08  sss, first implementation.
 * \endinternal
 */

#ifndef __AMHW_LPC82X_CRC_H
#define __AMHW_LPC82X_CRC_H

#include "ametal.h"

#ifdef __cplusplus 
extern "C" {
#endif

/*
 * \brief �����ṹ��εĿ�ʼ
 */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
#elif defined(__TMS470__)
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler t
#endif



/**
 * \addtogroup amhw_lpc82x_if_crc
 * \copydoc amhw_lpc82x_crc.h
 * @{
 */
 
/**
 * \brief CRC�Ĵ�����ṹ��
 */
typedef struct amhw_lpc82x_crc {
    __IO uint32_t mode;                /**< \brief crcģʽ�Ĵ���          */
    __IO uint32_t seed;                /**< \brief crc���ӼĴ���          */
    union {
        __I uint32_t sum;              /**< \brief crcУ��Ĵ���          */
        __O uint32_t wrdata32;         /**< \brief crcд���ݼĴ���(32λд)*/
        __O uint16_t wrdata16;         /**< \brief crcд���ݼĴ���(16λд)*/
        __O uint8_t  wrdata8;          /**< \brief crcд���ݼĴ���(8λд) */
    };
} amhw_lpc82x_crc_t;

/**
 * \brief crcģʽ����
 */

/** \brief ��������λ����           */
#define AMHW_LPC82X_CRC_MODE_WRDATA_BIT_RVS   AM_SBF(1, 2)  
/** \brief ��������λȡ1's �������� */
#define AMHW_LPC82X_CRC_MODE_WRDATA_BIT_CMPL  AM_SBF(1, 3)  
/** \brief У���λ����             */
#define AMHW_LPC82X_CRC_MODE_SUM_RVS          AM_SBF(1, 4)  
/** \brief У���λȡ1's��������    */
#define AMHW_LPC82X_CRC_MODE_SUM_CMPL         AM_SBF(1, 5)  

/**
 * \brief crc���ɶ���ʽ����
 */
typedef enum amhw_lpc82x_crc_poly {
    
    /** \brief CRC-CCITT ����ʽ*/
    AMHW_LPC82X_CRC_POLY_CCITT = 0,                
    /** \brief CRC-16����ʽ    */
    AMHW_LPC82X_CRC_POLY_CRC16 = 1,                
    /** \brief CRC-32����ʽ    */
    AMHW_LPC82X_CRC_POLY_CRC32 = 2    
    
} amhw_lpc82x_crc_poly_t;

/**
 * \brief ����ģʽ�����ɶ���ʽ
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \param[in] poly     : crc���ɵĶ���ʽ
 * \param[in] flags    : crc��־λ
 *                       ֻ��Ϊ 
 *                       #AMHW_LPC82X_CRC_MODE_WRDATA_BIT_RVS  ��
 *                       #AMHW_LPC82X_CRC_MODE_WRDATA_BIT_CMPL ��
 *                       #AMHW_LPC82X_CRC_MODE_SUM_RVS         ��
 *                       #AMHW_LPC82X_CRC_MODE_SUM_CMPL        ��
 *                       �ĸ�ֵ�е�һ�������Ļ�ֵ
 * \return ��
 */
am_static_inline
void amhw_lpc82x_crc_mode_set (amhw_lpc82x_crc_t     *p_hw_crc,
                               amhw_lpc82x_crc_poly_t poly,
                               uint32_t               flags)
{
    p_hw_crc->mode = (uint32_t)poly | flags;
}

/**
 * \brief �õ�crcģʽ�Ĵ�����ֵ
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \return ģʽ�Ĵ�����ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_crc_mode_get (amhw_lpc82x_crc_t *p_hw_crc)
{
    return p_hw_crc->mode; 
}

/**
 * \brief �������ӼĴ�����ֵ
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \param[in] seed     : ����ֵ
 * \return ��
 */
am_static_inline
void amhw_lpc82x_crc_seed_set (amhw_lpc82x_crc_t *p_hw_crc, uint32_t seed)
{
    p_hw_crc->seed = seed;
}

/**
 * \brief �õ����ӼĴ�����ֵ
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \return ��ǰ���ӼĴ�����ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_crc_seed_get (amhw_lpc82x_crc_t *p_hw_crc)
{
    return p_hw_crc->seed;
}

/**
 * \brief ��ȡcrcУ��ͼĴ�����ֵ
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \return У��͵�ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_crc_sum_get (amhw_lpc82x_crc_t *p_hw_crc)
{
    return p_hw_crc->sum;
}

/**
 * \brief д��һ����λ���ݵ�ֵ��crc������м���
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \param[in] data     : д��İ�λ����
 * \return ��
 */
am_static_inline
void amhw_lpc82x_crc_wrdata8 (amhw_lpc82x_crc_t *p_hw_crc, uint8_t data)
{
    p_hw_crc->wrdata8 = data;
}

/**
 * \brief д��һ��ʮ��λ���ݵ�ֵ��crc������м���
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \param[in] data     : д���ʮ��λ����
 * \return ��
 */
am_static_inline
void amhw_lpc82x_crc_wrdata16 (amhw_lpc82x_crc_t *p_hw_crc, uint16_t data)
{
    p_hw_crc->wrdata16 = data;
}

/**
 * \brief д��һ����ʮ��λ���ݵ�ֵ��crc������м���
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \param[in] data     : д�����ʮ��λ����
 * \return ��
 */
am_static_inline
void amhw_lpc82x_crc_wrdata32 (amhw_lpc82x_crc_t *p_hw_crc, uint32_t data)
{
    p_hw_crc->wrdata32 = data;
}

/**
 * \brief д��n����λ���ݵ�ֵ��crc������м���
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \param[in] p_data   : Ҫд������ݻ�����(8λ���ݻ�����)
 * \param[in] size     : 8λ���ݵĸ���
 * \return ��
 */
void amhw_lpc82x_crc_data8_write (amhw_lpc82x_crc_t *p_hw_crc,
                                  const uint8_t     *p_data,
                                  uint32_t           size);


/**
 * \brief д��n��ʮ��λ���ݵ�ֵ��crc������м���
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \param[in] p_data   : Ҫд������ݻ�����(16λ���ݻ�����)
 * \param[in] size     : 16λ���ݵĸ���
 * \return ��
 */
void amhw_lpc82x_crc_data16_write (amhw_lpc82x_crc_t *p_hw_crc, 
                                   const uint16_t    *p_data,
                                   uint32_t           size);


/**
 * \brief д��n����ʮ��λ���ݵ�ֵ��crc������м���
 * \param[in] p_hw_crc : crc�Ĵ������ָ��
 * \param[in] p_data   : Ҫд������ݻ�����(32λ���ݻ�����)
 * \param[in] size     : 32λ���ݵĸ���
 * \return ��
 */
void amhw_lpc82x_crc_data32_write (amhw_lpc82x_crc_t *p_hw_crc, 
                                   const uint32_t    *p_data,
                                   uint32_t           size);



/**
 * @} 
 */
/*
 * \brief �����ṹ��εĽ���
 */

#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#elif defined(__TMS470__)
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler t
#endif

#ifdef __cplusplus
}
#endif /* __AMHW_LPC82X_CRC_H */

#endif

/*end of file */
