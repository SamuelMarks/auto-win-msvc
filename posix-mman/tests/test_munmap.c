#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
/* clang-format on */

TEST test_munmap(void) {
  void *ptr;
  int rc;

  /* Invalid address failure */
  rc = munmap((void *)(size_t)1, 4096);
  ASSERT_EQ(-1, rc);

  ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
             -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);

  rc = munmap(ptr, 4096);
  ASSERT_EQ(0, rc);

  PASS();
}

SUITE(suite_posix_mman_munmap) { RUN_TEST(test_munmap); }
