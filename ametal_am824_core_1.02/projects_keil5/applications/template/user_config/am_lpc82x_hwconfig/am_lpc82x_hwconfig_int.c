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
 * \brief LPC82X �ж��û������ļ�
 * \sa am_lpc82x_hwconfig_int.c
 * 
 * \internal
 * \par Modification history
 * - 1.01 15-12-05  cyl, modify the namespace.
 * - 1.00 15-01-29  hbt, first implementation.
 * \endinternal
 */
 
#include "ametal.h"
#include "am_lpc82x.h"
#include "am_lpc82x_int.h"

/**
 * \addtogroup am_lpc82x_if_hwconfig_src_int
 * \copydoc am_lpc82x_hwconfig_int.c
 * @{
 */

/** 
 * \brief ISR ��Ϣ���� 
 *        ������Ĭ�϶���ΪMCU��֧�ֵ���������жϸ�����
 *        �û����Ը���Ӧ����ʵ������Ҫ�õ����жϸ���
 *        ���޸ĸú�ֵ���Դﵽ���ٲ���Ҫ���ڴ��˷ѵ�Ŀ�ġ�
 */
#define __ISRINFO_COUNT    INUM_INTERNAL_COUNT

/**
 * \brief �洢�û��жϻص���Ϣ
 */
static struct am_lpc82x_int_isr_info __int_isr_infor[__ISRINFO_COUNT];

/**
 * \brief �洢�жϱ�������Ӧ��__int_isr_infor�жϻص���Ϣ��
 *        λ�õ�ӳ�䣬�����Сһ����MCU��֧�ֵ���������жϸ�����ȡ�
 */
static uint8_t __int_isr_map[INUM_INTERNAL_COUNT];
 

/** \brief �ж��豸��Ϣ */
static const am_lpc82x_int_devinfo_t __g_int_devinfo =
{
    {
        INUM_INTERNAL_MIN, /**< \brief �ж���ʼ�� */
        INUM_INTERNAL_MAX  /**< \brief �ж�ĩβ�� */
    },                    
                          
    LPC82X_NVIC_BASE,      /**< \brief �жϿ������Ĵ������ַ                     */
    INUM_INTERNAL_COUNT,   /**< \brief ���ж�����                                */
    __int_isr_map,         /**< \brief ISR ��Ϣӳ��(��С�� input_cnt һ��)        */
    __ISRINFO_COUNT,       /**< \brief ISR ��Ϣ����                              */
    __int_isr_infor,       /**< \brief ISR ��Ϣӳ���ڴ�(��С�� isrinfo_cnt һ��)  */
    
    NULL,                  /**< \brief ����ƽ̨��ʼ�� */
    NULL                   /**< \brief ����ƽ̨ȥ��ʼ�� */
};

/** \brief �ж��豸ʵ�� */
static am_lpc82x_int_dev_t __g_int_dev;


/**
 * \brief  �ж�ʵ����ʼ������ʼ���ж�����
 */
int am_lpc82x_int_inst_init (void)
{
    return am_lpc82x_int_init(&__g_int_dev, &__g_int_devinfo);
}

/** 
 * \brief  �ж�ʵ�����ʼ��
 */
void am_lpc82x_int_inst_deinit (void)
{
    am_lpc82x_int_deinit();
}

/**
 * @}
 */

/* end of file */
