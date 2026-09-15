#ifndef LINUX_SYS_STATFS_H
#define LINUX_SYS_STATFS_H

/**
 * @file linux-sys-statfs.h
 * @brief Polyfill for Linux <sys/statfs.h> filesystem statistics.
 */

/* clang-format off */
#include <stddef.h>
#if defined(__linux__) || defined(__CYGWIN__)
#include <sys/statfs.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by linux-sys-statfs functions.
 */
enum linux_sys_statfs_error_code {
  /** @brief Successful operation. */
  LINUX_SYS_STATFS_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_SYS_STATFS_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed to function. */
  LINUX_SYS_STATFS_ERROR_INVALID_ARGUMENT = 2,
  /** @brief File system operation failed. */
  LINUX_SYS_STATFS_ERROR_IO = 3
};

/**
 * @brief Initializes and validates the linux-sys-statfs module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_SYS_STATFS_SUCCESS on success, or an error code on failure.
 */
enum linux_sys_statfs_error_code linux_sys_statfs_init(int *out_status);

#if defined(_MSC_VER)

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

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_SYS_STATFS_H */
