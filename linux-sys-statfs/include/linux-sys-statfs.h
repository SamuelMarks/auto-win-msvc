#ifndef LINUX_SYS_STATFS_H
#define LINUX_SYS_STATFS_H

/**
 * @file linux-sys-statfs.h
 * @brief Polyfill for Linux <sys/statfs.h> and POSIX <sys/statvfs.h> filesystem
 * statistics.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
#if defined(__linux__) || defined(__CYGWIN__)
#include <sys/statfs.h>
#endif
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(__CYGWIN__)
#include <sys/statvfs.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes and validates the linux-sys-statfs module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t linux_sys_statfs_init(int *out_status);

#if defined(_MSC_VER) || defined(_WIN32)

/**
 * @brief Structure representing file system statistics.
 */
struct statfs {
  /** @brief Type of filesystem. */
  long f_type;
  /** @brief Optimal transfer block size. */
  long f_bsize;
  /** @brief Total data blocks in filesystem. */
  long f_blocks;
  /** @brief Free blocks in filesystem. */
  long f_bfree;
  /** @brief Free blocks available to unprivileged user. */
  long f_bavail;
  /** @brief Total file nodes in filesystem. */
  long f_files;
  /** @brief Free file nodes in filesystem. */
  long f_ffree;
  /** @brief Filesystem ID. */
  long f_fsid[2];
  /** @brief Maximum length of filenames. */
  long f_namelen;
  /** @brief Fragment size. */
  long f_frsize;
  /** @brief Mount flags of filesystem. */
  long f_flags;
  /** @brief Padding bytes reserved. */
  long f_spare[4];
};

/**
 * @brief Structure representing POSIX virtual file system statistics.
 */
struct statvfs {
  /** @brief File system block size. */
  unsigned long f_bsize;
  /** @brief Fundamental file system block size. */
  unsigned long f_frsize;
  /** @brief Total number of blocks on file system in units of f_frsize. */
  unsigned long long f_blocks;
  /** @brief Total number of free blocks. */
  unsigned long long f_bfree;
  /** @brief Number of free blocks available to unprivileged process. */
  unsigned long long f_bavail;
  /** @brief Total number of file serial numbers. */
  unsigned long long f_files;
  /** @brief Total number of free file serial numbers. */
  unsigned long long f_ffree;
  /** @brief Number of file serial numbers available to unprivileged process. */
  unsigned long long f_favail;
  /** @brief File system ID. */
  unsigned long f_fsid;
  /** @brief Bit mask of f_flag values. */
  unsigned long f_flag;
  /** @brief Maximum filename length. */
  unsigned long f_namemax;
};

/**
 * @brief Gets file system statistics for a path.
 * @param path File system path.
 * @param buf Output buffer for statfs.
 * @return 0 on success, or -1 on failure with errno set.
 */
int statfs(const char *path, struct statfs *buf);

/**
 * @brief Gets file system statistics for an open file descriptor.
 * @param fd Open file descriptor.
 * @param buf Output buffer for statfs.
 * @return 0 on success, or -1 on failure with errno set.
 */
int fstatfs(int fd, struct statfs *buf);

/**
 * @brief Gets virtual file system statistics for a path (POSIX statvfs).
 * @param path File system path.
 * @param buf Output buffer for statvfs.
 * @return 0 on success, or -1 on failure with errno set.
 */
int statvfs(const char *path, struct statvfs *buf);

/**
 * @brief Gets virtual file system statistics for an open file descriptor (POSIX
 * fstatvfs).
 * @param fd Open file descriptor.
 * @param buf Output buffer for statvfs.
 * @return 0 on success, or -1 on failure with errno set.
 */
int fstatvfs(int fd, struct statvfs *buf);

#endif /* _MSC_VER || _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_SYS_STATFS_H */
