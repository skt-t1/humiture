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
 * \brief SCT驱动，服务CAP标准接口
 *
 * 1. SCT支持提供如下三种标准服务，本驱动提供的是服务CAP标准服务的驱动。
 *     - 定时；
 *     - PWM输出；
 *     - 捕获。
 * 2. 在本驱动中，SCT相关资源使用如下：
 *    - 捕获通道0用于捕获SCT输入0，与事件0相关联，使用捕获寄存器0，以产生中断；
 *    - 捕获通道1用于捕获SCT输入1，与事件1相关联，使用捕获寄存器1，以产生中断；
 *    - 捕获通道2用于捕获SCT输入2，与事件2相关联，使用捕获寄存器2，以产生中断；
 *    - 捕获通道3用于捕获SCT输入3，与事件3相关联，使用捕获寄存器3，以产生中断。
 *
 * \internal
 * \par Modification history
 * - 1.01 15-09-24  zxl, modified.
 * - 1.00 15-07-07  oce, first implementation.
 * \endinternal
 */

#ifndef __AM_LPC82X_SCT_CAP_H
#define __AM_LPC82X_SCT_CAP_H

#include "ametal.h"
#include "am_cap.h"
#include "amhw_lpc82x_inmux.h"
#include "am_lpc82x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup am_lpc82x_if_sct_cap
 * \copydoc am_lpc82x_sct_cap.h
 * @{
 */

/**
 * \brief SCT CAP输入功能相关的GPIO信息
 */
typedef struct am_lpc82x_sct_cap_ioinfo {
    uint32_t gpio;           /**< \brief GPIO引脚           */
    uint32_t func;           /**< \brief GPIO功能           */
    uint32_t dfunc;          /**< \brief 取消功能后默认功能 */
} am_lpc82x_sct_cap_ioinfo_t;


/**
 * \brief SCT捕获功能相关的设备信息
 */
typedef struct am_lpc82x_sct_cap_devinfo {
    uint32_t                     sct_regbase;  /**< \brief SCT寄存器块的基地址 */
    uint8_t                      inum;         /**< \brief SCT对应的中断号     */
    uint8_t                      channels_num; /**< \brief 使用的通道数，最大为4*/
    am_lpc82x_sct_cap_ioinfo_t  *p_ioinfo;     /**< \brief CAP所用到的捕获引脚    */

    /** \brief 平台初始化函数，如打开时钟，配置引脚等工作 */
    void     (*pfn_plfm_init)(void);

    /** \brief 平台解初始化函数 */
    void     (*pfn_plfm_deinit)(void);
}am_lpc82x_sct_cap_devinfo_t;

/**
 * \brief SCT捕获功能设备
 */
typedef struct am_lpc82x_sct_cap_dev {
    am_cap_serv_t          cap_serv;       /**< \brief 标准CAP服务                 */

    struct {
        am_cap_callback_t  callback_func;  /**< \brief 回调函数                    */
        void              *p_arg;          /**< \brief 回调函数的用户参数          */
    } callback_info[4];                    /**< \brief 存储最大4通道的回调函数信息 */

    /** \brief 指向SCT(捕获功能)设备信息常量的指针 */
    const am_lpc82x_sct_cap_devinfo_t *p_devinfo;

} am_lpc82x_sct_cap_dev_t;

/**
 * \brief 初始化SCT为捕获功能
 *
 * \param[in] p_dev     : 指向SCT(捕获功能)设备的指针
 * \param[in] p_devinfo : 指向SCT(捕获功能)设备信息常量的指针
 *
 * \return CAP标准服务操作句柄，值为NULL时表明初始化失败
 */
am_cap_handle_t am_lpc82x_sct_cap_init(am_lpc82x_sct_cap_dev_t     *p_dev,
                                 const am_lpc82x_sct_cap_devinfo_t *p_devinfo);


/**
 * \brief 不使用SCT捕获功能时，解初始化SCT捕获功能，释放相关资源
 *
 * \param[in] handle :  am_lpc82x_sct_cap_init() 初始化函数获得的CAP服务句柄
 * \return 无
 */
void am_lpc82x_sct_cap_deinit(am_cap_handle_t handle);


#define AM_LPC82X_SCT_CAP_INPUT_CHAN0       0   /**< \brief SCT选择输入源通道0  */
#define AM_LPC82X_SCT_CAP_INPUT_CHAN1       1   /**< \brief SCT选择输入源通道1  */
#define AM_LPC82X_SCT_CAP_INPUT_CHAN2       2   /**< \brief SCT选择输入源通道2  */
#define AM_LPC82X_SCT_CAP_INPUT_CHAN3       3   /**< \brief SCT选择输入源通道3  */

/**
 * \brief 修改抓取的选择输入源
 * \param[in] chan  SCT选择输入源通道 可选择的值为AM_LPC82X_SCT_CAP_INPUT_CHAN*
 * \param[in] input_src   具体的所有功能在amhw_lpc82x_inmux.h文件中关于SCT的定义
 * \return 无
 */
 
 
am_static_inline
void am_lpc82x_sct_cap_set_input(uint8_t                          chan,
                                 amhw_lpc82x_inmux_sct_trig_src_t input_src)
{
    amhw_lpc82x_inmux_sct_trig_set(AMHW_LPC82X_INMUX, chan, input_src);
}


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LPC82X_SCT_CAP_H */

/* end of file */
