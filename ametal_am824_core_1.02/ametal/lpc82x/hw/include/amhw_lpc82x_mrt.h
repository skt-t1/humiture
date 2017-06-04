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
 * \brief MRT(Multi-Rate timer) Ӳ�������ӿ�
 *
 * 1. 31λ�ж϶�ʱ����
 * 2. 4�����������¼���ͨ�����ɷֱ����ö�����ֵ��
 * 3. ÿ��ͨ��֧���ظ��ж�ģʽ��һ�μ�ʱģʽ��һ�μ�ʱ����ͣ��ģʽ��
 *
 * \internal
 * \par Modification history
 * - 1.02 15-12-14  hgo, improve exec efficiency.
 * - 1.01 15-12-02  mem, modified.
 * - 1.00 15-07-01  win, first implementation.
 * \endinternal
 */
 
#ifndef __AMHW_LPC82X_MRT_H
#define __AMHW_LPC82X_MRT_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup amhw_lpc82x_if_mrt
 * \copydoc amhw_lpc82x_mrt.h
 * @{
 */

/**
 * \brief MRT ͨ����
 */
#define AMHW_LPC82X_MRT_CHAN_NUM          4
    
/** \brief ͨ������ʱֵ */
#define AMHW_LPC82X_MRT_INVAL_MAX     (0x7FFFFFFFUL)

/**
 * \brief MRT ͨ���Ĵ�����ṹ
 */
typedef struct amhw_lpc82x_mrt_chan {
    __IO uint32_t intval;                /**< \brief ���ʱ�����üĴ���   */
    __I  uint32_t timer;                 /**< \brief ��ǰͨ������ֵ�Ĵ��� */
    __IO uint32_t ctrl;                  /**< \brief ��ǰͨ�����ƼĴ���   */
    __IO uint32_t stat;                  /**< \brief ��ǰͨ��״̬�Ĵ���   */
} amhw_lpc82x_mrt_chan_t;

/**
 * \brief MRT �Ĵ�����ṹ��
 */
typedef struct amhw_lpc82x_mrt {
    
    /** \brief ͨ���Ĵ�����   */
    amhw_lpc82x_mrt_chan_t channel[AMHW_LPC82X_MRT_CHAN_NUM];
    
    /** \brief ��������ʹ��   */
    __I  uint32_t   reserved0[45];
    
    /** \brief ͨ�����мĴ��� */
    __I  uint32_t   idle_ch;
    
    /** \brief �жϱ�־�Ĵ��� */    
    __IO uint32_t   irq_flag;     
    
} amhw_lpc82x_mrt_t;

/**
 * \name MRT ͨ�����
 * \anchor grp_amhw_lpc82x_mrt_chan
 * @{
 */

#define AMHW_LPC82X_MRT_CHAN_0     0     /**< \brief ͨ��0                */ 
#define AMHW_LPC82X_MRT_CHAN_1     1     /**< \brief ͨ��1                */ 
#define AMHW_LPC82X_MRT_CHAN_2     2     /**< \brief ͨ��2                */ 
#define AMHW_LPC82X_MRT_CHAN_3     3     /**< \brief ͨ��3                */ 

/** @} */

/**
 * \brief MRT ͨ��ģʽ����
 */
typedef enum amhw_lpc82x_mrt_mode{
    AMHW_LPC82X_MRT_MODE_REPEAT = 0,     /**< \brief �ظ���ʱ�ж�ģʽ     */
    AMHW_LPC82X_MRT_MODE_ONESHOT,        /**< \brief һ�ζ�ʱ�ж�ģʽ     */
    AMHW_LPC82X_MRT_MODE_ONESHOT_STALL   /**< \brief һ�ζ�ʱ����ͣ��ģʽ */
} amhw_lpc82x_mrt_mode_t;                            

/**
 * \brief ����ָ��ͨ���Ķ�ʱֵ
 * 
 *  ��ֵ�ᱻת�ؽ�timer(��ǰͨ������ֵֵ�Ĵ���)�У�ͨ������� 
 *  ����ֵ - 1 ��ʼ�ݼ����� 
 * 
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 * \param[in] value    : ��ʱֵ(��31λ��Ч)
 * \param[in] forced   : �Ƿ��������붨ʱֵ
 *      \arg TRUE      : �������붨ʱֵ
 *      \arg FALSE     : �¸���ʱ�����������붨ʱֵ
 *
 * \note ��������£����۲���forcedΪ TRUE ���� FALSE����ʱ�������������µĶ�ʱֵ��
 *       - ��ͨ������ #AMHW_LPC82X_MRT_MODE_ONESHOT_STALL ģʽʱ������forced��Ч��
 *       - ��ͨ�����ڿ���״̬��ʱ������forced��Ч��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_mrt_intval_set (amhw_lpc82x_mrt_t *p_hw_mrt,
                                 uint8_t            chan,
                                 uint32_t           value,
                                 bool_t             forced)
{
    value &= AMHW_LPC82X_MRT_INVAL_MAX;
    value = (forced) ? (value | AM_BIT(31)) : value;
    p_hw_mrt->channel[chan].intval = value;
}

/**
 * \brief ��ȡָ��ͨ���Ķ�ʱֵ
 *
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return ͨ���Ķ�ʱֵ(INTVAL)
 */
am_static_inline
uint32_t amhw_lpc82x_mrt_intval_get (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    return (p_hw_mrt->channel[chan].intval & AMHW_LPC82X_MRT_INVAL_MAX);
}

/**
 * \brief ��ȡָ��ͨ����ǰ��������ֵ
 * 
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return ͨ����ǰ��������ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_mrt_count_get (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    return p_hw_mrt->channel[chan].timer;
}

/**
 * \brief ʹ��ָ��ͨ�����ж�
 *
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_mrt_int_enable (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    AM_BIT_SET(p_hw_mrt->channel[chan].ctrl, 0);
}

/**
 * \brief ����ָ��ͨ�����ж�
 * 
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_mrt_int_disable (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    AM_BIT_CLR(p_hw_mrt->channel[chan].ctrl, 0);
}
    
/**
 * \brief ����ָ��ͨ���Ķ�ʱģʽ
 * 
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 * \param[in] mode     : ��ʱ��ͨ��ģʽ
 * 
 * \return ��
 */
am_static_inline
void amhw_lpc82x_mrt_mode_set (amhw_lpc82x_mrt_t     *p_hw_mrt, 
                               uint8_t                chan,
                               amhw_lpc82x_mrt_mode_t mode)
{
    AM_BITS_SET(p_hw_mrt->channel[chan].ctrl, 1, 2, mode);
}

/**
 * \brief ��ȡָ��ͨ���Ķ�ʱģʽ
 *
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return ͨ����ʱģʽ
 */
am_static_inline
amhw_lpc82x_mrt_mode_t amhw_lpc82x_mrt_mode_get (amhw_lpc82x_mrt_t *p_hw_mrt, 
                                                 uint8_t chan)
{
    return (amhw_lpc82x_mrt_mode_t)AM_BITS_GET(p_hw_mrt->channel[chan].ctrl, 
                                               1, 
                                               2);
}

/**
 * \brief �ж�ָ��ͨ�����ж��Ƿ����
 *
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 * 
 * \retval TRUE  : �жϹ���
 * \retval FALSE : �ж�δ����
 */
am_static_inline
bool_t amhw_lpc82x_mrt_is_pending (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    return (AM_BIT_ISSET(p_hw_mrt->channel[chan].stat, 0) ? TRUE : FALSE);
}

/**
 * \brief ����ָ��ͨ�����жϹ���״̬
 * 
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ����ţ�AMHW_LPC82X_MRT_CHAN_*(#AMHW_LPC82X_MRT_CHAN_0)
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_mrt_int_pending_clr (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    p_hw_mrt->channel[chan].stat = AM_BIT(0);
}

/**
 * \brief �ж�ָ��ͨ���Ƿ��ڿ���״̬
 *
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] chan     : ͨ��ID��ʹ�ú�#AMHW_LPC82X_MRT_CH(n)
 *
 * \retval TRUE  : ����״̬
 * \retval FALSE : ����״̬
 */
am_static_inline
bool_t amhw_lpc82x_mrt_is_idle (amhw_lpc82x_mrt_t *p_hw_mrt, uint8_t chan)
{
    return (AM_BIT_ISSET(p_hw_mrt->channel[chan].stat, 1) ? FALSE : TRUE);
}

/**
 * \brief ��ȡ��ǰ����ͨ���У�ͨ������С��ͨ��
 *
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 *
 * \return ��С����ͨ����
 */
am_static_inline
uint8_t amhw_lpc82x_mrt_idle_channel_get (amhw_lpc82x_mrt_t *p_hw_mrt)
{
    return (uint8_t)((p_hw_mrt->idle_ch) >> 4);
}

/**
 * \brief ���ͨ�����жϹ���״̬
 *
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \param[in] masks    : ͨ�����룬bit-n��Ӧͨ��n��Ϊ1��λ�жϹ���״̬�����
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_mrt_int_pending_maks_clr (amhw_lpc82x_mrt_t *p_hw_mrt, 
                                           uint8_t            masks)
{
    p_hw_mrt->irq_flag = masks;
}

/**
 * \brief ��ȡ����ͨ�����ж�״̬
 * \param[in] p_hw_mrt : ָ��MRT�Ĵ������ָ��
 * \return �жϹ���״̬��bit-n��Ӧͨ��n
 */
am_static_inline
uint8_t amhw_lpc82x_mrt_int_pending_get (amhw_lpc82x_mrt_t *p_hw_mrt)
{
    return p_hw_mrt->irq_flag & 0x0F;
}

/**
 * @} amhw_lpc82x_if_mrt
 */
 
#ifdef __cplusplus
}
#endif

#endif  /* __AMHW_LPC82X_MRT_H */

/* end of file */
