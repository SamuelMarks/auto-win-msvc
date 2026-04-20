/* clang-format off */
#include "bsd-sys-file.h"
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <errno.h>
#include <io.h>
#include <winsock2.h>
#endif

int posix_flock(int fd, int operation) {
  (void)fd;
  (void)operation;
  return 0;
}

int dummy_bsd_sys_file(void) { return 0; }
