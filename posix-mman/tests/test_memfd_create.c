#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
  unsigned long pid_val;
  unsigned int rnd_val;
  char num_buf[32];
  size_t pos;
  size_t i;

  srand(0);
  r = rand();
  pid_val = (unsigned long)getpid();
  rnd_val = (unsigned int)r;
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/tmp/mfd_");
#else
  strcpy(buf, "/tmp/mfd_");
#endif
  pos = strlen(buf);
  i = 0;
  do {
    num_buf[i++] = (char)('0' + (pid_val % 10UL));
    pid_val /= 10UL;
  } while (pid_val > 0UL && i < sizeof(num_buf));
  while (i > 0) {
    buf[pos++] = num_buf[--i];
  }
  buf[pos++] = '_';
  i = 0;
  do {
    num_buf[i++] = (char)('0' + (rnd_val % 10U));
    rnd_val /= 10U;
  } while (rnd_val > 0U && i < sizeof(num_buf));
  while (i > 0) {
    buf[pos++] = num_buf[--i];
  }
  buf[pos] = '\0';

  mkdir(buf, 0755);

  srand(0);
  fd = posix_memfd_create("test", 0);
  ASSERT(fd == -1);

  rmdir(buf);
#endif
  PASS();
}

TEST test_posix_memfd_create(void) {
  int fd;
  fd = posix_memfd_create(NULL, 0);
  ASSERT(fd == -1);
  PASS();
}

SUITE(suite_posix_mman_memfd_create) {
  RUN_TEST(test_memfd_create);
  RUN_TEST(test_memfd_create_fail);
  RUN_TEST(test_posix_memfd_create);
}
