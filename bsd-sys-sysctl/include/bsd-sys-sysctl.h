#ifndef BSD_SYS_SYSCTL_H
#define BSD_SYS_SYSCTL_H

/**
 * @file bsd-sys-sysctl.h
 * @brief Polyfill for BSD <sys/sysctl.h>.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
#include <sys/types.h>
#if (defined(__APPLE__) && (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))) || \
    defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/sysctl.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
#endif

/**
 * @brief Initializes and validates the bsd-sys-sysctl module.
 * @param[out] out_status Pointer receiving status (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code if NULL.
 */
auto_win_msvc_error_t bsd_sys_sysctl_init(int *out_status);

/**
 * @brief Retrieves the number of online processors via sysctl/system APIs.
 * @param[out] out_ncpu Pointer receiving CPU count.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code if NULL.
 */
auto_win_msvc_error_t bsd_sys_sysctl_get_ncpu(int *out_ncpu);

#ifndef CTL_KERN
#define CTL_KERN 1
#endif
#ifndef CTL_VM
#define CTL_VM 2
#endif
#ifndef CTL_VFS
#define CTL_VFS 3
#endif
#ifndef CTL_NET
#define CTL_NET 4
#endif
#ifndef CTL_DEBUG
#define CTL_DEBUG 5
#endif
#ifndef CTL_HW
#define CTL_HW 6
#endif
#ifndef CTL_MACHDEP
#define CTL_MACHDEP 7
#endif
#ifndef CTL_USER
#define CTL_USER 8
#endif
#ifndef CTL_P1003_1B
#define CTL_P1003_1B 9
#endif

#ifndef HW_MACHINE
#define HW_MACHINE 1
#endif
#ifndef HW_MODEL
#define HW_MODEL 2
#endif
#ifndef HW_NCPU
#define HW_NCPU 3
#endif
#ifndef HW_BYTEORDER
#define HW_BYTEORDER 4
#endif
#ifndef HW_PHYSMEM
#define HW_PHYSMEM 5
#endif
#ifndef HW_USERMEM
#define HW_USERMEM 6
#endif
#ifndef HW_PAGESIZE
#define HW_PAGESIZE 7
#endif
#ifndef HW_DISKNAMES
#define HW_DISKNAMES 8
#endif
#ifndef HW_DISKSTATS
#define HW_DISKSTATS 9
#endif
#ifndef HW_FLOATINGPT
#define HW_FLOATINGPT 10
#endif
#ifndef HW_MACHINE_ARCH
#define HW_MACHINE_ARCH 11
#endif
#ifndef HW_REALMEM
#define HW_REALMEM 12
#endif
#ifndef HW_MEMSIZE
#define HW_MEMSIZE 24
#endif

#ifndef KERN_OSTYPE
#define KERN_OSTYPE 1
#endif
#ifndef KERN_OSRELEASE
#define KERN_OSRELEASE 2
#endif
#ifndef KERN_OSREV
#define KERN_OSREV 3
#endif
#ifndef KERN_VERSION
#define KERN_VERSION 4
#endif
#ifndef KERN_MAXVNODES
#define KERN_MAXVNODES 5
#endif
#ifndef KERN_MAXPROC
#define KERN_MAXPROC 6
#endif
#ifndef KERN_MAXFILES
#define KERN_MAXFILES 7
#endif
#ifndef KERN_ARGMAX
#define KERN_ARGMAX 8
#endif
#ifndef KERN_SECURELVL
#define KERN_SECURELVL 9
#endif
#ifndef KERN_HOSTNAME
#define KERN_HOSTNAME 10
#endif
#ifndef KERN_HOSTID
#define KERN_HOSTID 11
#endif
#ifndef KERN_CLOCKRATE
#define KERN_CLOCKRATE 12
#endif
#ifndef KERN_VNODE
#define KERN_VNODE 13
#endif
#ifndef KERN_PROC
#define KERN_PROC 14
#endif
#ifndef KERN_FILE
#define KERN_FILE 15
#endif
#ifndef KERN_PROF
#define KERN_PROF 16
#endif
#ifndef KERN_POSIX1
#define KERN_POSIX1 17
#endif
#ifndef KERN_NGROUPS
#define KERN_NGROUPS 18
#endif
#ifndef KERN_JOB_CONTROL
#define KERN_JOB_CONTROL 19
#endif
#ifndef KERN_SAVED_IDS
#define KERN_SAVED_IDS 20
#endif
#ifndef KERN_BOOTTIME
#define KERN_BOOTTIME 21
#endif
#ifndef KERN_IPC
#define KERN_IPC 22
#endif
#ifndef KERN_OSRELDATE
#define KERN_OSRELDATE 24
#endif
#ifndef KERN_PROC_ALL
#define KERN_PROC_ALL 0
#endif
#ifndef KERN_PROC_PID
#define KERN_PROC_PID 1
#endif
#ifndef KERN_PROC_PGRP
#define KERN_PROC_PGRP 2
#endif
#ifndef KERN_PROC_SESSION
#define KERN_PROC_SESSION 3
#endif
#ifndef KERN_PROC_TTY
#define KERN_PROC_TTY 4
#endif
#ifndef KERN_PROC_UID
#define KERN_PROC_UID 5
#endif
#ifndef KERN_PROC_RUID
#define KERN_PROC_RUID 6
#endif
#ifndef KERN_PROC_ARGS
#define KERN_PROC_ARGS 7
#endif
#ifndef KERN_PROC_PATHNAME
#define KERN_PROC_PATHNAME 12
#endif
#ifndef KERN_PROC_INC_THREAD
#define KERN_PROC_INC_THREAD 0x10
#endif

#ifndef KIPC_SOMAXCONN
#define KIPC_SOMAXCONN 2
#endif
#ifndef VM_TOTAL
#define VM_TOTAL 1
#endif
#ifndef VM_OVERCOMMIT
#define VM_OVERCOMMIT 3
#endif
#ifndef CTL_MAXNAME
#define CTL_MAXNAME 24
#endif

#if (!defined(__APPLE__) ||                                                    \
     (defined(_POSIX_C_SOURCE) && !defined(_DARWIN_C_SOURCE))) &&              \
    !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)

/** @brief Retrieve or set system information by MIB array. */
int sysctl(const int *name, unsigned int namelen, void *oldp, size_t *oldlenp,
           const void *newp, size_t newlen);
/** @brief Retrieve or set system information by MIB name string. */
int sysctlbyname(const char *name, void *oldp, size_t *oldlenp,
                 const void *newp, size_t newlen);
/** @brief Translate MIB name string to integer MIB vector. */
int sysctlnametomib(const char *name, int *mibp, size_t *sizep);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_SYSCTL_H */
