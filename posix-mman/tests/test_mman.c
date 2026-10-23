#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
#if defined(_WIN32) || defined(_MSC_VER)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_mman(void) {
  void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);
  munmap(ptr, 4096);
  PASS();
}

TEST test_posix_cygwin_mmap(void) {
  void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);
  munmap(ptr, 4096);
  PASS();
}

TEST test_posix_cygwin_madvise(void) {
  void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);
  ASSERT_EQ(0, madvise(ptr, 4096, MADV_NORMAL));
  munmap(ptr, 4096);
  PASS();
}

TEST test_memfd_create(void) {
  int fd;
  int invalid_fd;

  invalid_fd = memfd_create(NULL, 0);
  ASSERT_EQ(-1, invalid_fd);

  invalid_fd = memfd_create("test_invalid", 0xFFFFFFFFU);
  ASSERT_EQ(-1, invalid_fd);

  fd = memfd_create("test_memfd", 0);
  ASSERT(fd >= 0);
#if defined(_WIN32) || defined(_MSC_VER)
  _close(fd);
#else
  close(fd);
#endif
  PASS();
}

SUITE(suite_posix_mman_mman) {
  RUN_TEST(test_mman);
  RUN_TEST(test_posix_cygwin_mmap);
  RUN_TEST(test_posix_cygwin_madvise);
  RUN_TEST(test_memfd_create);
}
