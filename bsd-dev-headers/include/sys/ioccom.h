#ifndef SYS_IOCCOM_H
#define SYS_IOCCOM_H

/**
 * @file ioccom.h
 * @brief BSD ioctl command encoding macros (<sys/ioccom.h>).
 */

/* clang-format off */
#include <sys/cdefs.h>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#elif (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#if defined(__has_include_next)
#if __has_include_next(<sys/ioccom.h>)
#pragma GCC system_header
#include_next <sys/ioccom.h>
#endif
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IOCPARM_SHIFT
#define IOCPARM_SHIFT 13
#endif
#ifndef IOCPARM_MASK
#define IOCPARM_MASK ((1 << IOCPARM_SHIFT) - 1)
#endif
#ifndef IOC_VOID
#define IOC_VOID 0x20000000UL
#endif
#ifndef IOC_OUT
#define IOC_OUT 0x40000000UL
#endif
#ifndef IOC_IN
#define IOC_IN 0x80000000UL
#endif
#ifndef IOC_INOUT
#define IOC_INOUT (IOC_IN | IOC_OUT)
#endif
#ifndef IOC_DIRMASK
#define IOC_DIRMASK (IOC_VOID | IOC_OUT | IOC_IN)
#endif

#ifndef _IOC
#define _IOC(inout, group, num, len)                                           \
  ((unsigned long)((inout) | (((len) & IOCPARM_MASK) << 16) | ((group) << 8) | \
                   (num)))
#endif
#ifndef _IO
#define _IO(g, n) _IOC(IOC_VOID, (g), (n), 0)
#endif
#ifndef _IOR
#define _IOR(g, n, t) _IOC(IOC_OUT, (g), (n), sizeof(t))
#endif
#ifndef _IOW
#define _IOW(g, n, t) _IOC(IOC_IN, (g), (n), sizeof(t))
#endif
#ifndef _IOWR
#define _IOWR(g, n, t) _IOC(IOC_INOUT, (g), (n), sizeof(t))
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_IOCCOM_H */
