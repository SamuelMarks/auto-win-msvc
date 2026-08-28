/* posix-poll.c */
/* clang-format off */
#include "posix-poll.h"




#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle((int)(fd)))
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif

#define GET_SOCKET(fd) ((safe_get_osfhandle(fd) == -1) ? (SOCKET)(fd) : (SOCKET)safe_get_osfhandle(fd))

#ifdef _WIN32
#include <stddef.h>
#include <stdio.h>

#ifdef _MSC_VER
#undef FD_ZERO
#define FD_ZERO(set) (((fd_set *)(set))->fd_count = 0)
#undef FD_SET
#define FD_SET(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set *)(set))->fd_count) { \
        if (((fd_set *)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set *)(set))->fd_array[__i] = (fd); \
            ((fd_set *)(set))->fd_count++; \
        } \
    } \
} while((void)0, 0)
#undef FD_CLR
#define FD_CLR(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == (fd)) { \
            while (__i < ((fd_set *)(set))->fd_count - 1) { \
                ((fd_set *)(set))->fd_array[__i] = \
                    ((fd_set *)(set))->fd_array[__i + 1]; \
                __i++; \
            } \
            ((fd_set *)(set))->fd_count--; \
            break; \
        } \
    } \
} while((void)0, 0)
#endif

/* clang-format on */

#undef poll
#undef WSAPoll

int posix_poll(struct pollfd *fds, unsigned long nfds, int timeout) {
  fd_set read_fds, write_fds, except_fds;
  struct timeval tv, *tvp;
  unsigned long i;
  int max_fd = 0;
  int result;

  if (!fds && nfds > 0)
    return -1;

  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);

  for (i = 0; i < nfds; ++i) {
    if (GET_SOCKET((intptr_t)fds[i].fd) == (SOCKET)-1)
      continue;
    if (fds[i].events & (POLLIN | POLLPRI)) /* POLLIN | POLLPRI */
      FD_SET(GET_SOCKET((intptr_t)fds[i].fd), &read_fds);
    if (fds[i].events & POLLOUT) /* POLLOUT */
      FD_SET(GET_SOCKET((intptr_t)fds[i].fd), &write_fds);
    FD_SET(GET_SOCKET((intptr_t)fds[i].fd), &except_fds);
    if ((int)GET_SOCKET((intptr_t)fds[i].fd) > max_fd)
      max_fd = (int)GET_SOCKET((intptr_t)fds[i].fd);
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
      if (GET_SOCKET((intptr_t)fds[i].fd) == (SOCKET)-1)
        continue;
      if (FD_ISSET(GET_SOCKET((intptr_t)fds[i].fd), &read_fds))
        fds[i].revents |= (fds[i].events & (1 | 2));
      if (FD_ISSET(GET_SOCKET((intptr_t)fds[i].fd), &write_fds))
        fds[i].revents |= (fds[i].events & 4);
      if (FD_ISSET(GET_SOCKET((intptr_t)fds[i].fd), &except_fds))
        fds[i].revents |= 8; /* POLLERR */
    }
  }
  return result;
}
#else
typedef int make_iso_compilers_happy_tu_posix_poll;
#endif
