#ifndef BSD_SYS_FILE_H
#define BSD_SYS_FILE_H

/**
 * @file bsd-sys-file.h
 * @brief Polyfill for BSD <sys/file.h> providing file locking functionality.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#if defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||                defined(__DOS__)
#include <stddef.h>
#else
#include <sys/file.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOCK_SH
/** @brief Shared file lock. */
#define LOCK_SH 1
#endif

#ifndef LOCK_EX
/** @brief Exclusive file lock. */
#define LOCK_EX 2
#endif

#ifndef LOCK_NB
/** @brief Do not block when locking (non-blocking). */
#define LOCK_NB 4
#endif

#ifndef LOCK_UN
/** @brief Unlock file. */
#define LOCK_UN 8
#endif

/**
 * @brief Apply or remove an advisory lock on the open file.
 * @param fd The file descriptor to lock.
 * @param operation Lock operation (LOCK_SH, LOCK_EX, LOCK_NB, LOCK_UN).
 * @return 0 on success, -1 on error with errno set.
 */
int posix_flock(int fd, int operation);

#if defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||            \
    defined(__DOS__)
#ifndef flock
#define flock posix_flock
#endif
#endif

/**
 * @brief Initializes and validates the bsd-sys-file module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t bsd_sys_file_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_FILE_H */
