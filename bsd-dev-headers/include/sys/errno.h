#ifndef BSD_SYS_ERRNO_H
#define BSD_SYS_ERRNO_H

/**
 * @file errno.h
 * @brief BSD <sys/errno.h> wrapper forwarding to <errno.h> on Win32/DOS.
 */

/* clang-format off */
#if defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
#include <errno.h>
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/errno.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_ERRNO_H */
