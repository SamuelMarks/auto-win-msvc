#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

SUITE_EXTERN(suite_posix_core_process);
SUITE_EXTERN(suite_posix_core_posix);
SUITE_EXTERN(suite_posix_core_misc);
SUITE_EXTERN(suite_posix_core_io);
SUITE_EXTERN(suite_posix_core_fs);
SUITE_EXTERN(suite_posix_core_ugid);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_posix_core_process);
  RUN_SUITE(suite_posix_core_posix);
  RUN_SUITE(suite_posix_core_misc);
  RUN_SUITE(suite_posix_core_io);
  RUN_SUITE(suite_posix_core_fs);
  RUN_SUITE(suite_posix_core_ugid);
  GREATEST_MAIN_END();
  return 0;
}
