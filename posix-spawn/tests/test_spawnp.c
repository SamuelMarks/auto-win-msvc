#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-spawn.h"
#include <stddef.h>
/* clang-format on */

TEST test_posix_spawnp(void) {
  pid_t pid;
  char *argv[2];
  int rc;

  argv[0] = "nonexistent_command_xyz_12345";
  argv[1] = NULL;

  rc = posix_spawnp(&pid, argv[0], NULL, NULL, argv, NULL);
  ASSERT(rc != 0);
  PASS();
}

SUITE(suite_posix_spawn_spawnp) { RUN_TEST(test_posix_spawnp); }
