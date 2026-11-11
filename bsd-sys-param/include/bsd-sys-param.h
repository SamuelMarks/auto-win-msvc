#ifndef BSD_SYS_PARAM_H
#define BSD_SYS_PARAM_H

/**
 * @file bsd-sys-param.h
 * @brief Polyfill for BSD <sys/param.h> macros and utilities.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
#include <limits.h>
#include <sys/types.h>
#if defined(__has_include)
#if __has_include(<sys/cdefs.h>)
#include <sys/cdefs.h>
#endif
#endif
#if defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) || \
    defined(__DOS__) || defined(__MSDOS__)
#include <stdlib.h>
#else
#include <sys/param.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __FreeBSD_version
#define __FreeBSD_version 1500000
#endif
#ifndef BSD
#define BSD 199506
#endif
#ifndef BSD4_3
#define BSD4_3 1
#endif
#ifndef BSD4_4
#define BSD4_4 1
#endif
#ifndef MAXCOMLEN
#define MAXCOMLEN 19
#endif
#ifndef MAXLOGNAME
#define MAXLOGNAME 33
#endif
#ifndef MAXUPRC
#define MAXUPRC 512
#endif
#ifndef NCARGS
#define NCARGS 262144
#endif
#ifndef ARG_MAX
#define ARG_MAX NCARGS
#endif
#ifndef NGROUPS_MAX
#define NGROUPS_MAX 1023
#endif
#ifndef NGROUPS
#define NGROUPS (NGROUPS_MAX + 1)
#endif
#ifndef NOGROUP
#define NOGROUP 65535
#endif
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif
#ifndef SPECNAMELEN
#define SPECNAMELEN 255
#endif
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#ifndef PIPE_BUF
#define PIPE_BUF 4096
#endif
#ifndef NOFILE
#define NOFILE 2048
#endif
#ifndef MAXBSIZE
#define MAXBSIZE 65536
#endif
#ifndef MAXPHYS
#define MAXPHYS 131072
#endif
#ifndef DEV_BSHIFT
#define DEV_BSHIFT 9
#endif
#ifndef DEV_BSIZE
#define DEV_BSIZE (1 << DEV_BSHIFT)
#endif
#ifndef BLKDEV_IOSIZE
#define BLKDEV_IOSIZE 4096
#endif
#ifndef PAGE_SHIFT
#define PAGE_SHIFT 12
#endif
#ifndef PAGE_SIZE
#define PAGE_SIZE (1 << PAGE_SHIFT)
#endif
#ifndef PAGE_MASK
#define PAGE_MASK (PAGE_SIZE - 1)
#endif
#ifndef NBBY
#define NBBY 8
#endif
#ifndef NBPW
#define NBPW sizeof(int)
#endif

#ifndef nitems
#define nitems(x) (sizeof((x)) / sizeof((x)[0]))
#endif
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef howmany
#define howmany(x, y) (((x) + ((y) - 1)) / (y))
#endif
#ifndef roundup
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#endif
#ifndef roundup2
#define roundup2(x, y) (((x) + ((y) - 1)) & (~((y) - 1)))
#endif
#ifndef rounddown
#define rounddown(x, y) (((x) / (y)) * (y))
#endif
#ifndef rounddown2
#define rounddown2(x, y) ((x) & (~((y) - 1)))
#endif
#ifndef powerof2
#define powerof2(x) ((((x) - 1) & (x)) == 0)
#endif
#ifndef setbit
#define setbit(a, i)                                                           \
  (((unsigned char *)(a))[(i) / NBBY] |= (unsigned char)(1U << ((i) % NBBY)))
#endif
#ifndef clrbit
#define clrbit(a, i)                                                           \
  (((unsigned char *)(a))[(i) / NBBY] &= (unsigned char)~(1U << ((i) % NBBY)))
#endif
#ifndef isset
#define isset(a, i)                                                            \
  ((((const unsigned char *)(a))[(i) / NBBY] &                                 \
    (unsigned char)(1U << ((i) % NBBY))) != 0)
#endif
#ifndef isclr
#define isclr(a, i)                                                            \
  ((((const unsigned char *)(a))[(i) / NBBY] &                                 \
    (unsigned char)(1U << ((i) % NBBY))) == 0)
#endif
#ifndef btodb
#define btodb(bytes) ((bytes) >> DEV_BSHIFT)
#endif
#ifndef dbtob
#define dbtob(db) ((db) << DEV_BSHIFT)
#endif

/**
 * @brief Retrieves MAXPATHLEN configured for the platform.
 * @param[out] out_maxlen Pointer receiving MAXPATHLEN value.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code if out_maxlen is
 * NULL.
 */
auto_win_msvc_error_t bsd_sys_param_get_maxpathlen(size_t *out_maxlen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_PARAM_H */
