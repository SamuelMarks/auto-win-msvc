/* posix-core.h - Strict C89 Header */
#ifndef POSIX_CORE_H
#define POSIX_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_MSC_VER)
#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
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

#ifndef _MODE_T_DEFINED
#define _MODE_T_DEFINED
typedef unsigned int mode_t;
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

#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <share.h>
#include <sys/stat.h>
__declspec(dllimport) void __stdcall Sleep(unsigned long dwMilliseconds);
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
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
#ifndef NUM_FORMAT
#if defined(_MSC_VER)
#define NUM_FORMAT "%I64d"
#else
#define NUM_FORMAT "%lld"
#endif
#endif

/* Functions */
/** @brief open */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef open
static __inline int posix_core_open(const char *filename, int oflag, ...) {
  int fd = -1;
  int pmode = 0;
  if (oflag & _O_CREAT) {
    va_list ap;
    va_start(ap, oflag);
    pmode = va_arg(ap, int);
    va_end(ap);
  }
#if defined(__STDC_SECURE_LIB__) || defined(__STDC_WANT_SECURE_LIB__) ||       \
    _MSC_VER >= 1400
  if (_sopen_s(&fd, filename, oflag, _SH_DENYNO, pmode) != 0) {
    return -1;
  }
  return fd;
#else
  return _open(filename, oflag, pmode);
#endif
}
#define open posix_core_open
#endif
#else
/* open */
#endif
/** @brief close */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef close
#define close _close
#endif
#else
/* close */
#endif
/** @brief read */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef read
#define read _read
#endif
#else
/* read */
#endif
/** @brief write */
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef write
#define write _write
#endif
#else
/* write */
#endif
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
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef dup2
#define dup2 _dup2
#endif
#else
/* dup2 */
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
#define ftruncate _chsize
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
#define isatty _isatty
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
#if defined(__STDC_SECURE_LIB__) || defined(__STDC_WANT_SECURE_LIB__) ||       \
    _MSC_VER >= 1400
  if (_sopen_s(&fd, filename, _O_WRONLY | _O_CREAT | _O_TRUNC, _SH_DENYNO,
               pmode) != 0) {
    return -1;
  }
  return fd;
#else
  return _creat(filename, pmode);
#endif
}
#define creat posix_core_creat
#endif
#else
/* creat */
#endif
/** @brief fcntl */
#if defined(_WIN32) && !defined(__CYGWIN__)
int fcntl(int fd, int cmd, ...);
#else
/* fcntl */
#endif
/** @brief openat */
#if defined(_WIN32) && !defined(__CYGWIN__)
int openat(int dirfd, const char *pathname, int flags, ...);
#else
/* openat */
#endif
/** @brief posix_fadvise */
#if defined(_WIN32) && !defined(__CYGWIN__)
int posix_fadvise(int fd, off_t offset, off_t len, int advice);
#else
/* posix_fadvise */
#endif
/** @brief posix_fallocate */
#if defined(_WIN32) && !defined(__CYGWIN__)
int posix_fallocate(int fd, off_t offset, off_t len);
#else
/* posix_fallocate */
#endif
/** @brief sync_file_range */
#if defined(_WIN32) && !defined(__CYGWIN__)
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
int sync_file_range(int fd, off_t offset, off_t nbytes, unsigned int flags);
#else
/* sync_file_range */
#endif
/** @brief alarm */
#if defined(_WIN32) && !defined(__CYGWIN__)
unsigned int alarm(unsigned int seconds);
#else
/* alarm */
#endif
/** @brief chown */
#if defined(_WIN32) && !defined(__CYGWIN__)
int chown(const char *pathname, uid_t owner, gid_t group);
#else
/* chown */
#endif
/** @brief confstr */
#if defined(_WIN32) && !defined(__CYGWIN__)
size_t confstr(int name, char *buf, size_t len);
#else
/* confstr */
#endif
/** @brief crypt */
#if defined(_WIN32) && !defined(__CYGWIN__)
char *crypt(const char *key, const char *salt);
#else
/* crypt */
#endif
/** @brief encrypt */
#if defined(_WIN32) && !defined(__CYGWIN__)
void encrypt(char block[64], int edflag);
#else
/* encrypt */
#endif
/** @brief faccessat */
#if defined(_WIN32) && !defined(__CYGWIN__)
int faccessat(int dirfd, const char *pathname, int mode, int flags);
#else
/* faccessat */
#endif
/** @brief fchown */
#if defined(_WIN32) && !defined(__CYGWIN__)
int fchown(int fd, uid_t owner, gid_t group);
#else
/* fchown */
#endif
/** @brief fchownat */
#if defined(_WIN32) && !defined(__CYGWIN__)
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group,
             int flags);
#else
/* fchownat */
#endif
/** @brief fdatasync */
#if defined(_WIN32) && !defined(__CYGWIN__)
int fdatasync(int fd);
#else
/* fdatasync */
#endif
/** @brief fexecve */
#if defined(_WIN32) && !defined(__CYGWIN__)
int fexecve(int fd, char *const argv[], char *const envp[]);
#else
/* fexecve */
#endif
/** @brief fork */
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t fork(void);
#else
/* fork */
#endif
/** @brief fpathconf */
#if defined(_WIN32) && !defined(__CYGWIN__)
long fpathconf(int fd, int name);
#else
/* fpathconf */
#endif
/** @brief getegid */
#if defined(_WIN32) && !defined(__CYGWIN__)
gid_t getegid(void);
#else
/* getegid */
#endif
/** @brief geteuid */
#if defined(_WIN32) && !defined(__CYGWIN__)
uid_t geteuid(void);
#else
/* geteuid */
#endif
/** @brief getgid */
#if defined(_WIN32) && !defined(__CYGWIN__)
gid_t getgid(void);
#else
/* getgid */
#endif
/** @brief getgroups */
#if defined(_WIN32) && !defined(__CYGWIN__)
int getgroups(int size, gid_t list[]);
#else
/* getgroups */
#endif
/** @brief gethostid */
#if defined(_WIN32) && !defined(__CYGWIN__)
long gethostid(void);
#else
/* gethostid */
#endif
/** @brief gethostname */
#if defined(_WIN32) && !defined(__CYGWIN__)
int gethostname(char *name, int len);
#else
/* gethostname */
#endif
/** @brief getlogin */
#if defined(_WIN32) && !defined(__CYGWIN__)
char *getlogin(void);
#else
/* getlogin */
#endif
/** @brief getlogin_r */
#if defined(_WIN32) && !defined(__CYGWIN__)
int getlogin_r(char *buf, size_t bufsize);
#else
/* getlogin_r */
#endif
/** @brief getopt */
#if defined(_WIN32) && !defined(__CYGWIN__)
int getopt(int argc, char *const argv[], const char *optstring);
#else
/* getopt */
#endif
/** @brief getpgid */
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t getpgid(pid_t pid);
#else
/* getpgid */
#endif
/** @brief getpgrp */
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t getpgrp(void);
#else
/* getpgrp */
#endif
/** @brief getppid */
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t getppid(void);
#else
/* getppid */
#endif
/** @brief getsid */
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t getsid(pid_t pid);
#else
/* getsid */
#endif
/** @brief getuid */
#if defined(_WIN32) && !defined(__CYGWIN__)
uid_t getuid(void);
#else
/* getuid */
#endif
/** @brief lchown */
#if defined(_WIN32) && !defined(__CYGWIN__)
int lchown(const char *pathname, uid_t owner, gid_t group);
#else
/* lchown */
#endif
/** @brief link */
#if defined(_WIN32) && !defined(__CYGWIN__)
int link(const char *oldpath, const char *newpath);
#else
/* link */
#endif
/** @brief linkat */
#if defined(_WIN32) && !defined(__CYGWIN__)
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags);
#else
/* linkat */
#endif
/** @brief lockf */
#if defined(_WIN32) && !defined(__CYGWIN__)
int lockf(int fd, int cmd, off_t len);
#else
/* lockf */
#endif
/** @brief pathconf */
#if defined(_WIN32) && !defined(__CYGWIN__)
long pathconf(const char *pathname, int name);
#else
/* pathconf */
#endif
/** @brief pause */
#if defined(_WIN32) && !defined(__CYGWIN__)
int pause(void);
#else
/* pause */
#endif
/** @brief pipe */
#if defined(_WIN32) && !defined(__CYGWIN__)
int pipe(int pipefd[2]);
#else
/* pipe */
#endif
/** @brief pipe2 */
#if defined(_WIN32) && !defined(__CYGWIN__)
int pipe2(int pipefd[2], int flags);
#else
/* pipe2 */
#endif
/** @brief pread */
#if defined(_WIN32) && !defined(__CYGWIN__)
ssize_t pread(int fd, void *buf, size_t count, off_t offset);
#else
/* pread */
#endif
/** @brief pwrite */
#if defined(_WIN32) && !defined(__CYGWIN__)
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
#else
/* pwrite */
#endif
/** @brief readlink */
#if defined(_WIN32) && !defined(__CYGWIN__)
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
#else
/* readlink */
#endif
/** @brief readlinkat */
#if defined(_WIN32) && !defined(__CYGWIN__)
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);
#else
/* readlinkat */
#endif
/** @brief setegid */
#if defined(_WIN32) && !defined(__CYGWIN__)
int setegid(gid_t egid);
#else
/* setegid */
#endif
/** @brief seteuid */
#if defined(_WIN32) && !defined(__CYGWIN__)
int seteuid(uid_t euid);
#else
/* seteuid */
#endif
/** @brief setgid */
#if defined(_WIN32) && !defined(__CYGWIN__)
int setgid(gid_t gid);
#else
/* setgid */
#endif
/** @brief setpgid */
#if defined(_WIN32) && !defined(__CYGWIN__)
int setpgid(pid_t pid, pid_t pgid);
#else
/* setpgid */
#endif
/** @brief setpgrp */
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t setpgrp(void);
#else
/* setpgrp */
#endif
/** @brief setregid */
#if defined(_WIN32) && !defined(__CYGWIN__)
int setregid(gid_t rgid, gid_t egid);
#else
/* setregid */
#endif
/** @brief setreuid */
#if defined(_WIN32) && !defined(__CYGWIN__)
int setreuid(uid_t ruid, uid_t euid);
#else
/* setreuid */
#endif
/** @brief setsid */
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t setsid(void);
#else
/* setsid */
#endif
/** @brief setuid */
#if defined(_WIN32) && !defined(__CYGWIN__)
int setuid(uid_t uid);
#else
/* setuid */
#endif
/** @brief symlink */
#if defined(_WIN32) && !defined(__CYGWIN__)
int symlink(const char *target, const char *linkpath);
#else
/* symlink */
#endif
/** @brief symlinkat */
#if defined(_WIN32) && !defined(__CYGWIN__)
int symlinkat(const char *target, int newdirfd, const char *linkpath);
#else
/* symlinkat */
#endif
/** @brief sync */
#if defined(_WIN32) && !defined(__CYGWIN__)
void sync(void);
#else
/* sync */
#endif
/** @brief sysconf */
#if defined(_WIN32) && !defined(__CYGWIN__)
long sysconf(int name);
#else
/* sysconf */
#endif
/** @brief tcgetpgrp */
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t tcgetpgrp(int fd);
#else
/* tcgetpgrp */
#endif
/** @brief tcsetpgrp */
#if defined(_WIN32) && !defined(__CYGWIN__)
int tcsetpgrp(int fd, pid_t pgrp);
#else
/* tcsetpgrp */
#endif
/** @brief truncate */
#if defined(_WIN32) && !defined(__CYGWIN__)
int truncate(const char *path, off_t length);
#else
/* truncate */
#endif
/** @brief ttyname */
#if defined(_WIN32) && !defined(__CYGWIN__)
char *ttyname(int fd);
#else
/* ttyname */
#endif
/** @brief ttyname_r */
#if defined(_WIN32) && !defined(__CYGWIN__)
int ttyname_r(int fd, char *buf, size_t buflen);
#else
/* ttyname_r */
#endif
/** @brief ualarm */
#if defined(_WIN32) && !defined(__CYGWIN__)
useconds_t ualarm(useconds_t value, useconds_t interval);
#else
/* ualarm */
#endif
/** @brief vfork */
#if defined(_WIN32) && !defined(__CYGWIN__)
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

#ifdef __cplusplus
}
#endif
#endif
