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
#else
/* Fallback, could cause include loop if -I is used instead of -isystem */
#include <fcntl.h>
#endif
#endif
#include "posix-core.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_MODE_T_DEFINED) && !defined(_MODE_T_DEFINED_) &&                 \
    !defined(_MODE_T_)
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
typedef unsigned short mode_t;
#endif

#ifndef creat
#define creat posix_creat
#endif
int posix_creat(const char *pathname, mode_t mode);

#ifndef fcntl
#define fcntl posix_fcntl
#endif
int posix_fcntl(int fd, int cmd, ...);

#ifndef open
#define open posix_open
#endif

#ifndef openat
#define openat posix_openat
#endif
int posix_openat(int dirfd, const char *pathname, int flags, ...);

#ifndef posix_fadvise
#define posix_fadvise posix_posix_fadvise
#endif
int posix_posix_fadvise(intptr_t fd, off_t offset, off_t len, int advice);

#ifndef posix_fallocate
#define posix_fallocate posix_posix_fallocate
#endif
int posix_posix_fallocate(intptr_t fd, off_t offset, off_t len);

#define __dependencies__ posix - core

#define __include_next__

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FCNTL_H */
