#ifndef POSIX_SPAWN_SPAWN_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_SPAWN_SPAWN_H

/**
 * @file spawn.h
 * @brief POSIX spawn.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <spawn.h>
#else
#include <spawn.h>
#endif
#else
#include "posix-spawn.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SPAWN_SPAWN_H */
