/* posix-stat.h - Strict C89 Header */
#ifndef POSIX_STAT_H
#define POSIX_STAT_H

/**
 * @file posix-stat.h
 * @brief Strict C89 POSIX sys/stat.h header and polyfills for MSVC.
 */

/* clang-format off */
#include <errno.h>
#include <stddef.h>
#include <time.h>

#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/sys/stat.h>
#include <../ucrt/sys/types.h>
#include <../ucrt/io.h>
#elif defined(_MSC_VER)
#include <../include/sys/stat.h>
#include <../include/sys/types.h>
#include <io.h>
#elif defined(__GNUC__) || defined(__clang__)
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
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-stat module functions.
 */
enum posix_stat_error_code {
  /** @brief Operation completed successfully. */
  POSIX_STAT_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_STAT_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument was provided. */
  POSIX_STAT_ERROR_INVALID_ARGUMENT = 2,
  /** @brief Path resolution failed. */
  POSIX_STAT_ERROR_PATH_RESOLUTION = 3,
  /** @brief IO or file system operation failed. */
  POSIX_STAT_ERROR_IO = 4,
  /** @brief Operation not supported on this platform. */
  POSIX_STAT_ERROR_NOT_SUPPORTED = 5
};

/**
 * @brief Retrieves information on posix-stat module availability and status.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_STAT_SUCCESS on success, or POSIX_STAT_ERROR_NULL_POINTER on
 * NULL pointer.
 */
enum posix_stat_error_code posix_stat_get_info(int *out_available);

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

#if !defined(_TIMESPEC_DEFINED) && !defined(HAVE_STRUCT_TIMESPEC)
#if defined(_MSC_VER) && _MSC_VER >= 1900
/* VS2015+ provides struct timespec in time.h */
#else
#define _TIMESPEC_DEFINED
#define HAVE_STRUCT_TIMESPEC 1
/**
 * @brief Structure for representing time with nanosecond precision.
 */
struct timespec {
  time_t tv_sec; /**< Seconds. */
  long tv_nsec;  /**< Nanoseconds. */
};
#endif
#endif

#ifndef S_IFMT
/** @brief Bit mask for the file type bit field. */
#define S_IFMT _S_IFMT
#endif
#ifndef S_IFDIR
/** @brief Directory file type. */
#define S_IFDIR _S_IFDIR
#endif
#ifndef S_IFCHR
/** @brief Character device file type. */
#define S_IFCHR _S_IFCHR
#endif
#ifndef S_IFREG
/** @brief Regular file type. */
#define S_IFREG _S_IFREG
#endif
#ifndef S_IFIFO
/** @brief FIFO or pipe file type. */
#define S_IFIFO _S_IFIFO
#endif

#ifndef S_IRUSR
/** @brief Read permission, owner. */
#define S_IRUSR _S_IREAD
#endif
#ifndef S_IWUSR
/** @brief Write permission, owner. */
#define S_IWUSR _S_IWRITE
#endif
#ifndef S_IXUSR
/** @brief Execute permission, owner. */
#define S_IXUSR _S_IEXEC
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
#define AT_FDCWD -100
#endif
#ifndef AT_SYMLINK_NOFOLLOW
/** @brief Do not follow symbolic links. */
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

#ifndef stat
/** @brief Shim for standard stat mapping to _stat64 on Windows. */
#define stat _stat64
#endif
#ifndef fstat
/** @brief Shim for standard fstat mapping to _fstat64 on Windows. */
#define fstat _fstat64
#endif
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
 * @return POSIX_STAT_SUCCESS on success, POSIX_STAT_ERROR_NULL_POINTER or
 * POSIX_STAT_ERROR_INVALID_ARGUMENT on failure.
 */
enum posix_stat_error_code posix_stat_safe_get_osfhandle(int fd,
                                                         ptrdiff_t *out_handle);

/**
 * @brief Resolves a pathname relative to a directory file descriptor on
 * Windows.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Relative or absolute path.
 * @param[out] out_path Output buffer receiving resolved path.
 * @param[in] out_size Size of output buffer in bytes.
 * @return POSIX_STAT_SUCCESS on success, or error code on failure.
 */
enum posix_stat_error_code posix_stat_resolve_at_path(int dirfd,
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
 * @return POSIX_STAT_SUCCESS on success, or error code on failure.
 */
enum posix_stat_error_code posix_stat_fill_filetime(const struct timespec *ts,
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
int fstatat(int dirfd, const char *pathname, struct _stat64 *statbuf,
            int flags);

/**
 * @brief Sets file access and modification times by file descriptor.
 * @param[in] fd File descriptor.
 * @param[in] times Array of two timespec structures.
 * @return 0 on success, -1 on failure with errno set.
 */
int futimens(int fd, const struct timespec times[2]);

/**
 * @brief Obtains file status without following symbolic links.
 * @param[in] pathname Path to file.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int lstat(const char *pathname, struct _stat64 *statbuf);

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

#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STAT_H */
