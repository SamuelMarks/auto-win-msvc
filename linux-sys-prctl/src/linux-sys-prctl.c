#include <errno.h>
#ifndef ENOSYS
#define ENOSYS 38
#endif
/* clang-format off */
#include "linux-sys-prctl.h"
/* clang-format on */
#if defined(_MSC_VER) && !defined(__clang__)
/** \brief prctl function. */
int prctl(int option, ...) {
  option = option;
  errno = ENOSYS;
  return -1;
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_linux_sys_prctl(void) { return 0; }

typedef int make_iso_compilers_happy_tu_linux_sys_prctl;
