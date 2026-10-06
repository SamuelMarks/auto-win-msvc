#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/stat.h>
#else
#include <io.h>
#endif
/* clang-format on */

TEST test_memfd_create(void) {
  int fd;

  /* NULL name should fail */
  fd = posix_memfd_create(NULL, 0);
  ASSERT(fd == -1);

  /* Valid name */
  fd = posix_memfd_create("test_memfd", MFD_CLOEXEC);
  ASSERT(fd >= 0);
#ifndef _MSC_VER
  close(fd);
#else
  _close(fd);
#endif

  /* Invalid flag */
  fd = posix_memfd_create("test_memfd2", 0xFFFFFFFF);
  ASSERT(fd == -1);

  PASS();
}

TEST test_memfd_create_fail(void) {
#if !defined(_WIN32) && !defined(_WIN64) && !defined(__linux__)
  int fd;
  char buf[256];
  int r;

  srand(0);
  r = rand();
  snprintf(buf, sizeof(buf), "/tmp/mfd_%ld_%u", (long)getpid(),
           (unsigned int)r);

  mkdir(buf, 0755);

  srand(0);
  fd = posix_memfd_create("test", 0);
  ASSERT(fd == -1);

  rmdir(buf);
#endif
  PASS();
}

SUITE(suite_posix_mman_memfd_create) {
  RUN_TEST(test_memfd_create);
  RUN_TEST(test_memfd_create_fail);
}
