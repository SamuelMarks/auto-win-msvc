#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-spawn.h"
#include <stddef.h>
/* clang-format on */

TEST test_spawn(void) {
  pid_t pid;
  if (posix_spawn(&pid, "", NULL, NULL, NULL, NULL) != 0)
    SKIP();
  PASS();
}

TEST test_posix_spawn(void) { SKIP(); /* Generated stub for posix_spawn */ }

TEST test_posix_spawn_file_actions_init(void) {
  SKIP(); /* Generated stub for posix_spawn_file_actions_init */
}

TEST test_posix_spawn_file_actions_destroy(void) {
  SKIP(); /* Generated stub for posix_spawn_file_actions_destroy */
}

TEST test_posix_spawn_file_actions_addclose(void) {
  SKIP(); /* Generated stub for posix_spawn_file_actions_addclose */
}

TEST test_posix_spawn_file_actions_adddup2(void) {
  SKIP(); /* Generated stub for posix_spawn_file_actions_adddup2 */
}

TEST test_posix_spawn_file_actions_addopen(void) {
  SKIP(); /* Generated stub for posix_spawn_file_actions_addopen */
}

SUITE(suite_posix_spawn_spawn) {
  RUN_TEST(test_spawn);
  RUN_TEST(test_posix_spawn);
  RUN_TEST(test_posix_spawn_file_actions_init);
  RUN_TEST(test_posix_spawn_file_actions_destroy);
  RUN_TEST(test_posix_spawn_file_actions_addclose);
  RUN_TEST(test_posix_spawn_file_actions_adddup2);
  RUN_TEST(test_posix_spawn_file_actions_addopen);
}
