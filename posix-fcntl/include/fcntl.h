#ifndef FCNTL_H
#define FCNTL_H

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <fcntl.h>
#else
#include <fcntl.h>
#endif
#else
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/fcntl.h>
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <fcntl.h>
#else
/* Fallback, could cause include loop if -I is used instead of -isystem */
#include <fcntl.h>
#endif
#endif
#include "posix-core.h"
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _OFF_T_POSIX_DEFINED
#define _OFF_T_POSIX_DEFINED
#if defined(_MSC_VER) && !defined(off_t)
typedef long off_t;
#endif
#endif

#if defined(_WIN32) && !defined(_MODE_T_DEFINED) &&                            \
    !defined(_MODE_T_DEFINED_) && !defined(_MODE_T_) &&                        \
    !defined(__mode_t_defined)
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
#define __mode_t_defined
typedef unsigned short mode_t;
#endif

#if defined(_WIN32)
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 0
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 0x4000
#endif
#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK
#endif
#ifndef O_SYNC
#define O_SYNC 0
#endif
#ifndef O_DSYNC
#define O_DSYNC 0
#endif
#ifndef O_NOCTTY
#define O_NOCTTY 0
#endif
#ifndef creat
#define creat posix_creat
#endif
/** @brief Creates a new file or rewrites an existing one. */
int posix_creat(const char *pathname, mode_t mode);

/** @brief Manipulates file descriptor properties. */
int fcntl(intptr_t fd, int cmd, ...);

#ifndef open
#define open posix_open
#endif

#ifndef openat
#define openat posix_openat
#endif
/** @brief Opens a file relative to a directory file descriptor. */
int posix_openat(int dirfd, const char *pathname, int flags, ...);

#ifndef posix_fadvise
#define posix_fadvise posix_posix_fadvise
#endif
/** @brief Predeclares an access pattern for file data. */
int posix_posix_fadvise(intptr_t fd, off_t offset, off_t len, int advice);

#ifndef posix_fallocate
#define posix_fallocate posix_posix_fallocate
#endif
/** @brief Ensures disk space is allocated for a file. */
int posix_posix_fallocate(intptr_t fd, off_t offset, off_t len);
#endif

#define __dependencies__ posix - core

#define __include_next__

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FCNTL_H */
