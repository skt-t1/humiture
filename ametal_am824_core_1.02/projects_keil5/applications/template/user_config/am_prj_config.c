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
 * \brief 工程配置文件 
 * \sa am_prj_config.c
 * 
 * \internal
 * \par Modification history
 * - 1.00 15-07-07  sss, first implementation.
 * \endinternal
 */

#include "ametal.h"
#include "am_prj_config.h"
#include "am_lpc82x_inst_init.h"
#include "am_board.h"

/**
 * \addtogroup am_prj_config
 * \copydoc am_prj_config.c
 * @{
 */

extern int am_main (void);


int main (void)
{
    
#if (AM_CFG_CLK_ENABLE == 1)
    am_lpc82x_clk_inst_init();
#endif

#if (AM_CFG_GPIO_ENABLE == 1)
    am_lpc82x_gpio_inst_init();
#endif
    
#if (AM_CFG_INT_ENABLE == 1)
    am_lpc82x_int_inst_init();
#endif
    
#if (AM_CFG_DMA_ENABLE == 1)
    am_lpc82x_dma_inst_init();
#endif

    /* 板级初始化 */
    am_board_init();
    
    return am_main();
}

/**
 * @}
 */

/* end of file */
