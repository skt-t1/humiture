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
 * \brief �¶ȴ�����LM75�����ļ�
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10  skt, first implementation.
 * \endinternal
 */

#include "am_lpc82x_inst_init.h"
#include "am_temp_lm75.h"

/* ƽ̨��ʼ�� */
am_local
void __temp_lm75_plfm_init (am_i2c_handle_t *p_retval)
{
    if (p_retval == NULL) {
        return ;
    }
    *p_retval = am_lpc82x_i2c0_inst_init();
}

/* LM75�豸��Ϣ�ṹ�� */
am_local am_const struct am_temp_lm75_devinfo __g_lm75_0_devinfo = {
    {
        0,
    },
    0x48,
    __temp_lm75_plfm_init
};

/* LM75�豸�ṹ�� */
am_local struct am_temp_lm75_dev __g_lm75_0_dev;

/* LM75��ʼ�� */
void am_lpc82x_temp_lm75_init (void)
{
    am_temp_lm75_init(&__g_lm75_0_dev, &__g_lm75_0_devinfo);
}
 
/* end of file */
