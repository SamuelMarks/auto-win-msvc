/**
 * @file posix-sys-select.c
 * @brief Implementation of posix-sys-select helpers.
 */

/* clang-format off */
#include "posix-sys-select.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on posix-sys-select availability.
 */
enum posix_sys_select_error_code posix_sys_select_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_SYS_SELECT_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_SYS_SELECT_SUCCESS;
}

#if defined(_WIN32) || defined(_MSC_VER)

void auto_win_msvc_fd_set(SOCKET fd, fd_set *set) {
  u_int i;
  if (set == NULL) {
    return;
  }
  for (i = 0; i < set->fd_count; i++) {
    if (set->fd_array[i] == fd) {
      return;
    }
  }
  if (set->fd_count < FD_SETSIZE) {
    set->fd_array[set->fd_count] = fd;
    set->fd_count++;
  }
}

void auto_win_msvc_fd_clr(SOCKET fd, fd_set *set) {
  u_int i;
  if (set == NULL) {
    return;
  }
  for (i = 0; i < set->fd_count; i++) {
    if (set->fd_array[i] == fd) {
      while (i < set->fd_count - 1) {
        set->fd_array[i] = set->fd_array[i + 1];
        i++;
      }
      set->fd_count--;
      return;
    }
  }
}

#endif /* defined(_WIN32) || defined(_MSC_VER) */

typedef int make_iso_compilers_happy_tu_posix_sys_select;
