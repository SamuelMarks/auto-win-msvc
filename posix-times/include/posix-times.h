
#ifndef POSIX_TIMES_H
#define POSIX_TIMES_H

/**
 * @file posix-times.h
 * @brief POSIX times and execution timing module.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include "sys/times.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-times availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_times_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_TIMES_H */
