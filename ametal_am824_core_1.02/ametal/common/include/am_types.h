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
 * \brief 通用数据类型相关定义头文件
 *
 * \internal
 * \par Modification history
 * - 1.00 14-11-01  tee, first implementation.
 * \endinternal
 */

#ifndef __AM_TYPES_H
#define __AM_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdio.h"
#include "am_errno.h"


/**
 * \addtogroup am_if_types
 * \copydoc am_types.h
 * @{
 */
#ifdef __GNUC__

typedef _Bool bool_t;
#define TRUE   1
#define FALSE  0
#else

#ifndef TRUE

/**
 * \brief 布尔类型定义
 */
typedef enum {
    FALSE = 0, 
    TRUE  = !FALSE
} bool_t;

#endif /* TRUE */

#endif

/**
 * \brief 空指针定义
 */
#ifndef NULL
#define NULL      ((void *)0)
#endif
 
/**
 * \brief inline 定义
 * @{
 */
#if   defined (__CC_ARM)     

#define am_inline   __inline             /**< inline for ARMCC    */
#define am_static_inline  static  __inline  
    
#elif defined (__ICCARM__)
#define am_inline   inline               /**< inline for IAR      */
#define am_static_inline  static inline 

#elif defined   (__GNUC__)
#define am_inline   inline               /**< inline for GNU      */
#define am_static_inline  static inline  

#elif defined   (__TASKING__)
#define am_inline   inline               /**< inline for TASKING  */
#define am_static_inline  static inline  

#endif /* __CC_ARM */
/** @} */

/** \todo  根据编译器类型选择 static 关键字 */
#define am_local    static

/** \todo  根据编译器类型选择 const 关键字 */
#define am_const    const
    
/** \todo  根据编译器类型选择 extern 关键字 */
#define am_extern   extern

/** \todo  根据编译器类型选择 extern 关键字 */
#define am_import   extern

/** \todo  根据编译器类型选择 extern 关键字 */
#define am_export   extern


#ifdef __cplusplus
  #define   __I     volatile             /**< Defines 只读     */
#else
  #define   __I     volatile const       /**< Defines 只读     */
#endif
#define     __O     volatile             /**< Defines 只写     */
#define     __IO    volatile             /**< Defines 可读可写 */


/** \brief void (*pfn) (void *) */
typedef void (*am_pfnvoid_t) (void *);

/** \brief unsigned int type for short   */
typedef unsigned int    uint_t;

/** \brief unsigned char type for short  */
typedef unsigned char   uchar_t;

/** \brief unsigned long type for short  */
typedef unsigned long   ulong_t;

/** \brief unsigned short type for short */
typedef unsigned short  ushort_t;   

/**
 * @} 
 */
 
#ifdef __cplusplus
}
#endif
 
#endif /* __AM_TYPES_H */

/* end of file */
