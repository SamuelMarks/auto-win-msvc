/* posix-core.c - Strict C89 Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "posix-core.h"

#ifdef _MSC_VER
int fcntl(int fd, int cmd, ...) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int openat(int dirfd, const char *pathname, int flags, ...) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int posix_fallocate(int fd, off_t offset, off_t len) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
unsigned int alarm(unsigned int seconds) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int chown(const char *pathname, uid_t owner, gid_t group) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
size_t confstr(int name, char *buf, size_t len) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
char *crypt(const char *key, const char *salt) {
    errno = ENOSYS;
    return NULL;
}
#endif
#ifdef _MSC_VER
void encrypt(char block[64], int edflag) {
    errno = ENOSYS;
    
}
#endif
#ifdef _MSC_VER
int faccessat(int dirfd, const char *pathname, int mode, int flags) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int fchown(int fd, uid_t owner, gid_t group) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int fdatasync(int fd) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int fexecve(int fd, char *const argv[], char *const envp[]) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t fork(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
long fpathconf(int fd, int name) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
gid_t getegid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
uid_t geteuid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
gid_t getgid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int getgroups(int size, gid_t list[]) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
long gethostid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int gethostname(char *name, size_t len) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
char *getlogin(void) {
    errno = ENOSYS;
    return NULL;
}
#endif
#ifdef _MSC_VER
int getlogin_r(char *buf, size_t bufsize) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int getopt(int argc, char * const argv[], const char *optstring) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t getpgid(pid_t pid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t getpgrp(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t getppid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t getsid(pid_t pid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
uid_t getuid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int lchown(const char *pathname, uid_t owner, gid_t group) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int link(const char *oldpath, const char *newpath) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int lockf(int fd, int cmd, off_t len) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
long pathconf(const char *pathname, int name) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int pause(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int pipe(int pipefd[2]) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int setegid(gid_t egid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int seteuid(uid_t euid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int setgid(gid_t gid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int setpgid(pid_t pid, pid_t pgid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t setpgrp(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int setregid(gid_t rgid, gid_t egid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int setreuid(uid_t ruid, uid_t euid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t setsid(void) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int setuid(uid_t uid) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int symlink(const char *target, const char *linkpath) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int symlinkat(const char *target, int newdirfd, const char *linkpath) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
void sync(void) {
    errno = ENOSYS;
    
}
#endif
#ifdef _MSC_VER
long sysconf(int name) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t tcgetpgrp(int fd) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int tcsetpgrp(int fd, pid_t pgrp) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
int truncate(const char *path, off_t length) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
char *ttyname(int fd) {
    errno = ENOSYS;
    return NULL;
}
#endif
#ifdef _MSC_VER
int ttyname_r(int fd, char *buf, size_t buflen) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
useconds_t ualarm(useconds_t value, useconds_t interval) {
    errno = ENOSYS;
    return -1;
}
#endif
#ifdef _MSC_VER
pid_t vfork(void) {
    errno = ENOSYS;
    return -1;
}
#endif
