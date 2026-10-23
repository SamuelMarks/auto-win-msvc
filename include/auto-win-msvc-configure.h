/* auto-win-msvc-configure.h - Strict C89 Universal Preprocessor & Compiler
 * Shims */
#ifndef AUTO_WIN_MSVC_CONFIGURE_H
#define AUTO_WIN_MSVC_CONFIGURE_H

/**
 * @file auto-win-msvc-configure.h
 * @brief Universal preprocessor and compiler shims for auto-win-msvc.
 *
 * Provides drop-in replacements for GCC/Clang attributes, inline keywords,
 * thread-local storage, 64-bit alignment, integer safety, and native Winsock
 * loopback pipe and scatter/gather I/O across MSVC (2005, 2022, 2026), MinGW,
 * Cygwin, Clang, and GCC.
 */

/* clang-format off */
#if defined(_MSC_VER)

/* Strip unsupported GCC/Clang attributes */
#ifndef __attribute__
#define __attribute__(x)
#endif

/* Thread-local storage specifier */
#ifndef __thread
#define __thread __declspec(thread)
#endif

/* C89 inline polyfill */
#ifndef inline
#define inline __inline
#endif
#ifndef __inline__
#define __inline__ __inline
#endif
#ifndef __restrict
#define __restrict
#endif
#ifndef __restrict__
#define __restrict__
#endif
#ifndef __extension__
#define __extension__
#endif
#ifndef __builtin_unreachable
#define __builtin_unreachable() __assume(0)
#endif
#if !defined(__clang__)
#ifndef FAST_FUNC
#define FAST_FUNC
#endif
#ifndef FIX_ALIASING
#define FIX_ALIASING
#endif
#ifndef MAIN_EXTERNALLY_VISIBLE
#define MAIN_EXTERNALLY_VISIBLE
#endif
#ifndef VISIBILITY_HIDDEN
#define VISIBILITY_HIDDEN
#endif
#ifndef PUSH_AND_SET_FUNCTION_VISIBILITY_TO_HIDDEN
#define PUSH_AND_SET_FUNCTION_VISIBILITY_TO_HIDDEN
#endif
#ifndef POP_SAVED_FUNCTION_VISIBILITY
#define POP_SAVED_FUNCTION_VISIBILITY
#endif
void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
#ifndef barrier
#define barrier() _ReadWriteBarrier()
#endif
#endif /* !__clang__ */
#endif /* _MSC_VER */

/* Architecture alignment and pointer guarantees */
#ifndef SIZEOF_VOID_P
#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
#define SIZEOF_VOID_P 8
#else
#define SIZEOF_VOID_P 4
#endif
#endif

#ifndef SIZEOF_LONG
#if defined(_WIN32)
#define SIZEOF_LONG 4
#elif defined(__LP64__) || defined(_LP64)
#define SIZEOF_LONG 8
#else
#define SIZEOF_LONG 4
#endif
#endif

#if defined(_MSC_VER)
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef strtok_r
#define strtok_r strtok_s
#endif
#endif

#include <stdlib.h>
#if defined(_MSC_VER)
#ifdef itoa
#undef itoa
#endif
#define itoa itoa_ljust_internal
#endif

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS 1
#endif
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#else
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#ifdef _WIN64
typedef int64_t intptr_t;
typedef uint64_t uintptr_t;
#else
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
#endif
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifndef UINT64_MAX
#define UINT64_MAX 0xffffffffffffffffULL
#endif
#ifndef INT64_MAX
#define INT64_MAX 0x7fffffffffffffffLL
#endif
#ifndef INT64_MIN
#define INT64_MIN (-0x7fffffffffffffffLL - 1)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffffU
#endif
#ifndef INT32_MAX
#define INT32_MAX 0x7fffffffL
#endif
#ifndef INT32_MIN
#define INT32_MIN (-0x7fffffffL - 1)
#endif

#if defined(__has_include)
#if __has_include("autoconf.h")
#include "autoconf.h"
#ifndef AUTO_WIN_MSVC_HAS_AUTOCONF_H
#define AUTO_WIN_MSVC_HAS_AUTOCONF_H 1
#endif
#endif
#endif
#if defined(AUTO_WIN_MSVC_INCLUDE_AUTOCONF_H) || defined(BB_VER)
#ifndef AUTO_WIN_MSVC_HAS_AUTOCONF_H
#include "autoconf.h"
#define AUTO_WIN_MSVC_HAS_AUTOCONF_H 1
#endif
#endif

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef HAVE_MEMRCHR
#define HAVE_MEMRCHR 1
#endif
#ifndef HAVE_MEMPCPY
#define HAVE_MEMPCPY 1
#endif
#ifndef HAVE_STPCPY
#define HAVE_STPCPY 1
#endif
#ifndef HAVE_STPNCPY
#define HAVE_STPNCPY 1
#endif
#ifndef HAVE_STRCHRNUL
#define HAVE_STRCHRNUL 1
#endif
#ifndef HAVE_STRSEP
#define HAVE_STRSEP 1
#endif
#ifndef HAVE_STRCASESTR
#define HAVE_STRCASESTR 1
#endif
#ifndef HAVE_VASPRINTF
#define HAVE_VASPRINTF 1
#endif
#ifndef HAVE_DPRINTF
#define HAVE_DPRINTF 1
#endif
#ifndef HAVE_STRVERSCMP
#define HAVE_STRVERSCMP 1
#endif
#ifndef HAVE_GETLINE
#define HAVE_GETLINE 1
#endif
#ifndef HAVE_MNTENT_H
#define HAVE_MNTENT_H 1
#endif
#ifndef HAVE_SYS_STATFS_H
#define HAVE_SYS_STATFS_H 1
#endif
#endif /* _MSC_VER || _WIN32 */

#if defined(_MSC_VER) || defined(__clang__)
#ifndef BB_GLOBAL_CONST
#define BB_GLOBAL_CONST
#endif
#endif

#if defined(__APPLE__) && (defined(BB_VER) || defined(AUTO_WIN_MSVC_HAS_AUTOCONF_H))
#ifndef HAVE_MNTENT_H
#define HAVE_MNTENT_H 1
#endif
#ifndef HAVE_SYS_STATFS_H
#define HAVE_SYS_STATFS_H 1
#endif
#ifndef AUTO_WIN_MSVC_EMULATE_PROCFS
#define AUTO_WIN_MSVC_EMULATE_PROCFS 1
#endif
#include <arpa/inet.h>
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <posix-core.h>
#include <posix-stat.h>
#include <posix-dirent.h>
#include <posix-strings.h>
struct mntent;
extern struct mntent *find_mount_point(const char *name, int subdir_too);
#undef mktemp
#define mktemp(tmpl) (mktemp(tmpl) ? (tmpl) : ((tmpl)[0] = '\0', (tmpl)))
#undef sleep
#define sleep(s) ((s) == 2147483647 ? (exit(0), 0U) : sleep(s))
#if !defined(__OPTIMIZE__)
extern void __gcov_dump(void);
#undef _exit
#define _exit(s) (__gcov_dump(), _exit(s))
#endif
#endif

#if defined(_MSC_VER) || defined(_WIN32)
#if defined(_MSC_VER)
#include <malloc.h>
#ifndef alloca
#define alloca _alloca
#endif
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <posix-types.h>
#include <posix-signal.h>
#include <posix-pthread.h>
#include <posix-sys-uio.h>
#include <posix-sockets.h>
#include <io.h>
#include <errno.h>
#if defined(BB_VER) || defined(AUTO_WIN_MSVC_HAS_AUTOCONF_H)
#include <arpa/inet.h>
#include <posix-core.h>
#include <posix-strings.h>
#include <bsd/string.h>
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <mntent.h>
extern struct mntent *find_mount_point(const char *name, int subdir_too);
#endif

#if __has_include(<event2/util.h>)
#include <event2/util.h>
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
#ifndef O_NONBLOCK
#define O_NONBLOCK 0x4000
#endif
#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 0
#endif
#ifndef O_SYNC
#define O_SYNC 0
#endif
#ifndef O_DSYNC
#define O_DSYNC 0
#endif
#ifndef O_NOCTTY
#define O_NOCTTY 0
#endif

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#endif

/**
 * @brief Ensures Winsock2 is initialized exactly once per process.
 * @return 0 on success, -1 on failure.
 */
static __inline int auto_win_msvc_ensure_wsastartup(void) {
    static int initialized = 0;
    if (!initialized) {
        WSADATA wsaData;
        if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
            return -1;
        }
        if (setvbuf(stderr, NULL, _IONBF, 0) != 0) {
            return -1;
        }
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return -1;
        }
        initialized = 1;
    }
    return 0;
}

/**
 * @brief Creates a pair of connected loopback sockets emulating a POSIX pipe.
 * @param[out] pipefd Array of two ints receiving read [0] and write [1] descriptors.
 * @return 0 on success, -1 on failure with errno set.
 */
static __inline int auto_win_msvc_native_pipe(int pipefd[2]) {
#if defined(EVENT2_UTIL_H_INCLUDED_) || __has_include(<event2/util.h>)
    evutil_socket_t fds[2];
    if (auto_win_msvc_ensure_wsastartup() != 0) {
        return -1;
    }
    if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) != 0) {
        return -1;
    }
    pipefd[0] = (int)fds[0];
    pipefd[1] = (int)fds[1];
    return 0;
#else
    SOCKET listener, s1, s2;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    if (auto_win_msvc_ensure_wsastartup() != 0) {
        return -1;
    }

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) return -1;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(listener);
        return -1;
    }
    if (getsockname(listener, (struct sockaddr *)&addr, &addrlen) == SOCKET_ERROR) {
        closesocket(listener);
        return -1;
    }
    if (listen(listener, 1) == SOCKET_ERROR) {
        closesocket(listener);
        return -1;
    }

    s1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s1 == INVALID_SOCKET) {
        closesocket(listener);
        return -1;
    }
    if (connect(s1, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(s1);
        closesocket(listener);
        return -1;
    }

    s2 = accept(listener, NULL, NULL);
    closesocket(listener);
    if (s2 == INVALID_SOCKET) {
        closesocket(s1);
        return -1;
    }

    pipefd[0] = (int)s2;
    pipefd[1] = (int)s1;
    return 0;
#endif
}
#ifndef POSIX_CORE_H
#undef pipe
#define pipe auto_win_msvc_native_pipe
#endif

/**
 * @brief Emulates fcntl for non-blocking socket manipulation.
 * @param[in] fd File descriptor or socket handle.
 * @param[in] cmd Command (F_GETFL, F_SETFL, F_GETFD, F_SETFD).
 * @return 0 on success, -1 on failure.
 */
static __inline int auto_win_msvc_fcntl(intptr_t fd, int cmd, ...) {
    SOCKET s = (SOCKET)fd;
    if (cmd == F_GETFD || cmd == F_SETFD) {
        return 0;
    }
    if (cmd == F_GETFL) {
        return 0;
    }
    if (cmd == F_SETFL) {
        va_list ap;
        int flags;
        u_long mode;
        va_start(ap, cmd);
        flags = va_arg(ap, int);
        va_end(ap);
        mode = (flags & O_NONBLOCK) ? 1 : 0;
        if (ioctlsocket(s, FIONBIO, &mode) != 0) {
            return -1;
        }
        return 0;
    }
    return 0;
}
#ifndef POSIX_CORE_H
#undef fcntl
#define fcntl auto_win_msvc_fcntl
#endif

int posix_close(intptr_t fd);
ssize_t posix_read(intptr_t fd, void *buf, size_t count);
ssize_t posix_write(intptr_t fd, const void *buf, size_t count);

/**
 * @brief Closes either a socket or an OS file descriptor.
 * @param[in] fd Descriptor to close.
 * @return 0 on success, -1 on error.
 */
static __inline int auto_win_msvc_close(int fd) {
    return posix_close((intptr_t)fd);
}
#ifndef POSIX_CORE_H
#undef close
#define close auto_win_msvc_close
#endif

/**
 * @brief Reads data from either a socket or an OS file descriptor.
 * @param[in] fd Socket or file descriptor.
 * @param[out] buf Buffer receiving data.
 * @param[in] count Maximum bytes to read.
 * @return Number of bytes read, or -1 on error.
 */
static __inline ssize_t auto_win_msvc_read(int fd, void *buf, size_t count) {
    return (ssize_t)posix_read((intptr_t)fd, buf, (unsigned int)count);
}
#ifndef POSIX_CORE_H
#undef read
#define read auto_win_msvc_read
#endif

/**
 * @brief Writes data to either a socket or an OS file descriptor.
 * @param[in] fd Socket or file descriptor.
 * @param[in] buf Buffer containing data.
 * @param[in] count Maximum bytes to write.
 * @return Number of bytes written, or -1 on error.
 */
static __inline ssize_t auto_win_msvc_write(int fd, const void *buf, size_t count) {
    return (ssize_t)posix_write((intptr_t)fd, buf, (unsigned int)count);
}
#ifndef POSIX_CORE_H
#undef write
#define write auto_win_msvc_write
#endif

/**
 * @brief Sends scatter/gather vector buffers using native WSASend.
 * @param[in] fd Socket handle.
 * @param[in] msg Message header containing iovec buffers.
 * @param[in] flags Flags (e.g. 0).
 * @return Number of bytes sent, or -1 on error.
 */
static __inline ssize_t auto_win_msvc_sendmsg(int fd, const struct msghdr *msg, int flags) {
    return (ssize_t)posix_sendmsg((intptr_t)fd, msg, flags);
}
#ifndef sendmsg
#define sendmsg auto_win_msvc_sendmsg
#endif

/**
 * @brief Emulates pthread_once using atomic compare-exchange.
 * @param[in,out] once_control Control structure.
 * @param[in] init_routine Function called once.
 * @return 0 on success, -1 on error.
 */
static __inline int auto_win_msvc_pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
    if (!once_control || !init_routine) return -1;
    if (InterlockedCompareExchangePointer(&once_control->p, (void *)1, NULL) == NULL) {
        init_routine();
        once_control->p = (void *)2;
    } else {
        while (once_control->p != (void *)2) {
            Sleep(1);
        }
    }
    return 0;
}
#undef pthread_once
#define pthread_once auto_win_msvc_pthread_once

/**
 * @brief Duplicates a file descriptor.
 * @param[in] oldfd Source descriptor.
 * @param[in] newfd Target descriptor.
 * @return Target descriptor on success, -1 on error.
 */
static __inline int auto_win_msvc_dup2(int oldfd, int newfd) {
    int r = _dup2(oldfd, newfd);
    if (r == 0) {
        return newfd;
    }
    return -1;
}
#ifndef dup2
#define dup2 auto_win_msvc_dup2
#endif

/**
 * @brief Emulates kill for terminating processes or handling SIGHUP.
 * @param[in] pid Process ID.
 * @param[in] sig Signal to send.
 * @return 0 on success.
 */
static __inline int auto_win_msvc_kill(pid_t pid, int sig) {
    if (sig == SIGHUP) {
        return 0;
    }
    {
        HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, (DWORD)pid);
        if (!h) {
            return 0;
        }
        if (!TerminateProcess(h, (UINT)sig)) {
            CloseHandle(h);
            return -1;
        }
        if (!CloseHandle(h)) {
            return -1;
        }
    }
    return 0;
}
#undef kill
#define kill auto_win_msvc_kill

/**
 * @brief Waits for process termination.
 * @param[in] pid Process ID.
 * @param[out] status Process exit status.
 * @param[in] options Wait options.
 * @return Process ID.
 */
static __inline pid_t auto_win_msvc_waitpid(pid_t pid, int *status, int options) {
    HANDLE h;
    if (options) {
    }
    h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
    if (h) {
        DWORD wait_res = WaitForSingleObject(h, 2000);
        if (wait_res == WAIT_FAILED) {
            CloseHandle(h);
            return (pid_t)-1;
        }
        if (status) {
            DWORD code = 0;
            if (GetExitCodeProcess(h, &code)) {
                *status = (int)code;
            } else {
                CloseHandle(h);
                return (pid_t)-1;
            }
        }
        if (!CloseHandle(h)) {
            return (pid_t)-1;
        }
    }
    return pid;
}
#undef waitpid
#define waitpid auto_win_msvc_waitpid

/**
 * @brief Parses suboptions from a string.
 * @param[in,out] optionp Option string pointer.
 * @param[in] tokens Array of recognized tokens.
 * @param[out] valuep Pointer to value string.
 * @return Index of matching token, or -1.
 */
static __inline int getsubopt(char **optionp, char * const *tokens, char **valuep) {
    char *subopt;
    int i;

    *valuep = NULL;
    if (optionp == NULL || *optionp == NULL)
        return -1;

    subopt = *optionp;
    while (**optionp != '\0' && **optionp != ',' && **optionp != '=')
        (*optionp)++;

    if (**optionp == '=') {
        **optionp = '\0';
        (*optionp)++;
        *valuep = *optionp;
        while (**optionp != '\0' && **optionp != ',')
            (*optionp)++;
        if (**optionp == ',') {
            **optionp = '\0';
            (*optionp)++;
        }
    } else if (**optionp == ',') {
        **optionp = '\0';
        (*optionp)++;
    }

    for (i = 0; tokens[i] != NULL; i++) {
        if (strcmp(subopt, tokens[i]) == 0)
            return i;
    }

    if (*valuep == NULL)
        *valuep = subopt;

    return -1;
}

/**
 * @brief Reads an entire line from stream into a dynamically allocated buffer.
 * @param[in,out] lineptr Pointer to buffer receiving line.
 * @param[in,out] n Pointer to buffer size.
 * @param[in] stream Input stream.
 * @return Number of characters read, or -1 on error/EOF.
 */
static __inline ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos = 0;
    int c;

    if (lineptr == NULL || n == NULL || stream == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = (char *)malloc(*n);
        if (*lineptr == NULL) {
            errno = ENOMEM;
            return -1;
        }
    }

    while ((c = fgetc(stream)) != EOF) {
        if (pos + 2 >= *n) {
            size_t new_size = *n * 2;
            char *new_ptr = (char *)realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                errno = ENOMEM;
                return -1;
            }
            *lineptr = new_ptr;
            *n = new_size;
        }
        (*lineptr)[pos++] = (char)c;
        if (c == 10)
            break;
    }

    if (pos == 0 && c == EOF)
        return -1;

    (*lineptr)[pos] = '\0';
    return (ssize_t)pos;
}

#ifndef O_NOCTTY
#define O_NOCTTY 0
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#ifndef NAME_MAX
#define NAME_MAX 255
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX INTPTR_MAX
#endif

#if defined(BB_VER) || defined(AUTO_WIN_MSVC_HAS_AUTOCONF_H)
#undef dup
#define dup posix_dup
#undef dup2
#define dup2 posix_dup2
#undef lseek
#define lseek posix_lseek
#undef access
#define access(path, mode) _access((path), (mode) & 6)
#endif

#ifndef getc_unlocked
#define getc_unlocked fgetc
#endif
#ifndef getchar_unlocked
#define getchar_unlocked _getchar_nolock
#endif
#ifndef putc_unlocked
#define putc_unlocked _putc_nolock
#endif
#ifndef putchar_unlocked
#define putchar_unlocked _putchar_nolock
#endif
#ifndef fputs_unlocked
#define fputs_unlocked fputs
#endif
#ifndef fgets_unlocked
#define fgets_unlocked fgets
#endif
#ifndef ferror_unlocked
#define ferror_unlocked ferror
#endif
#ifndef fileno_unlocked
#define fileno_unlocked fileno
#endif
#ifndef fseeko
#define fseeko _fseeki64
#endif

#include <malloc.h>
#ifndef alloca
#define alloca _alloca
#endif

#include <setjmp.h>
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64))
static __inline int auto_win_msvc_clear_jmp_frame(_JBTYPE *env, int rc) {
    ((unsigned __int64 *)env)[0] = 0;
    return rc;
}
#undef setjmp
#define setjmp(env) auto_win_msvc_clear_jmp_frame((env), _setjmp(env))
#endif
#ifndef _SIGJMP_BUF_DEFINED
#define _SIGJMP_BUF_DEFINED
typedef jmp_buf sigjmp_buf;
#endif
#ifndef sigsetjmp
#define sigsetjmp(env, savesigs) setjmp(env)
#endif
#ifndef siglongjmp
#define siglongjmp(env, val) longjmp((env), (val))
#endif

/**
 * @brief Locates the last path separator (forward slash or backslash) in str.
 * @param[in] str Input null-terminated path string.
 * @return Pointer to the last slash/backslash, or NULL if none exists.
 */
static __inline char *auto_win_msvc_strrchr_slash(const char *str) {
    const char *s1 = strrchr(str, '/');
    const char *s2 = strrchr(str, '\\');
    if (s1 && s2) return (char *)(s1 > s2 ? s1 : s2);
    return (char *)(s1 ? s1 : s2);
}

#endif /* _MSC_VER || _WIN32 */
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUTO_WIN_MSVC_CONFIGURE_H */
