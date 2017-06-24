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

#include "ametal.h"
#include "amhw_lpc82x_crc.h"

 
 
/**
 * \brief 写n个8位数据到crc引擎
 */
void amhw_lpc82x_crc_data8_write (amhw_lpc82x_crc_t    *p_hw_crc,
                                  const uint8_t        *p_data,
                                  uint32_t              nbytes)
{
    while (nbytes > 0) {
        amhw_lpc82x_crc_wrdata8(p_hw_crc, *p_data);
        p_data++;
        nbytes--;
    }
}

/**
 * \brief 写n个16位数据到crc引擎
 */
void amhw_lpc82x_crc_data16_write (amhw_lpc82x_crc_t      *p_hw_crc,
                                   const uint16_t         *p_data,
                                   uint32_t                nhwords)
{
    while (nhwords > 0) {
        amhw_lpc82x_crc_wrdata16(p_hw_crc, *p_data);
        p_data++;
        nhwords--;
    }
}

/**
 * \brief 写n个32位的数据到crc引擎
 */
void amhw_lpc82x_crc_data32_write (amhw_lpc82x_crc_t     *p_hw_crc, 
                                   const uint32_t        *p_data,
                                   uint32_t               nwords)
{
    while (nwords > 0) {
        amhw_lpc82x_crc_wrdata32(p_hw_crc, *p_data);
        p_data++;
        nwords--;
    }
}

/* end of file */
