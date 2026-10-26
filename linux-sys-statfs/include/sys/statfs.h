#ifndef SYS_STATFS_H
#define SYS_STATFS_H

/**
 * @file statfs.h
 * @brief Forwarding header to linux-sys-statfs.h or platform <sys/statvfs.h>.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#include "../linux-sys-statfs.h"
#elif defined(__APPLE__)
#include <sys/statvfs.h>
struct statfs {
  unsigned long f_type;
  unsigned long f_bsize;
  fsblkcnt_t f_blocks;
  fsblkcnt_t f_bfree;
  fsblkcnt_t f_bavail;
  fsfilcnt_t f_files;
  fsfilcnt_t f_ffree;
  unsigned long f_fsid;
  unsigned long f_namelen;
  unsigned long f_frsize;
  unsigned long f_flags;
};
static __inline int statfs(const char *path, struct statfs *buf) {
  struct statvfs vfs;
  if (statvfs(path, &vfs) != 0) return -1;
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_STATFS_H */
