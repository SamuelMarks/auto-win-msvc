#ifndef LINUX_SYS_PROCFS_H
#define LINUX_SYS_PROCFS_H

/**
 * @file linux-sys-procfs.h
 * @brief Polyfill for Linux <sys/procfs.h>.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

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
