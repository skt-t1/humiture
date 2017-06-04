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
 * \brief Generic ioctl command encode and decode
 *
 * \par ioctl command encoding
 * - 32 bits total, command in lower 16 bits, size of the parameter structure
 *   in the lower 14 bits of the upper 16 bits.
 *   - Encoding the size of the parameter structure in the ioctl request
 *     is useful for catching programs compiled with old versions
 *     and to avoid overwriting user space outside the user buffer area.
 * - The highest 2 bits are reserved for indicating the ``access mode''.
 *   \note This limits the max parameter size to 16kB -1 !
 *
 * \attention The generic ioctl numbering scheme doesn't really enforce
 *  a type field. Please be sure to use the decoding macros below.
 *
 * \internal
 * \par modification history
 * - 1.00 17-04-02  skt, first implementation.
 * \endinternal
 */

#ifndef __AM_IOCTL_GENERIC_H
#define __AM_IOCTL_GENERIC_H

/**
 * \name encodeing bits
 * @{
 */
#define __AM_IOC_NRBITS     8
#define __AM_IOC_TYPEBITS   8
/** @} */

/**
 * \name Let any architecture override either of the following before
 *      including this file.
 * @{
 */

#ifndef __AM_IOC_SIZEBITS
#define __AM_IOC_SIZEBITS   14
#endif

#ifndef __AM_IOC_DIRBITS
#define __AM_IOC_DIRBITS    2
#endif

#define __AM_IOC_NRMASK     ((1 << __AM_IOC_NRBITS)   - 1)
#define __AM_IOC_TYPEMASK   ((1 << __AM_IOC_TYPEBITS) - 1)
#define __AM_IOC_SIZEMASK   ((1 << __AM_IOC_SIZEBITS) - 1)
#define __AM_IOC_DIRMASK    ((1 << __AM_IOC_DIRBITS)  - 1)

#define __AM_IOC_NRSHIFT    0
#define __AM_IOC_TYPESHIFT  (__AM_IOC_NRSHIFT   + __AM_IOC_NRBITS)
#define __AM_IOC_SIZESHIFT  (__AM_IOC_TYPESHIFT + __AM_IOC_TYPEBITS)
#define __AM_IOC_DIRSHIFT   (__AM_IOC_SIZESHIFT + __AM_IOC_SIZEBITS)

/** @} */

/**
 * \name Direction bits, which any architecture can choose to override
 *      before including this file.
 * @{
 */

#ifndef __AM_IOC_NONE
#define __AM_IOC_NONE   0U
#endif

#ifndef __AM_IOC_WRITE
#define __AM_IOC_WRITE  1U
#endif

#ifndef __AM_IOC_READ
#define __AM_IOC_READ   2U
#endif

#define __AM_IOC(dir, type, nr, size) \
    (((dir)  << __AM_IOC_DIRSHIFT)  | \
     ((type) << __AM_IOC_TYPESHIFT) | \
     ((nr)   << __AM_IOC_NRSHIFT)   | \
     ((size) << __AM_IOC_SIZESHIFT))

#if defined(AM_DEBUG) && defined(__GNUC__)
/* provoke compile error for invalid uses of size argument */
extern unsigned int g_am_invalid_size_argument_for_IOC;
#define __AM_IOC_TYPECHECK(t) \
    (((sizeof(t) == sizeof(t[1])) && (sizeof(t) < (1 << __AM_IOC_SIZEBITS))) ? \
      sizeof(t) : g_am_invalid_size_argument_for_IOC)
#else
#define __AM_IOC_TYPECHECK(t) (sizeof(t))
#endif

/** @} */

/**
 * \name macros used to create ioctl command numbers
 * @{
 */

/** \brief define a ioctl command without argument */
#define __AM_IO(type, nr) \
    __AM_IOC(__AM_IOC_NONE, type, nr, 0)

/** \brief define a read ioctl command */
#define __AM_IOR(type, nr, size) \
    __AM_IOC(__AM_IOC_READ, type, nr, (__AM_IOC_TYPECHECK(size)))

/** \brief define a write ioctl command */
#define __AM_IOW(type, nr, size) \
    __AM_IOC(__AM_IOC_WRITE, type, nr, (__AM_IOC_TYPECHECK(size)))

/** \brief define a read and write ioctl command */
#define __AM_IOWR(type, nr, size) \
    __AM_IOC(__AM_IOC_READ | __AM_IOC_WRITE, type, nr, (__AM_IOC_TYPECHECK(size)))

/** \brief define a read ioctl command without type check */
#define __AM_IOR_BAD(type, nr, size) \
    __AM_IOC(__AM_IOC_READ, type, nr, sizeof(size))

/** \brief define a write ioctl command without type check */
#define __AM_IOW_BAD(type, nr, size) \
    __AM_IOC(__AM_IOC_WRITE, type, nr, sizeof(size))

/** \brief define a read and write ioctl command without type check */
#define __AM_IOWR_BAD(type, nr, size) \
    __AM_IOC(__AW_IOC_READ|__AM_IOC_WRITE, type, nr, sizeof(size))

/** @} */

/**
 * \name macros used to decode ioctl numbers
 * @{
 */

/** \brief macro to decode direction of ioctl number */
#define __AM_IOC_DIR(nr)        (((nr) >> __AM_IOC_DIRSHIFT) & __AM_IOC_DIRMASK)

/** \brief macro to decode command type of ioctl number */
#define __AM_IOC_TYPE(nr)       (((nr) >> __AM_IOC_TYPESHIFT) & __AM_IOC_TYPEMASK)

/** \brief macro to decode request number of ioctl number */
#define __AM_IOC_NR(nr)     (((nr) >> __AM_IOC_NRSHIFT) & __AM_IOC_NRMASK)

/** \brief macro to decode argument size of ioctl number */
#define __AM_IOC_SIZE(nr)       (((nr) >> __AM_IOC_SIZESHIFT) & __AM_IOC_SIZEMASK)

/** @} */

/**
 * \name help macros used for the drivers/sound files...
 * @{
 */

/** \brief in direction of ioctl number */
#define AM_IOC_IN          (__AM_IOC_WRITE << __AM_IOC_DIRSHIFT)

/** \brief out direction of ioctl number */
#define AM_IOC_OUT         (__AM_IOC_READ  << __AM_IOC_DIRSHIFT)

/** \brief in/out direction of ioctl number */
#define AM_IOC_INOUT       ((__AM_IOC_WRITE | __AM_IOC_READ) << __AM_IOC_DIRSHIFT)

/** \brief size mask of ioctl number */
#define AM_IOCSIZE_MASK    (__AM_IOC_SIZEMASK << __AM_IOC_SIZESHIFT)

/** \brief size shift of ioctl number */
#define AM_IOCSIZE_SHIFT   (__AM_IOC_SIZESHIFT)

/** @} */

#endif /* __AM_IOCTL_GENERIC_H */

/* end of file */
