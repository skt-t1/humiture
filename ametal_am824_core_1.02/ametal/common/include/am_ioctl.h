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
 * \brief Generic ioctl commands
 *
 * \internal
 * \par modification history
 * - 1.00 17-04-02  skt, first implementation.
 * \endinternal
 */

#ifndef __AM_IOCTL_H
#define __AM_IOCTL_H

/*
 * \name generic ioctl function codes
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


#include "am_ioctl_generic.h"

/******************************************************************************/

/*
 * \name ioctl types
 *
 * ametal platform use type number 0~99, the rest is for user use,
 * max type number is 255
 * @{
 */
#define AM_IOC_TYPE_FIO     0   /**< \brief File ioctl command type */
#define AM_IOC_TYPE_TIO     1   /**< \brief TTY ioctl command type */
#define AM_IOC_TYPE_SIO     2   /**< \brief serial device ioctl command type */

#define AM_IOC_TYPE_USER    100

/** @} */

/*
 * \name file stream ioctl function codes
 * @{
 */

#define _AM_FIO(nr)            __AM_IO(AM_IOC_TYPE_FIO, nr)
#define _AM_FIOR(nr, size)     __AM_IOR(AM_IOC_TYPE_FIO, nr, size)
#define _AM_FIOW(nr, size)     __AM_IOW(AM_IOC_TYPE_FIO, nr, size)
#define _AM_FIOWR(nr, size)    __AM_IOWR(AM_IOC_TYPE_FIO, nr, size)

#define AM_FIONREAD            _AM_FIOR(1, int) /**< \brief get num chars available to read */
#define AM_FIOFLUSH            _AM_FIO (2)      /**< \brief flush any chars in buffers */
#define AM_FIOOPTIONS          _AM_FIOW(3, int) /**< \brief set options (FIOSETOPTIONS) */
#define AM_FIOBAUDRATE         _AM_FIOW(4, int) /**< \brief set serial baud rate */

#define AM_FIODISKFORMAT       _AM_FIO(5)       /**< \brief format disk */
#define AM_FIODISKINIT         _AM_FIO(6)       /**< \brief format disk */

#define AM_FIOSEEK             _AM_FIOW(7, int) /**< \brief set current file char position */
#define AM_FIOWHERE            _AM_FIOR(8, int) /**< \brief get current file char position */
#define AM_FIODIRENTRY         _AM_FIOR(9, int) /**< \brief return a directory entry */
#define AM_FIORENAME           _AM_FIOW(10,int) /**< \brief rename a directory entry */

#define AM_FIONWRITE           _AM_FIOR(12,int) /**< \brief get num chars still to be written */

#define AM_FIOCANCEL           _AM_FIO(14)      /**< \brief cancel read or write on the device */

#define AM_FIONBIO             _AM_FIO(16)      /**< \brief set non-blocking I/O; SOCKETS ONLY!*/

#define AM_FIONMSGS            _AM_FIOR(17, int) /**< \brief return num msgs in pipe */

#define AM_FIOISATTY           _AM_FIOR(20, int) /**< \brief is a tty */
#define AM_FIOSYNC             _AM_FIO (21)      /**< \brief sync to disk */

#define AM_FIORBUFSET          _AM_FIOW(24, int) /**< \brief alter the size of read buffer  */
#define AM_FIOWBUFSET          _AM_FIOW(25, int) /**< \brief alter the size of write buffer */
#define AM_FIORFLUSH           _AM_FIO (26)      /**< \brief flush any chars in read buffers */
#define AM_FIOWFLUSH           _AM_FIO (27)      /**< \brief flush any chars in write buffers */
#define AM_FIOSELECT           _AM_FIO (28)      /**< \brief wake up process in select on I/O */
#define AM_FIOUNSELECT         _AM_FIO (29)      /**< \brief wake up process in select on I/O */
#define AM_FIONFREE            _AM_FIOR(30, int) /**< \brief get free byte count on device */
#define AM_FIOMKDIR            _AM_FIOW(31, int) /**< \brief create a directory */
#define AM_FIORMDIR            _AM_FIOW(32, int) /**< \brief remove a directory */

#define AM_FIOREADDIR          _AM_FIOR(37, int) /**< \brief read a directory entry (POSIX) */

#define AM_FIOTRUNC            _AM_FIOW(42, int) /**< \brief truncate file to specified length */

#define AM_FIOFSTATFSGET       _AM_FIOR(46, int) /**< \brief get FS status info */

#define AM_FIODATASYNC         _AM_FIO(57)       /**< \brief sync to I/O data integrity */

#define AM_FIOLINK             _AM_FIO(58)       /**< \brief create a link */
#define AM_FIOUNLINK           _AM_FIO(59)       /**< \brief remove a link */

#define AM_FIOSTATGET          _AM_FIOR(64, int) /**< \brief get stat info (POSIX) */
#define AM_FIOFSTATGET         _AM_FIOR(66, int) /**< \brief get fstat info (POSIX) */


/** @} */

/******************************************************************************/

/**
 * \name serial device ioctl function codes
 * @{
 */
#define _AM_TIO(nr)          __AM_IO(AM_IOC_TYPE_TIO, nr)
#define _AM_TIOR(nr, size)   __AM_IOR(AM_IOC_TYPE_TIO, nr, size)
#define _AM_TIOW(nr, size)   __AM_IOW(AM_IOC_TYPE_TIO, nr, size)

#define AM_TIOPROTOHOOK      _AM_TIOW(1, int)   /**< \brief specify protocol hook routine */
#define AM_TIOPROTOARG       _AM_TIOW(2, int)   /**< \brief specify protocol argument */
#define AM_TIOGETOPTIONS     _AM_TIOR(3, int)   /**< \brief get serial device options */
#define AM_TIOSETOPTIONS     _AM_TIOW(4, int)   /**< \brief set serial device options */
#define AM_TIOISATTY         _AM_TIOR(5, int)   /**< \brief is a tty */

#define AM_TIOCBAUDRATE      _AM_TIOW(6, int)   /**< \brief set serial baud rate */
#define AM_TIOCGRS485        _AM_TIOR(7, int)   /**< \brief Get serial RS-485 options */
#define AM_TIOCSRS485        _AM_TIOW(8, int)   /**< \brief Set serial RS-485 options */

#define AM_TIOCRDTIMEOUT     _AM_TIOW(9, int)   /**< \brief set serial read timeout */
#define AM_TIOCGSTATUS       _AM_TIOW(10, int)  /**< \brief get serial status */


/**
 * \name options to AM_TIOCSRS485 , bitwise or'ed together
 * @{
 */
#define AM_RS485_ENABLE          0x1     /**< \brief enable rs485 mode */
#define AM_RS485_DISABLE         0x0     /**< \brief disable rs485 mode */

/**
 * \name status return by AM_TIOCGSTATUS
 * @{
 */
#define AM_SERIAL_IDLE          0x0     /**< \brief serial device idle */
#define AM_SERIAL_BUSY          0x1     /**< \brief serial device busy */




/******************************************************************************/
/**< \brief structure for rename and move */
struct am_rename_struct {
    const char *p_old_name;
    const char *p_new_name;
};

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AM_IOCTL_H */

/* end of file */
