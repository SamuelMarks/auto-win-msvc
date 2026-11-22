#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
/* clang-format on */

TEST test_msync(void) {
  void *ptr;
  int rc;

  ptr = mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
             -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);

  /* Invalid / misaligned address failure */
#if defined(__linux__)
  rc = msync((void *)((char *)ptr + 1), 4096, MS_SYNC);
#else
  rc = msync(NULL, 4096, MS_SYNC);
#endif
  ASSERT_EQ(-1, rc);

  rc = msync(ptr, 4096, MS_SYNC);
  ASSERT_EQ(0, rc);

  rc = msync(ptr, 4096, MS_ASYNC);
  ASSERT_EQ(0, rc);

  rc = msync(ptr, 4096, MS_INVALIDATE);
  ASSERT_EQ(0, rc);

  rc = munmap(ptr, 8192);
  ASSERT_EQ(0, rc);

  PASS();
}

SUITE(suite_posix_mman_msync) { RUN_TEST(test_msync); }
