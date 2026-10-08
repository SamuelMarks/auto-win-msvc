/* clang-format off */
#include "greatest.h"
#include "posix-spawn.h"
#include <stdlib.h>
/* clang-format on */

#if defined(_WIN32)
__declspec(dllimport)
#endif
extern void *(*posix_spawn_mock_malloc_ptr)(size_t);

static int mock_oom = 0;
static void *my_malloc(size_t size) {
  if (mock_oom > 0) {
    mock_oom--;
    if (mock_oom == 0)
      return NULL;
  }
  return malloc(size);
}

TEST test_posix_spawn_oom(void) {
  posix_spawn_file_actions_t fa;

  posix_spawn_mock_malloc_ptr = my_malloc;

  posix_spawn_file_actions_init(&fa);

  mock_oom = 1;
  ASSERT_EQ(ENOMEM, posix_spawn_file_actions_addclose(&fa, 1));
  mock_oom = 1;
  ASSERT_EQ(ENOMEM, posix_spawn_file_actions_adddup2(&fa, 1, 2));
  mock_oom = 1;
  ASSERT_EQ(ENOMEM, posix_spawn_file_actions_addopen(&fa, 1, "test", 0, 0));
  mock_oom = 0;

  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 1));

  mock_oom = 2; /* First malloc succeeds, second malloc (path) fails */
  ASSERT_EQ(ENOMEM, posix_spawn_file_actions_addopen(&fa, 2, "test2", 0, 0));
  mock_oom = 0;

  posix_spawn_file_actions_destroy(&fa);

  posix_spawn_mock_malloc_ptr = NULL;
  PASS();
}

SUITE(suite_posix_spawn_mock_oom) { RUN_TEST(test_posix_spawn_oom); }
