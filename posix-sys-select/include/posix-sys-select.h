#ifndef POSIX_SYS_SELECT_H
#define POSIX_SYS_SELECT_H

/**
 * @file posix-sys-select.h
 * @brief POSIX sys/select.h compatibility layer.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <sys/select.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-sys-select availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_sys_select_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_SELECT_H */
