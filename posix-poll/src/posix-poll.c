/* posix-poll.c */
/* clang-format off */
#include "posix-poll.h"

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
#if defined(_WIN32)
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle(fd))
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif

#ifdef _WIN32
#include <stddef.h>
/* clang-format on */

#undef poll

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
    if (((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)) == (SOCKET)-1)
      continue;
    if (fds[i].events & (1 | 2)) /* POLLIN | POLLPRI */
      FD_SET(((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)), &read_fds);
    if (fds[i].events & 4) /* POLLOUT */
      FD_SET(((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)), &write_fds);
    FD_SET(((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)), &except_fds);
    if ((int)((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)) > max_fd)
      max_fd = (int)((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd));
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
      if (((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)) == (SOCKET)-1)
        continue;
      if (FD_ISSET(((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)),
                   &read_fds))
        fds[i].revents |= (fds[i].events & (1 | 2));
      if (FD_ISSET(((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)),
                   &write_fds))
        fds[i].revents |= (fds[i].events & 4);
      if (FD_ISSET(((SOCKET)safe_get_osfhandle((intptr_t)fds[i].fd)),
                   &except_fds))
        fds[i].revents |= 8; /* POLLERR */
    }
  }
  return result;
}
#else
typedef int make_iso_compilers_happy_tu_posix_poll;
#endif
