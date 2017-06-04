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
 * \brief LPC82X CRC �û������ļ� 
 * \sa am_lpc82x_hwconfig_crc.c
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-07-08  sss, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_lpc82x.h"
#include "amhw_lpc82x_clk.h"
#include "am_lpc82x_crc.h"

/**
 * \addtogroup am_lpc82x_if_hwconfig_src_crc
 * \copydoc am_lpc82x_hwconfig_crc.c
 * @{
 */
 
/** \brief CRCƽ̨��ʼ�� */
void __lpc82x_crc_plfm_init (void)
{
    /* ʹ�ܲ���λCRCʱ�� */
    amhw_lpc82x_clk_periph_enable(AMHW_LPC82X_CLK_CRC);
}

/** \brief ���CRCƽ̨��ʼ�� */
void __lpc82x_crc_plfm_deinit (void)
{
    amhw_lpc82x_clk_periph_disable(AMHW_LPC82X_CLK_CRC);
} 

/** \brief CRC �豸��Ϣ */
static const am_lpc82x_crc_devinfo_t __g_crc_devinfo = {
    LPC82X_CRC_BASE,                /**< \brief CRC�Ĵ������ַ */
    
    __lpc82x_crc_plfm_init,         /**< \brief ƽ̨��ʼ�� */
    __lpc82x_crc_plfm_deinit        /**< \brief ƽ̨ȥ��ʼ�� */
};

/** \brief CRC�豸���� */
static am_lpc82x_crc_dev_t __g_crc_dev;


/** \brief CRC ʵ����ʼ�������CRC��׼������ */
am_crc_handle_t am_lpc82x_crc_inst_init (void)
{
    return am_lpc82x_crc_init(&__g_crc_dev, &__g_crc_devinfo);
}

/** \brief CRC ʵ�����ʼ�� */
void am_lpc82x_crc_inst_deinit (am_crc_handle_t handle)
{
    am_lpc82x_crc_deinit(handle);
}

/**
 * @}
 */

/* end of file */
