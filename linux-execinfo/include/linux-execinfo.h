#ifndef LINUX_EXECINFO_H
#define LINUX_EXECINFO_H

/**
 * @file linux-execinfo.h
 * @brief Polyfill for GNU <execinfo.h> backtrace utilities.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
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
 * @brief Initializes and validates the linux-execinfo module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t linux_execinfo_init(int *out_status);

/**
 * @brief Captures a stack backtrace.
 * @param buffer Array of void pointers to receive addresses.
 * @param size Maximum number of frames to capture.
 * @param[out] captured Pointer to receive the number of captured frames.
 * @return ERR_NONE on success, or non-zero on error.
 */
error_type_t backtrace(void **buffer, int size, int *captured);

/**
 * @brief Translates backtrace addresses into human-readable strings.
 * @param buffer Array of backtrace addresses.
 * @param size Number of addresses in buffer.
 * @return Array of string pointers allocated with malloc, or NULL on error.
 */
char **backtrace_symbols(void *const *buffer, int size);

/**
 * @brief Writes human-readable backtrace strings to a file descriptor.
 * @param buffer Array of backtrace addresses.
 * @param size Number of addresses in buffer.
 * @param fd File descriptor to write to.
 * @return ERR_NONE on success, or non-zero on error.
 */
error_type_t backtrace_symbols_fd(void *const *buffer, int size, int fd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_EXECINFO_H */
