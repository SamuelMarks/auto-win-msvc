#ifndef POSIX_SYS_MMAN_SYS_MMAN_H
#if defined(__GNUC__)
#endif
#define POSIX_SYS_MMAN_SYS_MMAN_H

/**
 * @file mman.h
 * @brief POSIX sys/mman.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/mman.h>
#else
#include <sys/mman.h>
#endif
#endif
#include "posix-mman.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_MMAN_SYS_MMAN_H */

/* API Contract symbols: mmap, munmap, mprotect, __dependencies__ */
