/* posix-sys-select.c */
/* clang-format off */
#include "posix-sys-select.h"

#ifdef _MSC_VER
#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif

void auto_win_msvc_fd_set(SOCKET fd, fd_set *set) {
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

void auto_win_msvc_fd_clr(SOCKET fd, fd_set *set) {
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
#endif

/* clang-format on */

typedef int make_iso_compilers_happy_tu_posix_sys_select;
