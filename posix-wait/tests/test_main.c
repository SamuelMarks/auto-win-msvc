#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

SUITE_EXTERN(suite_posix_wait_core);

#if defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
int awm_vfork_try_reap(int pid, int *stat_loc);
int awm_vfork_try_reap(int pid, int *stat_loc) {
  (void)pid;
  (void)stat_loc;
  return 0;
}
#endif

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_posix_wait_core);
  GREATEST_MAIN_END();
}
