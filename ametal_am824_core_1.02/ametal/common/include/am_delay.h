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
 * \brief ��ʱ��׼�ӿ�
 * 
 * �������뼶����ʱ��΢�뼶����ʱ
 * 
 * \internal
 * \par Modification history
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */

#ifndef __AM_DELAY_H
#define __AM_DELAY_H


#ifdef __cplusplus
extern "C" {
#endif

#include "am_types.h"

/**
 * \addtogroup am_if_delay
 * \copydoc am_delay.h
 * @{
 */
     
/** 
 * \brief ���뼶����ʱ
 * \param[in] nms : ��ʱ�ĺ�����
 * \return ��
 */   
void am_mdelay(uint32_t nms);
    
/** 
 * \brief ΢�뼶����ʱ
 * \param[in] nus : ��ʱ��΢����
 * \return ��
 */    
void am_udelay(uint32_t nus);
 
/**
 * @} 
 */
 
#ifdef __cplusplus
}
#endif

#endif /* __AM_DELAY_H */

/* end of file */
