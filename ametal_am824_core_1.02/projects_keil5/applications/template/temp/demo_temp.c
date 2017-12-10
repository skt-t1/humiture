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
 * \brief 温度传感器LM75驱动
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10  skt, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_vdebug.h"
#include "am_temp.h"

void demo_temp_entry (void)
{
    am_temp_info_t info;
    int value;
    
    am_temp_info_get(0, &info);
    am_temp_read(0, &value);
    AM_DBG_INFO("current tempture is %.2f\n", value / (float)info.multiple);
}
 
/* end of file */
