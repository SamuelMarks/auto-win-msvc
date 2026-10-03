#ifndef POSIX_LANGINFO_H
#define POSIX_LANGINFO_H

/**
 * @file posix-langinfo.h
 * @brief POSIX langinfo module interface and availability queries.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include "langinfo.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-langinfo availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_langinfo_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_LANGINFO_H */
