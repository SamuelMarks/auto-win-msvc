/* posix-sys-select.c */
/* clang-format off */
#include "posix-sys-select.h"

#ifdef _MSC_VER






#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif
#undef FD_ZERO
static void posix_fd_zero(fd_set *set) { set->fd_count = 0; }
#define FD_ZERO(set) posix_fd_zero((fd_set*)set)
#undef FD_SET
static void posix_fd_set(SOCKET fd, fd_set *set) {
  u_int __i;
  for (__i = 0; __i < set->fd_count; __i++) {
    if (set->fd_array[__i] == fd) {
      break;
    }
  }
  if (__i == set->fd_count) {
    if (set->fd_count < FD_SETSIZE) {
      set->fd_array[__i] = fd;
      set->fd_count++;
    }
  }
}
#define FD_SET(fd, set) posix_fd_set((SOCKET)(fd), (fd_set *)(set))
#undef FD_CLR
static void posix_fd_clr(SOCKET fd, fd_set *set) {
  u_int __i;
  for (__i = 0; __i < set->fd_count; __i++) {
    if (set->fd_array[__i] == fd) {
      while (__i < set->fd_count - 1) {
        set->fd_array[__i] = set->fd_array[__i + 1];
        __i++;
      }
      set->fd_count--;
      break;
    }
  }
}
#define FD_CLR(fd, set) posix_fd_clr((SOCKET)(fd), (fd_set *)(set))


#endif

/* clang-format on */

typedef int make_iso_compilers_happy_tu_posix_sys_select;
