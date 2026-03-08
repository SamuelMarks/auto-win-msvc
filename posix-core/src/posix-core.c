/* posix-core.c - Strict C89 Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "posix-core.h"

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif


#if defined(_WIN32) && !defined(__CYGWIN__)
int fcntl(int fd, int cmd, ...) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int openat(int dirfd, const char *pathname, int flags, ...) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int posix_fallocate(int fd, off_t offset, off_t len) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
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
unsigned int alarm(unsigned int seconds) {
    errno = ENOSYS;
    return (unsigned int)-1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int chown(const char *pathname, uid_t owner, gid_t group) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
size_t confstr(int name, char *buf, size_t len) {
    errno = ENOSYS;
    return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
char *crypt(const char *key, const char *salt) {
    errno = ENOSYS;
    return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
void encrypt(char block[64], int edflag) {
    errno = ENOSYS;
    
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int faccessat(int dirfd, const char *pathname, int mode, int flags) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int fchown(int fd, uid_t owner, gid_t group) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int fdatasync(int fd) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int fexecve(int fd, char *const argv[], char *const envp[]) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t fork(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
long fpathconf(int fd, int name) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
gid_t getegid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
uid_t geteuid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
gid_t getgid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int getgroups(int size, gid_t list[]) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
long gethostid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int gethostname(char *name, int len) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
char *getlogin(void) {
    errno = ENOSYS;
    return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int getlogin_r(char *buf, size_t bufsize) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int getopt(int argc, char * const argv[], const char *optstring) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t getpgid(pid_t pid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t getpgrp(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t getppid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t getsid(pid_t pid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
uid_t getuid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int lchown(const char *pathname, uid_t owner, gid_t group) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int link(const char *oldpath, const char *newpath) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int lockf(int fd, int cmd, off_t len) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
long pathconf(const char *pathname, int name) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int pause(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int pipe(int pipefd[2]) {
    if (_pipe(pipefd, 4096, _O_BINARY) == -1) {
        return -1;
    }
    return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int pipe2(int pipefd[2], int flags) {
    int textmode = _O_BINARY;
    if (flags & O_CLOEXEC) {
        textmode |= _O_NOINHERIT;
    }
    if (_pipe(pipefd, 4096, textmode) == -1) {
        return -1;
    }
    return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int setegid(gid_t egid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int seteuid(uid_t euid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int setgid(gid_t gid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int setpgid(pid_t pid, pid_t pgid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t setpgrp(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int setregid(gid_t rgid, gid_t egid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int setreuid(uid_t ruid, uid_t euid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t setsid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int setuid(uid_t uid) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int symlink(const char *target, const char *linkpath) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int symlinkat(const char *target, int newdirfd, const char *linkpath) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
void sync(void) {
    errno = ENOSYS;
    
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
long sysconf(int name) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t tcgetpgrp(int fd) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int tcsetpgrp(int fd, pid_t pgrp) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int truncate(const char *path, off_t length) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
char *ttyname(int fd) {
    errno = ENOSYS;
    return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
int ttyname_r(int fd, char *buf, size_t buflen) {
    errno = ENOSYS;
    return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
useconds_t ualarm(useconds_t value, useconds_t interval) {
    errno = ENOSYS;
    return (useconds_t)-1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
pid_t vfork(void) {
    errno = ENOSYS;
    return -1;
}
#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_core;

