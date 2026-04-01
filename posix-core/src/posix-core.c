/* posix-core.c - Strict C89 Implementation */
/* clang-format off */
#include "posix-core.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
/* clang-format on */

#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fcntl function. */
int fcntl(int fd, int cmd, ...) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief openat function. */
int openat(int dirfd, const char *pathname, int flags, ...) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief posix_fadvise function. */
int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief posix_fallocate function. */
int posix_fallocate(int fd, off_t offset, off_t len) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief sync_file_range function. */
int sync_file_range(int fd, off_t offset, off_t nbytes, unsigned int flags) {
  (void)offset;
  (void)nbytes;
  (void)flags;
  if (_commit(fd) == 0) {
    return 0;
  }
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief alarm function. */
unsigned int alarm(unsigned int seconds) {
  errno = EINVAL;
  return (unsigned int)-1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief chown function. */
int chown(const char *pathname, uid_t owner, gid_t group) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief confstr function. */
size_t confstr(int name, char *buf, size_t len) {
  errno = EINVAL;
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
char *crypt(const char *key, const char *salt) {
  errno = EINVAL;
  return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief encrypt function. */
void encrypt(char block[64], int edflag) { errno = EINVAL; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief faccessat function. */
int faccessat(int dirfd, const char *pathname, int mode, int flags) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fchown function. */
int fchown(int fd, uid_t owner, gid_t group) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fchownat function. */
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group,
             int flags) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fdatasync function. */
int fdatasync(int fd) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fexecve function. */
int fexecve(int fd, char *const argv[], char *const envp[]) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fork function. */
pid_t fork(void) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fpathconf function. */
long fpathconf(int fd, int name) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getegid function. */
gid_t getegid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief geteuid function. */
uid_t geteuid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getgid function. */
gid_t getgid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getgroups function. */
int getgroups(int size, gid_t list[]) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief gethostid function. */
long gethostid(void) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief gethostname function. */
int gethostname(char *name, int len) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
char *getlogin(void) {
  errno = EINVAL;
  return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getlogin_r function. */
int getlogin_r(char *buf, size_t bufsize) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__) &&                                 \
    !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
/** \brief getopt function. */
int getopt(int argc, char *const argv[], const char *optstring) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getpgid function. */
pid_t getpgid(pid_t pid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getpgrp function. */
pid_t getpgrp(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getppid function. */
pid_t getppid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getsid function. */
pid_t getsid(pid_t pid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getuid function. */
uid_t getuid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief lchown function. */
int lchown(const char *pathname, uid_t owner, gid_t group) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief link function. */
int link(const char *oldpath, const char *newpath) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief linkat function. */
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief lockf function. */
int lockf(int fd, int cmd, off_t len) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pathconf function. */
long pathconf(const char *pathname, int name) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pause function. */
int pause(void) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pipe function. */
int pipe(int pipefd[2]) {
  if (_pipe(pipefd, 4096, _O_BINARY) == -1) {
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pipe2 function. */


int posix_socketpair(int domain, int type, int protocol, int socket_vector[2]);
int pipe2(int pipefd[2], int flags) {
  int textmode = _O_BINARY;
  if (flags & O_CLOEXEC) {
    textmode |= _O_NOINHERIT;
  }
  if (posix_socketpair(2, 1, 0, pipefd) != 0) {
    return -1;
  }
  return 0;
}

#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pread function. */
ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pwrite function. */
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief readlink function. */
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief readlinkat function. */
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setegid function. */
int setegid(gid_t egid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief seteuid function. */
int seteuid(uid_t euid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setgid function. */
int setgid(gid_t gid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setpgid function. */
int setpgid(pid_t pid, pid_t pgid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setpgrp function. */
pid_t setpgrp(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setregid function. */
int setregid(gid_t rgid, gid_t egid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setreuid function. */
int setreuid(uid_t ruid, uid_t euid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setsid function. */
pid_t setsid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setuid function. */
int setuid(uid_t uid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief symlink function. */
int symlink(const char *target, const char *linkpath) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief symlinkat function. */
int symlinkat(const char *target, int newdirfd, const char *linkpath) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief sync function. */
void sync(void) { errno = EINVAL; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief sysconf function. */
long sysconf(int name) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief tcgetpgrp function. */
pid_t tcgetpgrp(int fd) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief tcsetpgrp function. */
int tcsetpgrp(int fd, pid_t pgrp) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief truncate function. */
int truncate(const char *path, off_t length) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
char *ttyname(int fd) {
  errno = EINVAL;
  return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief ttyname_r function. */
int ttyname_r(int fd, char *buf, size_t buflen) {
  errno = EINVAL;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief ualarm function. */
useconds_t ualarm(useconds_t value, useconds_t interval) {
  errno = EINVAL;
  return (useconds_t)-1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief vfork function. */
pid_t vfork(void) {
  errno = EINVAL;
  return -1;
}
#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_core;
