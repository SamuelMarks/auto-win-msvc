/* sys/stat.h - Strict C89 Header */
#ifndef SYS_STAT_WRAPPER_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define SYS_STAT_WRAPPER_H

/**
 * @file stat.h
 * @brief Standard POSIX sys/stat.h header wrapper for MSVC.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/stat.h>
#else
#include <sys/stat.h>
#endif
#else
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/sys/stat.h>
#elif defined(_MSC_VER)
#include <../include/sys/stat.h>
#elif defined(__GNUC__) || defined(__clang__)
#include_next <sys/stat.h>
#else
#include <sys/stat.h>
#endif
#include "posix-stat.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_MSC_VER)

#ifndef S_IRUSR
/** @brief Read permission, owner. */
#define S_IRUSR 0400
#endif
#ifndef S_IWUSR
/** @brief Write permission, owner. */
#define S_IWUSR 0200
#endif
#ifndef S_IXUSR
/** @brief Execute permission, owner. */
#define S_IXUSR 0100
#endif
#ifndef S_IRWXU
/** @brief Read, write, execute permissions, owner. */
#define S_IRWXU 0700
#endif

#ifndef S_IRGRP
/** @brief Read permission, group. */
#define S_IRGRP 0040
#endif
#ifndef S_IWGRP
/** @brief Write permission, group. */
#define S_IWGRP 0020
#endif
#ifndef S_IXGRP
/** @brief Execute permission, group. */
#define S_IXGRP 0010
#endif
#ifndef S_IRWXG
/** @brief Read, write, execute permissions, group. */
#define S_IRWXG 0070
#endif

#ifndef S_IROTH
/** @brief Read permission, others. */
#define S_IROTH 0004
#endif
#ifndef S_IWOTH
/** @brief Write permission, others. */
#define S_IWOTH 0002
#endif
#ifndef S_IXOTH
/** @brief Execute permission, others. */
#define S_IXOTH 0001
#endif
#ifndef S_IRWXO
/** @brief Read, write, execute permissions, others. */
#define S_IRWXO 0007
#endif

#endif /* defined(_WIN32) || defined(_MSC_VER) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_STAT_WRAPPER_H */
