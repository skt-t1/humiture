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
 * \brief �¶ȴ�����LM75����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10  skt, first implementation.
 * \endinternal
 */
 
#ifndef __AM_TEMP_LM75_H
#define __AM_TEMP_LM75_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "am_temp.h"
#include "am_i2c.h"
    
/** 
 * \brief �豸��Ϣ�ṹ��
 */
struct am_temp_lm75_devinfo {
    
    /** \brief ������Ϣ */
    struct am_temp_servinfo info;
    
    /** \brief �豸I2C��ַ */
    int addr;
    
    /** \brief ƽ̨��ʼ������ */
    void (*pfn_plfm_init) (am_i2c_handle_t *p_retval);
    
};
    
/**
 * \brief �豸�ṹ��
 */
struct am_temp_lm75_dev {
    
    am_i2c_device_t i2c;
    
};
    
/**
 * \brief �豸��ʼ������
 */
void am_temp_lm75_init (struct am_temp_lm75_dev           *p_dev,
                        const struct am_temp_lm75_devinfo *p_devinfo);
    
#ifdef __cplusplus
}
#endif

#endif /* __AM_TEMP_LM75_H */

/* end of file */
