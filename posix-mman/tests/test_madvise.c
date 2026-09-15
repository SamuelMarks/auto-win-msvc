#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
/* clang-format on */

TEST test_madvise(void) {
  void *ptr;
  int rc;

  /* Invalid arguments */
  rc = madvise(NULL, 4096, MADV_NORMAL);
  ASSERT_EQ(-1, rc);
#if defined(_WIN32)
  ASSERT_EQ(EINVAL, errno);
#else
  ASSERT(errno == EINVAL || errno == EFAULT || errno == ENOMEM);
#endif

  ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
             -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);

  rc = madvise(ptr, 0, MADV_NORMAL);
#if defined(_WIN32)
  ASSERT_EQ(-1, rc);
  ASSERT_EQ(EINVAL, errno);
#else
  (void)rc;
#endif

  /* MADV_DONTNEED and MADV_FREE */
  rc = madvise(ptr, 4096, MADV_DONTNEED);
  ASSERT_EQ(0, rc);

  rc = madvise(ptr, 4096, MADV_FREE);
  ASSERT_EQ(0, rc);

  /* Other advice flags */
  rc = madvise(ptr, 4096, MADV_NORMAL);
  ASSERT_EQ(0, rc);

  rc = madvise(ptr, 4096, MADV_RANDOM);
  ASSERT_EQ(0, rc);

  rc = madvise(ptr, 4096, MADV_SEQUENTIAL);
  ASSERT_EQ(0, rc);

  rc = madvise(ptr, 4096, MADV_WILLNEED);
  ASSERT_EQ(0, rc);

  rc = munmap(ptr, 4096);
  ASSERT_EQ(0, rc);

  PASS();
}

SUITE(suite_posix_mman_madvise) { RUN_TEST(test_madvise); }
