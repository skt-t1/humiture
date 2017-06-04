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
 * \brief CLK ��׼�ӿ�
 *
 * \internal
 * \par Modification History
 * - 1.00 15-12-01  LNK, first implementation.
 * \endinternal
 */

#ifndef __AM_CLK_H
#define __AM_CLK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_common.h" 

/**
 * \addtogroup am_if_clk
 * \copydoc am_clk.h
 * @{
 */

/** \brief ʱ��ID��������*/
typedef int am_clk_id_t;

/**
 * \brief ʹ��ʱ��
 *
 * \param[in]   clk_id  ʱ�� ID (��ƽ̨����), �μ� \ref grp_clk_id
 *
 * \retval      AM_OK   �ɹ�
 * \retval     -AM_ENXIO   ʱ��Ƶ��ID������
 * \retval     -AM_EIO     ʹ��ʧ��
 */
int am_clk_enable (am_clk_id_t clk_id);

/**
 * \brief ����ʱ��
 *
 * \param[in]   clk_id  ʱ�� ID (��ƽ̨����), �μ� \ref grp_clk_id
 *
 * \retval      AM_OK     �ɹ�
 * \retval     -AM_ENXIO  ʱ��Ƶ��ID������
 * \retval     -AM_EIO       ����ʧ��
 */
int am_clk_disable (am_clk_id_t clk_id);

/**
 * \brief ��ȡʱ��Ƶ��
 *
 * \param[in]   clk_id  ʱ�� ID (��ƽ̨����), �μ� \ref grp_clk_id
 *
 * \retval      0       ʱ��Ƶ��Ϊ0�����ȡʱ��Ƶ��ʧ��
 * \retval      ��ֵ           ʱ��Ƶ��
 */
int am_clk_rate_get (am_clk_id_t clk_id);


/** 
 * @}  
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_CLK_H */

/* end of file */
