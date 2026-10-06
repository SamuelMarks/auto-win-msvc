#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

SUITE_EXTERN(suite_linux_sys_statfs_core);
SUITE_EXTERN(suite_linux_sys_statfs_extra);
SUITE_EXTERN(suite_linux_sys_statfs_extra);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_linux_sys_statfs_core);
  RUN_SUITE(suite_linux_sys_statfs_extra);
  RUN_SUITE(suite_linux_sys_statfs_extra);
  GREATEST_MAIN_END();
}
