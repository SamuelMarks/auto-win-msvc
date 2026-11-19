#ifndef SYS_STATFS_H
#define SYS_STATFS_H

/**
 * @file statfs.h
 * @brief Forwarding header to linux-sys-statfs.h or platform <sys/statvfs.h>.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||            \
    defined(__MSDOS__) || defined(__DOS__)
#include "../linux-sys-statfs.h"
#elif defined(__APPLE__)
#include <sys/statvfs.h>
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/statfs.h>
#else
#include <sys/statfs.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__APPLE__) && !defined(_MSC_VER) && !defined(_WIN32)
/**
 * @brief Linux-compatible statfs structure mapped over POSIX statvfs on macOS.
 */
struct statfs {
  /** @brief Type of filesystem. */
  unsigned long f_type;
  /** @brief Optimal transfer block size. */
  unsigned long f_bsize;
  /** @brief Total data blocks in filesystem. */
  fsblkcnt_t f_blocks;
  /** @brief Free blocks in filesystem. */
  fsblkcnt_t f_bfree;
  /** @brief Free blocks available to unprivileged user. */
  fsblkcnt_t f_bavail;
  /** @brief Total file nodes in filesystem. */
  fsfilcnt_t f_files;
  /** @brief Free file nodes in filesystem. */
  fsfilcnt_t f_ffree;
  /** @brief Filesystem ID. */
  unsigned long f_fsid;
  /** @brief Maximum length of filenames. */
  unsigned long f_namelen;
  /** @brief Fragment size. */
  unsigned long f_frsize;
  /** @brief Mount flags of filesystem. */
  unsigned long f_flags;
};

/**
 * @brief Queries filesystem statistics for path via POSIX statvfs on macOS.
 * @param[in] path Filesystem path.
 * @param[out] buf Output statfs structure.
 * @return 0 on success, or -1 on error.
 */
static __inline int statfs(const char *path, struct statfs *buf) {
  struct statvfs vfs;
  if (statvfs(path, &vfs) != 0) {
    return -1;
  }
  buf->f_type = 0;
  buf->f_bsize = vfs.f_frsize;
  buf->f_blocks = vfs.f_blocks;
  buf->f_bfree = vfs.f_bfree;
  buf->f_bavail = vfs.f_bavail;
  buf->f_files = vfs.f_files;
  buf->f_ffree = vfs.f_ffree;
  buf->f_fsid = vfs.f_fsid;
  buf->f_namelen = vfs.f_namemax;
  buf->f_frsize = vfs.f_frsize;
  buf->f_flags = vfs.f_flag;
  return 0;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_STATFS_H */
