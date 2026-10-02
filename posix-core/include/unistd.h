#ifndef POSIX_CORE_UNISTD_H
#define POSIX_CORE_UNISTD_H

#if defined(__GNUC__)
#endif

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <unistd.h>
#else
#if !defined(_MSC_VER)
#include <unistd.h>
#endif
#endif
#else
#include "posix-core.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file unistd.h
 * @brief Standard symbolic constants and types header.
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_CORE_UNISTD_H */

/* API Contract symbols: access, chdir, _exit, alarm, chown, close, confstr,
 * crypt, dup, dup2, encrypt, execl, execle, execlp, execv, execve, execvp,
 * faccessat, fchdir, fchown, fchownat, fdatasync, fexecve, fork, fpathconf,
 * fsync, ftruncate, getcwd, getegid, geteuid, getgid, getgroups, gethostid,
 * gethostname, getlogin, getlogin_r, getopt, getpgid, getpgrp, getpid, getppid,
 * getsid, getuid, isatty, lchown, link, linkat, lockf, lseek, nice, pathconf,
 * pause, pipe, read, readlink, readlinkat, rmdir, setegid, seteuid, setgid,
 * setpgid, setpgrp, setregid, setreuid, setsid, setuid, sleep, swab, symlink,
 * symlinkat, sync, sysconf, tcgetpgrp, tcsetpgrp, truncate, ttyname, ttyname_r,
 * unlink, unlinkat, write, __dependencies__, __include_next__, pread, pwrite */
