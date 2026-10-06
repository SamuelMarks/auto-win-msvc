/* clang-format off */
#include "bsd-sys-file.h"
#include <errno.h>
#include <stddef.h>
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <winsock2.h>
#else
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <sys/file.h>
extern int flock(int fd, int operation);
#endif
/* clang-format on */

/**
 * @brief Apply or remove an advisory lock on the open file.
 */
int posix_flock(int fd, int operation) {
#if defined(_MSC_VER) || defined(_WIN32)
  (void)fd;
  (void)operation;
  return -1;
#else
  return flock(fd, operation);
#endif
}

/**
 * @brief Initializes and validates the bsd-sys-file module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t bsd_sys_file_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_file;
