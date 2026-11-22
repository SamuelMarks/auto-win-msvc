/* posix-core.h - Strict C89 Header */
#ifndef POSIX_CORE_H
#define POSIX_CORE_H

/**
 * @file posix-core.h
 * @brief POSIX core standard polyfills and utilities.
 */

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
#endif

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
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#ifndef EOPNOTSUPP
#define EOPNOTSUPP 95
#endif
#ifndef ENOTSUP
#define ENOTSUP EOPNOTSUPP
#endif
#ifndef ELOOP
#define ELOOP 40
#endif
#ifndef EOVERFLOW
#define EOVERFLOW 75
#endif
#ifndef ETXTBSY
#define ETXTBSY 26
#endif
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <direct.h>
#include <fcntl.h>
#include <process.h>
#include <share.h>
#include <sys/stat.h>
#elif defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
#include <direct.h>
#include <fcntl.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <process.h>
#include <share.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#if !defined(_MSC_VER) && !defined(__WATCOMC__)
#include <unistd.h>
#endif
#else
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#if !defined(_MSC_VER)
#ifdef __CYGWIN__
#define posix_close _cygwin_orig_posix_close
#include <unistd.h>
#undef posix_close
#else
#include <unistd.h>
#endif
#endif
#endif
#include "auto-win-msvc-error.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes and validates the posix-core module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return POSIX_CORE_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t posix_core_init(int *out_status);

#ifndef NUM_FORMAT_CAST
#if defined(_MSC_VER)
#define NUM_FORMAT_CAST __int64
#else
#define NUM_FORMAT_CAST long
#endif
#endif

#ifndef NUM_FORMAT
#if defined(_MSC_VER)
#define NUM_FORMAT "%I64d"
#else
#define NUM_FORMAT "%ld"
#endif
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_MSC_VER)
#ifndef EACCES
#define EACCES 13
#endif

#ifndef EISDIR
#define EISDIR 21
#endif

#ifndef ECONNRESET
#define ECONNRESET 108
#endif
#ifndef EINPROGRESS
#define EINPROGRESS 112
#endif
#ifndef EALREADY
#define EALREADY 103
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK 140
#endif

#ifndef ENOTSOCK
#define ENOTSOCK 128
#endif
#ifndef EMSGSIZE
#define EMSGSIZE 115
#endif
#ifndef EADDRINUSE
#define EADDRINUSE 100
#endif
#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL 101
#endif
#ifndef ECONNABORTED
#define ECONNABORTED 106
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED 107
#endif

#ifndef _SSIZE_T_DEFINED
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef int ssize_t;
#endif
#endif

#ifndef _PID_T_DEFINED
#define _PID_T_DEFINED
typedef int pid_t;
#endif

#if !defined(_MODE_T_DEFINED) && !defined(_MODE_T_DEFINED_) &&                 \
    !defined(_MODE_T_)
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
typedef unsigned short mode_t;
#endif

#ifndef _OFF_T_DEFINED
#define _OFF_T_DEFINED
typedef long _off_t;
typedef _off_t off_t;
#elif !defined(_OFF_T_POSIX_DEFINED)
#define _OFF_T_POSIX_DEFINED
#if defined(_MSC_VER) && !defined(off_t)
typedef _off_t off_t;
#endif
#endif

#ifndef _USECONDS_T_DEFINED
#define _USECONDS_T_DEFINED
typedef unsigned int useconds_t;
#endif

#ifndef _UID_T_DEFINED
#define _UID_T_DEFINED
typedef int uid_t;
typedef int gid_t;
#endif
#endif /* _MSC_VER */
#if defined(__MINGW32__) || defined(__MINGW64__)
#ifndef _UID_T_DEFINED
#define _UID_T_DEFINED
typedef int uid_t;
typedef int gid_t;
#endif
#endif

__declspec(dllimport) void __stdcall Sleep(unsigned long dwMilliseconds);
#endif

#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__DOS__) ||           \
    defined(__MSDOS__) || defined(__WATCOMC__)
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#else
#ifndef STDIN_FILENO
#define STDIN_FILENO STDIN_FILENO
#endif
#endif
#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__DOS__) ||           \
    defined(__MSDOS__) || defined(__WATCOMC__)
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#else
#ifndef STDOUT_FILENO
#define STDOUT_FILENO STDOUT_FILENO
#endif
#endif
#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__DOS__) ||           \
    defined(__MSDOS__) || defined(__WATCOMC__)
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
#else
#ifndef STDERR_FILENO
#define STDERR_FILENO STDERR_FILENO
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_RDONLY
#define O_RDONLY _O_RDONLY
#endif
#else
#ifndef O_RDONLY
#define O_RDONLY O_RDONLY
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_WRONLY
#define O_WRONLY _O_WRONLY
#endif
#else
#ifndef O_WRONLY
#define O_WRONLY O_WRONLY
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_RDWR
#define O_RDWR _O_RDWR
#endif
#else
#ifndef O_RDWR
#define O_RDWR O_RDWR
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_APPEND
#define O_APPEND _O_APPEND
#endif
#else
#ifndef O_APPEND
#define O_APPEND O_APPEND
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_CREAT
#define O_CREAT _O_CREAT
#endif
#else
#ifndef O_CREAT
#define O_CREAT O_CREAT
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_TRUNC
#define O_TRUNC _O_TRUNC
#endif
#else
#ifndef O_TRUNC
#define O_TRUNC O_TRUNC
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_EXCL
#define O_EXCL _O_EXCL
#endif
#else
#ifndef O_EXCL
#define O_EXCL O_EXCL
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_TEXT
#define O_TEXT _O_TEXT
#endif
#else
#ifndef O_TEXT
#define O_TEXT 0
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_BINARY
#define O_BINARY _O_BINARY
#endif
#else
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_CLOEXEC
#define O_CLOEXEC _O_NOINHERIT
#endif
#elif defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)
#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif
#else
#ifndef O_CLOEXEC
#define O_CLOEXEC O_CLOEXEC
#endif
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_NONBLOCK
#define O_NONBLOCK 0x4000
#endif
#ifndef F_DUPFD
#define F_DUPFD 0
#endif
#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC 67
#endif
#ifndef FD_CLOEXEC
#define FD_CLOEXEC 1
#endif
#ifndef F_GETFD
#define F_GETFD 1
#endif
#ifndef F_SETFD
#define F_SETFD 2
#endif
#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif
#ifndef F_GETLK
#define F_GETLK 5
#endif
#ifndef F_SETLK
#define F_SETLK 6
#endif
#ifndef F_SETLKW
#define F_SETLKW 7
#endif
#ifndef F_RDLCK
#define F_RDLCK 1
#endif
#ifndef F_WRLCK
#define F_WRLCK 2
#endif
#ifndef F_UNLCK
#define F_UNLCK 3
#endif

#ifndef _FLOCK_DEFINED
#define _FLOCK_DEFINED
/** @brief flock structure */
struct flock {
  short l_type;
  short l_whence;
  off_t l_start;
  off_t l_len;
  pid_t l_pid;
};
#endif
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef F_OK
#define F_OK 0
#endif
#ifndef X_OK
#define X_OK 0
#endif
#ifndef W_OK
#define W_OK 2
#endif
#ifndef R_OK
#define R_OK 4
#endif
#else
#ifndef F_OK
#define F_OK 0
#endif
#ifndef X_OK
#define X_OK 1
#endif
#ifndef W_OK
#define W_OK 2
#endif
#ifndef R_OK
#define R_OK 4
#endif
#endif

/* NUM_FORMAT macro for cross-platform printf of 64-bit integers */

/* Functions */
/** @brief Open file descriptor with POSIX semantics. */
int posix_open(const char *pathname, int flags, ...);

/** @brief open */
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Opens a file in binary mode on Windows forwarding to posix_open. */
static __inline int posix_core_open(const char *filename, int oflag, ...) {
  int pmode = 0;
  oflag |= _O_BINARY;
  if (oflag & _O_CREAT) {
    va_list ap;
    va_start(ap, oflag);
    pmode = va_arg(ap, int) & (_S_IREAD | _S_IWRITE);
    va_end(ap);
    if ((pmode & _S_IWRITE) == 0) {
      pmode |= (_S_IREAD | _S_IWRITE);
    }
  }
  return posix_open(filename, oflag, pmode);
}
#ifndef open
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#define open posix_core_open
#endif
#endif
#elif (defined(__APPLE__) || defined(AUTO_WIN_MSVC_EMULATE_PROCFS)) &&         \
    (defined(BB_VER) || defined(AUTO_WIN_MSVC_EMULATE_PROCFS)) &&              \
    !defined(AUTO_WIN_MSVC_NO_FUNCTION_MACROS)
#ifndef open
#define open posix_open
#endif
#else
/* open */
#endif
/** @brief close */
#if !defined(_WIN32)
#undef posix_close
#define posix_close awm_posix_close
#endif
int posix_close(intptr_t fd);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef close
#define close posix_close
#endif
#endif

/** @brief read */
ssize_t posix_read(intptr_t fd, void *buf, size_t count);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef read
#define read posix_read
#endif
#endif

/** @brief write */
ssize_t posix_write(intptr_t fd, const void *buf, size_t count);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef write
#define write posix_write
#endif
#endif

/** @brief Open file descriptor with POSIX semantics. */
int posix_open(const char *pathname, int flags, ...);

/**
 * @brief Create a file with POSIX semantics.
 * @param[in] pathname Path to the file to create.
 * @param[in] mode Permissions for the new file.
 * @return File descriptor on success, -1 on failure.
 */
int posix_creat(const char *pathname, mode_t mode);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef creat
#define creat posix_creat
#endif
#endif

/** @brief lseek */
#if defined(_MSC_VER)
__int64 posix_lseek(int fd, __int64 offset, int whence);
#else
off_t posix_lseek(int fd, off_t offset, int whence);
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef lseek
#define lseek posix_lseek
#endif
#endif
#else
/* lseek */
#endif
/** @brief dup */
int posix_dup(int oldfd);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef dup
#define dup posix_dup
#endif
#endif
#else
/* dup */
#endif
/** @brief dup2 */
int posix_dup2(int oldfd, int newfd);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef dup2
#define dup2 posix_dup2
#endif
#endif
#else
/* dup2 */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef tell
#define tell _tell
#endif

#ifndef fsync
#define fsync fdatasync
#endif
#else
/* fsync */
#endif
/** @brief ftruncate */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef ftruncate
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define ftruncate(fd, size) _chsize_s((fd), (__int64)(size))
#else
#define ftruncate(fd, size) _chsize((fd), (long)(size))
#endif
#endif
#else
/* ftruncate */
#endif
/** @brief access */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef access
#define access _access
#endif
#else
/* access */
#endif
/** @brief rmdir */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef rmdir
#define rmdir _rmdir
#endif
#else
/* rmdir */
#endif
/** @brief chdir */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef chdir
#define chdir _chdir
#endif
#else
/* chdir */
#endif
/** @brief getcwd */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef getcwd
#define getcwd _getcwd
#endif
#else
/* getcwd */
#endif
/** @brief unlink */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef unlink
#define unlink _unlink
#endif
#else
/* unlink */
#endif
/** @brief execve */
#if defined(_WIN32) && !defined(__CYGWIN__)
int posix_execve(const char *pathname, char *const argv[], char *const envp[]);
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef execve
#define execve(p, a, e)                                                        \
  posix_execve((p), (char *const *)(a), (char *const *)(e))
#endif
#else
/* execve */
#endif
/** @brief execv */
#if defined(_WIN32) && !defined(__CYGWIN__)
int posix_execv(const char *pathname, char *const argv[]);
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef execv
#define execv(p, a) posix_execv((p), (char *const *)(a))
#endif
#else
/* execv */
#endif
/** @brief execvp */
#if defined(_WIN32) && !defined(__CYGWIN__)
int posix_execvp(const char *file, char *const argv[]);
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef execvp
#define execvp(f, a) posix_execvp((f), (char *const *)(a))
#endif
#else
/* execvp */
#endif
/** @brief execl */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef execl
#define execl _execl
#endif
#else
/* execl */
#endif
/** @brief execlp */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef execlp
#define execlp _execlp
#endif
#else
/* execlp */
#endif
/** @brief getpid */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef getpid
#define getpid _getpid
#endif
#else
/* getpid */
#endif
/** @brief sleep */
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sleeps for the specified number of seconds. */
static __inline unsigned int posix_core_sleep(unsigned int seconds) {
  Sleep(seconds * 1000);
  return 0;
}
#ifndef sleep
#define sleep posix_core_sleep
#endif
#else
/* sleep */
#endif
/** @brief usleep */
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sleeps for the specified number of microseconds. */
static __inline int posix_core_usleep(unsigned int usec) {
  Sleep(usec / 1000);
  return 0;
}
#ifndef usleep
#define usleep posix_core_usleep
#endif
#else
/* usleep */
#endif
/** @brief isatty */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef isatty
#define isatty posix_isatty
/** @brief Check if file descriptor is a tty. */
extern int posix_isatty(intptr_t fd);
#endif
#else
/* isatty */
#endif
/** @brief swab */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef swab
#define swab _swab
#endif
#else
/* swab */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief fcntl */
int fcntl(intptr_t fd, int cmd, ...);
#else
/* fcntl */
#endif
/** \brief Special value to indicate current working directory for at-functions.
 */
#ifndef AT_FDCWD
#if defined(__CYGWIN__) || defined(__APPLE__)
#define AT_FDCWD -2
#else
#define AT_FDCWD -100
#endif
#endif

/** \brief Flag to not follow symlinks for at-functions. */
#ifndef AT_SYMLINK_NOFOLLOW
#if defined(__CYGWIN__)
#define AT_SYMLINK_NOFOLLOW 2
#elif defined(__APPLE__)
#define AT_SYMLINK_NOFOLLOW 0x0020
#else
#define AT_SYMLINK_NOFOLLOW 0x100
#endif
#endif

/** \brief Flag to check effective user/group IDs for at-functions. */
#ifndef AT_EACCESS
#define AT_EACCESS 0x200
#endif

/** \brief Flag to remove directory instead of unlinking file for unlinkat. */
#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR 0x200
#endif

/**
 * @brief Sets an environment variable.
 * @param[in] name Variable name.
 * @param[in] value Variable value.
 * @param[in] overwrite Flag whether to overwrite existing variable.
 * @return 0 on success, -1 on failure.
 */
int setenv(const char *name, const char *value, int overwrite);

/**
 * @brief Unsets an environment variable.
 * @param[in] name Variable name.
 * @return 0 on success, -1 on failure.
 */
int unsetenv(const char *name);

/**
 * @brief Clears the environment.
 * @return 0 on success, -1 on failure.
 */
int clearenv(void);

/**
 * @brief Duplicates at most n bytes of a string.
 * @param[in] s String to duplicate.
 * @param[in] n Maximum number of bytes to copy.
 * @return Pointer to newly allocated duplicate string, or NULL on error.
 */
char *strndup(const char *s, size_t n);

#if defined(__GNUC__) || defined(__clang__)
#define POSIX_CORE_PRINTF_FORMAT(fmt_idx, arg_idx)                             \
  __attribute__((format(printf, fmt_idx, arg_idx)))
#else
#define POSIX_CORE_PRINTF_FORMAT(fmt_idx, arg_idx)
#endif

/**
 * @brief Allocates and prints to a string buffer using va_list.
 * @param[out] strp Pointer to output string buffer.
 * @param[in] fmt Format string.
 * @param[in] ap Argument list.
 * @return Number of characters printed on success, -1 on error.
 */
int vasprintf(char **strp, const char *fmt, va_list ap)
    POSIX_CORE_PRINTF_FORMAT(2, 0);

/**
 * @brief Allocates and prints to a string buffer.
 * @param[out] strp Pointer to output string buffer.
 * @param[in] fmt Format string.
 * @return Number of characters printed on success, -1 on error.
 */
int asprintf(char **strp, const char *fmt, ...) POSIX_CORE_PRINTF_FORMAT(2, 3);

/**
 * @brief Prints formatted output to a file descriptor.
 * @param[in] fd File descriptor.
 * @param[in] fmt Format string.
 * @return Number of characters printed on success, -1 on error.
 */
int dprintf(int fd, const char *fmt, ...) POSIX_CORE_PRINTF_FORMAT(2, 3);

/**
 * @brief Gets the memory page size.
 * @return System page size in bytes.
 */
int getpagesize(void);

/**
 * @brief Resolves an absolute or canonical path.
 * @param[in] path Input pathname.
 * @param[out] resolved_path Buffer to store resolved path, or NULL to allocate.
 * @return Pointer to resolved path, or NULL on error.
 */
char *realpath(const char *path, char *resolved_path);

/**
 * @brief Changes working directory given an open directory file descriptor.
 * @param[in] fd Open directory file descriptor.
 * @return 0 on success, -1 on error.
 */
int fchdir(int fd);

/**
 * @brief Changes the root directory.
 * @param[in] path Pathname of the new root directory.
 * @return 0 on success, -1 on error.
 */
int chroot(const char *path);

/** \brief Flag to follow symlinks for linkat. */
#ifndef AT_SYMLINK_FOLLOW
#define AT_SYMLINK_FOLLOW 0x400
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief openat */
int openat(int dirfd, const char *pathname, int flags, ...);
/** @brief unlinkat */
int unlinkat(int dirfd, const char *pathname, int flags);
/** @brief renameat */
int renameat(int olddirfd, const char *oldpath, int newdirfd,
             const char *newpath);
#else
/* openat */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief posix_fadvise */
int posix_fadvise(intptr_t fd, off_t offset, off_t len, int advice);
#elif (defined(__APPLE__) || !defined(__linux__)) && !defined(__CYGWIN__) &&   \
    !defined(__EMSCRIPTEN__)
int posix_fadvise(intptr_t fd, off_t offset, off_t len, int advice);
#else
/* posix_fadvise */
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief posix_fallocate */
int posix_fallocate(intptr_t fd, off_t offset, off_t len);
#elif (defined(__APPLE__) || !defined(__linux__)) && !defined(__CYGWIN__) &&   \
    !defined(__EMSCRIPTEN__)
int posix_fallocate(intptr_t fd, off_t offset, off_t len);
#else
/* posix_fallocate */
#endif
/** @brief sync_file_range */
#ifndef SYNC_FILE_RANGE_WAIT_BEFORE
#define SYNC_FILE_RANGE_WAIT_BEFORE 1
#endif
#ifndef SYNC_FILE_RANGE_WRITE
#define SYNC_FILE_RANGE_WRITE 2
#endif
#ifndef SYNC_FILE_RANGE_WAIT_AFTER
#define SYNC_FILE_RANGE_WAIT_AFTER 4
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief sync_file_range function. */
int sync_file_range(intptr_t fd, off_t offset, off_t nbytes,
                    unsigned int flags);
#elif defined(__APPLE__) || !defined(__linux__)
int sync_file_range(intptr_t fd, off_t offset, off_t nbytes,
                    unsigned int flags);
#else
#if !defined(_GNU_SOURCE)
extern int sync_file_range(int fd, off_t offset, off_t nbytes,
                           unsigned int flags);
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief alarm */
unsigned int alarm(unsigned int seconds);
#else
/* alarm */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief chown */
int chown(const char *pathname, uid_t owner, gid_t group);
#else
/* chown */
#endif
#ifndef _CS_PATH
#define _CS_PATH 1
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief confstr */
size_t confstr(int name, char *buf, size_t len);
#else
/* confstr */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief crypt */
char *crypt(const char *key, const char *salt);
#else
/* crypt */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief encrypt */
void encrypt(char block[64], int edflag);
#else
/* encrypt */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief faccessat */
int faccessat(int dirfd, const char *pathname, int mode, int flags);
#else
/* faccessat */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief fchown */
int fchown(intptr_t fd, uid_t owner, gid_t group);
#else
/* fchown */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief fchownat */
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group,
             int flags);
#else
/* fchownat */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief fdatasync */
int fdatasync(intptr_t fd);
#elif defined(__APPLE__)
int fdatasync(intptr_t fd);
#else
/* fdatasync */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief fexecve */
int fexecve(intptr_t fd, char *const argv[], char *const envp[]);
#elif (defined(__APPLE__) || !defined(__linux__)) && !defined(__CYGWIN__) &&   \
    !defined(__EMSCRIPTEN__)
int fexecve(intptr_t fd, char *const argv[], char *const envp[]);
#else
/* fexecve */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief fork */
int fork(void);
#else
/* fork */
#endif
#ifndef _PC_LINK_MAX
#define _PC_LINK_MAX 1
#define _PC_MAX_CANON 2
#define _PC_MAX_INPUT 3
#define _PC_NAME_MAX 4
#define _PC_PATH_MAX 5
#define _PC_PIPE_BUF 6
#define _PC_CHOWN_RESTRICTED 7
#define _PC_NO_TRUNC 8
#define _PC_VDISABLE 9
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief fpathconf */
long fpathconf(intptr_t fd, int name);
#else
/* fpathconf */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getegid */
gid_t getegid(void);
#else
/* getegid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief geteuid */
uid_t geteuid(void);
#else
/* geteuid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getgid */
gid_t getgid(void);
#else
/* getgid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getgroups */
int getgroups(int size, gid_t list[]);
#else
/* getgroups */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setgroups */
int setgroups(size_t size, const gid_t *list);
#else
/* setgroups */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief gethostid */
long gethostid(void);
#else
/* gethostid */
#endif
/** @brief gethostname */
#if defined(_WIN32) && !defined(__CYGWIN__)
/* int gethostname(char *name, int len); */
#else
/* gethostname */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getlogin */
char *getlogin(void);
#else
/* getlogin */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getlogin_r */
int getlogin_r(char *buf, size_t bufsize);
#else
/* getlogin_r */
#endif
/** @brief getopt */
#if defined(_WIN32) && !defined(__CYGWIN__) && defined(_MSC_VER)
#if defined(posix_core_BUILD_DLL)
#define POSIX_CORE_DECL __declspec(dllexport)
#elif defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
#define POSIX_CORE_DECL
#elif defined(posix_core_SHARED) || defined(BUILD_SHARED_LIBS)
#define POSIX_CORE_DECL __declspec(dllimport)
#else
#define POSIX_CORE_DECL
#endif

/** @brief Current option argument pointer. */
extern POSIX_CORE_DECL char *optarg;
/** @brief Index of next element to be processed in argv. */
extern POSIX_CORE_DECL int optind;
/** @brief Whether getopt prints error messages. */
extern POSIX_CORE_DECL int opterr;
/** @brief Last unknown option character. */
extern POSIX_CORE_DECL int optopt;
/** @brief Parse command-line options. */
POSIX_CORE_DECL int getopt(int argc, char *const argv[], const char *optstring);
#elif defined(_WIN32) && !defined(__CYGWIN__) && !defined(_MSC_VER)
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
int getopt(int argc, char *const argv[], const char *optstring);
#else
/* getopt */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getsubopt */
int getsubopt(char **optionp, char *const *tokens, char **valuep);
#else
/* getsubopt */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getpgid */
pid_t getpgid(pid_t pid);
#else
/* getpgid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getpgrp */
pid_t getpgrp(void);
#else
/* getpgrp */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getppid */
pid_t getppid(void);
#else
/* getppid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getsid */
pid_t getsid(pid_t pid);
#else
/* getsid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getuid */
uid_t getuid(void);
#else
/* getuid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief lchown */
int lchown(const char *pathname, uid_t owner, gid_t group);
#else
/* lchown */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief link */
int link(const char *oldpath, const char *newpath);
#else
/* link */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief linkat */
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags);
#else
/* linkat */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief lockf */
int lockf(intptr_t fd, int cmd, off_t len);
#else
/* lockf */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief pathconf */
long pathconf(const char *pathname, int name);
#else
/* pathconf */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief pause */
int pause(void);
#else
/* pause */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#undef pipe
/** @brief pipe */
int pipe(int pipefd[2]);
#else
/* pipe */
#endif
/** @brief pipe2 */
int pipe2(int pipefd[2], int flags);
/* pipe2 */
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief pread */
ssize_t pread(intptr_t fd, void *buf, size_t count, off_t offset);
#else
/* pread */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief pwrite */
ssize_t pwrite(intptr_t fd, const void *buf, size_t count, off_t offset);
#else
/* pwrite */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief readlink */
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
#else
/* readlink */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief readlinkat */
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);
/** @brief Unlink a file or directory relative to a directory fd on DOS. */
int unlinkat(int dirfd, const char *pathname, int flags);
/** @brief Rename a file relative to directory fds on DOS. */
int renameat(int olddirfd, const char *oldpath, int newdirfd,
             const char *newpath);
/** @brief Create a unique temporary directory on DOS. */
char *mkdtemp(char *template_str);
/** @brief Stub password encryption function on DOS. */
char *crypt(const char *key, const char *salt);
#else
/* readlinkat */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setegid */
int setegid(gid_t egid);
#else
/* setegid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief seteuid */
int seteuid(uid_t euid);
#else
/* seteuid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setgid */
int setgid(gid_t gid);
#else
/* setgid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setpgid */
int setpgid(pid_t pid, pid_t pgid);
#else
/* setpgid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setpgrp */
pid_t setpgrp(void);
#else
/* setpgrp */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setregid */
int setregid(gid_t rgid, gid_t egid);
#else
/* setregid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setreuid */
int setreuid(uid_t ruid, uid_t euid);
#else
/* setreuid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setsid */
pid_t setsid(void);
#else
/* setsid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief setuid */
int setuid(uid_t uid);
#else
/* setuid */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief symlink */
int symlink(const char *target, const char *linkpath);
#else
/* symlink */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief symlinkat */
int symlinkat(const char *target, int newdirfd, const char *linkpath);
#else
/* symlinkat */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief sync */
void sync(void);
#else
/* sync */
#endif
#ifndef _SC_PAGESIZE
#define _SC_PAGESIZE 1
#define _SC_PAGE_SIZE 1
#define _SC_NPROCESSORS_ONLN 2
#define _SC_NPROCESSORS_CONF 3
#define _SC_CLK_TCK 4
#define _SC_PHYS_PAGES 5
#define _SC_AVPHYS_PAGES 6
#define _SC_OPEN_MAX 7
#endif
#ifndef _SC_OPEN_MAX
#define _SC_OPEN_MAX 7
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief sysconf */
long sysconf(int name);
#else
/* sysconf */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief tcgetpgrp */
pid_t tcgetpgrp(intptr_t fd);
#else
/* tcgetpgrp */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief tcsetpgrp */
int tcsetpgrp(intptr_t fd, pid_t pgrp);
#else
/* tcsetpgrp */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief truncate */
int truncate(const char *path, off_t length);
#else
/* truncate */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief ttyname */
char *ttyname(intptr_t fd);
#else
/* ttyname */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief ttyname_r */
int ttyname_r(intptr_t fd, char *buf, size_t buflen);
#else
/* ttyname_r */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief ualarm */
useconds_t ualarm(useconds_t value, useconds_t interval);
#else
/* ualarm */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief vfork */
pid_t vfork(void);
#else
/* vfork */
#endif

/** @brief fileno */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef fileno
#define fileno _fileno
#endif
#else
/* fileno */
#endif

/** @brief pclose */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef pclose
#define pclose _pclose
#endif
#else
/* pclose */
#endif

/** @brief popen */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef popen
#define popen _popen
#endif
#else
/* popen */
#endif

/** @brief tell */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef tell
#define tell _tell
#endif
#else
/* tell */
#endif

/** @brief strdup */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef strdup
#define strdup _strdup
#endif
#else
/* strdup */
#endif

/** @brief rename (POSIX semantics) */
int posix_rename(const char *oldpath, const char *newpath);
#if (defined(_WIN32) && !defined(__CYGWIN__)) ||                               \
    ((defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)) &&       \
     !defined(_WIN32))
#undef rename
#ifndef rename
#define rename(oldpath, newpath) posix_rename((oldpath), (newpath))
#endif
#endif

/**
 * @brief Formats a mkstemp template into a strict 8.3 MS-DOS filename.
 * @param[out] out_buf Destination buffer of capacity out_cap.
 * @param[in] out_cap Capacity of out_buf in bytes (including NUL).
 * @param[in] tmpl Input template ending with XXXXXX.
 * @param[in] attempt Monotonic collision retry index.
 * @return 0 on success, -1 on invalid template or insufficient buffer capacity.
 */
int awm_dos_mkstemp_format_83(char *out_buf, size_t out_cap, const char *tmpl,
                              unsigned int attempt);

/** @brief mkstemp (POSIX semantics with SHARE_DELETE) */
int posix_mkstemp(char *tmpl);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef mkstemp
#define mkstemp posix_mkstemp
#endif
#endif

/**
 * @brief mkostemp creates and opens a unique temporary file with extra flags.
 * @param[in,out] tmpl File name template ending with XXXXXX.
 * @param[in] flags Additional open flags (such as O_BINARY or O_APPEND).
 * @return Non-negative file descriptor on success, -1 on error with errno set.
 */
int posix_mkostemp(char *tmpl, int flags);
#if (defined(_WIN32) && !defined(__CYGWIN__)) ||                               \
    ((defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)) &&       \
     !defined(_WIN32))
#ifndef mkostemp
#define mkostemp posix_mkostemp
#endif
#endif

/**
 * @brief Opens a scratch-file simulated pipe stream for single-tasking targets.
 * @param[in] command Shell command string to execute.
 * @param[in] type Stream mode ("r", "rb", "w", or "wb").
 * @return FILE pointer on success, NULL on failure with errno set.
 */
FILE *awm_popen_scratch(const char *command, const char *type);

/**
 * @brief Closes a scratch-file simulated pipe stream opened by
 * awm_popen_scratch.
 * @param[in] stream Pipe stream handle.
 * @return Command exit status on success, -1 on failure with errno set.
 */
int awm_pclose_scratch(FILE *stream);

/**
 * @brief Returns non-zero if stream is open for writing or last I/O was write.
 * @param[in] stream Standard C FILE stream pointer.
 * @return Non-zero if writing, 0 otherwise.
 */
int awm_fwriting(FILE *stream);

/**
 * @brief mkdtemp creates a unique temporary directory from a template.
 * @param[in,out] tmpl Directory name template ending with XXXXXX.
 * @return Pointer to modified tmpl on success, NULL on failure with errno set.
 */
char *posix_mkdtemp(char *tmpl);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef mkdtemp
#define mkdtemp posix_mkdtemp
#endif
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getdelim */
ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);
#else
/* getdelim */
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief getline */
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#else
/* getline */
#endif

/** @brief Open stream with POSIX semantics. */
FILE *posix_fopen(const char *pathname, const char *mode);
#if (defined(_WIN32) && !defined(__CYGWIN__)) ||                               \
    ((defined(__APPLE__) || defined(AUTO_WIN_MSVC_EMULATE_PROCFS)) &&          \
     (defined(BB_VER) || defined(AUTO_WIN_MSVC_EMULATE_PROCFS)))
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef fopen
#define fopen posix_fopen
#endif
#endif
#endif

/** @brief Associate stream with file descriptor or socket. */
FILE *posix_fdopen(intptr_t fd, const char *mode);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef fdopen
#define fdopen posix_fdopen
#endif
#endif

/** @brief Read a line from stream or socket-backed stream. */
char *posix_fgets(char *s, int size, FILE *stream);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef fgets
#undef fgets_unlocked
#define fgets posix_fgets
#define fgets_unlocked posix_fgets
#endif
#endif

/** @brief Write formatted output to stream or socket-backed stream. */
int posix_fprintf(FILE *stream, const char *format, ...)
    POSIX_CORE_PRINTF_FORMAT(2, 3);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef fprintf
#define fprintf posix_fprintf
#endif
#endif

/** @brief Read binary data from stream or socket-backed stream. */
size_t posix_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef fread
#define fread posix_fread
#endif
#endif

/** @brief Write binary data to stream or socket-backed stream. */
size_t posix_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef fwrite
#define fwrite posix_fwrite
#endif
#endif

/** @brief Read a character from stream or socket-backed stream. */
int posix_fgetc(FILE *stream);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef fgetc
#define fgetc posix_fgetc
#undef getc
#define getc posix_fgetc
#undef getc_unlocked
#define getc_unlocked posix_fgetc
#endif
#endif

/** @brief Close stream or socket-backed stream. */
int posix_fclose(FILE *stream);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef fclose
#define fclose posix_fclose
#endif
#endif

/** @brief Test end-of-file indicator on stream or socket-backed stream. */
int posix_feof(FILE *stream);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef feof
#undef feof_unlocked
#define feof posix_feof
#define feof_unlocked posix_feof
#endif
#endif

/** @brief Test error indicator on stream or socket-backed stream. */
int posix_ferror(FILE *stream);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef ferror
#undef ferror_unlocked
#define ferror posix_ferror
#define ferror_unlocked posix_ferror
#endif
#endif

/** @brief Return integer file descriptor or socket for stream. */
int posix_fileno(FILE *stream);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#undef fileno
#undef fileno_unlocked
#define fileno posix_fileno
#define fileno_unlocked posix_fileno
#endif
#endif

#if (defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)) &&        \
    !defined(_WIN32)
#ifndef F_OK
#define F_OK 0
#endif
#ifndef W_OK
#define W_OK 2
#endif
#ifndef R_OK
#define R_OK 4
#endif
#ifndef X_OK
#define X_OK 1
#endif
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
#ifndef F_DUPFD
#define F_DUPFD 0
#endif
#ifndef F_GETFD
#define F_GETFD 1
#endif
#ifndef F_SETFD
#define F_SETFD 2
#endif
#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif
#ifndef F_GETLK
#define F_GETLK 5
#endif
#ifndef F_SETLK
#define F_SETLK 6
#endif
#ifndef F_SETLKW
#define F_SETLKW 7
#endif
#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC 1030
#endif
#ifndef FD_CLOEXEC
#define FD_CLOEXEC 1
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 0x0004
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC 0x0080
#endif
#ifndef ftruncate
#define ftruncate(fd, len) chsize((int)(fd), (long)(len))
#endif
#ifndef _USECONDS_T_DEFINED
#define _USECONDS_T_DEFINED
typedef unsigned long useconds_t;
#endif
/** @brief Synchronizes file state on DOS/OpenWatcom. */
int fsync(int fd);
/** @brief Synchronizes file data on DOS/OpenWatcom. */
int fdatasync(intptr_t fd);
/** @brief File control operations on DOS/OpenWatcom. */
int fcntl(intptr_t fd, int cmd, ...);
/** @brief Sets an environment variable on DOS/OpenWatcom. */
int setenv(const char *name, const char *value, int overwrite);
/** @brief Unsets an environment variable on DOS/OpenWatcom. */
int unsetenv(const char *name);
/** @brief Duplicates at most n bytes of string s on DOS/OpenWatcom. */
char *strndup(const char *s, size_t n);
/** @brief Formats a string into a newly allocated buffer (va_list) on
 * DOS/OpenWatcom. */
int vasprintf(char **strp, const char *fmt, va_list ap);
/** @brief Formats a string into a newly allocated buffer on DOS/OpenWatcom. */
int asprintf(char **strp, const char *fmt, ...);
/** @brief Reads a delimited record from stream on DOS/OpenWatcom. */
ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);
/** @brief Reads a line from stream on DOS/OpenWatcom. */
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
/** @brief Parses suboptions from a string on DOS/OpenWatcom. */
int getsubopt(char **optionp, char *const *tokens, char **valuep);
/** @brief Creates and opens a unique temporary file on DOS/OpenWatcom. */
int mkstemp(char *tmpl);
/** @brief Creates and opens a unique temporary file with flags on
 * DOS/OpenWatcom. */
int mkostemp(char *tmpl, int flags);
/** @brief Creates a unique temporary directory on DOS/OpenWatcom. */
char *mkdtemp(char *tmpl);
/** @brief Opens a simulated pipe stream on DOS/OpenWatcom. */
FILE *popen(const char *command, const char *type);
/** @brief Closes a simulated pipe stream on DOS/OpenWatcom. */
int pclose(FILE *stream);
/** @brief Stubs fork on single-tasking DOS. */
int fork(void);
/** @brief Stubs vfork on single-tasking DOS. */
int vfork(void);
/** @brief Stubs pipe on single-tasking DOS. */
int pipe(int pipefd[2]);
/** @brief Stubs waitpid on single-tasking DOS. */
int waitpid(int pid, int *status, int options);
/** @brief Stubs alarm on DOS/OpenWatcom. */
unsigned int alarm(unsigned int seconds);
/** @brief Stubs ualarm on DOS/OpenWatcom. */
useconds_t ualarm(useconds_t value, useconds_t interval);
/** @brief Stubs pause on DOS/OpenWatcom. */
int pause(void);
/** @brief Stubs chown on DOS/OpenWatcom. */
int chown(const char *pathname, uid_t owner, gid_t group);
/** @brief Stubs fchown on DOS/OpenWatcom. */
int fchown(intptr_t fd, uid_t owner, gid_t group);
/** @brief Stubs lchown on DOS/OpenWatcom. */
int lchown(const char *pathname, uid_t owner, gid_t group);
/** @brief Stubs fchownat on DOS/OpenWatcom. */
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group,
             int flags);
/** @brief Stubs faccessat on DOS/OpenWatcom. */
int faccessat(int dirfd, const char *pathname, int mode, int flags);
/** @brief Stubs openat on DOS/OpenWatcom. */
int openat(int dirfd, const char *pathname, int flags, ...);
/** @brief Stubs link on DOS/OpenWatcom. */
int link(const char *oldpath, const char *newpath);
/** @brief Stubs linkat on DOS/OpenWatcom. */
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags);
/** @brief Stubs symlink on DOS/OpenWatcom. */
int symlink(const char *target, const char *linkpath);
/** @brief Stubs symlinkat on DOS/OpenWatcom. */
int symlinkat(const char *target, int newdirfd, const char *linkpath);
/** @brief Stubs readlink on DOS/OpenWatcom. */
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
/** @brief Stubs readlinkat on DOS/OpenWatcom. */
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);
/** @brief Unlink a file or directory relative to a directory fd on DOS. */
int unlinkat(int dirfd, const char *pathname, int flags);
/** @brief Rename a file relative to directory fds on DOS. */
int renameat(int olddirfd, const char *oldpath, int newdirfd,
             const char *newpath);
/** @brief Create a unique temporary directory on DOS. */
char *mkdtemp(char *template_str);
/** @brief Stub password encryption function on DOS. */
char *crypt(const char *key, const char *salt);
/** @brief Stubs pread on DOS/OpenWatcom. */
ssize_t pread(intptr_t fd, void *buf, size_t count, off_t offset);
/** @brief Stubs pwrite on DOS/OpenWatcom. */
ssize_t pwrite(intptr_t fd, const void *buf, size_t count, off_t offset);
/** @brief Stubs sync on DOS/OpenWatcom. */
void sync(void);
/** @brief Stubs truncate on DOS/OpenWatcom. */
int truncate(const char *path, off_t length);
/** @brief Stubs lockf on DOS/OpenWatcom. */
int lockf(intptr_t fd, int cmd, off_t len);
/** @brief Stubs confstr on DOS/OpenWatcom. */
size_t confstr(int name, char *buf, size_t len);
/** @brief Stubs fpathconf on DOS/OpenWatcom. */
long fpathconf(intptr_t fd, int name);
/** @brief Stubs pathconf on DOS/OpenWatcom. */
long pathconf(const char *pathname, int name);
/** @brief Stubs sysconf on DOS/OpenWatcom. */
long sysconf(int name);
/** @brief Stubs gethostid on DOS/OpenWatcom. */
long gethostid(void);
/** @brief Stubs gethostname on DOS/OpenWatcom. */
int gethostname(char *name, size_t len);
/** @brief Stubs getlogin on DOS/OpenWatcom. */
char *getlogin(void);
/** @brief Stubs getlogin_r on DOS/OpenWatcom. */
int getlogin_r(char *buf, size_t bufsize);
/** @brief Stubs ttyname on DOS/OpenWatcom. */
char *ttyname(intptr_t fd);
/** @brief Stubs ttyname_r on DOS/OpenWatcom. */
int ttyname_r(intptr_t fd, char *buf, size_t buflen);
/** @brief Stubs tcgetpgrp on DOS/OpenWatcom. */
pid_t tcgetpgrp(intptr_t fd);
/** @brief Stubs tcsetpgrp on DOS/OpenWatcom. */
int tcsetpgrp(intptr_t fd, pid_t pgrp);
/** @brief Stubs getuid on DOS/OpenWatcom. */
uid_t getuid(void);
/** @brief Stubs geteuid on DOS/OpenWatcom. */
uid_t geteuid(void);
/** @brief Stubs getgid on DOS/OpenWatcom. */
gid_t getgid(void);
/** @brief Stubs getegid on DOS/OpenWatcom. */
gid_t getegid(void);
/** @brief Stubs getpgid on DOS/OpenWatcom. */
pid_t getpgid(pid_t pid);
/** @brief Stubs getpgrp on DOS/OpenWatcom. */
pid_t getpgrp(void);
/** @brief Stubs getppid on DOS/OpenWatcom. */
pid_t getppid(void);
/** @brief Stubs getsid on DOS/OpenWatcom. */
pid_t getsid(pid_t pid);
/** @brief Stubs setuid on DOS/OpenWatcom. */
int setuid(uid_t uid);
/** @brief Stubs seteuid on DOS/OpenWatcom. */
int seteuid(uid_t euid);
/** @brief Stubs setreuid on DOS/OpenWatcom. */
int setreuid(uid_t ruid, uid_t euid);
/** @brief Stubs setgid on DOS/OpenWatcom. */
int setgid(gid_t gid);
/** @brief Stubs setegid on DOS/OpenWatcom. */
int setegid(gid_t egid);
/** @brief Stubs setregid on DOS/OpenWatcom. */
int setregid(gid_t rgid, gid_t egid);
/** @brief Stubs setpgid on DOS/OpenWatcom. */
int setpgid(pid_t pid, pid_t pgid);
/** @brief Stubs setpgrp on DOS/OpenWatcom. */
pid_t setpgrp(void);
/** @brief Stubs setsid on DOS/OpenWatcom. */
pid_t setsid(void);
/** @brief Stubs getgroups on DOS/OpenWatcom. */
int getgroups(int size, gid_t list[]);
/** @brief Stubs setgroups on DOS/OpenWatcom. */
int setgroups(size_t size, const gid_t *list);
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_MSC_VER)
/** @brief 64-bit file offset type used by posix_read_write hooks. */
typedef __int64 posix_rw_off_t;
#else
/** @brief Platform file offset type used by posix_read_write hooks. */
typedef off_t posix_rw_off_t;
#endif

/** @brief Hook table for mocking Win32 and CRT I/O calls in posix_read_write.
 */
struct posix_rw_win32_hooks {
  void *(*malloc_fn)(size_t size);      /**< Heap allocation callback. */
  void (*free_fn)(void *ptr);           /**< Heap deallocation callback. */
  intptr_t (*get_osfhandle_fn)(int fd); /**< CRT _get_osfhandle callback. */
  int (*open_osfhandle_fn)(intptr_t osfhandle,
                           int flags); /**< CRT _open_osfhandle callback. */
  FILE *(*fdopen_fn)(int fd, const char *mode); /**< CRT _fdopen callback. */
  int (*fileno_fn)(FILE *stream);               /**< CRT _fileno callback. */
  int (*close_fn)(int fd);                      /**< CRT _close callback. */
  int (*read_fn)(int fd, void *buf,
                 unsigned int count); /**< CRT _read callback. */
  int (*write_fn)(int fd, const void *buf,
                  unsigned int count); /**< CRT _write callback. */
  int (*dup_fn)(int fd);               /**< CRT _dup callback. */
  int (*dup2_fn)(int fd1, int fd2);    /**< CRT _dup2 callback. */
  posix_rw_off_t (*lseek_fn)(int fd, posix_rw_off_t offset,
                             int whence); /**< CRT lseek callback. */
  int (*setmode_fn)(int fd, int mode);    /**< CRT _setmode callback. */
  int (*vscprintf_fn)(const char *format, va_list argptr)
      POSIX_CORE_PRINTF_FORMAT(1, 0); /**< Format length callback. */
  int (*vsprintf_buf_fn)(char *buf, size_t cap, const char *format,
                         va_list argptr)
      POSIX_CORE_PRINTF_FORMAT(3, 0); /**< Format buffer callback. */
  unsigned long (*get_temp_path_fn)(unsigned long buf_len,
                                    char *buf); /**< GetTempPathA callback. */
  unsigned int (*get_temp_file_name_fn)(
      const char *path, const char *prefix, unsigned int unique,
      char *out_file); /**< GetTempFileNameA callback. */
  void *(*create_file_fn)(const char *name, unsigned long access,
                          unsigned long share, void *sa, unsigned long disp,
                          unsigned long flags,
                          void *tmpl);  /**< CreateFileA callback. */
  int (*close_handle_fn)(void *handle); /**< CloseHandle callback. */
  unsigned long (*get_current_dir_fn)(
      unsigned long buf_len, char *buf); /**< GetCurrentDirectoryA callback. */
  unsigned long (*get_file_attributes_fn)(
      const char *name); /**< GetFileAttributesA callback. */
  unsigned long (*get_file_type_fn)(void *handle); /**< GetFileType callback. */
  unsigned long (*get_last_error_fn)(void); /**< GetLastError callback. */
  void *(*get_module_handle_fn)(
      const char *name); /**< GetModuleHandleA callback. */
  void *(*get_proc_address_fn)(
      void *module, const char *proc_name); /**< GetProcAddress callback. */
  unsigned long (*get_tick_count_fn)(void); /**< GetTickCount callback. */
  unsigned long (*get_current_pid_fn)(
      void); /**< GetCurrentProcessId callback. */
  int (*read_file_fn)(void *handle, void *buf, unsigned long to_read,
                      unsigned long *bytes_read,
                      void *overlapped); /**< ReadFile callback. */
  int (*write_file_fn)(void *handle, const void *buf, unsigned long to_write,
                       unsigned long *bytes_written,
                       void *overlapped); /**< WriteFile callback. */
  int (*get_overlapped_result_fn)(
      void *handle, void *overlapped, unsigned long *bytes_transferred,
      int wait); /**< GetOverlappedResult callback. */
  int (*set_std_handle_fn)(unsigned long std_id,
                           void *handle); /**< SetStdHandle callback. */
  int (*wsa_startup_fn)(unsigned short ver,
                        void *wsa_data); /**< WSAStartup callback. */
  int (*wsa_get_last_error_fn)(void);    /**< WSAGetLastError callback. */
  int (*getsockopt_fn)(uintptr_t s, int level, int optname, char *optval,
                       int *optlen); /**< getsockopt callback. */
  int (*recv_fn)(uintptr_t s, char *buf, int len,
                 int flags); /**< recv callback. */
  int (*send_fn)(uintptr_t s, const char *buf, int len,
                 int flags);                /**< send callback. */
  int (*shutdown_fn)(uintptr_t s, int how); /**< shutdown callback. */
  int (*closesocket_fn)(uintptr_t s);       /**< closesocket callback. */
  int (*epoll_close_fn)(int fd);            /**< posix_epoll_close callback. */
};

/** @brief Copies default Win32 hooks for posix_read_write into out_hooks. */
void posix_rw_get_default_win32_hooks(struct posix_rw_win32_hooks *out_hooks);
/** @brief Installs custom Win32 hooks for posix_read_write (NULL resets). */
void posix_rw_set_win32_hooks(const struct posix_rw_win32_hooks *hooks);
/** @brief Runs binary stdio initialization (optionally forcing re-init). */
void posix_rw_ensure_binary_stdio(int force_reinit);
/** @brief Clears all registered socket stream entries for isolated testing. */
void posix_rw_reset_socket_streams(void);
#elif !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__) &&     \
    !defined(__linux__) && !defined(__CYGWIN__)
/** @brief Hook table for mocking POSIX calls in posix_read_write. */
struct posix_rw_posix_hooks {
  FILE *(*tmpfile_fn)(void); /**< Temporary file stream creation callback. */
};

/** @brief Copies default POSIX hooks for posix_read_write into out_hooks. */
void posix_rw_get_default_posix_hooks(struct posix_rw_posix_hooks *out_hooks);
/** @brief Installs custom POSIX hooks for posix_read_write (NULL resets). */
void posix_rw_set_posix_hooks(const struct posix_rw_posix_hooks *hooks);
#endif

/** @brief Hook table for cross-platform scratch/mkostemp helpers in posix-core.
 */
struct posix_core_common_hooks {
  void *(*malloc_fn)(size_t size); /**< Heap allocation callback. */
  void (*free_fn)(void *ptr);      /**< Heap deallocation callback. */
  int (*open_scratch_fn)(
      const char *path); /**< Scratch file creation callback. */
  FILE *(*fopen_fn)(const char *path,
                    const char *mode); /**< Stream open callback. */
  int (*system_fn)(const char *cmd);   /**< Command execution callback. */
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  int (*mkstemp_fn)(char *tmpl); /**< POSIX mkstemp callback. */
#endif
};

/** @brief Copies default cross-platform hooks for posix-core into out_hooks. */
void posix_core_get_default_common_hooks(
    struct posix_core_common_hooks *out_hooks);
/** @brief Installs custom cross-platform hooks for posix-core (NULL resets). */
void posix_core_set_common_hooks(const struct posix_core_common_hooks *hooks);

#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/** @brief Checks whether a file descriptor refers to a terminal device. */
int posix_isatty(intptr_t fd);
#endif

#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
/** @brief Assigns a pointer value through a const-qualified pointer slot. */
void ASSIGN_CONST_PTR(const void *pptr, void *v);
/** @brief Zero-allocates memory into a const-qualified pointer slot. */
void XZALLOC_CONST_PTR(const void *pptr, size_t size);
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(__GNUC__) || defined(__clang__)
/** @brief Signed 64-bit integer type for Win32 file size/offset hooks. */
__extension__ typedef long long posix_core_i64_t;
/** @brief Unsigned 64-bit integer type for Win32 memory status hooks. */
__extension__ typedef unsigned long long posix_core_u64_t;
#else
/** @brief Signed 64-bit integer type for Win32 file size/offset hooks. */
typedef __int64 posix_core_i64_t;
/** @brief Unsigned 64-bit integer type for Win32 memory status hooks. */
typedef unsigned __int64 posix_core_u64_t;
#endif

/** @brief Timer callback signature used by Win32 timer queue hooks. */
typedef void(__stdcall *posix_core_timer_cb_t)(void *param,
                                               unsigned char fired);

/** @brief Hook table for mocking Win32 and CRT calls in posix-core. */
struct posix_core_win32_hooks {
  void *(*malloc_fn)(size_t size);             /**< malloc callback. */
  void *(*realloc_fn)(void *ptr, size_t sz);   /**< realloc callback. */
  void (*free_fn)(void *ptr);                  /**< free callback. */
  intptr_t (*get_osfhandle_fn)(int fd);        /**< _get_osfhandle callback. */
  int (*open_osfhandle_fn)(intptr_t h, int f); /**< _open_osfhandle callback. */
  int (*dup_fn)(int fd);                       /**< _dup callback. */
  int (*dup2_fn)(int fd1, int fd2);            /**< _dup2 callback. */
  int (*close_fn)(int fd);                     /**< _close callback. */
  int (*commit_fn)(int fd);                    /**< _commit callback. */
  int (*locking_fn)(int fd, int mode, long nbytes); /**< _locking callback. */
  int (*pipe_fn)(int *pfds, unsigned int psize,
                 int textmode);              /**< _pipe callback. */
  int (*isatty_fn)(int fd);                  /**< _isatty callback. */
  int (*open_rdwr_fn)(const char *path);     /**< Read-write open callback. */
  int (*chsize_fn)(int fd, long size);       /**< _chsize callback. */
  int (*access_fn)(const char *p, int m);    /**< _access callback. */
  int (*chmod_fn)(const char *p, int m);     /**< _chmod callback. */
  int (*unlink_fn)(const char *p);           /**< _unlink callback. */
  int (*rmdir_fn)(const char *p);            /**< _rmdir callback. */
  char *(*getcwd_fn)(char *buf, int maxlen); /**< _getcwd callback. */
  int (*chdir_fn)(const char *p);            /**< _chdir callback. */
  char *(*fullpath_fn)(char *abs_p, const char *rel_p,
                       size_t max_len);      /**< _fullpath callback. */
  int (*mktemp_s_fn)(char *tmpl, size_t sz); /**< _mktemp_s callback. */
  intptr_t (*execve_fn)(const char *cmd, const char *const *argv,
                        const char *const *envp); /**< _execve callback. */
  intptr_t (*spawnve_fn)(int mode, const char *cmd, const char *const *argv,
                         const char *const *envp); /**< _spawnve callback. */
  intptr_t (*spawnvpe_fn)(int mode, const char *cmd, const char *const *argv,
                          const char *const *envp); /**< _spawnvpe callback. */
  intptr_t (*spawnv_fn)(int mode, const char *cmd,
                        const char *const *argv); /**< _spawnv callback. */
  int (*raise_fn)(int sig);                       /**< raise callback. */
  void *(*get_module_handle_fn)(const char *n);   /**< GetModuleHandleA. */
  void *(*get_proc_address_fn)(void *m, const char *p); /**< GetProcAddress. */
  unsigned long (*get_last_error_fn)(void);             /**< GetLastError. */
  int (*get_file_size_ex_fn)(void *h,
                             posix_core_i64_t *sz); /**< GetFileSizeEx. */
  int (*set_file_pointer_ex_fn)(void *h, posix_core_i64_t dist,
                                posix_core_i64_t *newp,
                                unsigned long method); /**< SetFilePointerEx. */
  int (*set_end_of_file_fn)(void *h);                  /**< SetEndOfFile. */
  int (*lock_file_ex_fn)(void *h, unsigned long fl, unsigned long res,
                         unsigned long lo, unsigned long hi,
                         void *ov); /**< LockFileEx. */
  int (*unlock_file_ex_fn)(void *h, unsigned long res, unsigned long lo,
                           unsigned long hi, void *ov); /**< UnlockFileEx. */
  void *(*create_file_mapping_fn)(void *h, void *sa, unsigned long prot,
                                  unsigned long hi, unsigned long lo,
                                  const char *name); /**< CreateFileMappingA. */
  void *(*map_view_of_file_fn)(void *h, unsigned long acc, unsigned long hi,
                               unsigned long lo,
                               size_t bytes);     /**< MapViewOfFile. */
  int (*unmap_view_of_file_fn)(const void *addr); /**< UnmapViewOfFile. */
  int (*create_timer_fn)(void **out_t, void *q, posix_core_timer_cb_t cb,
                         void *param, unsigned long due, unsigned long period,
                         unsigned long fl); /**< CreateTimerQueueTimer. */
  int (*delete_timer_fn)(void *q, void *t,
                         void *ev);         /**< DeleteTimerQueueTimer. */
  unsigned long (*get_tick_count_fn)(void); /**< GetTickCount. */
  unsigned long (*get_file_attributes_fn)(
      const char *p); /**< GetFileAttributesA. */
  unsigned long (*get_current_dir_fn)(unsigned long len,
                                      char *buf); /**< GetCurrentDirectoryA. */
  int (*set_current_dir_fn)(const char *p);       /**< SetCurrentDirectoryA. */
  int (*create_directory_fn)(const char *p, void *sa); /**< CreateDirectoryA. */
  void *(*create_file_fn)(const char *n, unsigned long acc, unsigned long sh,
                          void *sa, unsigned long disp, unsigned long fl,
                          void *tmpl); /**< CreateFileA. */
  int (*close_handle_fn)(void *h);     /**< CloseHandle. */
  int (*read_file_fn)(void *h, void *buf, unsigned long n, unsigned long *br,
                      void *ov); /**< ReadFile. */
  int (*write_file_fn)(void *h, const void *buf, unsigned long n,
                       unsigned long *bw, void *ov); /**< WriteFile. */
  int (*device_io_control_fn)(void *h, unsigned long code, void *in_b,
                              unsigned long in_sz, void *out_b,
                              unsigned long out_sz, unsigned long *ret_b,
                              void *ov); /**< DeviceIoControl. */
  int (*wide_char_to_multi_byte_fn)(unsigned int cp, unsigned long fl,
                                    const wchar_t *ws, int wlen, char *mb,
                                    int mblen, const char *def_c,
                                    int *used_def); /**< WideCharToMultiByte. */
  int (*create_hard_link_fn)(const char *new_p, const char *old_p,
                             void *sa); /**< CreateHardLinkA. */
  int (*move_file_ex_fn)(const char *old_p, const char *new_p,
                         unsigned long fl); /**< MoveFileExA. */
  int (*copy_file_fn)(const char *old_p, const char *new_p,
                      int fail_if_exists);               /**< CopyFileA. */
  int (*get_user_name_fn)(char *buf, unsigned long *sz); /**< GetUserNameA. */
  unsigned long (*sleep_ex_fn)(unsigned long ms,
                               int alertable);         /**< SleepEx. */
  unsigned long (*get_logical_drives_fn)(void);        /**< GetLogicalDrives. */
  unsigned int (*get_drive_type_fn)(const char *root); /**< GetDriveTypeA. */
  int (*flush_file_buffers_fn)(void *h);               /**< FlushFileBuffers. */
  void (*get_system_info_fn)(void *si);                /**< GetSystemInfo. */
  int (*global_memory_status_ex_fn)(void *ms); /**< GlobalMemoryStatusEx. */
  unsigned long (*get_module_file_name_fn)(
      void *m, char *buf, unsigned long sz); /**< GetModuleFileNameA. */
  int (*set_std_handle_fn)(unsigned long id, void *h); /**< SetStdHandle. */
  int (*set_handle_information_fn)(void *h, unsigned long mask,
                                   unsigned long fl); /**< SetHandleInfo. */
  unsigned long (*wait_for_single_object_fn)(
      void *h, unsigned long ms); /**< WaitForSingleObject. */
  int (*get_exit_code_process_fn)(
      void *h, unsigned long *code); /**< GetExitCodeProcess. */
  int (*ioctlsocket_fn)(uintptr_t s, long cmd,
                        unsigned long *argp); /**< ioctlsocket. */
  int (*wsa_get_last_error_fn)(void);         /**< WSAGetLastError. */
  int (*socketpair_fn)(int dom, int type, int proto,
                       intptr_t sv[2]); /**< posix_socketpair. */
  int (*crypt_acquire_context_fn)(size_t *prov, const char *cont,
                                  const char *prv, unsigned long type,
                                  unsigned long fl); /**< CryptAcquireCtxA. */
  int (*crypt_create_hash_fn)(size_t prov, unsigned int alg, size_t key,
                              unsigned long fl,
                              size_t *hash); /**< CryptCreateHash. */
  int (*crypt_hash_data_fn)(size_t hash, const unsigned char *data,
                            unsigned long len,
                            unsigned long fl); /**< CryptHashData. */
  int (*crypt_get_hash_param_fn)(size_t hash, unsigned long param,
                                 unsigned char *data, unsigned long *len,
                                 unsigned long fl); /**< CryptGetHashParam. */
  int (*crypt_destroy_hash_fn)(size_t hash);        /**< CryptDestroyHash. */
  int (*crypt_release_context_fn)(size_t prov,
                                  unsigned long fl); /**< CryptReleaseCtx. */
  int (*crypt_derive_key_fn)(size_t prov, unsigned int alg, size_t base,
                             unsigned long fl,
                             size_t *key); /**< CryptDeriveKey. */
  int (*crypt_encrypt_fn)(size_t key, size_t hash, int final_b,
                          unsigned long fl, unsigned char *data,
                          unsigned long *len,
                          unsigned long cap); /**< CryptEncrypt. */
  int (*crypt_decrypt_fn)(size_t key, size_t hash, int final_b,
                          unsigned long fl, unsigned char *data,
                          unsigned long *len); /**< CryptDecrypt. */
  int (*crypt_destroy_key_fn)(size_t key);     /**< CryptDestroyKey. */
};

/** @brief Copies default Win32 hooks for posix-core into out_hooks. */
void posix_core_get_default_win32_hooks(
    struct posix_core_win32_hooks *out_hooks);
/** @brief Installs custom Win32 hooks for posix-core (NULL resets). */
void posix_core_set_win32_hooks(const struct posix_core_win32_hooks *hooks);
/** @brief Resets internal timer, fork, and vfork tracking tables for tests. */
void posix_core_reset_process_state(int child_count, int vfork_depth,
                                    int reap_count);
/** @brief Child exit hook used by forked processes on Windows. */
void my_exit_hook(int status);
/** @brief Terminates the current process via Kernel32 TerminateProcess. */
auto_win_msvc_error_t auto_win_exit(int code);
/** @brief Sets the system clock (stub on Windows). */
int settimeofday(const void *tv, const void *tz);
/** @brief Pushes a simulated vfork state frame on Windows. */
int awm_vfork_push(void **globals_slot);
/** @brief Retrieves the active jmp_buf pointer for the current vfork frame. */
void *awm_vfork_jb(void);
/** @brief Returns the simulated child PID to the parent after vfork longjmp. */
int awm_vfork_parent_pid(void);
/** @brief Returns current nesting depth of simulated vfork frames. */
int awm_vfork_depth(void);
/** @brief Exits a simulated vfork child frame and restores parent context. */
void awm_vfork_exit(int status);
/** @brief Reaps a completed simulated vfork child process. */
int awm_vfork_try_reap(int pid, int *stat_loc);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_CORE_H */
