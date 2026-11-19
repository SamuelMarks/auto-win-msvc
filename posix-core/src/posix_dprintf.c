/**
 * @file posix_dprintf.c
 * @brief Portable C89 implementation of POSIX dprintf.
 */

/* clang-format off */
#include "posix-core.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#elif !defined(_MSC_VER) && !defined(__WATCOMC__)
#include <unistd.h>
#endif
/* clang-format on */

#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @brief Prints formatted output directly to a file descriptor.
 * @param[in] fd Target file descriptor.
 * @param[in] fmt Format string.
 * @return Number of bytes written, or -1 on error.
 */
int dprintf(int fd, const char *fmt, ...) {
  char *buf = NULL;
  va_list ap;
  int ret;
  int written;
  va_start(ap, fmt);
  ret = vasprintf(&buf, fmt, ap);
  va_end(ap);
  if (ret < 0) {
    return -1;
  }
#if defined(_WIN32)
  written = (int)posix_write(fd, buf, (size_t)ret);
#else
  written = (int)write(fd, buf, (size_t)ret);
#endif
  free(buf);
  return written;
}

#endif
typedef int make_iso_compilers_happy_posix_dprintf;
