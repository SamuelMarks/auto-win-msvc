#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-spawn.h"
#include <stdio.h>
#include <stddef.h>
/* clang-format on */

TEST test_posix_spawn_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = posix_spawn_init(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_spawn_init(&status);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_spawn_init failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_spawn(void) {
  pid_t pid;
  char *argv[2];
  int rc;

  argv[0] = "";
  argv[1] = NULL;
  rc = posix_spawn(&pid, "", NULL, NULL, argv, NULL);
  ASSERT(rc != 0);
  PASS();
}

TEST test_posix_spawn(void) {
  pid_t pid;
  char *argv[2];
  int rc;

  argv[0] = "nonexistent_executable_12345.exe";
  argv[1] = NULL;

  rc = posix_spawn(&pid, argv[0], NULL, NULL, argv, NULL);
  ASSERT(rc != 0);
  PASS();
}

TEST test_posix_spawn_file_actions_init(void) {
  posix_spawn_file_actions_t fa;
  ASSERT(posix_spawn_file_actions_init(NULL) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_init(&fa));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

TEST test_posix_spawn_file_actions_destroy(void) {
  posix_spawn_file_actions_t fa;
  ASSERT(posix_spawn_file_actions_destroy(NULL) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_init(&fa));
  ASSERT_EQ(0, posix_spawn_file_actions_destroy(&fa));
  PASS();
}

TEST test_posix_spawn_file_actions_addclose(void) {
  posix_spawn_file_actions_t fa;
  posix_spawn_file_actions_init(&fa);
  ASSERT(posix_spawn_file_actions_addclose(NULL, 3) != 0);
  ASSERT(posix_spawn_file_actions_addclose(&fa, -1) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 3));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

TEST test_posix_spawn_file_actions_adddup2(void) {
  posix_spawn_file_actions_t fa;
  posix_spawn_file_actions_init(&fa);
  ASSERT(posix_spawn_file_actions_adddup2(NULL, 1, 2) != 0);
  ASSERT(posix_spawn_file_actions_adddup2(&fa, -1, 2) != 0);
  ASSERT(posix_spawn_file_actions_adddup2(&fa, 1, -1) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 1, 2));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

TEST test_posix_spawn_file_actions_addopen(void) {
  posix_spawn_file_actions_t fa;
  posix_spawn_file_actions_init(&fa);
  ASSERT(posix_spawn_file_actions_addopen(NULL, 0, "dummy.txt", 0, 0666) != 0);
  ASSERT(posix_spawn_file_actions_addopen(&fa, -1, "dummy.txt", 0, 0666) != 0);
  ASSERT(posix_spawn_file_actions_addopen(&fa, 0, NULL, 0, 0666) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 0, "dummy.txt", 0, 0666));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

TEST test_posix_spawn_file_actions_multiple(void) {
  posix_spawn_file_actions_t fa;
  posix_spawn_file_actions_init(&fa);
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 0, "dummy.txt", 0, 0666));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 0, 1));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 2));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

SUITE(suite_posix_spawn_spawn) {
  RUN_TEST(test_posix_spawn_init);
  RUN_TEST(test_spawn);
  RUN_TEST(test_posix_spawn);
  RUN_TEST(test_posix_spawn_file_actions_init);
  RUN_TEST(test_posix_spawn_file_actions_destroy);
  RUN_TEST(test_posix_spawn_file_actions_addclose);
  RUN_TEST(test_posix_spawn_file_actions_adddup2);
  RUN_TEST(test_posix_spawn_file_actions_addopen);
  RUN_TEST(test_posix_spawn_file_actions_multiple);
}
