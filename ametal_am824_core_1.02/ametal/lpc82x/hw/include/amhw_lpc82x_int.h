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
 * \brief ��Ƕ�������жϿ�����(NVIC) Ӳ�������ӿ�
 *
 * 1. ֧��32���ж�������
 * 2. 4���ɱ�����ȼ�(��Ӳ�����ȼ����ι���)��
 * 3. ��ARM�쳣(SVCall��PendSV)��������жϣ�
 * 4. ֧��NMI��
 *
 * \internal
 * \par Modification history
 * - 1.02 15-12-05  cyl, modify the namespace.
 * - 1.01 15-09-23  win, modify amhw_lpc82x_int_set_priority().
 * - 1.00 14-12-04  hbt, first implementation.
 * \endinternal
 */


#ifndef __AMHW_LPC82X_INT_H
#define __AMHW_LPC82X_INT_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * \addtogroup amhw_lpc82x_if_int
 * \copydoc amhw_lpc82x_int.h
 * @{
 */

/**
 * \brief NVIC �Ĵ�����ṹ��(����LPC5410x��NVIC�Ĵ�����ṹ�嶨��)
 */
typedef struct amhw_lpc82x_int {
    __IO uint32_t  iser[2];                    /**< \brief �ж�ʹ�ܼĴ���       */
    __I  uint32_t  reserved1[30];              /**< \brief ����                 */
    __IO uint32_t  icer[2];                    /**< \brief ����ж�ʹ�ܼĴ���   */   
    __I  uint32_t  reserved2[30];              /**< \brief ����                 */
    __IO uint32_t  ispr[2];                    /**< \brief �жϵȴ��Ĵ���       */ 
    __I  uint32_t  reserved3[30];              /**< \brief ����                 */
    __IO uint32_t  icpr[2];                    /**< \brief ����жϵȴ��Ĵ���   */  
    __I  uint32_t  reserved4[30];              /**< \brief ����                 */
    __I  uint32_t  iabr[2];                    /**< \brief �ж���Ч�Ĵ���       */ 
    __I  uint32_t  reserved5[62];              /**< \brief ����                 */
    __IO uint32_t  ipr[11];                    /**< \brief �ж����ȼ��Ĵ���     */
    __I  uint32_t  reserved6[566];             /**< \brief ����                 */
    __I  uint32_t  icsr;                       /**< \brief �жϿ��ƺ�״̬�Ĵ��� */
    __I  uint32_t  reserved7[126];             /**< \brief ����                 */
    __O  uint32_t  stir;                       /**< \brief ��������жϼĴ���   */
} amhw_lpc82x_int_t;

  
/**
 * \brief ��ȡ��ǰ�жϺ�
 * \param[in] p_hw_int : ָ��NVIC�Ĵ������ָ��
 * \return �жϺ�
 */
am_static_inline 
int amhw_inum_get (amhw_lpc82x_int_t *p_hw_int)
{
    return (int)(AM_BITS_GET((p_hw_int->icsr - 16), 0, 8));
}

/**
 * \brief ʹ���ж�
 *
 * \param[in] p_hw_int : ָ��NVIC�Ĵ������ָ��
 * \param[in] inum     : �жϺţ� �μ� 
 *                       \sa lpc8xx_inum.h
 *
 * \return ��
 */
am_static_inline 
void amhw_lpc82x_int_enable (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    p_hw_int->iser[(uint32_t)(inum) >> 5] = AM_BIT(((uint32_t)(inum) & 0x1F));
}

/**
 * \brief �����ж�
 *
 * \param[in] p_hw_int : ָ��NVIC�Ĵ������ָ��
 * \param[in] inum     : �жϺţ� �μ� 
 *                       \sa lpc8xx_inum.h
 *
 * \return ��
 */
am_static_inline 
void amhw_lpc82x_int_disable (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    p_hw_int->icer[(uint32_t)(inum) >> 5] = AM_BIT(((uint32_t)(inum) & 0x1F));
}

/**
 * \brief ��λ�жϵȴ�
 *
 * \param[in] p_hw_int : ָ��NVIC�Ĵ������ָ��
 * \param[in] inum     : �жϺţ� �μ� 
 *                       \sa lpc8xx_inum.h
 *
 * \return    ��
 */
am_static_inline 
void amhw_lpc82x_int_pending_set (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    p_hw_int->ispr[(uint32_t)(inum) >> 5] = AM_BIT(((uint32_t)(inum) & 0x1F));
}

/**
 * \brief ����ȴ��ж�
 *
 * \param[in] p_hw_int : ָ��NVIC�Ĵ������ָ��
 * \param[in] inum     : �жϺţ� �μ� 
 *                       \sa lpc8xx_inum.h
 *
 * \return    ��
 */
am_static_inline 
void amhw_lpc82x_int_pending_clr (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    p_hw_int->icpr[(uint32_t)(inum) >> 5] = AM_BIT(((uint32_t)(inum) & 0x1F));
}

/**
 * \brief ��ȡ�ж���Ч״̬
 *
 * \param[in] p_hw_int : ָ��NVIC�Ĵ������ָ��
 * \param[in] inum     : �жϺţ� �μ� 
 *                       \sa lpc8xx_inum.h
 * 
 * \retval TRUE  : ��Ч�ж�
 * \retval FALSE : ��Ч�ж�
 */
am_static_inline 
bool_t amhw_lpc82x_int_active_state_get (amhw_lpc82x_int_t *p_hw_int, int inum)
{
    return (AM_BIT_ISSET(p_hw_int->iabr[(uint32_t)(inum) >> 5], 
                         ((uint32_t)(inum) & 0x1F))) ? TRUE : FALSE;
}

/**
 * \brief �����ж����ȼ�
 *
 * \param[in] p_hw_int : ָ��NVIC�Ĵ������ָ��
 * \param[in] inum     : �жϺţ� �μ� 
 *                       \sa lpc8xx_inum.h
 * \param[in] prio     : �ж����ȼ�
 * 
 * \return   ��
 */
am_static_inline 
void amhw_lpc82x_int_priority_set (amhw_lpc82x_int_t *p_hw_int,
                                   int                inum, 
                                   uint32_t           prio)
{
    /* num = inum / 4 */
    uint32_t num   = inum >> 2;
    /* value = (inum % 4) * 8 + 6 */
    uint32_t value = ((inum & 0x3) << 3) + 6;

    AM_BIT_CLR_MASK(p_hw_int->ipr[num], AM_SBF(0x3, value));
    p_hw_int->ipr[num] |= AM_SBF((prio & 0x3), value);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AMHW_LPC82X_INT_H */

/* end of file */

