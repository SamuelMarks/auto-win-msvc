#ifndef POSIX_DLFCN_DLFCN_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_DLFCN_DLFCN_H

/**
 * @file dlfcn.h
 * @brief Standard POSIX dlfcn.h header wrapper.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <dlfcn.h>
#else
#include <dlfcn.h>
#endif
#else
#include "posix-dlfcn.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_DLFCN_DLFCN_H */
