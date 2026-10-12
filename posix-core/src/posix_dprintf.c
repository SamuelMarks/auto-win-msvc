/* clang-format off */
#include "posix-core.h"
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#endif
/* clang-format on */

/** \brief dprintf function. */
int dprintf(int fd, const char *fmt, ...) {
  char *buf = NULL;
  va_list ap;
  int ret, written;
  va_start(ap, fmt);
  ret = vasprintf(&buf, fmt, ap);
  va_end(ap);
  if (ret < 0 || buf == NULL) {
    return -1;
  }
#if defined(_WIN32)
  written = _write(fd, buf, (unsigned int)ret);
#else
  written = (int)write(fd, buf, (size_t)ret);
#endif
  free(buf);
  return written;
}
