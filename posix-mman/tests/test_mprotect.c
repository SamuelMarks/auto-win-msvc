#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
/* clang-format on */

TEST test_mprotect(void) {
  void *ptr;
  int rc;

  /* Invalid address failure */
  rc = mprotect(NULL, 4096, PROT_READ);
  ASSERT_EQ(-1, rc);

  rc = mprotect((void *)(size_t)1, 4096, PROT_READ);
  ASSERT_EQ(-1, rc);

  ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
             -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);

  /* Test all protection flag transitions */
  rc = mprotect(ptr, 4096, PROT_READ);
  ASSERT_EQ(0, rc);

  rc = mprotect(ptr, 4096, PROT_READ | PROT_WRITE);
  ASSERT_EQ(0, rc);

  rc = mprotect(ptr, 4096, PROT_EXEC);
  ASSERT_EQ(0, rc);

  rc = mprotect(ptr, 4096, PROT_EXEC | PROT_READ);
  ASSERT_EQ(0, rc);

  rc = mprotect(ptr, 4096, PROT_EXEC | PROT_WRITE);
#if defined(_WIN32)
  ASSERT_EQ(0, rc);
#else
  (void)rc;
#endif

  rc = mprotect(ptr, 4096, PROT_NONE);
  ASSERT_EQ(0, rc);

  rc = munmap(ptr, 4096);
  ASSERT_EQ(0, rc);

  PASS();
}

SUITE(suite_posix_mman_mprotect) { RUN_TEST(test_mprotect); }
