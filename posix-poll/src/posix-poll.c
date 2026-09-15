/**
 * @file posix-poll.c
 * @brief Implementation of posix-poll compatibility functions.
 */

/* clang-format off */
#include "posix-poll.h"
#include <stddef.h>

#if defined(_WIN32)
#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle((int)(fd)))
#endif

#define GET_SOCKET(fd) ((safe_get_osfhandle(fd) == -1) ? (SOCKET)(fd) : (SOCKET)safe_get_osfhandle(fd))

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
enum posix_poll_error_code posix_poll_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_POLL_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_POLL_SUCCESS;
}

#if defined(_WIN32)

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
    fds[i].revents = 0;
    if (GET_SOCKET((intptr_t)fds[i].fd) == (SOCKET)-1) {
      continue;
    }
    num_sockets++;
    if (fds[i].events & (POLLIN | POLLPRI)) {
      FD_SET(GET_SOCKET((intptr_t)fds[i].fd), &read_fds);
    }
    if (fds[i].events & POLLOUT) {
      FD_SET(GET_SOCKET((intptr_t)fds[i].fd), &write_fds);
    }
    FD_SET(GET_SOCKET((intptr_t)fds[i].fd), &except_fds);
    if ((int)GET_SOCKET((intptr_t)fds[i].fd) > max_fd) {
      max_fd = (int)GET_SOCKET((intptr_t)fds[i].fd);
    }
  }

  if (num_sockets == 0) {
    if (timeout > 0) {
      Sleep((DWORD)timeout);
    }
    return 0;
  }

  if (timeout >= 0) {
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    tvp = &tv;
  } else {
    tvp = NULL;
  }

  result = select(max_fd + 1, &read_fds, &write_fds, &except_fds, tvp);

  if (result > 0) {
    for (i = 0; i < nfds; ++i) {
      fds[i].revents = 0;
      if (GET_SOCKET((intptr_t)fds[i].fd) == (SOCKET)-1) {
        continue;
      }
      if (FD_ISSET(GET_SOCKET((intptr_t)fds[i].fd), &read_fds)) {
        fds[i].revents |= (fds[i].events & (POLLIN | POLLPRI));
      }
      if (FD_ISSET(GET_SOCKET((intptr_t)fds[i].fd), &write_fds)) {
        fds[i].revents |= (fds[i].events & POLLOUT);
      }
      if (FD_ISSET(GET_SOCKET((intptr_t)fds[i].fd), &except_fds)) {
        fds[i].revents |= POLLERR;
      }
    }
  }
  return result;
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
  (void)timeout;
  return 0;
#endif
}

#endif

typedef int make_iso_compilers_happy_tu_posix_poll;
