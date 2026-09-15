#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
/* clang-format on */

TEST test_mlock(void) {
  void *ptr;
  int rc;

#if defined(_WIN32)
  /* Invalid arguments failure */
  rc = mlock(NULL, 0);
  ASSERT_EQ(-1, rc);
#endif

  ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
             -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);

  rc = mlock(ptr, 4096);
#if defined(_WIN32)
  ASSERT_EQ(0, rc);
  rc = munlock(ptr, 4096);
  ASSERT_EQ(0, rc);
#else
  if (rc == 0) {
    (void)munlock(ptr, 4096);
  }
#endif

  munmap(ptr, 4096);
  PASS();
}

SUITE(suite_posix_mman_mlock) { RUN_TEST(test_mlock); }
