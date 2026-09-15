#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
/* clang-format on */

TEST test_ftok(void) {
  key_t k1, k2;

  /* Null path should fail */
  ASSERT_EQ((key_t)-1, ftok(NULL, 1));

  /* Non-existent path should fail */
  ASSERT_EQ((key_t)-1, ftok("non_existent_file_xyz_12345", 1));

  /* Valid path */
  k1 = ftok(".", 1);
  (void)k1;
  k2 = ftok(".", 2);
  (void)k2;

  PASS();
}

SUITE(suite_posix_ipc_ftok) { RUN_TEST(test_ftok); }
