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

/* clang-format off */
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
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
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#include <fcntl.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <process.h>
#include <share.h>
#include <sys/stat.h>
#include <sys/types.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-core operations.
 */
enum posix_core_error_code {
  /** @brief Successful operation. */
  POSIX_CORE_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  POSIX_CORE_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed. */
  POSIX_CORE_ERROR_INVALID_ARGUMENT = 2,
  /** @brief Operation failed or system error. */
  POSIX_CORE_ERROR_OPERATION_FAILED = 3
};

/**
 * @brief Initializes and validates the posix-core module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return POSIX_CORE_SUCCESS on success, or an error code on failure.
 */
enum posix_core_error_code posix_core_init(int *out_status);

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

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#else
#ifndef STDIN_FILENO
#define STDIN_FILENO STDIN_FILENO
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#else
#ifndef STDOUT_FILENO
#define STDOUT_FILENO STDOUT_FILENO
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
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
#define O_TEXT O_TEXT
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_BINARY
#define O_BINARY _O_BINARY
#endif
#else
#ifndef O_BINARY
#define O_BINARY O_BINARY
#endif
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef O_CLOEXEC
#define O_CLOEXEC _O_NOINHERIT
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
/** @brief open */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef open
static __inline int posix_core_open(const char *filename, int oflag, ...) {
  int fd = -1;
  int pmode = 0;
  oflag |= _O_BINARY;
  if (oflag & _O_CREAT) {
    va_list ap;
    va_start(ap, oflag);
    pmode = va_arg(ap, int) & (_S_IREAD | _S_IWRITE);
    va_end(ap);
  }
#if defined(__STDC_SECURE_LIB__) || defined(__STDC_WANT_SECURE_LIB__) ||       \
    _MSC_VER >= 1400
  if (_sopen_s(&fd, filename, oflag, _SH_DENYNO, pmode) != 0) {
    if (errno == EACCES) {
      struct _stat st;
      if (_stat(filename, &st) == 0 && (st.st_mode & _S_IFDIR)) {
        errno = EISDIR;
      }
    }
    return -1;
  }
  return fd;
#else
  int ret = _open(filename, oflag, pmode);
  if (ret == -1 && errno == EACCES) {
    struct _stat st;
    if (_stat(filename, &st) == 0 && (st.st_mode & _S_IFDIR)) {
      errno = EISDIR;
    }
  }
  return ret;
#endif
}
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#define open posix_core_open
#endif
#endif
#else
/* open */
#endif
/** @brief close */
int posix_close(intptr_t fd);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef close
#define close posix_close
#endif
#endif
#endif

/** @brief read */
ssize_t posix_read(intptr_t fd, void *buf, size_t count);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef read
#define read posix_read
#endif
#endif
#endif

/** @brief write */
ssize_t posix_write(intptr_t fd, const void *buf, size_t count);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef write
#define write posix_write
#endif
#endif
#endif

/** @brief Open file descriptor with POSIX semantics. */
int posix_open(const char *pathname, int flags, ...);

/** @brief lseek */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef lseek
#define lseek _lseek
#endif
#else
/* lseek */
#endif
/** @brief dup */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef dup
#define dup _dup
#endif
#else
/* dup */
#endif
/** @brief dup2 */
int posix_dup2(int oldfd, int newfd);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef dup2
#define dup2 posix_dup2
#endif
#endif
/** @brief fsync */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef tell
#define tell _tell
#endif

#ifndef fsync
#define fsync _commit
#endif
#else
/* fsync */
#endif
/** @brief ftruncate */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef ftruncate
#define ftruncate(fd, size) _chsize(fd, (long)(size))
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
#ifndef execve
#define execve _execve
#endif
#else
/* execve */
#endif
/** @brief execv */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef execv
#define execv _execv
#endif
#else
/* execv */
#endif
/** @brief execvp */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef execvp
#define execvp _execvp
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
#ifndef sleep
#endif
#else
/* sleep */
#endif
/** @brief usleep */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef usleep

static __inline unsigned int posix_core_sleep(unsigned int seconds) {
  Sleep(seconds * 1000);
  return 0;
}
#define sleep posix_core_sleep

/** \brief posix_core_usleep function. */
static __inline int posix_core_usleep(unsigned int usec) {
  Sleep(usec / 1000);
  return 0;
}
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
/** @brief creat */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef creat
static __inline int posix_core_creat(const char *filename, int pmode) {
  int fd = -1;
  int win_pmode = pmode & (_S_IREAD | _S_IWRITE);
  if (win_pmode == 0) {
    win_pmode = _S_IREAD | _S_IWRITE;
  }
#if defined(__STDC_SECURE_LIB__) || defined(__STDC_WANT_SECURE_LIB__) ||       \
    _MSC_VER >= 1400
  if (_sopen_s(&fd, filename, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY,
               _SH_DENYNO, win_pmode) != 0) {
    return -1;
  }
  return fd;
#else
  return _creat(filename, win_pmode);
#endif
}
#define creat posix_core_creat
#endif
#else
/* creat */
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
#define AT_FDCWD -100
#endif

/** \brief Flag to not follow symlinks for at-functions. */
#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif

/** \brief Flag to check effective user/group IDs for at-functions. */
#ifndef AT_EACCESS
#define AT_EACCESS 0x200
#endif

/** \brief Flag to remove directory instead of unlinking file for unlinkat. */
#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR 0x200
#endif

/** \brief Flag to follow symlinks for linkat. */
#ifndef AT_SYMLINK_FOLLOW
#define AT_SYMLINK_FOLLOW 0x400
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief openat */
int openat(int dirfd, const char *pathname, int flags, ...);
#else
/* openat */
#endif
/** @brief posix_fadvise */
int posix_fadvise(intptr_t fd, off_t offset, off_t len, int advice);
/* posix_fadvise */

/** @brief posix_fallocate */
int posix_fallocate(intptr_t fd, off_t offset, off_t len);
/* posix_fallocate */
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
/** \brief sync_file_range function. */
int sync_file_range(intptr_t fd, off_t offset, off_t nbytes,
                    unsigned int flags);
/* sync_file_range */
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
/** @brief fdatasync */
int fdatasync(intptr_t fd);
/* fdatasync */
/** @brief fexecve */
int fexecve(intptr_t fd, char *const argv[], char *const envp[]);
/* fexecve */
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
/* getopt provided by environment */
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
#if defined(_WIN32) && !defined(__CYGWIN__)
#undef rename
#ifndef rename
#define rename(oldpath, newpath) posix_rename((oldpath), (newpath))
#endif
#endif

/** @brief mkstemp (POSIX semantics with SHARE_DELETE) */
int posix_mkstemp(char *tmpl);
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef mkstemp
#define mkstemp posix_mkstemp
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
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef fopen
#define fopen posix_fopen
#endif
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_CORE_H */
