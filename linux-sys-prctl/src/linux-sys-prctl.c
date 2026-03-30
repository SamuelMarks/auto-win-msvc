#include "linux-sys-prctl.h"
#if defined(_MSC_VER) && !defined(__clang__)
/** \brief prctl function. */
int prctl(int option, ...) {
  (void)option;
  return 0;
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_linux_sys_prctl(void) { return 0; }

typedef int make_iso_compilers_happy_tu_linux_sys_prctl;
