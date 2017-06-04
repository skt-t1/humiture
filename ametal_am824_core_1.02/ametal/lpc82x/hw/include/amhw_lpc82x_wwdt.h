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
 * \brief ���ڿ��Ź�Ӳ������������
 * 
 * 1. ����ڿɱ�̳�ʱ������δ�������룬������ڲ���λоƬ��
 * 2. ��ѡ�Ĵ��ڻ�����Ҫ����������С��ʱ����֮�䷢���������룬����ʱ�޶����趨��
 * 3. ��ѡ�����жϿ��ڿ��Ź���ʱ֮ǰ�Ŀɱ��ʱ�����ɡ�
 * 4. ���ڲ��̶�Ԥ��Ƶ���Ŀɱ�� 24 λ��ʱ����
 * 5. ʱ�����ڿ�ѡ���� 1,024 �����Ź�ʱ�� (TWDCLK �� 256 �� 4) ������ 6,700 ������Ź�
 *    ʱ�� (TWDCLK �� 224 �� 4)��ȡ 4 �����Ź�ʱ�ӵ�λ����
 * 6.�� ��ȫ �� ���Ź�������һ��ʹ�ܣ���Ҫ����������λ���Ź���λ��
 * 7. ι�����в���ȷ�ᵼ�¿��Ź���ʱ�¼� ����ʹ�ܣ���
 * 8. ���ڶԿ��Ź�����ֵ�ı���Ϊѡ���ԣ����ֻ���� �� �����ж� �� ʱ�����ĸ�ֵ��
 * 9. ָʾ���Ź���λ�ı�־��
 * 9. ���Ź�ʱ�� (WDCLK) Դ�ǿ��Ź�������
 * 10. ���Ź���ʱ��������Ϊ�����˯��ģʽ�����ģʽ�����С�
 * 11. ����ģʽ��
 *
 * \internal
 * \par Modification history
 * - 1.01 15-12-14  hgo, del amhw_lpc82x_wwdt_option_clr() .
 * - 1.00 15-07-07  sss, first implementation.
 * \endinternal
 */
#ifndef __AMHW_LPC82X_WWDT_H
#define __AMHW_LPC82X_WWDT_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup amhw_lpc82x_if_wwdt
 * \copydoc amhw_lpc82x_wwdt.h
 * @{
 */
 
/**
 * \brief WWDT�Ĵ�����Ľṹ��
 */
typedef struct amhw_lpc82x_wwdt {
    __IO uint32_t  mod;         /**< \brief ���Ź���ʱ����ģʽ�Ĵ���*/
    __IO uint32_t  tc;          /**< \brief ι����װ��������е�ֵ*/
    __O  uint32_t  feed;        /**< \brief ι�����мĴ���(0xAA + 0x55)*/
    __I  uint32_t  tv;          /**< \brief ���Ź�������ֵ����24λ��Ч*/
    __I  uint32_t  reserved0;   /**< \brief ����*/
    __IO uint32_t  warnint;     /**< \brief ���Ź��жϱȽ���ֵ*/
    __IO uint32_t  window;      /**< \brief ���Ź����ڱȽ���ֵ*/
} amhw_lpc82x_wwdt_t;

/**
 * \name ���Ź�ģʽ����λ����
 * \anchor grp_wwdt_mode
 * @{
 */

/** \brief ���Ź�ģʽλ����*/
#define AMHW_LPC82X_WWDT_WDMOD_BITMASK     ((uint32_t)0x3F)     

/** \brief ���Ź�ʹ��λ*/
#define AMHW_LPC82X_WWDT_WDMOD_WDEN        AM_BIT(0)           

/** \brief ���Ź���λλ*/
#define AMHW_LPC82X_WWDT_WDMOD_WDRESET     AM_BIT(1)            

/** \brief ���Ź���ʱ��־λ*/
#define AMHW_LPC82X_WWDT_WDMOD_WDTOF       AM_BIT(2)            

/** \brief ���Ź��жϱ�־λ*/
#define AMHW_LPC82X_WWDT_WDMOD_WDINT       AM_BIT(3)            

/** \brief ���Ź�����ģʽλ*/
#define AMHW_LPC82X_WWDT_WDMOD_WDPROTECT   AM_BIT(4)       
     
/** \brief ���Ź�����ʱ��Դλ*/
#define AMHW_LPC82X_WWDT_WDMOD_LOCK        AM_BIT(5)            

/** @} */


/**
 * \brief ��ʼ��WWDT
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_wwdt_init (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    /* ��ֹ���Ź� */ 
    p_hw_wwdt->mod     = 0;
    p_hw_wwdt->tc      = 0xFF;
    p_hw_wwdt->warnint = 0x0;
    p_hw_wwdt->window  = 0xFFFFFF;
}

/**
 * \brief ���ÿ��Ź��ĳ�ʱʱ�䣬׼��ι��
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 * \param[in] timeout   : ���ó�ʱʱ���ֵ�� ��Χ0xFF~0xFFFFFF
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_wwdt_timeout_set (amhw_lpc82x_wwdt_t *p_hw_wwdt,
                                   uint32_t            timeout)
{
    p_hw_wwdt->tc = timeout;
}

/**
 * \brief ���Ź�ι������
 * 
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 *
 * \return ��
 *
 * \note �������������ι������ǰ���ر�ϵͳ�ж�
 */
am_static_inline
void amhw_lpc82x_wwdt_feed (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    p_hw_wwdt->feed = 0xAA;
    p_hw_wwdt->feed = 0x55;
}

/**
 * \brief ���ÿ��Ź�������ֵ
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 * \param[in] timeout   : ���Ź���ʱʱ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_wwdt_warning_set (amhw_lpc82x_wwdt_t *p_hw_wwdt,
                                   uint32_t            timeout)
{
    p_hw_wwdt->warnint = timeout;
}

/**
 * \brief ��ȡ���Ź��жϼĴ�����ֵ
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 *
 * \return ��ǰ�����жϼĴ�����ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_wwdt_warning_get (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    return p_hw_wwdt->warnint;
}

/**
 * \brief ���ÿ��Ź����ڶ�ʱ����ֵ
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 * \param[in] timeout   : ���ڶ�ʱ����ֵ
 *
 * \return ��
 *
 * \note ������ֵ����windowʱ��ι��������һ�����Ź��¼�
 */
am_static_inline
void amhw_lpc82x_wwdt_window_set (amhw_lpc82x_wwdt_t *p_hw_wwdt, 
                                  uint32_t            timeout)
{
    p_hw_wwdt->window = timeout;
}

/**
 * \brief ��ȡ���Ź���ʱ�����ڵ�ֵ
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 *
 * \return ��ǰ���ڼĴ�����ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_wwdt_window_get (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    return p_hw_wwdt->window;
}

/**
 * \brief ʹ�ܿ��Ź���ʱ��ѡ��
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 * \param[in] options   : ʹ�ܿ��Ź���ѡ��
 *                        #AMHW_LPC82X_WWDT_WDMOD_WDEN    ��
 *                        #AMHW_LPC82X_WWDT_WDMOD_WDRESET ��
 *                        #AMHW_LPC82X_WWDT_WDMOD_WDPROTECT ��
 *                        #AMHW_LPC82X_WWDT_WDMOD_LOCK
 *                        ���ĸ���Ļ�ֵ
 * \return ��
 *
 * \note ʹ��һ���������ɽ���־λ�����
 */
am_static_inline
void amhw_lpc82x_wwdt_option_set (amhw_lpc82x_wwdt_t *p_hw_wwdt, 
                                  uint32_t            options)
{
    p_hw_wwdt->mod = (p_hw_wwdt->mod & AMHW_LPC82X_WWDT_WDMOD_BITMASK) | 
                      options;
}

/**
 * \brief ʹ�ܿ��Ź�
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_wwdt_start (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    amhw_lpc82x_wwdt_option_set(p_hw_wwdt,AMHW_LPC82X_WWDT_WDMOD_WDEN);
    amhw_lpc82x_wwdt_feed(p_hw_wwdt);
}

/**
 * \brief ��ȡ���Ź�״̬��־λ
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 * \param[in] flag      : ��־λ��#AMHW_LPC82X_WWDT_WDMOD_WDINT ��
 *                               #AMHW_LPC82X_WWDT_WDMOD_WDTOF
 *
 * \return ��־λ
 */
am_static_inline
bool_t amhw_lpc82x_wwdt_status_get (amhw_lpc82x_wwdt_t *p_hw_wwdt, 
                                    uint32_t            flag)
{
    return (bool_t) (p_hw_wwdt->mod & flag);
}

/**
 * \brief ������Ź�״̬��־λ
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 * \param[in] flags     : ��־λ��#AMHW_LPC82X_WWDT_WDMOD_WDINT ��
 *                               #AMHW_LPC82X_WWDT_WDMOD_WDTOF
 *
 * \return ��
 */
am_static_inline
void amhw_lpc82x_wwdt_status_clr (amhw_lpc82x_wwdt_t *p_hw_wwdt, 
                                  uint32_t            flags)
{
    if (flags & AMHW_LPC82X_WWDT_WDMOD_WDTOF) {
        p_hw_wwdt->mod &= (~AMHW_LPC82X_WWDT_WDMOD_WDTOF) &
                            AMHW_LPC82X_WWDT_WDMOD_BITMASK;
    }
    
    /* д1������жϱ�־λ */
    if (flags & AMHW_LPC82X_WWDT_WDMOD_WDINT) {
        p_hw_wwdt->mod = (p_hw_wwdt->mod & AMHW_LPC82X_WWDT_WDMOD_BITMASK) |
                          AMHW_LPC82X_WWDT_WDMOD_WDINT;
    }
}

/**
 * \brief ��ȡ���Ź���������ֵ
 *
 * \param[in] p_hw_wwdt : ָ���Ź���ʱ���Ĵ������ָ��
 *
 * \return ��ǰ���Ź��ļ�������ֵ
 */
am_static_inline
uint32_t amhw_lpc82x_wwdt_currentcount_get (amhw_lpc82x_wwdt_t *p_hw_wwdt)
{
    return p_hw_wwdt->tv;
}

/**
 * @} 
 */
 
#ifdef __cplusplus
}
#endif /* __AMHW_LPC82X_WWDT_H */

#endif

/*end of file */

