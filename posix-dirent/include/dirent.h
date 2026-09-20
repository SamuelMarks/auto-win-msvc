#ifndef POSIX_DIRENT_DIRENT_H
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
#define POSIX_DIRENT_DIRENT_H

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <dirent.h>
#else
#include <dirent.h>
#endif
#else
#include "posix-dirent.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_DIRENT_DIRENT_H */
