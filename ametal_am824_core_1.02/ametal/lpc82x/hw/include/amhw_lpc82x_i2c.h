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
 * \brief I2C 硬件操作接口
 * 
 * 1. 具有独立主机、从机和监控等功能；
 * 2. 支持多个主机和具有从机的多主机；
 * 3. 在硬件上支持多个从机地址；
 * 5. 一个从机地址可以选择性地限定一个位掩码或一个地址范围，以便向多个I2C
 *    总线地址响应；
 * 6. 通过软件配置，可支持10bits地址的设备；
 * 7. 支持系统管理总线；
 * 8. 支持快速模式plus（总线速率可达1MHz）。
 *
 * \internal
 * \par Modification History
 * - 1.02 15-12-05  cyl, modify the namespace.
 * - 1.01 15-11-30  cyl, bug fixed.
 * - 1.00 15-07-06  win, first implementation.
 * \endinternal
 */

#ifndef __AMHW_LPC82X_I2C_H
#define __AMHW_LPC82X_I2C_H

#include "ametal.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * \addtogroup amhw_lpc82x_if_i2c 
 * \copydoc amhw_lpc82x_i2c.h
 * @{
 */
    
/* 使用匿名联合体段结束 */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
    
  /* 默认使能匿名联合体 */
#elif defined(__TMS470__)

  /* 默认使能匿名联合体 */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler t
#endif
    
/**
 * \brief I2C 寄存器块结构体
 */
typedef struct amhw_lpc82x_i2c {
    __IO uint32_t  cfg;           /**< \brief I2C 配置寄存器              */
    __IO uint32_t  stat;          /**< \brief I2C 状态寄存器              */
    __IO uint32_t  intenset;      /**< \brief I2C 中断位使能或读取寄存器  */
    __O  uint32_t  intenclr;      /**< \brief I2C 中断位禁能寄存器        */
    __IO uint32_t  timeout;       /**< \brief I2C 超时时间设定寄存器      */
    __IO uint32_t  clkdiv;        /**< \brief I2C 时钟分频数值设定寄存器  */
    __I  uint32_t  intstat;       /**< \brief I2C 中断状态                */
    __I  uint32_t  reserved;      /**< \brief 保留位                      */
    __IO uint32_t  mstctl;        /**< \brief I2C 主机控制寄存器          */
    
    /** \brief I2C 主机SCL高电平和低电平时间设定寄存器 */
    __IO uint32_t  msttime;       
    
    __IO uint32_t  mstdat;        /**< \brief I2C 主机数据寄存器          */
    __I  uint32_t  reserved1[5];  /**< \brief 保留位                      */
    __IO uint32_t  slvctl;        /**< \brief I2C 从机控制寄存器          */
    __IO uint32_t  slvdat;        /**< \brief I2C 从机数据寄存器          */
    
    union {
        __IO uint32_t slvadr[4];
        struct {
            __IO uint32_t  slvadr0;  /**< \brief 从机地址0                */
            __IO uint32_t  slvadr1;  /**< \brief 从机地址1                */
            __IO uint32_t  slvadr2;  /**< \brief 从机地址2                */
            __IO uint32_t  slvadr3;  /**< \brief 从机地址3                */
        };
    };
    
    __IO uint32_t  slvqual0;      /**< \brief 从机地址限定寄存器          */
    __I  uint32_t  reserved2[9];  /**< \brief 保留位                      */
    __I  uint32_t  monrxdat;      /**< \brief 监控模式数据寄存器          */
} amhw_lpc82x_i2c_t;


/**
 * \name I2C 从机地址编号
 * \anchor grp_amhw_lpc82x_i2c_addr_num
 * @{
 */

#define AMHW_LPC82X_I2C_SLVADR_0   (0)    /**< \brief I2C 从机地址编号0 */
#define AMHW_LPC82X_I2C_SLVADR_1   (1)    /**< \brief I2C 从机地址编号1 */
#define AMHW_LPC82X_I2C_SLVADR_2   (2)    /**< \brief I2C 从机地址编号2 */
#define AMHW_LPC82X_I2C_SLVADR_3   (3)    /**< \brief I2C 从机地址编号3 */

/** @} */

/**
 * \name I2C 状态寄存器各个位详细定义
 * \anchor grp_amhw_lpc82x_i2c_stat
 * @{
 */

/** \brief I2C 主机挂起状态          */
#define AMHW_LPC82X_I2C_STAT_MSTPEND          AM_SBF(0x1, 0)  

/** \brief I2C 主机状态掩码          */
#define AMHW_LPC82X_I2C_MASTER_STATE_MASK     AM_SBF(0x7, 1)

/** \brief I2C 主机空闲状态          */
#define AMHW_LPC82X_I2C_STAT_MSTIDLE          AM_SBF(0x0, 1)

/** \brief I2C 主机接收就绪状态      */
#define AMHW_LPC82X_I2C_STAT_MSTRX            AM_SBF(0x1, 1)

/** \brief I2C 主机发送就绪状态      */
#define AMHW_LPC82X_I2C_STAT_MSTTX            AM_SBF(0x2, 1)

/** \brief I2C 主机地址无响应        */
#define AMHW_LPC82X_I2C_STAT_MSTNACKADDR      AM_SBF(0x3, 1)

/** \brief I2C 主机发送无响应        */
#define AMHW_LPC82X_I2C_STAT_MSTNACKTX        AM_SBF(0x4, 1)

/** \brief I2C 主机仲裁失败标志      */
#define AMHW_LPC82X_I2C_STAT_MSTARBLOSS       AM_SBF(0x1, 4)

/** \brief I2C 主机启动/停止错误标志 */
#define AMHW_LPC82X_I2C_STAT_MSTSSERR         AM_SBF(0x1, 6)


/** \brief I2C 从机挂起状态          */
#define AMHW_LPC82X_I2C_STAT_SLVPEND          AM_SBF(0x1, 8)

/** \brief I2C 从机状态掩码          */
#define AMHW_LPC82X_I2C_SLAVE_STATE_MASK      AM_SBF(0x3, 9)

/** \brief I2C 从机地址              */
#define AMHW_LPC82X_I2C_STAT_SLVADDR          AM_SBF(0x0, 9)

/** \brief I2C 从机接收就绪状态      */
#define AMHW_LPC82X_I2C_STAT_SLVRX            AM_SBF(0x1, 9)

/** \brief I2C 从机发送就绪状态      */
#define AMHW_LPC82X_I2C_STAT_SLVTX            AM_SBF(0x2, 9)

/** \brief I2C 从机未延伸。此时，可以进入深度睡眠或掉电模式 */
#define AMHW_LPC82X_I2C_STAT_SLVNOTSTR        AM_SBF(0x1, 11) 

/** \brief I2C 从机地址匹配状态      */
#define AMHW_LPC82X_I2C_STAT_SLVIDX_MASK      AM_SBF(0x3, 12)

/** \brief I2C 从机选择              */
#define AMHW_LPC82X_I2C_STAT_SLVSEL           AM_SBF(0x1, 14)

/** \brief I2C 从机取消选定标志      */
#define AMHW_LPC82X_I2C_STAT_SLVDESEL         AM_SBF(0x1, 15)  


/** \brief I2C 监控模式接受就绪      */
#define AMHW_LPC82X_I2C_STAT_MONRDY           AM_SBF(0x1, 16)

/** \brief I2C 监控模式溢出标志      */
#define AMHW_LPC82X_I2C_STAT_MONOV            AM_SBF(0x1, 17)

/** \brief I2C 监控模式激活标志      */
#define AMHW_LPC82X_I2C_STAT_MONACTIVE        AM_SBF(0x1, 18)

/** \brief I2C 监控模式空闲标志      */
#define AMHW_LPC82X_I2C_STAT_MONIDLE          AM_SBF(0x1, 19)
                               

/** \brief I2C 事件超时标志          */
#define AMHW_LPC82X_I2C_STAT_EVTIMEOUT        AM_SBF(0x1, 24)  

/** \brief I2C 时钟超时标志          */
#define AMHW_LPC82X_I2C_STAT_SCLTIMEOUT       AM_SBF(0x1, 25)

/** @} */

/**
 * \name I2C 中断使能寄存器各个位详细定义
 * \anchor grp_amhw_lpc82x_i2c_intenset
 * @{
 */

/** \brief I2C 主机挂起中断使能          */
#define AMHW_LPC82X_I2C_INTENSET_MSTPEND      AM_SBF(0x1, 0)

/** \brief I2C 主机仲裁失败中断使能      */
#define AMHW_LPC82X_I2C_INTENSET_MSTARBLOSS   AM_SBF(0x1, 4)

/** \brief I2C 主机启动/停止错误中断使能 */
#define AMHW_LPC82X_I2C_INTENSET_MSTSSERR     AM_SBF(0x1, 6)
                                       
                                       
/** \brief I2C 从机挂起中断使能          */
#define AMHW_LPC82X_I2C_INTENSET_SLVPEND      AM_SBF(0x1, 8) 

/** \brief I2C 从机未延伸使能            */
#define AMHW_LPC82X_I2C_INTENSET_SLVNOTSTR    AM_SBF(0x1, 11) 

/** \brief I2C 从机取消选定中断使能      */
#define AMHW_LPC82X_I2C_INTENSET_SLVDESEL     AM_SBF(0x1, 15)
                

/** \brief I2C 监控模式接收就绪中断使能  */
#define AMHW_LPC82X_I2C_INTENSET_MONRX        AM_SBF(0x1, 16)

/** \brief I2C 监控超限中断使能          */
#define AMHW_LPC82X_I2C_INTENSET_MONOVEN      AM_SBF(0x1, 17)

/** \brief I2C 监控模式空闲中断使能      */
#define AMHW_LPC82X_I2C_INTENSET_MONIDLE      AM_SBF(0x1, 19)
                                       
                                       
/** \brief I2C 事件超时中断使能          */
#define AMHW_LPC82X_I2C_INTENSET_EVTIMEOUT    AM_SBF(0x1, 24)

/** \brief I2C 时钟超时中断使能          */
#define AMHW_LPC82X_I2C_INTENSET_SCLTIMEOUT   AM_SBF(0x1, 25)

/** @} */

/**
 * \name I2C 中断禁能寄存器各个位详细定义
 * \anchor grp_amhw_lpc82x_i2c_intenclr
 * @{
 */

/** \brief I2C 主机挂起中断禁能          */
#define AMHW_LPC82X_I2C_INTENCLR_MSTPEND      AM_SBF(0x1, 0)

/** \brief I2C 主机仲裁失败中断禁能      */
#define AMHW_LPC82X_I2C_INTENCLR_MSTARBLOSS   AM_SBF(0x1, 4)

/** \brief I2C 主机启动/停止错误中断禁能 */
#define AMHW_LPC82X_I2C_INTENCLR_MSTSSERR     AM_SBF(0x1, 6)
                                                  

/** \brief I2C 从机挂起中断禁能          */
#define AMHW_LPC82X_I2C_INTENCLR_SLVPEND      AM_SBF(0x1, 8)

/** \brief I2C 从机未延伸中断禁能        */
#define AMHW_LPC82X_I2C_INTENCLR_SLVNOTSTR    AM_SBF(0x1, 11)

/** \brief I2C 从机取消选定中断禁能      */
#define AMHW_LPC82X_I2C_INTENCLR_SLVDESEL     AM_SBF(0x1, 15)
                                                   
                                                   
/** \brief I2C 监控模式接收就绪中断禁能  */
#define AMHW_LPC82X_I2C_INTENCLR_MONRX        AM_SBF(0x1, 16)

/** \brief I2C 监控超限中断禁能          */
#define AMHW_LPC82X_I2C_INTENCLR_MONOVEN      AM_SBF(0x1, 17)

/** \brief I2C 监控模式空闲中断禁能      */
#define AMHW_LPC82X_I2C_INTENCLR_MONIDLE      AM_SBF(0x1, 19) 
                                 

/** \brief I2C 事件超时中断禁能          */
#define AMHW_LPC82X_I2C_INTENCLR_EVTIMEOUT    AM_SBF(0x1, 24)

/** \brief I2C 时钟超时中断禁能          */
#define AMHW_LPC82X_I2C_INTENCLR_SCLTIMEOUT   AM_SBF(0x1, 25)

/** @} */

/**
 * \name I2C 主机控制寄存器位定义
 * \anchor grp_amhw_lpc82x_i2c_mstctl
 * @{
 */

/** \brief I2C 主机继续（响应）控制位 */ 
#define AMHW_LPC82X_I2C_CTL_MSTCONTINUE       AM_SBF(0x1, 0)

/** \brief I2C 主机启动控制位         */
#define AMHW_LPC82X_I2C_CTL_MSTSTART          AM_SBF(0x1, 1)

/** \brief I2C 主机停止控制位         */
#define AMHW_LPC82X_I2C_CTL_MSTSTOP           AM_SBF(0x1, 2)

/** \brief I2C 主机DMA控制位          */
#define AMHW_LPC82X_I2C_CTL_MSTDMA            AM_SBF(0x1, 3)

/** @} */

/**
 * \name I2C 从机控制寄存器位定义
 * \anchor grp_amhw_lpc82x_i2c_slvctl
 * @{
 */ 

/** \brief I2C 从机继续（响应）控制位 */
#define AMHW_LPC82X_I2C_CTL_SLVCONTINUE       AM_SBF(0x1, 0)

/** \brief I2C 从机无响应控制位       */
#define AMHW_LPC82X_I2C_CTL_SLVNACK           AM_SBF(0x1, 1)

/** \brief I2C 从机DMA控制位          */
#define AMHW_LPC82X_I2C_CTL_SLVDMA            AM_SBF(0x1, 3)

/** @} */

/**
 * \name I2C 监控数据寄存器位定义
 * \anchor grp_amhw_lpc82x_i2c_monrxdat
 * @{
 */

/** \brief 检测到I2C总线上的启动事件     */
#define AMHW_LPC82X_I2C_MONRXDAT_START        AM_SBF(0x1, 8)

/** \brief 检测到I2C总线上的重复启动事件 */
#define AMHW_LPC82X_I2C_MONRXDAT_RESTART      AM_SBF(0x1, 9)

/** \brief 检测数据应答                  */
#define AMHW_LPC82X_I2C_MONRXDAT_NACK         AM_SBF(0x1, 10)

/** @} */

/**
 * \anchor I2C_mode
 * \brief I2C 模式定义（参考I2C配置寄存器） 
 */
typedef enum amhw_lpc82x_i2c_mode {
    
    /**< \brief 主机模式使能         */
    AMHW_LPC82X_I2C_MASTER_MODE    = AM_SBF(0x1, 0), 
    /**< \brief 从机模式使能         */
    AMHW_LPC82X_I2C_SLAVE_MODE     = AM_SBF(0x1, 1), 
    /**< \brief 监控模式使能         */
    AMHW_LPC82X_I2C_MONITOR_MODE   = AM_SBF(0x1, 2), 
    /**< \brief 超时使能             */
    AMHW_LPC82X_I2C_TIMEOUT_MODE   = AM_SBF(0x1, 3), 
    /**< \brief 监控模式时钟延伸使能 */
    AMHW_LPC82X_I2C_MONCLKSTR_MODE = AM_SBF(0x1, 4)  
    
} amhw_lpc82x_i2c_mode_t;

/**
 * \brief  使能设定的I2C模式
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] mode     : 需要使能的I2C 模式,参考 \ref I2C_mode "I2C 模式定义"
 *
 * \return    无
 */
am_static_inline
void amhw_lpc82x_i2c_mode_enable (amhw_lpc82x_i2c_t      *p_hw_i2c, 
                                  amhw_lpc82x_i2c_mode_t  mode)
{
    AM_BIT_SET_MASK(p_hw_i2c->cfg, mode);
}

/**
 * \brief  禁能设定的I2C模式
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] mode     : 需要禁能的I2C 模式,参考 \ref I2C_mode "I2C 模式定义"
 *
 * \return    无
 */
am_static_inline 
void amhw_lpc82x_i2c_mode_disable (amhw_lpc82x_i2c_t      *p_hw_i2c, 
                                   amhw_lpc82x_i2c_mode_t  mode)
{
    AM_BIT_CLR_MASK(p_hw_i2c->cfg, mode);
}

/**
 * \brief 获取 I2C 状态寄存器的值
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return I2C 状态寄存器值
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_stat_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->stat);
}
    
/**
 * \brief 清除 I2C 状态寄存器标志
 * 
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] flags    : 需要清除的状态标志，AMHW_LPC82X_I2C_STAT_* 宏或
 *                       多个AMHW_LPC82X_I2C_STAT_*宏
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_i2c_stat_clr (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t flags)
{
    p_hw_i2c->stat = flags;
}

/**
 * \brief I2C 中断使能
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] flags    : AMHW_LPC82X_I2C_INTENSET_* 宏值或多个 
 *                       AMHW_LPC82X_I2C_INTENSET_*宏的 或（OR）
 *                       值(#AMHW_LPC82X_I2C_INTENSET_MSTPEND)
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_i2c_int_enable (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t flags)
{
    AM_BIT_SET_MASK(p_hw_i2c->intenset, flags);
}

/**
 * \brief I2C 中断禁能
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] flags    : AMHW_LPC82X_I2C_INTENSET_* 宏值或多个 
 *                       AMHW_LPC82X_I2C_INTENSET_*宏的 或（OR）
 *                       值(#AMHW_LPC82X_I2C_INTENSET_MSTPEND)
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_i2c_int_disable (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t flags)
{
    AM_BIT_SET_MASK(p_hw_i2c->intenclr, flags);
}

/**
 * \brief I2C 超时值设置
 * 
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] value    : I2C时钟经过((value + 1) * 16)次计数后超时一次
 *
 * \return 无
 * \note 超出设定值通过状态标志和/或中断进行通知。若要在I2C工作时改变该
 *       数值，则需禁用所有超时并向TIMEOUT寄存器写入新值，重新使能超时。
 */
am_static_inline
void amhw_lpc82x_i2c_timeout_set (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t value)
{
    uint32_t time_out;
    
    if (value <= 0xFFF) {
        time_out = value;
    } else {
        time_out = 0;
    }
    
    p_hw_i2c->timeout = (AM_SBF(time_out, 4) | 0xF); 
}

/**
 * \brief I2C 超时值获取
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return I2C 超时值
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_timeout_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (uint32_t)AM_BITS_GET(p_hw_i2c->timeout, 4, 12);
}

/**
 * \brief 获取时钟分频寄存器
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  时钟分频寄存器的值
 */
am_static_inline 
uint32_t amhw_lpc82x_i2c_clkdiv_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->clkdiv);
}

/**
 * \brief 设定时钟分频函数
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] div      : 时钟分频参数
 *
 * \return  无
 */
am_static_inline 
void amhw_lpc82x_i2c_clkdiv_set (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t div)
{
    uint32_t clkdiv;
    
    if ((div >= 1) && (div <= 65536)) {
        clkdiv = div - 1;
    } else {
        clkdiv = 0;
    }
    
    p_hw_i2c->clkdiv = clkdiv;
}

/**
 * \brief 获取当前中断状态的函数
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  中断状态寄存器的值 
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_intstat_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->intstat);
}

/**
 * \brief I2C 主机启动控制函数
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  无
 */
am_static_inline 
void amhw_lpc82x_i2c_mst_start (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = AMHW_LPC82X_I2C_CTL_MSTSTART;  
}

/**
 * \brief I2C 主机继续操作函数（响应ACK）
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return    无
 */
am_static_inline 
void amhw_lpc82x_i2c_mst_continue (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = AMHW_LPC82X_I2C_CTL_MSTCONTINUE;  
}

/**
 * \brief I2C 主机停止控制函数
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  无
 */
am_static_inline 
void amhw_lpc82x_i2c_mst_stop (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = AMHW_LPC82X_I2C_CTL_MSTSTOP;
}

/**
 * \brief I2C 主机 DMA 使能
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  无
 */
am_static_inline
void amhw_lpc82x_i2c_mst_dma_enable (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = AMHW_LPC82X_I2C_CTL_MSTDMA;
}    

/**
 * \brief I2C 主机 DMA 禁能
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  无
 */
am_static_inline
void amhw_lpc82x_i2c_mst_dma_disable (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->mstctl = ~AMHW_LPC82X_I2C_CTL_MSTDMA;
} 

/**
 * \brief 设定MSTTIME（SCL高电平和SCL低电平时间）
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] scl_h    : SCL高电平时间为 scl_h 个 I2C 预分频时钟周期
 * \param[in] scl_l    : SCL低电平时间为 scl_l 个 I2C 预分频时钟周期
 *
 * \return  无
 * \note MSTTIME 时间设置范围为2 ~ 9 个 I2C 预分频时钟周期
 */
am_static_inline 
void amhw_lpc82x_i2c_msttime_set (amhw_lpc82x_i2c_t *p_hw_i2c, 
                                  uint32_t           scl_h, 
                                  uint32_t           scl_l)
{
    p_hw_i2c->msttime = AM_SBF(scl_h - 2, 4) | (scl_l - 2);
}

/**
 * \brief 获取MSTTIME（SCL高电平和SCL低电平）
 *
 * \param[in]  p_hw_i2c : 指向I2C寄存器块的指针
 * \param[out] p_scl_h  : 保存SCL高电平时间
 * \param[out] p_scl_l  : 保存SCL低电平时间
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_i2c_msttime_get (amhw_lpc82x_i2c_t *p_hw_i2c,
                                  uint32_t          *p_scl_h,
                                  uint32_t          *p_scl_l)
{
    *p_scl_l = AM_BITS_GET(p_hw_i2c->msttime, 0, 3) + 2;
    *p_scl_h = AM_BITS_GET(p_hw_i2c->msttime, 4, 3) + 2;
}

/**
 * \brief I2C 写数据到主机数据寄存器
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] value    : 写入主机数据寄存器的数据
 *
 * \return  无
 */
am_static_inline 
void amhw_lpc82x_i2c_mstdat_write (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t value)
{
    p_hw_i2c->mstdat = value;  
}

/**
 * \brief I2C 读取数据从主机数据寄存器
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  主机数据寄存器的值
 */
am_static_inline 
uint32_t amhw_lpc82x_i2c_mstdat_read (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->mstdat);  
}

/**
 * \brief I2C 从机继续操作（响应ACK）
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  无
 */
am_static_inline 
void amhw_lpc82x_i2c_slv_continue (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->slvctl = AMHW_LPC82X_I2C_CTL_SLVCONTINUE;
}

/**
 * \brief I2C 从机否定应答(结束通信)
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  无
 */
am_static_inline
void amhw_lpc82x_i2c_slv_nack (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->slvctl = AMHW_LPC82X_I2C_CTL_SLVNACK;
}

/**
 * \brief I2C 从机DAM使能
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  无
 */
am_static_inline
void amhw_lpc82x_i2c_slv_dma_enable (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->slvctl = AMHW_LPC82X_I2C_CTL_SLVDMA;
}

/**
 * \brief I2C 从机DAM禁能
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  无
 */
am_static_inline
void amhw_lpc82x_i2c_slv_dma_disable (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    p_hw_i2c->slvctl = ~AMHW_LPC82X_I2C_CTL_SLVDMA;
}

/**
 * \brief 写数据到从机数据寄存器
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] value    : 写入从机数据寄存器的数据
 *
 * \return  无
 */
am_static_inline 
void amhw_lpc82x_i2c_slvdat_write (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t value)
{
    p_hw_i2c->slvdat = value;  
}

/**
 * \brief I2C 读取数据从从机数据寄存器
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return  从机数据寄存器的值
 */
am_static_inline 
uint32_t amhw_lpc82x_i2c_slvdat_read (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->slvdat);  
}

/**
 * \brief I2C 从机地址设置 
 *
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] addr_num : 从机地址编号，AMHW_LPC82X_I2C_SLVADR_*宏
 * \param[in] sla_addr : 7位从机地址值
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_i2c_slv_addr_set (amhw_lpc82x_i2c_t *p_hw_i2c,
                                   uint32_t           addr_num,
                                   uint32_t           sla_addr)
{
    p_hw_i2c->slvadr[addr_num & 0x3] = AM_SBF(sla_addr, 1);
}

/**
 * \brief 设置 I2C 从机地址验证寄存器0的值
 * 
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \param[in] flags    : 需要设置的值
 *
 * \return 无
 */
am_static_inline
void amhw_lpc82x_i2c_slvqual_set (amhw_lpc82x_i2c_t *p_hw_i2c, uint32_t flags)
{
    p_hw_i2c->slvqual0 = flags;
}

/**
 * \brief 获取 I2C 从机地址验证寄存器0的值 
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return 从机地址验证寄存器0的值 
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_slvqual_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->slvqual0);
}

/**
 * \brief 获取 I2C 监控模式数据寄存器的值
 * \param[in] p_hw_i2c : 指向I2C寄存器块的指针
 * \return 监控数据寄存器的值
 */
am_static_inline
uint32_t amhw_lpc82x_i2c_mon_dat_get (amhw_lpc82x_i2c_t *p_hw_i2c)
{
    return (p_hw_i2c->monrxdat);
}



/* 使用匿名联合体段结束 */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
 
  /* 允许匿名联合体使能 */
#elif defined(__GNUC__)

  /* 默认使用匿名联合体 */
#elif defined(__TMS470__)

  /* 默认使用匿名联合体 */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler t
#endif


/**
 *@} 
 */

#ifdef __cplusplus
}
#endif

#endif /* __AMHW_LPC82X_I2C_H */

/* end of file */
