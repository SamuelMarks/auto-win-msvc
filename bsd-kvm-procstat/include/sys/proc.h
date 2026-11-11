#ifndef SYS_PROC_H
#define SYS_PROC_H

/**
 * @file proc.h
 * @brief FreeBSD kernel process status (<sys/proc.h>) definitions.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/time.h>
#if (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#if !defined(__APPLE__) || !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#if defined(__has_include_next)
#if __has_include_next(<sys/proc.h>)
#pragma GCC system_header
#include_next <sys/proc.h>
#endif
#endif
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/* Process status values */
#ifndef SIDL
#define SIDL 1
#endif
#ifndef SRUN
#define SRUN 2
#endif
#ifndef SSLEEP
#define SSLEEP 3
#endif
#ifndef SSTOP
#define SSTOP 4
#endif
#ifndef SZOMB
#define SZOMB 5
#endif
#ifndef SWAIT
#define SWAIT 6
#endif
#ifndef SLOCK
#define SLOCK 7
#endif

/* Process flags */
#ifndef P_ADVLOCK
#define P_ADVLOCK 0x00001
#endif
#ifndef P_CONTROLT
#define P_CONTROLT 0x00002
#endif
#ifndef P_KPROC
#define P_KPROC 0x00004
#endif
#ifndef P_SYSTEM
#define P_SYSTEM 0x00200
#endif
#ifndef P_INMEM
#define P_INMEM 0x10000000
#endif

/** @brief CPU states for cp_time sysctl. */
#ifndef CP_USER
#define CP_USER 0
#define CP_NICE 1
#define CP_SYS 2
#define CP_INTR 3
#define CP_IDLE 4
#define CPUSTATES 5
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_PROC_H */
