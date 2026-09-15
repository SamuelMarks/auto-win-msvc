#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER) || defined(_WIN32)
#include <process.h>
#include <io.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_posix_core_init(void) {
  enum posix_core_error_code rc;
  int status = 0;

  rc = posix_core_init(NULL);
  if (rc != POSIX_CORE_ERROR_NULL_POINTER) {
    printf("Expected POSIX_CORE_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_core_init(&status);
  if (rc != POSIX_CORE_SUCCESS) {
    printf("posix_core_init failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, status);

  PASS();
}

TEST test_posix_close(void) {
  int fd;

  ASSERT_EQ(-1, posix_close(-1));

  fd = open("test_pclose.tmp", O_RDWR | O_CREAT, 0666);
  ASSERT(fd >= 0);
  ASSERT_EQ(0, posix_close(fd));
  remove("test_pclose.tmp");
  PASS();
}

TEST test_posix_read(void) {
  int fd;
  char buf[16];
  ssize_t bytes_read;

  ASSERT_EQ(-1, posix_read(-1, NULL, 0));

  fd = open("test_pread_t.tmp", O_RDWR | O_CREAT, 0666);
  ASSERT(fd >= 0);
  posix_write(fd, "testdata", 8);
  posix_close(fd);

  fd = open("test_pread_t.tmp", O_RDONLY, 0);
  ASSERT(fd >= 0);
  memset(buf, 0, sizeof(buf));
  bytes_read = posix_read(fd, buf, 8);
  ASSERT_EQ(8, bytes_read);
  ASSERT_STR_EQ("testdata", buf);
  posix_close(fd);

  remove("test_pread_t.tmp");
  PASS();
}

TEST test_posix_write(void) {
  int fd;
  ssize_t written;

  ASSERT_EQ(-1, posix_write(-1, NULL, 0));

  fd = open("test_pwrite_t.tmp", O_RDWR | O_CREAT, 0666);
  ASSERT(fd >= 0);
  written = posix_write(fd, "write_check", 11);
  ASSERT_EQ(11, written);
  posix_close(fd);

  remove("test_pwrite_t.tmp");
  PASS();
}

TEST test_posix_fopen(void) {
  FILE *f;

  ASSERT_EQ(NULL, posix_fopen(NULL, NULL));

  f = posix_fopen("test_pfopen.tmp", "w+");
  ASSERT(f != NULL);
  fputs("abc", f);
  fclose(f);

  remove("test_pfopen.tmp");
  PASS();
}

TEST test_posix_fadvise(void) {
  int fd;

  ASSERT_EQ(EBADF, posix_fadvise(-1, 0, 10, 0));

  fd = open("test_pfadv.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    posix_write(fd, "1234567890", 10);
    posix_fadvise(fd, 0, 10, 1);
    posix_close(fd);
    remove("test_pfadv.tmp");
  }
  PASS();
}

TEST test_posix_fallocate(void) {
  int fd;

  ASSERT_EQ(EBADF, posix_fallocate(-1, 0, 10));

  fd = open("test_pfalloc.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    posix_fallocate(fd, 0, 100);
    posix_close(fd);
    remove("test_pfalloc.tmp");
  }
  PASS();
}

TEST test_posix_rename(void) {
  FILE *f;

  f = fopen("test_pren1.tmp", "w");
  ASSERT(f != NULL);
  fputs("data", f);
  fclose(f);

  ASSERT_EQ(0, posix_rename("test_pren1.tmp", "test_pren2.tmp"));
  remove("test_pren2.tmp");
  PASS();
}

TEST test_posix_mkstemp(void) {
  char tmpl[32];
  int fd;

#if defined(_MSC_VER)
  strcpy_s(tmpl, sizeof(tmpl), "test_pmk_XXXXXX");
#else
  strcpy(tmpl, "test_pmk_XXXXXX");
#endif

  fd = posix_mkstemp(tmpl);
  if (fd >= 0) {
    posix_close(fd);
    remove(tmpl);
  }
  PASS();
}

SUITE(suite_posix_core_posix) {
  RUN_TEST(test_posix_core_init);
  RUN_TEST(test_posix_close);
  RUN_TEST(test_posix_read);
  RUN_TEST(test_posix_write);
  RUN_TEST(test_posix_fopen);
  RUN_TEST(test_posix_fadvise);
  RUN_TEST(test_posix_fallocate);
  RUN_TEST(test_posix_rename);
  RUN_TEST(test_posix_mkstemp);
}
