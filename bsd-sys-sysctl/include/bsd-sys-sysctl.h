#ifndef BSD_SYS_SYSCTL_H
#define BSD_SYS_SYSCTL_H

/**
 * @file bsd-sys-sysctl.h
 * @brief Polyfill for BSD <sys/sysctl.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
/** @brief Generic error type code. */
typedef int error_type_t;
/** @brief Success indicator for error_type_t. */
#define ERR_NONE 0
#endif

/**
 * @brief Error codes returned by bsd-sys-sysctl functions.
 */
enum bsd_sys_sysctl_error_code {
  /** @brief Successful operation. */
  BSD_SYS_SYSCTL_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  BSD_SYS_SYSCTL_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed to function. */
  BSD_SYS_SYSCTL_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Initializes and validates the bsd-sys-sysctl module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return BSD_SYS_SYSCTL_SUCCESS on success, or an error code on failure.
 */
enum bsd_sys_sysctl_error_code bsd_sys_sysctl_init(int *out_status);

/**
 * @brief Retrieves the number of CPUs via sysctl.
 * @param[out] out_ncpu Pointer to an integer receiving the CPU count.
 * @return BSD_SYS_SYSCTL_SUCCESS on success, or an error code on failure.
 */
enum bsd_sys_sysctl_error_code bsd_sys_sysctl_get_ncpu(int *out_ncpu);

#ifndef CTL_KERN
/** @brief Kernel management control name. */
#define CTL_KERN 1
#endif
#ifndef CTL_VM
/** @brief Virtual memory management control name. */
#define CTL_VM 2
#endif
#ifndef CTL_VFS
/** @brief Virtual file system control name. */
#define CTL_VFS 3
#endif
#ifndef CTL_NET
/** @brief Networking control name. */
#define CTL_NET 4
#endif
#ifndef CTL_DEBUG
/** @brief Debugging control name. */
#define CTL_DEBUG 5
#endif
#ifndef CTL_HW
/** @brief Hardware control name. */
#define CTL_HW 6
#endif
#ifndef CTL_MACHDEP
/** @brief Machine dependent control name. */
#define CTL_MACHDEP 7
#endif
#ifndef CTL_USER
/** @brief User-level control name. */
#define CTL_USER 8
#endif
#ifndef CTL_P1003_1B
/** @brief POSIX 1003.1B control name. */
#define CTL_P1003_1B 9
#endif

/* CTL_HW identifiers */
#ifndef HW_MACHINE
/** @brief Hardware machine type. */
#define HW_MACHINE 1
#endif
#ifndef HW_MODEL
/** @brief Hardware model. */
#define HW_MODEL 2
#endif
#ifndef HW_NCPU
/** @brief Hardware number of CPUs. */
#define HW_NCPU 3
#endif
#ifndef HW_BYTEORDER
/** @brief Hardware byte order. */
#define HW_BYTEORDER 4
#endif
#ifndef HW_PHYSMEM
/** @brief Hardware physical memory. */
#define HW_PHYSMEM 5
#endif
#ifndef HW_USERMEM
/** @brief Hardware user memory. */
#define HW_USERMEM 6
#endif
#ifndef HW_PAGESIZE
/** @brief Hardware page size. */
#define HW_PAGESIZE 7
#endif
#ifndef HW_DISKNAMES
/** @brief Hardware disk names. */
#define HW_DISKNAMES 8
#endif
#ifndef HW_DISKSTATS
/** @brief Hardware disk statistics. */
#define HW_DISKSTATS 9
#endif
#ifndef HW_FLOATINGPT
/** @brief Hardware floating point presence. */
#define HW_FLOATINGPT 10
#endif
#ifndef HW_MACHINE_ARCH
/** @brief Hardware machine architecture. */
#define HW_MACHINE_ARCH 11
#endif
#ifndef HW_REALMEM
/** @brief Hardware real memory. */
#define HW_REALMEM 12
#endif
#ifndef HW_MEMSIZE
/** @brief Hardware memory size. */
#define HW_MEMSIZE 24
#endif

/* CTL_KERN identifiers */
#ifndef KERN_OSTYPE
/** @brief Operating system type. */
#define KERN_OSTYPE 1
#endif
#ifndef KERN_OSRELEASE
/** @brief Operating system release. */
#define KERN_OSRELEASE 2
#endif
#ifndef KERN_OSREV
/** @brief Operating system revision. */
#define KERN_OSREV 3
#endif
#ifndef KERN_VERSION
/** @brief Operating system version. */
#define KERN_VERSION 4
#endif
#ifndef KERN_MAXVNODES
/** @brief Maximum vnodes. */
#define KERN_MAXVNODES 5
#endif
#ifndef KERN_MAXPROC
/** @brief Maximum processes. */
#define KERN_MAXPROC 6
#endif
#ifndef KERN_MAXFILES
/** @brief Maximum open files. */
#define KERN_MAXFILES 7
#endif
#ifndef KERN_ARGMAX
/** @brief Maximum argument list size. */
#define KERN_ARGMAX 8
#endif
#ifndef KERN_SECURELVL
/** @brief Security level. */
#define KERN_SECURELVL 9
#endif
#ifndef KERN_HOSTNAME
/** @brief System host name. */
#define KERN_HOSTNAME 10
#endif
#ifndef KERN_HOSTID
/** @brief System host ID. */
#define KERN_HOSTID 11
#endif
#ifndef KERN_CLOCKRATE
/** @brief Clock rate. */
#define KERN_CLOCKRATE 12
#endif
#ifndef KERN_VNODE
/** @brief Vnode info. */
#define KERN_VNODE 13
#endif
#ifndef KERN_PROC
/** @brief Process info. */
#define KERN_PROC 14
#endif
#ifndef KERN_FILE
/** @brief File info. */
#define KERN_FILE 15
#endif
#ifndef KERN_PROF
/** @brief Profiling info. */
#define KERN_PROF 16
#endif
#ifndef KERN_POSIX1
/** @brief POSIX.1 version. */
#define KERN_POSIX1 17
#endif
#ifndef KERN_NGROUPS
/** @brief Number of groups. */
#define KERN_NGROUPS 18
#endif
#ifndef KERN_JOB_CONTROL
/** @brief Job control support. */
#define KERN_JOB_CONTROL 19
#endif
#ifndef KERN_SAVED_IDS
/** @brief Saved IDs support. */
#define KERN_SAVED_IDS 20
#endif
#ifndef KERN_BOOTTIME
/** @brief System boot time. */
#define KERN_BOOTTIME 21
#endif
#ifndef KERN_IPC
/** @brief IPC information. */
#define KERN_IPC 22
#endif

#ifndef KIPC_SOMAXCONN
/** @brief Maximum socket listen backlog. */
#define KIPC_SOMAXCONN 2
#endif

#ifndef VM_OVERCOMMIT
/** @brief Memory overcommit setting. */
#define VM_OVERCOMMIT 3
#endif

#if defined(_WIN32) || defined(_MSC_VER)

/**
 * @brief Implements the sysctl POSIX interface for Windows.
 * @param name Array of integers specifying the information to get/set.
 * @param namelen Number of integers in the name array.
 * @param oldp Pointer to buffer where information is returned.
 * @param oldlenp Pointer to size of oldp buffer.
 * @param newp Pointer to buffer with new information.
 * @param newlen Size of newp buffer.
 * @return ERR_NONE on success, or appropriate error code.
 */
error_type_t sysctl(const int *name, unsigned int namelen, void *oldp,
                    size_t *oldlenp, const void *newp, size_t newlen);

/**
 * @brief Implements the sysctlbyname interface for Windows.
 * @param name String specifying the information to get/set.
 * @param oldp Pointer to buffer where information is returned.
 * @param oldlenp Pointer to size of oldp buffer.
 * @param newp Pointer to buffer with new information.
 * @param newlen Size of newp buffer.
 * @return ERR_NONE on success, or appropriate error code.
 */
error_type_t sysctlbyname(const char *name, void *oldp, size_t *oldlenp,
                          const void *newp, size_t newlen);

#endif /* defined(_WIN32) || defined(_MSC_VER) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_SYSCTL_H */
