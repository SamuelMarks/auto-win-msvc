/* inttypes.h - Strict C89 Header */
#ifndef INTTYPES_WRAPPER_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define INTTYPES_WRAPPER_H

/**
 * @file inttypes.h
 * @brief Standard POSIX inttypes.h header wrapper for MSVC.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <inttypes.h>
#else
#include <inttypes.h>
#endif
#else
#include "posix-inttypes.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INTTYPES_WRAPPER_H */
