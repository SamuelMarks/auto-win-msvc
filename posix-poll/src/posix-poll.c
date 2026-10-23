/**
 * @file posix-poll.c
 * @brief Implementation of posix-poll compatibility functions.
 */

/* clang-format off */
#include "posix-poll.h"
#include <stddef.h>
#include <time.h>

#if defined(_WIN32)
#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <errno.h>
#include <stdlib.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
static void poll_null_invalid_param_handler(const wchar_t *expression,
                                            const wchar_t *function,
                                            const wchar_t *file,
                                            unsigned int line,
                                            uintptr_t pReserved) {
  if (expression || function || file || line || pReserved) {
  }
}
#endif
static ptrdiff_t safe_get_osfhandle(intptr_t fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
  if (fd < 0 || fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(poll_null_invalid_param_handler);
#if defined(_DEBUG) && !defined(_DLL)
    _CrtSetReportMode(2, 0);
    _CrtSetReportMode(1, 0);
    _CrtSetReportMode(0, 0);
#endif
    h = (ptrdiff_t)_get_osfhandle((int)fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle((int)fd);
#endif
  errno = saved_err;
  if (h == -2) {
    return -1;
  }
  return h;
}
#endif

#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif

extern intptr_t posix_get_dup_socket(intptr_t fd);
extern intptr_t posix_lookup_crt_socket(intptr_t fd);

static SOCKET resolve_socket_or_fd_poll(intptr_t fd) {
  ptrdiff_t h;
  intptr_t s = posix_get_dup_socket(fd);
  if (s != 0) {
    return (SOCKET)s;
  }
  s = posix_lookup_crt_socket(fd);
  if (s != 0) {
    return (SOCKET)s;
  }
  if (fd >= 0 && fd < 64) {
    h = safe_get_osfhandle(fd);
    if (h != -1) {
      return (SOCKET)h;
    }
  }
  return (SOCKET)fd;
}

#define GET_SOCKET(fd) resolve_socket_or_fd_poll(fd)
#else
#include <poll.h>
#endif
/* clang-format on */

#undef poll
#undef WSAPoll

/**
 * @brief Retrieves information on posix-poll availability.
 */
auto_win_msvc_error_t posix_poll_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

#if defined(_WIN32)

__declspec(dllimport) unsigned long __stdcall GetFileType(void *hFile);
__declspec(dllimport) int __stdcall
PeekNamedPipe(void *hNamedPipe, void *lpBuffer, unsigned long nBufferSize,
              unsigned long *lpBytesRead, unsigned long *lpTotalBytesAvail,
              unsigned long *lpBytesLeftThisMessage);

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
    if (getsockopt(s, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen) ==
        SOCKET_ERROR) {
      /* Non-socket handle (pipe/file/console) */
      unsigned long ftype = GetFileType((void *)s);
      short rev = 0;
      if (fds[i].events & POLLOUT) {
        rev |= POLLOUT;
      }
      if (fds[i].events & POLLIN) {
        if (ftype == 3 /* FILE_TYPE_PIPE */) {
          unsigned long avail = 0;
          if (!PeekNamedPipe((void *)s, NULL, 0, NULL, &avail, NULL)) {
            /* Pipe broken / EOF -> readable for EOF or HUP */
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

  result = select(max_fd + 1, &read_fds, &write_fds, &except_fds, tvp);

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
  if (fds == NULL && nfds > 0) {
    return -1;
  }
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) ||           \
    defined(__CYGWIN__)
  return poll((struct pollfd *)fds, (nfds_t)nfds, timeout);
#else
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
  if (sigmask != NULL) {
    return res;
  }
  return res;
}

typedef int make_iso_compilers_happy_tu_posix_poll;
