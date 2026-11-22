#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-spawn.h"
#include <stddef.h>
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) &&         \
    !defined(__DOS__)
#include <sys/wait.h>
#endif
/* clang-format on */

TEST test_posix_spawnp(void) {
  pid_t pid;
  char cmd[] = "nonexistent_command_xyz_12345";
  char *argv[2];
  int rc;

  argv[0] = cmd;
  argv[1] = NULL;

  rc = posix_spawnp(&pid, argv[0], NULL, NULL, argv, NULL);
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) &&        \
    !defined(__DOS__)
  if (rc == 0) {
    int status = 0;
    ASSERT_EQ(pid, waitpid(pid, &status, 0));
    ASSERT(WIFEXITED(status));
    ASSERT_EQ(127, WEXITSTATUS(status));
  } else {
    ASSERT(rc != 0);
  }
#else
  ASSERT(rc != 0);
#endif
  PASS();
}

SUITE(suite_posix_spawn_spawnp) { RUN_TEST(test_posix_spawnp); }
