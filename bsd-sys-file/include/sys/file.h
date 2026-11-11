#ifndef SYS_FILE_H
#define SYS_FILE_H

/**
 * @file file.h
 * @brief Polyfill forwarding header for BSD <sys/file.h>.
 */

/* clang-format off */
#if !defined(_MSC_VER) && !defined(_WIN32) && !defined(__WATCOMC__) && \
    !defined(__DOS__) && !defined(__MSDOS__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/file.h>
#endif
#endif
#include "../bsd-sys-file.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_FILE_H */
