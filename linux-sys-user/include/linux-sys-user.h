#ifndef LINUX_SYS_USER_H
#define LINUX_SYS_USER_H

/**
 * @file linux-sys-user.h
 * @brief Polyfill for Linux <sys/user.h>.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes and validates the linux-sys-user module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t linux_sys_user_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_SYS_USER_H */
