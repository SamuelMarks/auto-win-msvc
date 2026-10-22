/* posix-stat.h - Strict C89 Header */
#ifndef POSIX_STAT_H
#define POSIX_STAT_H

/**
 * @file posix-stat.h
 * @brief Strict C89 POSIX sys/stat.h header and polyfills for MSVC.
 */

#if !defined(_WIN32) && !defined(_MSC_VER) && !defined(__APPLE__)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#endif

/* clang-format off */
#include <errno.h>
#include <stddef.h>
#include <time.h>

#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#define stat _crt_orig_stat
#define fstat _crt_orig_fstat
#include <../ucrt/sys/stat.h>
#undef stat
#undef fstat
#include <../ucrt/sys/types.h>
#include <../ucrt/io.h>
#elif defined(_MSC_VER)
#define stat _crt_orig_stat
#define fstat _crt_orig_fstat
#include <../include/sys/stat.h>
#undef stat
#undef fstat
#include <../include/sys/types.h>
#include <io.h>
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#if !defined(_MSC_VER) && !defined(_WIN32)
#include <unistd.h>
#endif
#endif
#include "auto-win-msvc-error.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-stat module availability and status.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_stat_get_info(int *out_available);

#if !defined(_TIMESPEC_DEFINED) && !defined(HAVE_STRUCT_TIMESPEC) &&           \
    !defined(__timespec_defined) && !defined(_STRUCT_TIMESPEC) &&              \
    !defined(_SYS__TIMESPEC_H_) && !defined(__CYGWIN__)
#if defined(_MSC_VER) && _MSC_VER >= 1900
/* VS2015+ provides struct timespec in time.h */
#else
#define _TIMESPEC_DEFINED
#define HAVE_STRUCT_TIMESPEC 1
#define __timespec_defined 1
#define _STRUCT_TIMESPEC 1
/**
 * @brief Structure for representing time with nanosecond precision.
 */
struct timespec {
  time_t tv_sec; /**< Seconds. */
  long tv_nsec;  /**< Nanoseconds. */
};
#endif
#endif

#if defined(_WIN32)
/* Standard Types (if missing) */
#if !defined(_MODE_T_DEFINED) && !defined(_MODE_T_DEFINED_) &&                 \
    !defined(_MODE_T_) && !defined(HAVE_MODE_T)
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
#define HAVE_MODE_T 1
/** @brief POSIX file mode type. */
typedef unsigned short mode_t;
#endif
#endif /* _WIN32 */

#ifndef S_IFMT
#ifdef _S_IFMT
/** @brief Bit mask for the file type bit field. */
#define S_IFMT _S_IFMT
#else
/** @brief Bit mask for the file type bit field. */
#define S_IFMT 0170000
#endif
#endif
#ifndef S_IFDIR
#ifdef _S_IFDIR
/** @brief Directory file type. */
#define S_IFDIR _S_IFDIR
#else
/** @brief Directory file type. */
#define S_IFDIR 0040000
#endif
#endif
#ifndef S_IFCHR
#ifdef _S_IFCHR
/** @brief Character device file type. */
#define S_IFCHR _S_IFCHR
#else
/** @brief Character device file type. */
#define S_IFCHR 0020000
#endif
#endif
#ifndef S_IFREG
#ifdef _S_IFREG
/** @brief Regular file type. */
#define S_IFREG _S_IFREG
#else
/** @brief Regular file type. */
#define S_IFREG 0100000
#endif
#endif
#ifndef S_IFIFO
#ifdef _S_IFIFO
/** @brief FIFO or pipe file type. */
#define S_IFIFO _S_IFIFO
#else
/** @brief FIFO or pipe file type. */
#define S_IFIFO 0010000
#endif
#endif
#ifndef S_IFBLK
#ifdef _S_IFBLK
/** @brief Block device file type. */
#define S_IFBLK _S_IFBLK
#else
/** @brief Block device file type. */
#define S_IFBLK 0060000
#endif
#endif
#ifndef S_IFLNK
/** @brief Symbolic link file type. */
#define S_IFLNK 0120000
#endif
#ifndef S_IFSOCK
/** @brief Socket file type. */
#define S_IFSOCK 0140000
#endif

#ifndef S_IRUSR
#ifdef _S_IREAD
/** @brief Read permission, owner. */
#define S_IRUSR _S_IREAD
#else
/** @brief Read permission, owner. */
#define S_IRUSR 0400
#endif
#endif
#ifndef S_IWUSR
#ifdef _S_IWRITE
/** @brief Write permission, owner. */
#define S_IWUSR _S_IWRITE
#else
/** @brief Write permission, owner. */
#define S_IWUSR 0200
#endif
#endif
#ifndef S_IXUSR
#ifdef _S_IEXEC
/** @brief Execute permission, owner. */
#define S_IXUSR _S_IEXEC
#else
/** @brief Execute permission, owner. */
#define S_IXUSR 0100
#endif
#endif

#ifndef S_IFLNK
/** @brief Symbolic link file type. */
#define S_IFLNK 0120000
#endif
#ifndef S_IFSOCK
/** @brief Socket file type. */
#define S_IFSOCK 0140000
#endif
#ifndef S_IFBLK
/** @brief Block special file type. */
#define S_IFBLK 0060000
#endif

#ifndef S_IRWXU
/** @brief Read, write, execute permissions, owner. */
#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
#endif

#ifndef S_IRGRP
/** @brief Read permission, group. */
#define S_IRGRP (S_IRUSR >> 3)
#endif
#ifndef S_IWGRP
/** @brief Write permission, group. */
#define S_IWGRP (S_IWUSR >> 3)
#endif
#ifndef S_IXGRP
/** @brief Execute permission, group. */
#define S_IXGRP (S_IXUSR >> 3)
#endif
#ifndef S_IRWXG
/** @brief Read, write, execute permissions, group. */
#define S_IRWXG (S_IRWXU >> 3)
#endif

#ifndef S_IROTH
/** @brief Read permission, others. */
#define S_IROTH (S_IRGRP >> 3)
#endif
#ifndef S_IWOTH
/** @brief Write permission, others. */
#define S_IWOTH (S_IWGRP >> 3)
#endif
#ifndef S_IXOTH
/** @brief Execute permission, others. */
#define S_IXOTH (S_IXGRP >> 3)
#endif
#ifndef S_IRWXO
/** @brief Read, write, execute permissions, others. */
#define S_IRWXO (S_IRWXG >> 3)
#endif

#ifndef S_ISUID
/** @brief Set user ID on execution. */
#define S_ISUID 04000
#endif
#ifndef S_ISGID
/** @brief Set group ID on execution. */
#define S_ISGID 02000
#endif
#ifndef S_ISVTX
/** @brief Save swapped text after use (sticky bit). */
#define S_ISVTX 01000
#endif

#ifndef S_ISDIR
/** @brief Test for directory. */
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISCHR
/** @brief Test for character device. */
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#endif
#ifndef S_ISREG
/** @brief Test for regular file. */
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISFIFO
/** @brief Test for FIFO or pipe. */
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#endif
#ifndef S_ISLNK
/** @brief Test for symbolic link. */
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif
#ifndef S_ISSOCK
/** @brief Test for socket. */
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#endif
#ifndef S_ISBLK
/** @brief Test for block device. */
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#endif

#ifndef UTIME_NOW
/** @brief Special value for utimensat: set to current time. */
#define UTIME_NOW ((1L << 30) - 1L)
#endif
#ifndef UTIME_OMIT
/** @brief Special value for utimensat: leave timestamp unchanged. */
#define UTIME_OMIT ((1L << 30) - 2L)
#endif

#ifndef AT_FDCWD
/** @brief Special value indicating current working directory. */
#if defined(__CYGWIN__) || defined(__APPLE__)
#define AT_FDCWD -2
#else
#define AT_FDCWD -100
#endif
#endif
#ifndef AT_SYMLINK_NOFOLLOW
/** @brief Do not follow symbolic links. */
#if defined(__CYGWIN__)
#define AT_SYMLINK_NOFOLLOW 2
#elif defined(__APPLE__)
#define AT_SYMLINK_NOFOLLOW 0x0020
#else
#define AT_SYMLINK_NOFOLLOW 0x100
#endif
#endif

#if defined(_WIN32)
#ifndef _BLKSIZE_T_DEFINED
#define _BLKSIZE_T_DEFINED
/** @brief Block size type. */
typedef long blksize_t;
#endif
#ifndef _BLKCNT_T_DEFINED
#define _BLKCNT_T_DEFINED
/** @brief Block count type. */
typedef long blkcnt_t;
#endif

#ifdef st_blocks
#undef st_blocks
#endif
#ifdef st_blksize
#undef st_blksize
#endif

/**
 * @brief 64-bit file status structure compatible with struct _stat64 and
 * POSIX.1-2008 struct stat.
 */
struct posix_stat {
  _dev_t st_dev;           /**< ID of device containing file. */
  _ino_t st_ino;           /**< Inode number. */
  unsigned short st_mode;  /**< File type and mode. */
  short st_nlink;          /**< Number of hard links. */
  short st_uid;            /**< User ID of owner. */
  short st_gid;            /**< Group ID of owner. */
  _dev_t st_rdev;          /**< Device ID (if special file). */
  __int64 st_size;         /**< Total size, in bytes. */
  __time64_t st_atime;     /**< Time of last access (seconds). */
  __time64_t st_mtime;     /**< Time of last modification (seconds). */
  __time64_t st_ctime;     /**< Time of last status change (seconds). */
  blksize_t st_blksize;    /**< Block size for filesystem I/O. */
  blkcnt_t st_blocks;      /**< Number of 512B blocks allocated. */
  struct timespec st_atim; /**< Time of last access (timespec). */
  struct timespec st_mtim; /**< Time of last modification (timespec). */
  struct timespec st_ctim; /**< Time of last status change (timespec). */
};

/**
 * @brief Obtains file status, supporting virtual /proc paths and Windows drive
 * paths.
 * @param[in] pathname Path to file or directory.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_stat(const char *pathname, void *statbuf);

/**
 * @brief Obtains file status by file descriptor.
 * @param[in] fd File descriptor.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_fstat(int fd, void *statbuf);

/**
 * @brief Obtains file status without following symbolic links.
 * @param[in] pathname Path to file.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_lstat(const char *pathname, void *statbuf);

#undef stat
/** @brief Shim for standard stat mapping to posix_stat on Windows. */
#define stat posix_stat
#undef fstat
/** @brief Shim for standard fstat mapping to posix_fstat on Windows. */
#define fstat posix_fstat
#undef lstat
/** @brief Shim for standard lstat mapping to posix_lstat on Windows. */
#define lstat posix_lstat
#ifndef chmod
/** @brief Shim for standard chmod mapping to _chmod on Windows. */
#define chmod _chmod
#endif
#ifndef umask
/** @brief Shim for standard umask mapping to _umask on Windows. */
#define umask _umask
#endif
#ifndef mkdir
/** @brief Shim for standard mkdir mapping to _mkdir on Windows. */
#define mkdir(path, mode) _mkdir(path)
#endif

/**
 * @brief Safely retrieves the OS file handle for a given CRT file descriptor.
 * @param[in] fd File descriptor.
 * @param[out] out_handle Pointer to ptrdiff_t receiving the handle or -1.
 * @return AUTO_WIN_MSVC_SUCCESS on success,
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT or POSIX_STAT_ERROR_INVALID_ARGUMENT on
 * failure.
 */
auto_win_msvc_error_t posix_stat_safe_get_osfhandle(int fd,
                                                    ptrdiff_t *out_handle);

/**
 * @brief Resolves a pathname relative to a directory file descriptor on
 * Windows.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Relative or absolute path.
 * @param[out] out_path Output buffer receiving resolved path.
 * @param[in] out_size Size of output buffer in bytes.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code on failure.
 */
auto_win_msvc_error_t posix_stat_resolve_at_path(int dirfd,
                                                 const char *pathname,
                                                 char *out_path,
                                                 size_t out_size);

/**
 * @brief Converts timespec structure to Win32 FILETIME.
 * @param[in] ts Pointer to timespec structure or NULL for current time.
 * @param[out] out_filetime Pointer to FILETIME buffer (as void *) receiving
 * converted timestamp.
 * @param[out] out_omit Pointer to integer receiving 1 if time should be
 * omitted, 0 otherwise.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code on failure.
 */
auto_win_msvc_error_t posix_stat_fill_filetime(const struct timespec *ts,
                                               void *out_filetime,
                                               int *out_omit);

/**
 * @brief Changes permissions of a file descriptor.
 * @param[in] fd File descriptor.
 * @param[in] mode Target file mode.
 * @return 0 on success, -1 on failure with errno set.
 */
int fchmod(int fd, mode_t mode);

/**
 * @brief Changes permissions of a file relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to file.
 * @param[in] mode Target file mode.
 * @param[in] flags Bitwise flags controlling behavior.
 * @return 0 on success, -1 on failure with errno set.
 */
int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);

/**
 * @brief Obtains file status relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to file.
 * @param[out] statbuf Structure receiving file status.
 * @param[in] flags Flags controlling behavior (e.g. AT_SYMLINK_NOFOLLOW).
 * @return 0 on success, -1 on failure with errno set.
 */
int fstatat(int dirfd, const char *pathname, void *statbuf, int flags);

/**
 * @brief Sets file access and modification times by file descriptor.
 * @param[in] fd File descriptor.
 * @param[in] times Array of two timespec structures.
 * @return 0 on success, -1 on failure with errno set.
 */
int futimens(int fd, const struct timespec times[2]);

/**
 * @brief Creates a filesystem node.
 * @param[in] pathname Path to create.
 * @param[in] mode Mode specifying type and permissions.
 * @param[in] dev Device identifier.
 * @return 0 on success, -1 on failure with errno set.
 */
int mknod(const char *pathname, mode_t mode, unsigned int dev);

/**
 * @brief Creates a filesystem node relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to create.
 * @param[in] mode Mode specifying type and permissions.
 * @param[in] dev Device identifier.
 * @return 0 on success, -1 on failure with errno set.
 */
int mknodat(int dirfd, const char *pathname, mode_t mode, unsigned int dev);

/**
 * @brief Sets file timestamps relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to file.
 * @param[in] times Array of two timespec structures.
 * @param[in] flags Flags (e.g. AT_SYMLINK_NOFOLLOW).
 * @return 0 on success, -1 on failure with errno set.
 */
int utimensat(int dirfd, const char *pathname, const struct timespec times[2],
              int flags);

#else /* !_WIN32 */

#if !defined(__USE_XOPEN2K8)
extern int fchmod(int fd, mode_t mode);
extern int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);
extern int fstatat(int dirfd, const char *pathname, struct stat *statbuf,
                   int flags);
extern int futimens(int fd, const struct timespec times[2]);
extern int lstat(const char *pathname, struct stat *statbuf);
extern int mknod(const char *pathname, mode_t mode, dev_t dev);
extern int mknodat(int dirfd, const char *pathname, mode_t mode, dev_t dev);
extern int utimensat(int dirfd, const char *pathname,
                     const struct timespec times[2], int flags);
#endif

/**
 * @brief Obtains file status, supporting virtual /proc paths on non-Linux
 * POSIX systems.
 * @param[in] pathname Path to file or directory.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_stat(const char *pathname, void *statbuf);

/**
 * @brief Obtains file status without following symbolic links, supporting
 * virtual /proc paths on non-Linux POSIX systems.
 * @param[in] pathname Path to file or directory.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_lstat(const char *pathname, void *statbuf);

#if (defined(__APPLE__) || defined(AUTO_WIN_MSVC_EMULATE_PROCFS)) &&           \
    !defined(AUTO_WIN_MSVC_NO_FUNCTION_MACROS)
#if defined(BB_VER) || defined(AUTO_WIN_MSVC_EMULATE_PROCFS)
#ifndef stat
static __inline int auto_win_msvc_stat_wrap(const char *p, struct stat *b) {
  return posix_stat(p, b);
}
#define stat(p, b) auto_win_msvc_stat_wrap((p), (b))
#endif
#ifndef lstat
static __inline int auto_win_msvc_lstat_wrap(const char *p, struct stat *b) {
  return posix_lstat(p, b);
}
#define lstat(p, b) auto_win_msvc_lstat_wrap((p), (b))
#endif
#endif
#endif

#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STAT_H */
