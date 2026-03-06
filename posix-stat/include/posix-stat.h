/* posix-stat.h - Strict C89 Header */
#ifndef POSIX_STAT_H
#define POSIX_STAT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <io.h>
#include <direct.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Standard Types (if missing) */
#ifndef _MODE_T_DEFINED
#define _MODE_T_DEFINED
typedef unsigned short mode_t;
#endif

/* POSIX timespec */
#if !defined(_TIMESPEC_DEFINED) && !defined(HAVE_STRUCT_TIMESPEC)
#define _TIMESPEC_DEFINED
struct timespec {
    time_t tv_sec;
    long   tv_nsec;
};
#endif

/* Macros from mappings.json */
#define S_IFMT   _S_IFMT
#define S_IFDIR  _S_IFDIR
#define S_IFCHR  _S_IFCHR
#define S_IFREG  _S_IFREG
#define S_IFIFO  _S_IFIFO

#define S_IRUSR  _S_IREAD
#define S_IWUSR  _S_IWRITE
#define S_IXUSR  _S_IEXEC

/* Polyfill Macros */
#define S_IFLNK  0120000
#define S_IFSOCK 0140000
#define S_IFBLK  0060000

#define S_IRWXU  (S_IRUSR | S_IWUSR | S_IXUSR)

#define S_IRGRP  (S_IRUSR >> 3)
#define S_IWGRP  (S_IWUSR >> 3)
#define S_IXGRP  (S_IXUSR >> 3)
#define S_IRWXG  (S_IRWXU >> 3)

#define S_IROTH  (S_IRGRP >> 3)
#define S_IWOTH  (S_IWGRP >> 3)
#define S_IXOTH  (S_IXGRP >> 3)
#define S_IRWXO  (S_IRWXG >> 3)

#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)

/* Special values for utimensat / futimens */
#define UTIME_NOW  ((1L << 30) - 1L)
#define UTIME_OMIT ((1L << 30) - 2L)

/* Flags for *at functions */
#define AT_FDCWD            -100
#define AT_SYMLINK_NOFOLLOW 0x100

/* Function Shims */
#define stat _stat64
#define fstat _fstat64
#define chmod _chmod
#define umask _umask
#define mkdir(path, mode) _mkdir(path)

/* Functions requiring polyfill */
int fchmod(int fd, mode_t mode);
int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);
int fstatat(int dirfd, const char *pathname, struct _stat64 *statbuf, int flags);
int futimens(int fd, const struct timespec times[2]);
int lstat(const char *pathname, struct _stat64 *statbuf);
int mknod(const char *pathname, mode_t mode, unsigned int dev);
int mknodat(int dirfd, const char *pathname, mode_t mode, unsigned int dev);
int utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags);

#ifdef __cplusplus
}
#endif

#endif /* POSIX_STAT_H */