/* posix-core.h - Strict C89 Header */
#ifndef POSIX_CORE_H
#define POSIX_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdarg.h>

#ifdef _MSC_VER
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
typedef long off_t;
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

#include <io.h>
#include <fcntl.h>
#include <process.h>
#include <direct.h>
#include <sys/stat.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#endif

#ifdef _MSC_VER
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#else
#ifndef STDIN_FILENO
#define STDIN_FILENO STDIN_FILENO
#endif
#endif
#ifdef _MSC_VER
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#else
#ifndef STDOUT_FILENO
#define STDOUT_FILENO STDOUT_FILENO
#endif
#endif
#ifdef _MSC_VER
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
#else
#ifndef STDERR_FILENO
#define STDERR_FILENO STDERR_FILENO
#endif
#endif
#ifdef _MSC_VER
#ifndef O_RDONLY
#define O_RDONLY _O_RDONLY
#endif
#else
#ifndef O_RDONLY
#define O_RDONLY O_RDONLY
#endif
#endif
#ifdef _MSC_VER
#ifndef O_WRONLY
#define O_WRONLY _O_WRONLY
#endif
#else
#ifndef O_WRONLY
#define O_WRONLY O_WRONLY
#endif
#endif
#ifdef _MSC_VER
#ifndef O_RDWR
#define O_RDWR _O_RDWR
#endif
#else
#ifndef O_RDWR
#define O_RDWR O_RDWR
#endif
#endif
#ifdef _MSC_VER
#ifndef O_APPEND
#define O_APPEND _O_APPEND
#endif
#else
#ifndef O_APPEND
#define O_APPEND O_APPEND
#endif
#endif
#ifdef _MSC_VER
#ifndef O_CREAT
#define O_CREAT _O_CREAT
#endif
#else
#ifndef O_CREAT
#define O_CREAT O_CREAT
#endif
#endif
#ifdef _MSC_VER
#ifndef O_TRUNC
#define O_TRUNC _O_TRUNC
#endif
#else
#ifndef O_TRUNC
#define O_TRUNC O_TRUNC
#endif
#endif
#ifdef _MSC_VER
#ifndef O_EXCL
#define O_EXCL _O_EXCL
#endif
#else
#ifndef O_EXCL
#define O_EXCL O_EXCL
#endif
#endif
#ifdef _MSC_VER
#ifndef O_TEXT
#define O_TEXT _O_TEXT
#endif
#else
#ifndef O_TEXT
#define O_TEXT O_TEXT
#endif
#endif
#ifdef _MSC_VER
#ifndef O_BINARY
#define O_BINARY _O_BINARY
#endif
#else
#ifndef O_BINARY
#define O_BINARY O_BINARY
#endif
#endif
#ifdef _MSC_VER
#ifndef O_CLOEXEC
#define O_CLOEXEC _O_NOINHERIT
#endif
#else
#ifndef O_CLOEXEC
#define O_CLOEXEC O_CLOEXEC
#endif
#endif

/* Functions */
/** @brief open */
#ifdef _MSC_VER
#ifndef open
#define open _open
#endif
#else
/* open */
#endif
/** @brief close */
#ifdef _MSC_VER
#ifndef close
#define close _close
#endif
#else
/* close */
#endif
/** @brief read */
#ifdef _MSC_VER
#ifndef read
#define read _read
#endif
#else
/* read */
#endif
/** @brief write */
#ifdef _MSC_VER
#ifndef write
#define write _write
#endif
#else
/* write */
#endif
/** @brief lseek */
#ifdef _MSC_VER
#ifndef lseek
#define lseek _lseek
#endif
#else
/* lseek */
#endif
/** @brief dup */
#ifdef _MSC_VER
#ifndef dup
#define dup _dup
#endif
#else
/* dup */
#endif
/** @brief dup2 */
#ifdef _MSC_VER
#ifndef dup2
#define dup2 _dup2
#endif
#else
/* dup2 */
#endif
/** @brief fsync */
#ifdef _MSC_VER
#ifndef fsync
#define fsync _commit
#endif
#else
/* fsync */
#endif
/** @brief ftruncate */
#ifdef _MSC_VER
#ifndef ftruncate
#define ftruncate _chsize
#endif
#else
/* ftruncate */
#endif
/** @brief access */
#ifdef _MSC_VER
#ifndef access
#define access _access
#endif
#else
/* access */
#endif
/** @brief rmdir */
#ifdef _MSC_VER
#ifndef rmdir
#define rmdir _rmdir
#endif
#else
/* rmdir */
#endif
/** @brief chdir */
#ifdef _MSC_VER
#ifndef chdir
#define chdir _chdir
#endif
#else
/* chdir */
#endif
/** @brief getcwd */
#ifdef _MSC_VER
#ifndef getcwd
#define getcwd _getcwd
#endif
#else
/* getcwd */
#endif
/** @brief unlink */
#ifdef _MSC_VER
#ifndef unlink
#define unlink _unlink
#endif
#else
/* unlink */
#endif
/** @brief execve */
#ifdef _MSC_VER
#ifndef execve
#define execve _execve
#endif
#else
/* execve */
#endif
/** @brief execv */
#ifdef _MSC_VER
#ifndef execv
#define execv _execv
#endif
#else
/* execv */
#endif
/** @brief execvp */
#ifdef _MSC_VER
#ifndef execvp
#define execvp _execvp
#endif
#else
/* execvp */
#endif
/** @brief execl */
#ifdef _MSC_VER
#ifndef execl
#define execl _execl
#endif
#else
/* execl */
#endif
/** @brief execlp */
#ifdef _MSC_VER
#ifndef execlp
#define execlp _execlp
#endif
#else
/* execlp */
#endif
/** @brief getpid */
#ifdef _MSC_VER
#ifndef getpid
#define getpid _getpid
#endif
#else
/* getpid */
#endif
/** @brief sleep */
#ifdef _MSC_VER
#ifndef sleep
#define sleep(x) (Sleep((x) * 1000), 0)
#endif
#else
/* sleep */
#endif
/** @brief usleep */
#ifdef _MSC_VER
#ifndef usleep
#define sleep(x) (Sleep((x) * 1000), 0)
#endif
#else
/* usleep */
#endif
/** @brief isatty */
#ifdef _MSC_VER
#ifndef isatty
#define isatty _isatty
#endif
#else
/* isatty */
#endif
/** @brief swab */
#ifdef _MSC_VER
#ifndef swab
#define swab _swab
#endif
#else
/* swab */
#endif
/** @brief creat */
#ifdef _MSC_VER
#ifndef creat
#define creat _creat
#endif
#else
/* creat */
#endif
/** @brief fcntl */
#ifdef _MSC_VER
int fcntl(int fd, int cmd, ...);
#else
/* fcntl */
#endif
/** @brief openat */
#ifdef _MSC_VER
int openat(int dirfd, const char *pathname, int flags, ...);
#else
/* openat */
#endif
/** @brief posix_fadvise */
#ifdef _MSC_VER
int posix_fadvise(int fd, off_t offset, off_t len, int advice);
#else
/* posix_fadvise */
#endif
/** @brief posix_fallocate */
#ifdef _MSC_VER
int posix_fallocate(int fd, off_t offset, off_t len);
#else
/* posix_fallocate */
#endif
/** @brief alarm */
#ifdef _MSC_VER
unsigned int alarm(unsigned int seconds);
#else
/* alarm */
#endif
/** @brief chown */
#ifdef _MSC_VER
int chown(const char *pathname, uid_t owner, gid_t group);
#else
/* chown */
#endif
/** @brief confstr */
#ifdef _MSC_VER
size_t confstr(int name, char *buf, size_t len);
#else
/* confstr */
#endif
/** @brief crypt */
#ifdef _MSC_VER
char *crypt(const char *key, const char *salt);
#else
/* crypt */
#endif
/** @brief encrypt */
#ifdef _MSC_VER
void encrypt(char block[64], int edflag);
#else
/* encrypt */
#endif
/** @brief faccessat */
#ifdef _MSC_VER
int faccessat(int dirfd, const char *pathname, int mode, int flags);
#else
/* faccessat */
#endif
/** @brief fchown */
#ifdef _MSC_VER
int fchown(int fd, uid_t owner, gid_t group);
#else
/* fchown */
#endif
/** @brief fchownat */
#ifdef _MSC_VER
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);
#else
/* fchownat */
#endif
/** @brief fdatasync */
#ifdef _MSC_VER
int fdatasync(int fd);
#else
/* fdatasync */
#endif
/** @brief fexecve */
#ifdef _MSC_VER
int fexecve(int fd, char *const argv[], char *const envp[]);
#else
/* fexecve */
#endif
/** @brief fork */
#ifdef _MSC_VER
pid_t fork(void);
#else
/* fork */
#endif
/** @brief fpathconf */
#ifdef _MSC_VER
long fpathconf(int fd, int name);
#else
/* fpathconf */
#endif
/** @brief getegid */
#ifdef _MSC_VER
gid_t getegid(void);
#else
/* getegid */
#endif
/** @brief geteuid */
#ifdef _MSC_VER
uid_t geteuid(void);
#else
/* geteuid */
#endif
/** @brief getgid */
#ifdef _MSC_VER
gid_t getgid(void);
#else
/* getgid */
#endif
/** @brief getgroups */
#ifdef _MSC_VER
int getgroups(int size, gid_t list[]);
#else
/* getgroups */
#endif
/** @brief gethostid */
#ifdef _MSC_VER
long gethostid(void);
#else
/* gethostid */
#endif
/** @brief gethostname */
#ifdef _MSC_VER
int gethostname(char *name, size_t len);
#else
/* gethostname */
#endif
/** @brief getlogin */
#ifdef _MSC_VER
char *getlogin(void);
#else
/* getlogin */
#endif
/** @brief getlogin_r */
#ifdef _MSC_VER
int getlogin_r(char *buf, size_t bufsize);
#else
/* getlogin_r */
#endif
/** @brief getopt */
#ifdef _MSC_VER
int getopt(int argc, char * const argv[], const char *optstring);
#else
/* getopt */
#endif
/** @brief getpgid */
#ifdef _MSC_VER
pid_t getpgid(pid_t pid);
#else
/* getpgid */
#endif
/** @brief getpgrp */
#ifdef _MSC_VER
pid_t getpgrp(void);
#else
/* getpgrp */
#endif
/** @brief getppid */
#ifdef _MSC_VER
pid_t getppid(void);
#else
/* getppid */
#endif
/** @brief getsid */
#ifdef _MSC_VER
pid_t getsid(pid_t pid);
#else
/* getsid */
#endif
/** @brief getuid */
#ifdef _MSC_VER
uid_t getuid(void);
#else
/* getuid */
#endif
/** @brief lchown */
#ifdef _MSC_VER
int lchown(const char *pathname, uid_t owner, gid_t group);
#else
/* lchown */
#endif
/** @brief link */
#ifdef _MSC_VER
int link(const char *oldpath, const char *newpath);
#else
/* link */
#endif
/** @brief linkat */
#ifdef _MSC_VER
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags);
#else
/* linkat */
#endif
/** @brief lockf */
#ifdef _MSC_VER
int lockf(int fd, int cmd, off_t len);
#else
/* lockf */
#endif
/** @brief pathconf */
#ifdef _MSC_VER
long pathconf(const char *pathname, int name);
#else
/* pathconf */
#endif
/** @brief pause */
#ifdef _MSC_VER
int pause(void);
#else
/* pause */
#endif
/** @brief pipe */
#ifdef _MSC_VER
int pipe(int pipefd[2]);
#else
/* pipe */
#endif
/** @brief pread */
#ifdef _MSC_VER
ssize_t pread(int fd, void *buf, size_t count, off_t offset);
#else
/* pread */
#endif
/** @brief pwrite */
#ifdef _MSC_VER
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
#else
/* pwrite */
#endif
/** @brief readlink */
#ifdef _MSC_VER
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
#else
/* readlink */
#endif
/** @brief readlinkat */
#ifdef _MSC_VER
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);
#else
/* readlinkat */
#endif
/** @brief setegid */
#ifdef _MSC_VER
int setegid(gid_t egid);
#else
/* setegid */
#endif
/** @brief seteuid */
#ifdef _MSC_VER
int seteuid(uid_t euid);
#else
/* seteuid */
#endif
/** @brief setgid */
#ifdef _MSC_VER
int setgid(gid_t gid);
#else
/* setgid */
#endif
/** @brief setpgid */
#ifdef _MSC_VER
int setpgid(pid_t pid, pid_t pgid);
#else
/* setpgid */
#endif
/** @brief setpgrp */
#ifdef _MSC_VER
pid_t setpgrp(void);
#else
/* setpgrp */
#endif
/** @brief setregid */
#ifdef _MSC_VER
int setregid(gid_t rgid, gid_t egid);
#else
/* setregid */
#endif
/** @brief setreuid */
#ifdef _MSC_VER
int setreuid(uid_t ruid, uid_t euid);
#else
/* setreuid */
#endif
/** @brief setsid */
#ifdef _MSC_VER
pid_t setsid(void);
#else
/* setsid */
#endif
/** @brief setuid */
#ifdef _MSC_VER
int setuid(uid_t uid);
#else
/* setuid */
#endif
/** @brief symlink */
#ifdef _MSC_VER
int symlink(const char *target, const char *linkpath);
#else
/* symlink */
#endif
/** @brief symlinkat */
#ifdef _MSC_VER
int symlinkat(const char *target, int newdirfd, const char *linkpath);
#else
/* symlinkat */
#endif
/** @brief sync */
#ifdef _MSC_VER
void sync(void);
#else
/* sync */
#endif
/** @brief sysconf */
#ifdef _MSC_VER
long sysconf(int name);
#else
/* sysconf */
#endif
/** @brief tcgetpgrp */
#ifdef _MSC_VER
pid_t tcgetpgrp(int fd);
#else
/* tcgetpgrp */
#endif
/** @brief tcsetpgrp */
#ifdef _MSC_VER
int tcsetpgrp(int fd, pid_t pgrp);
#else
/* tcsetpgrp */
#endif
/** @brief truncate */
#ifdef _MSC_VER
int truncate(const char *path, off_t length);
#else
/* truncate */
#endif
/** @brief ttyname */
#ifdef _MSC_VER
char *ttyname(int fd);
#else
/* ttyname */
#endif
/** @brief ttyname_r */
#ifdef _MSC_VER
int ttyname_r(int fd, char *buf, size_t buflen);
#else
/* ttyname_r */
#endif
/** @brief ualarm */
#ifdef _MSC_VER
useconds_t ualarm(useconds_t value, useconds_t interval);
#else
/* ualarm */
#endif
/** @brief vfork */
#ifdef _MSC_VER
pid_t vfork(void);
#else
/* vfork */
#endif

#ifdef __cplusplus
}
#endif
#endif
