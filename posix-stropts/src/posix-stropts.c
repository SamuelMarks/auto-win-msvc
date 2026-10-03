/**
 * @file posix-stropts.c
 * @brief Implementation of POSIX STREAMS interface stubs.
 */

/* clang-format off */
#include "stropts.h"
#include <errno.h>
/* clang-format on */

int posix_isastream(int fildes) {
  if (fildes < 0) {
    errno = EBADF;
    return -1;
  }
  /* STREAMS is not implemented on Windows */
  return 0;
}

int dummy_posix_stropts(void) { return 0; }
