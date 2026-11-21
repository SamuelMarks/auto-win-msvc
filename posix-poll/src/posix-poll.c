/**
 * @file posix-poll.c
 * @brief Implementation of posix-poll compatibility functions.
 */

/* clang-format off */
#include "posix-poll.h"
#include <stddef.h>
#include <time.h>

#if defined(_WIN32)
#include <errno.h>
#include <stdlib.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#endif
#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif
#else
#include <poll.h>
#endif
/* clang-format on */

#undef poll
#undef WSAPoll

/**
 * @brief Retrieves information on posix-poll availability.
 */
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
auto_win_msvc_error_t posix_poll_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}
#endif

#if defined(_WIN32)

#if defined(_MSC_VER)
#if defined(_M_IX86)
#pragma comment(linker,                                                        \
                "/alternatename:_posix_get_dup_socket=_poll_dup_socket_stub")
#pragma comment(                                                               \
    linker, "/alternatename:_posix_lookup_crt_socket=_poll_crt_socket_stub")
#else
#pragma comment(linker,                                                        \
                "/alternatename:posix_get_dup_socket=poll_dup_socket_stub")
#pragma comment(linker,                                                        \
                "/alternatename:posix_lookup_crt_socket=poll_crt_socket_stub")
#endif
/** @brief Fallback stub for posix_get_dup_socket when posix-sockets is absent.
 */
intptr_t poll_dup_socket_stub(intptr_t fd) {
  (void)fd;
  return 0;
}
/** @brief Fallback stub for posix_lookup_crt_socket when posix-sockets is
 * absent. */
intptr_t poll_crt_socket_stub(intptr_t fd) {
  (void)fd;
  return 0;
}
#elif defined(__GNUC__) || defined(__clang__)
/** @brief Weak fallback stub for posix_get_dup_socket when posix-sockets is
 * absent. */
intptr_t posix_get_dup_socket(intptr_t fd);
intptr_t __attribute__((weak)) posix_get_dup_socket(intptr_t fd) {
  (void)fd;
  return 0;
}
/** @brief Weak fallback stub for posix_lookup_crt_socket when posix-sockets is
 * absent. */
intptr_t posix_lookup_crt_socket(intptr_t fd);
intptr_t __attribute__((weak)) posix_lookup_crt_socket(intptr_t fd) {
  (void)fd;
  return 0;
}
#endif

extern intptr_t posix_get_dup_socket(intptr_t fd);
extern intptr_t posix_lookup_crt_socket(intptr_t fd);

__declspec(dllimport) unsigned long __stdcall GetFileType(void *hFile);
__declspec(dllimport) int __stdcall
PeekNamedPipe(void *hNamedPipe, void *lpBuffer, unsigned long nBufferSize,
              unsigned long *lpBytesRead, unsigned long *lpTotalBytesAvail,
              unsigned long *lpBytesLeftThisMessage);

#if defined(_MSC_VER) && _MSC_VER >= 1400
static void poll_null_invalid_param_handler(const wchar_t *expression,
                                            const wchar_t *function,
                                            const wchar_t *file,
                                            unsigned int line,
                                            uintptr_t pReserved) {
  if (expression || function || file || line || pReserved) {
  }
}
#endif

static intptr_t default_poll_raw_osfhandle(int fd) {
  return (intptr_t)_get_osfhandle(fd);
}
static intptr_t default_poll_dup_sock(intptr_t fd) {
  return posix_get_dup_socket(fd);
}
static intptr_t default_poll_crt_sock(intptr_t fd) {
  return posix_lookup_crt_socket(fd);
}
static int default_poll_getsockopt(SOCKET s, char *optval, int *optlen) {
  return getsockopt(s, SOL_SOCKET, SO_TYPE, optval, optlen);
}
static unsigned long default_poll_file_type(void *h) { return GetFileType(h); }
static int default_poll_peek_pipe(void *h, unsigned long *avail) {
  return PeekNamedPipe(h, NULL, 0, NULL, avail, NULL);
}
static int default_poll_select(int nfds, fd_set *rfds, fd_set *wfds,
                               fd_set *efds, struct timeval *tv) {
  return select(nfds, rfds, wfds, efds, tv);
}

static intptr_t (*g_poll_raw_osfhandle)(int) = default_poll_raw_osfhandle;
static intptr_t (*g_poll_dup_sock)(intptr_t) = default_poll_dup_sock;
static intptr_t (*g_poll_crt_sock)(intptr_t) = default_poll_crt_sock;
static int (*g_poll_getsockopt)(SOCKET, char *,
                                int *) = default_poll_getsockopt;
static unsigned long (*g_poll_file_type)(void *) = default_poll_file_type;
static int (*g_poll_peek_pipe)(void *,
                               unsigned long *) = default_poll_peek_pipe;
static int (*g_poll_select)(int, fd_set *, fd_set *, fd_set *,
                            struct timeval *) = default_poll_select;

/**
 * @brief Overrides Win32/Winsock hooks used by posix-poll for unit testing.
 */
void posix_poll_set_hooks(
    intptr_t (*raw_osf_fn)(int), intptr_t (*dup_sock_fn)(intptr_t),
    intptr_t (*crt_sock_fn)(intptr_t),
    int (*getsockopt_fn)(uintptr_t, char *, int *),
    unsigned long (*ftype_fn)(void *), int (*peek_fn)(void *, unsigned long *),
    int (*select_fn)(int, void *, void *, void *, void *));
void posix_poll_set_hooks(
    intptr_t (*raw_osf_fn)(int), intptr_t (*dup_sock_fn)(intptr_t),
    intptr_t (*crt_sock_fn)(intptr_t),
    int (*getsockopt_fn)(uintptr_t, char *, int *),
    unsigned long (*ftype_fn)(void *), int (*peek_fn)(void *, unsigned long *),
    int (*select_fn)(int, void *, void *, void *, void *)) {
  g_poll_raw_osfhandle = raw_osf_fn;
  g_poll_dup_sock = dup_sock_fn;
  g_poll_crt_sock = crt_sock_fn;
  g_poll_getsockopt = (int (*)(SOCKET, char *, int *))getsockopt_fn;
  g_poll_file_type = ftype_fn;
  g_poll_peek_pipe = peek_fn;
  g_poll_select =
      (int (*)(int, fd_set *, fd_set *, fd_set *, struct timeval *))select_fn;
}

/**
 * @brief Resets Win32/Winsock hooks used by posix-poll to defaults.
 */
void posix_poll_reset_hooks(void);
void posix_poll_reset_hooks(void) {
  g_poll_raw_osfhandle = default_poll_raw_osfhandle;
  g_poll_dup_sock = default_poll_dup_sock;
  g_poll_crt_sock = default_poll_crt_sock;
  g_poll_getsockopt = default_poll_getsockopt;
  g_poll_file_type = default_poll_file_type;
  g_poll_peek_pipe = default_poll_peek_pipe;
  g_poll_select = default_poll_select;
}

static ptrdiff_t safe_get_osfhandle(intptr_t fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(poll_null_invalid_param_handler);
    AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
    h = (ptrdiff_t)g_poll_raw_osfhandle((int)fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)g_poll_raw_osfhandle((int)fd);
#endif
  errno = saved_err;
  if (h == -2) {
    return -1;
  }
  return h;
}

static SOCKET resolve_socket_or_fd_poll(intptr_t fd) {
  ptrdiff_t h;
  intptr_t s = g_poll_dup_sock(fd);
  if (s != 0) {
    return (SOCKET)s;
  }
  s = g_poll_crt_sock(fd);
  if (s != 0) {
    return (SOCKET)s;
  }
  if (fd < 64) {
    h = safe_get_osfhandle(fd);
    if (h != -1) {
      return (SOCKET)h;
    }
  }
  return (SOCKET)fd;
}

#define GET_SOCKET(fd) resolve_socket_or_fd_poll(fd)

static void posix_fd_zero(fd_set *set) { set->fd_count = 0; }
#undef FD_ZERO
#define FD_ZERO(set) posix_fd_zero((fd_set *)(set))

static void posix_fd_set(SOCKET fd, fd_set *set) {
  u_int i;
  for (i = 0; i < set->fd_count; i++) {
    if (set->fd_array[i] == fd) {
      break;
    }
  }
  if (i == set->fd_count) {
    if (set->fd_count < FD_SETSIZE) {
      set->fd_array[i] = fd;
      set->fd_count++;
    }
  }
}
#undef FD_SET
#define FD_SET(fd, set) posix_fd_set((SOCKET)(fd), (fd_set *)(set))

int posix_poll(struct pollfd *fds, unsigned long nfds, int timeout) {
  fd_set read_fds, write_fds, except_fds;
  struct timeval tv, *tvp;
  unsigned long i;
  int max_fd = 0;
  int num_sockets = 0;
  int num_other_ready = 0;
  int result;

  if (fds == NULL && nfds > 0) {
    return -1;
  }

  if (nfds == 0) {
    if (timeout > 0) {
      Sleep((DWORD)timeout);
    }
    return 0;
  }

  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);

  for (i = 0; i < nfds; ++i) {
    SOCKET s;
    int optval = 0;
    int optlen = sizeof(optval);
    fds[i].revents = 0;
    if ((intptr_t)fds[i].fd < 0) {
      continue;
    }
    s = GET_SOCKET((intptr_t)fds[i].fd);
    if (s == (SOCKET)-1) {
      continue;
    }
    if (g_poll_getsockopt(s, (char *)&optval, &optlen) == SOCKET_ERROR) {
      /* Non-socket handle (pipe/file/console) */
      void *h = ((intptr_t)fds[i].fd < 2048)
                    ? (void *)safe_get_osfhandle((intptr_t)fds[i].fd)
                    : (void *)s;
      unsigned long ftype;
      short rev = 0;
      if (h == (void *)(intptr_t)-1) {
        h = (void *)s;
      }
      ftype = g_poll_file_type(h);
      if (fds[i].events & POLLOUT) {
        rev |= POLLOUT;
      }
      if (fds[i].events & POLLIN) {
        if (ftype == 3 /* FILE_TYPE_PIPE */) {
          unsigned long avail = 0;
          if (!g_poll_peek_pipe(h, &avail)) {
            rev |= POLLIN;
          } else if (avail > 0) {
            rev |= POLLIN;
          }
        } else if (ftype == 1 /* FILE_TYPE_DISK */) {
          rev |= POLLIN;
        }
      }
      fds[i].revents = rev;
      if (rev != 0) {
        num_other_ready++;
      }
      continue;
    }
    num_sockets++;
    if (fds[i].events & (POLLIN | POLLPRI)) {
      FD_SET(s, &read_fds);
    }
    if (fds[i].events & POLLOUT) {
      FD_SET(s, &write_fds);
    }
    FD_SET(s, &except_fds);
    if ((int)s > max_fd) {
      max_fd = (int)s;
    }
  }

  if (num_sockets == 0) {
    if (num_other_ready > 0) {
      return num_other_ready;
    }
    if (timeout > 0) {
      Sleep((DWORD)timeout);
    }
    return 0;
  }

  if (num_other_ready > 0) {
    tv.tv_sec = 0;
    tv.tv_usec = 50000;
    tvp = &tv;
  } else if (timeout >= 0) {
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    tvp = &tv;
  } else {
    tvp = NULL;
  }

  result = g_poll_select(max_fd + 1, &read_fds, &write_fds, &except_fds, tvp);

  if (result > 0) {
    for (i = 0; i < nfds; ++i) {
      SOCKET s;
      if ((intptr_t)fds[i].fd < 0) {
        continue;
      }
      s = GET_SOCKET((intptr_t)fds[i].fd);
      if (s == (SOCKET)-1) {
        continue;
      }
      if (FD_ISSET(s, &read_fds)) {
        fds[i].revents |= (fds[i].events & (POLLIN | POLLPRI));
      }
      if (FD_ISSET(s, &write_fds)) {
        fds[i].revents |= (fds[i].events & POLLOUT);
      }
      if (FD_ISSET(s, &except_fds)) {
        fds[i].revents |= POLLERR;
      }
    }
  } else if (result < 0) {
    if (num_other_ready > 0) {
      return num_other_ready;
    }
    return -1;
  }
  return result + num_other_ready;
}

#else

int posix_poll(struct pollfd *fds, unsigned long nfds, int timeout) {
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (fds == NULL && nfds > 0) {
    return -1;
  }
#endif
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) ||           \
    defined(__CYGWIN__)
  return poll((struct pollfd *)fds, (nfds_t)nfds, timeout);
#else
  (void)fds;
  (void)nfds;
  if (timeout > 0) {
    return 0;
  }
  return 0;
#endif
}

#endif

int posix_ppoll(struct pollfd *fds, unsigned long nfds,
                const struct timespec *tmo_p, const sigset_t *sigmask) {
  int timeout_ms = -1;
  int res;
  if (tmo_p != NULL) {
    timeout_ms = (int)(tmo_p->tv_sec * 1000 + tmo_p->tv_nsec / 1000000);
  }
  res = posix_poll(fds, nfds, timeout_ms);
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (sigmask != NULL) {
    return res;
  }
#else
  (void)sigmask;
#endif
  return res;
}

typedef int make_iso_compilers_happy_tu_posix_poll;
