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
 * \brief LPC82X SCT0 ���ڲ����ܵ��û������ļ�
 * \sa am_lpc82x_hwconfig_sct0_cap.c
 * 
 * \internal
 * \par Modification history
 * - 1.02 15-11-19  hgo, add annotations.
 * - 1.01 15-09-24  zxl, modified.
 * - 1.00 15-07-07  oce, first implementation.
 * \endinternal
 */
#include "ametal.h"
#include "am_lpc82x.h"
#include "amhw_lpc82x_clk.h"
#include "am_lpc82x_sct_cap.h"

/**
 * \addtogroup am_lpc82x_if_hwconfig_src_sct0_cap
 * \copydoc am_lpc82x_hwconfig_sct0_cap.c
 * @{
 */


/** \brief SCT0����CAP���������������Ϣ�б�  CAP���������Ϊ4��    */
static am_lpc82x_sct_cap_ioinfo_t __g_sct0_cap_ioinfo_list[] = {
    /** \brief ���� 0 */
    {PIO0_25, PIO_FUNC_SCT_PIN0 , PIO0_25_GPIO | PIO0_25_GPIO_INPUT},
    /** \brief ���� 1 */
    {PIO0_26, PIO_FUNC_SCT_PIN1 , PIO0_26_GPIO | PIO0_26_GPIO_INPUT},
    /** \brief ���� 2 */
    {PIO0_27, PIO_FUNC_SCT_PIN2 , PIO0_27_GPIO | PIO0_27_GPIO_INPUT},
    /** \brief ���� 3 */
    {PIO0_28, PIO_FUNC_SCT_PIN3 , PIO0_28_GPIO | PIO0_28_GPIO_INPUT},
};

/** \brief SCT0���ڲ����ܵ�ƽ̨��ʼ�� */
static void __lpc82x_sct0_cap_plfm_init ()
{

    /**
     * \brief һ����4������ͨ��, ÿ��ͨ����ѡ������빦�ܴﵽ8��,
     *        ��������й�����amhw_lpc82x_inmux.h�ļ��й���SCT�Ķ��塣
     *        ����ֻ����һ����õ���, �����ͨ��am_lpc82x_sct_cap_set_input
     *        �������������
     */
    am_lpc82x_sct_cap_set_input(0, AMHW_LPC82X_INMUX_SCTTRIG_SCT_PIN0);
    am_lpc82x_sct_cap_set_input(1, AMHW_LPC82X_INMUX_SCTTRIG_SCT_PIN1);
    am_lpc82x_sct_cap_set_input(2, AMHW_LPC82X_INMUX_SCTTRIG_SCT_PIN2);
    am_lpc82x_sct_cap_set_input(3, AMHW_LPC82X_INMUX_SCTTRIG_SCT_PIN3);

    amhw_lpc82x_clk_periph_enable(AMHW_LPC82X_CLK_SCT);
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_SCT);
}

/** \brief  ���SCT0ƽ̨��ʼ�� */
static void __lpc82x_sct0_cap_plfm_deinit (void)
{
    amhw_lpc82x_syscon_periph_reset(AMHW_LPC82X_RESET_SCT);
    amhw_lpc82x_clk_periph_disable(AMHW_LPC82X_CLK_SCT);
}

/** \brief SCT0���ڲ����ܵ��豸��Ϣ */
static const am_lpc82x_sct_cap_devinfo_t  __g_sct0_cap_devinfo = {
    LPC82X_SCT0_BASE,            /**< \brief SCT0�Ĵ�����Ļ���ַ */
    INUM_SCT0,                   /**< \brief SCT0�жϱ��           */
    4,                           /**< \brief 4������ͨ��            */
    &__g_sct0_cap_ioinfo_list[0],/**< \brief ����PWM����������Ϣ�����׵�ַ�������� */

    __lpc82x_sct0_cap_plfm_init,   /**< \brief ƽ̨��ʼ������         */
    __lpc82x_sct0_cap_plfm_deinit, /**< \brief ƽ̨���ʼ������       */
};

/** \brief SCT0���ڲ����ܵ��豸���� */
static am_lpc82x_sct_cap_dev_t  __g_sct0_cap_dev;

/** \brief SCT0 CAP ʵ����ʼ�������CAP��׼������ */
am_cap_handle_t am_lpc82x_sct0_cap_inst_init (void)
{
    return am_lpc82x_sct_cap_init(&__g_sct0_cap_dev, &__g_sct0_cap_devinfo);
}

/** \brief SCT0 CAP ʵ�����ʼ�� */
void am_lpc82x_sct0_cap_inst_deinit (am_cap_handle_t handle)
{
    am_lpc82x_sct_cap_deinit(handle);
}
/**
 * @}
 */

/* end of file */
