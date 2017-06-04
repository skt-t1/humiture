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
 * \brief 循环冗余校验(CRC) 操作接口
 *
 * 1. 支持3种CRC生成多项式： CRC-CCITT, CRC-16, and CRC-32
 *    - CRC-CCITT : x16 + x12 + x5 + 1   (0x1021);
 *    - CRC-16    : x16 + x15 + x2 + 1   (0x8005);
 *    - CRC-32    : x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 +
 *                  x5 + x4 + x2+ x + 1  (0x04C11DB7);
 * 2. 支持输入数据和CRC校验和的位反序和1's补码;
 * 3. 可编程种子数据设置;
 * 4. 支持三种数据宽度的输入数据: 8位、16位和32位
 *    - 8 位数据写入: 运行1个周期;
 *    - 16位数据写入: 运行2个周期;
 *    - 32位数据写入: 运行4个周期。
 *
 * \internal
 * \par Modification history
 * - 1.00 15-7-08  sss, first implementation.
 * \endinternal
 */

#ifndef __AMHW_LPC82X_CRC_H
#define __AMHW_LPC82X_CRC_H

#include "ametal.h"

#ifdef __cplusplus 
extern "C" {
#endif

/*
 * \brief 匿名结构体段的开始
 */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
#elif defined(__TMS470__)
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler t
#endif



/**
 * \addtogroup amhw_lpc82x_if_crc
 * \copydoc amhw_lpc82x_crc.h
 * @{
 */
 
/**
 * \brief CRC寄存器块结构体
 */
typedef struct amhw_lpc82x_crc {
    __IO uint32_t mode;                /**< \brief crc模式寄存器          */
    __IO uint32_t seed;                /**< \brief crc种子寄存器          */
    union {
        __I uint32_t sum;              /**< \brief crc校验寄存器          */
        __O uint32_t wrdata32;         /**< \brief crc写数据寄存器(32位写)*/
        __O uint16_t wrdata16;         /**< \brief crc写数据寄存器(16位写)*/
        __O uint8_t  wrdata8;          /**< \brief crc写数据寄存器(8位写) */
    };
} amhw_lpc82x_crc_t;

/**
 * \brief crc模式定义
 */

/** \brief 输入数据位反序           */
#define AMHW_LPC82X_CRC_MODE_WRDATA_BIT_RVS   AM_SBF(1, 2)  
/** \brief 输入数据位取1's 补码运算 */
#define AMHW_LPC82X_CRC_MODE_WRDATA_BIT_CMPL  AM_SBF(1, 3)  
/** \brief 校验和位反序             */
#define AMHW_LPC82X_CRC_MODE_SUM_RVS          AM_SBF(1, 4)  
/** \brief 校验和位取1's补码运算    */
#define AMHW_LPC82X_CRC_MODE_SUM_CMPL         AM_SBF(1, 5)  

/**
 * \brief crc生成多项式定义
 */
typedef enum amhw_lpc82x_crc_poly {
    
    /** \brief CRC-CCITT 多项式*/
    AMHW_LPC82X_CRC_POLY_CCITT = 0,                
    /** \brief CRC-16多项式    */
    AMHW_LPC82X_CRC_POLY_CRC16 = 1,                
    /** \brief CRC-32多项式    */
    AMHW_LPC82X_CRC_POLY_CRC32 = 2    
    
} amhw_lpc82x_crc_poly_t;

/**
 * \brief 设置模式和生成多项式
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \param[in] poly     : crc生成的多项式
 * \param[in] flags    : crc标志位
 *                       只能为 
 *                       #AMHW_LPC82X_CRC_MODE_WRDATA_BIT_RVS  或
 *                       #AMHW_LPC82X_CRC_MODE_WRDATA_BIT_CMPL 或
 *                       #AMHW_LPC82X_CRC_MODE_SUM_RVS         或
 *                       #AMHW_LPC82X_CRC_MODE_SUM_CMPL        或
 *                       四个值中的一个或多个的或值
 * \return 无
 */
am_static_inline
void amhw_lpc82x_crc_mode_set (amhw_lpc82x_crc_t     *p_hw_crc,
                               amhw_lpc82x_crc_poly_t poly,
                               uint32_t               flags)
{
    p_hw_crc->mode = (uint32_t)poly | flags;
}

/**
 * \brief 得到crc模式寄存器的值
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \return 模式寄存器的值
 */
am_static_inline
uint32_t amhw_lpc82x_crc_mode_get (amhw_lpc82x_crc_t *p_hw_crc)
{
    return p_hw_crc->mode; 
}

/**
 * \brief 设置种子寄存器的值
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \param[in] seed     : 种子值
 * \return 无
 */
am_static_inline
void amhw_lpc82x_crc_seed_set (amhw_lpc82x_crc_t *p_hw_crc, uint32_t seed)
{
    p_hw_crc->seed = seed;
}

/**
 * \brief 得到种子寄存器的值
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \return 当前种子寄存器的值
 */
am_static_inline
uint32_t amhw_lpc82x_crc_seed_get (amhw_lpc82x_crc_t *p_hw_crc)
{
    return p_hw_crc->seed;
}

/**
 * \brief 读取crc校验和寄存器的值
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \return 校验和的值
 */
am_static_inline
uint32_t amhw_lpc82x_crc_sum_get (amhw_lpc82x_crc_t *p_hw_crc)
{
    return p_hw_crc->sum;
}

/**
 * \brief 写入一个八位数据的值到crc引擎进行计算
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \param[in] data     : 写入的八位数据
 * \return 无
 */
am_static_inline
void amhw_lpc82x_crc_wrdata8 (amhw_lpc82x_crc_t *p_hw_crc, uint8_t data)
{
    p_hw_crc->wrdata8 = data;
}

/**
 * \brief 写入一个十六位数据的值到crc引擎进行计算
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \param[in] data     : 写入的十六位数据
 * \return 无
 */
am_static_inline
void amhw_lpc82x_crc_wrdata16 (amhw_lpc82x_crc_t *p_hw_crc, uint16_t data)
{
    p_hw_crc->wrdata16 = data;
}

/**
 * \brief 写入一个三十二位数据的值到crc引擎进行计算
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \param[in] data     : 写入的三十二位数据
 * \return 无
 */
am_static_inline
void amhw_lpc82x_crc_wrdata32 (amhw_lpc82x_crc_t *p_hw_crc, uint32_t data)
{
    p_hw_crc->wrdata32 = data;
}

/**
 * \brief 写入n个八位数据的值到crc引擎进行计算
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \param[in] p_data   : 要写入的数据缓冲区(8位数据缓冲区)
 * \param[in] size     : 8位数据的个数
 * \return 无
 */
void amhw_lpc82x_crc_data8_write (amhw_lpc82x_crc_t *p_hw_crc,
                                  const uint8_t     *p_data,
                                  uint32_t           size);


/**
 * \brief 写入n个十六位数据的值到crc引擎进行计算
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \param[in] p_data   : 要写入的数据缓冲区(16位数据缓冲区)
 * \param[in] size     : 16位数据的个数
 * \return 无
 */
void amhw_lpc82x_crc_data16_write (amhw_lpc82x_crc_t *p_hw_crc, 
                                   const uint16_t    *p_data,
                                   uint32_t           size);


/**
 * \brief 写入n个三十二位数据的值到crc引擎进行计算
 * \param[in] p_hw_crc : crc寄存器块的指针
 * \param[in] p_data   : 要写入的数据缓冲区(32位数据缓冲区)
 * \param[in] size     : 32位数据的个数
 * \return 无
 */
void amhw_lpc82x_crc_data32_write (amhw_lpc82x_crc_t *p_hw_crc, 
                                   const uint32_t    *p_data,
                                   uint32_t           size);



/**
 * @} 
 */
/*
 * \brief 匿名结构体段的结束
 */

#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#elif defined(__TMS470__)
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler t
#endif

#ifdef __cplusplus
}
#endif /* __AMHW_LPC82X_CRC_H */

#endif

/*end of file */
