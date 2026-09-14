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

#endif /* _MSC_VER */

/* Architecture alignment and pointer guarantees */
#ifndef SIZEOF_VOID_P
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
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
#include <stdint.h>
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

#if defined(_MSC_VER) || defined(_WIN32)
#include <posix-types.h>
#include <posix-signal.h>
#include <posix-pthread.h>
#include <posix-sys-uio.h>
#include <posix-sockets.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <errno.h>

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
#define O_NONBLOCK 0x0004
#endif

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#endif

/**
 * @brief Ensures Winsock2 is initialized exactly once per process.
 */
static __inline void auto_win_msvc_ensure_wsastartup(void) {
    static int initialized = 0;
    if (!initialized) {
        WSADATA wsaData;
        (void)setvbuf(stdout, NULL, _IONBF, 0);
        (void)setvbuf(stderr, NULL, _IONBF, 0);
        (void)WSAStartup(MAKEWORD(2, 2), &wsaData);
        initialized = 1;
    }
}

/**
 * @brief Creates a pair of connected loopback sockets emulating a POSIX pipe.
 * @param[out] pipefd Array of two ints receiving read [0] and write [1] descriptors.
 * @return 0 on success, -1 on failure with errno set.
 */
static __inline int auto_win_msvc_native_pipe(int pipefd[2]) {
#if defined(EVENT2_UTIL_H_INCLUDED_) || __has_include(<event2/util.h>)
    evutil_socket_t fds[2];
    auto_win_msvc_ensure_wsastartup();
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
    auto_win_msvc_ensure_wsastartup();

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) return -1;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
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
#undef pipe
#define pipe auto_win_msvc_native_pipe

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
#undef fcntl
#define fcntl auto_win_msvc_fcntl

/**
 * @brief Closes either a socket or an OS file descriptor.
 * @param[in] fd Descriptor to close.
 * @return 0 on success, -1 on error.
 */
static __inline int auto_win_msvc_close(int fd) {
    if (closesocket((SOCKET)fd) == 0) {
        return 0;
    }
    return _close(fd);
}
#undef close
#define close auto_win_msvc_close

/**
 * @brief Reads data from either a socket or an OS file descriptor.
 * @param[in] fd Socket or file descriptor.
 * @param[out] buf Buffer receiving data.
 * @param[in] count Maximum bytes to read.
 * @return Number of bytes read, or -1 on error.
 */
static __inline ssize_t auto_win_msvc_read(int fd, void *buf, size_t count) {
    int r = recv((SOCKET)fd, (char *)buf, (int)count, 0);
    if (r != SOCKET_ERROR) {
        return (ssize_t)r;
    }
    {
        int werr = WSAGetLastError();
        if (werr == WSAENOTSOCK) {
            return (ssize_t)_read(fd, buf, (unsigned int)count);
        }
        if (werr == WSAEWOULDBLOCK || werr == WSAEINPROGRESS) {
            errno = EAGAIN;
        } else if (werr == WSAECONNRESET) {
            errno = ECONNRESET;
        } else {
            errno = EIO;
        }
        return -1;
    }
}
#undef read
#define read auto_win_msvc_read

/**
 * @brief Writes data to either a socket or an OS file descriptor.
 * @param[in] fd Socket or file descriptor.
 * @param[in] buf Buffer containing data.
 * @param[in] count Maximum bytes to write.
 * @return Number of bytes written, or -1 on error.
 */
static __inline ssize_t auto_win_msvc_write(int fd, const void *buf, size_t count) {
    int r = send((SOCKET)fd, (const char *)buf, (int)count, 0);
    if (r != SOCKET_ERROR) {
        return (ssize_t)r;
    }
    {
        int werr = WSAGetLastError();
        if (werr == WSAENOTSOCK) {
            return (ssize_t)_write(fd, buf, (unsigned int)count);
        }
        if (werr == WSAEWOULDBLOCK || werr == WSAEINPROGRESS) {
            errno = EAGAIN;
        } else if (werr == WSAECONNRESET) {
            errno = ECONNRESET;
        } else {
            errno = EIO;
        }
        return -1;
    }
}
#undef write
#define write auto_win_msvc_write

/**
 * @brief Sends scatter/gather vector buffers using native WSASend.
 * @param[in] fd Socket handle.
 * @param[in] msg Message header containing iovec buffers.
 * @param[in] flags Flags (e.g. 0).
 * @return Number of bytes sent, or -1 on error.
 */
static __inline ssize_t auto_win_msvc_sendmsg(int fd, const struct msghdr *msg, int flags) {
    WSABUF bufs[64];
    DWORD sent = 0;
    int i;
    if (!msg || msg->msg_iovlen <= 0) return -1;
    for (i = 0; i < (int)msg->msg_iovlen && i < 64; i++) {
        bufs[i].buf = (char *)msg->msg_iov[i].iov_base;
        bufs[i].len = (ULONG)msg->msg_iov[i].iov_len;
    }
    if (WSASend((SOCKET)fd, bufs, (DWORD)msg->msg_iovlen, &sent, (DWORD)flags, NULL, NULL) == SOCKET_ERROR) {
        int werr = WSAGetLastError();
        if (werr == WSAEWOULDBLOCK) errno = EAGAIN;
        else errno = EIO;
        return -1;
    }
    return (ssize_t)sent;
}
#ifdef sendmsg
#undef sendmsg
#endif
#define sendmsg auto_win_msvc_sendmsg

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
#undef dup2
#define dup2 auto_win_msvc_dup2

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
        (void)TerminateProcess(h, (UINT)sig);
        (void)CloseHandle(h);
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
    (void)options;
    h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
    if (h) {
        (void)WaitForSingleObject(h, 2000);
        if (status) {
            DWORD code = 0;
            (void)GetExitCodeProcess(h, &code);
            *status = (int)code;
        }
        (void)CloseHandle(h);
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

#endif /* _MSC_VER || _WIN32 */
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUTO_WIN_MSVC_CONFIGURE_H */
