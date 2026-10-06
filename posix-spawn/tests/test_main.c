#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

SUITE_EXTERN(suite_posix_spawn_spawn);
SUITE_EXTERN(suite_posix_spawn_spawnp);
SUITE_EXTERN(suite_posix_spawn_spawnattr);
SUITE_EXTERN(suite_posix_spawn_header);
SUITE_EXTERN(suite_posix_spawn_mock_oom);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_posix_spawn_spawn);
  RUN_SUITE(suite_posix_spawn_spawnp);
  RUN_SUITE(suite_posix_spawn_spawnattr);
  RUN_SUITE(suite_posix_spawn_header);
  RUN_SUITE(suite_posix_spawn_mock_oom);
  GREATEST_MAIN_END();
}
