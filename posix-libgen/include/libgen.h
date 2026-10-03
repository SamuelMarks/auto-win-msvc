#ifndef LIBGEN_H
#define LIBGEN_H

/**
 * @file libgen.h
 * @brief Forwarding header to posix-libgen.h.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#include "posix-libgen.h"
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <libgen.h>
#else
#include <libgen.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBGEN_H */
