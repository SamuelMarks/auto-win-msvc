#ifndef REGEX_H
#define REGEX_H

/**
 * @file regex.h
 * @brief Forwarding header to posix-regex.h.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#include "posix-regex.h"
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <regex.h>
#else
#include <regex.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* REGEX_H */
