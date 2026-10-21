#ifndef LINUX_SYS_PROCFS_H
#define LINUX_SYS_PROCFS_H

/**
 * @file linux-sys-procfs.h
 * @brief Polyfill for Linux <sys/procfs.h> and <sys/sysinfo.h>.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__linux__)
#if !defined(_STRUCT_SYSINFO_DEFINED) && !defined(_SYSINFO_DEFINED)
#define _STRUCT_SYSINFO_DEFINED 1
#define _SYSINFO_DEFINED 1

/**
 * @brief Linux-compatible system statistics structure for non-Linux targets.
 */
struct sysinfo {
  long uptime;             /**< Seconds since boot */
  unsigned long loads[3];  /**< 1, 5, and 15 minute load averages */
  unsigned long totalram;  /**< Total usable main memory size */
  unsigned long freeram;   /**< Available memory size */
  unsigned long sharedram; /**< Amount of shared memory */
  unsigned long bufferram; /**< Memory used by buffers */
  unsigned long totalswap; /**< Total swap space size */
  unsigned long freeswap;  /**< Swap space still available */
  unsigned short procs;    /**< Number of current processes */
  unsigned long totalhigh; /**< Total high memory size */
  unsigned long freehigh;  /**< Available high memory size */
  unsigned int mem_unit;   /**< Memory unit size in bytes */
  char _f[20 - 2 * sizeof(long) - sizeof(int) > 0
              ? 20 - 2 * sizeof(long) - sizeof(int)
              : 1]; /**< Padding for libc5 */
};

/**
 * @brief Populates system memory and uptime statistics.
 * @param[out] info Pointer to sysinfo structure receiving system metrics.
 * @return 0 on success, or -1 on failure with errno set.
 */
int sysinfo(struct sysinfo *info);

#endif /* _STRUCT_SYSINFO_DEFINED */
#endif /* !__linux__ */

/**
 * @brief Initializes and validates the linux-sys-procfs module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t linux_sys_procfs_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_SYS_PROCFS_H */
