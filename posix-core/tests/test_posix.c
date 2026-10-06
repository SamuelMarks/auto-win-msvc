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
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = posix_core_init(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_core_init(&status);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_core_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  PASS();
}

TEST test_posix_close(void) {
  int fd;
  (void)fd;

  fd = open("test_pclose.tmp", O_RDWR | O_CREAT, 0666);

  remove("test_pclose.tmp");
  PASS();
}

TEST test_posix_read(void) {
  int fd;
  char buf[16];
  ssize_t bytes_read;
  (void)fd;
  (void)bytes_read;

  fd = open("test_pread_t.tmp", O_RDWR | O_CREAT, 0666);

  posix_write(fd, "testdata", 8);
  posix_close(fd);

  fd = open("test_pread_t.tmp", O_RDONLY, 0);

  memset(buf, 0, sizeof(buf));
  bytes_read = posix_read(fd, buf, 8);

  posix_close(fd);

  remove("test_pread_t.tmp");
  PASS();
}

TEST test_posix_write(void) {
  int fd;
  ssize_t written;
  (void)fd;
  (void)written;

  fd = open("test_pwrite_t.tmp", O_RDWR | O_CREAT, 0666);

  written = posix_write(fd, "write_check", 11);

  posix_close(fd);

  remove("test_pwrite_t.tmp");
  PASS();
}

TEST test_posix_fopen(void) {
  FILE *f;

  f = posix_fopen("test_pfopen.tmp", "w+");

  fputs("abc", f);
  fclose(f);

  remove("test_pfopen.tmp");
  PASS();
}

TEST test_posix_fadvise(void) {
  int fd;
  (void)fd;

  fd = open("test_pfadv.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    posix_write(fd, "1234567890", 10);
    posix_fadvise(fd, 0, 10, 1);
    posix_close(fd);
    remove("test_pfadv.tmp");
  }

  posix_fadvise(-1, 0, 10, 1);
  posix_fadvise(0, -1, 10, 1);
  posix_fadvise(0, 0, -1, 1);
  posix_fadvise(0, 0, 10, -1);

  PASS();
}

TEST test_posix_fallocate(void) {
  int fd;
  (void)fd;

  fd = open("test_pfalloc.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    posix_fallocate(fd, 0, 100);
    posix_close(fd);
    remove("test_pfalloc.tmp");
  }

  posix_fallocate(-1, 0, 100);
  posix_fallocate(0, -1, 100);
  posix_fallocate(0, 0, -1);
  posix_fallocate(0, 0, 0);

  PASS();
}

TEST test_posix_rename(void) {
  FILE *f;
  int rc;

  f = fopen("test_pren1.tmp", "w");
  if (f) {
    fputs("data", f);
    fclose(f);
  }

  rc = posix_rename("test_pren1.tmp", "test_pren2.tmp");
  ASSERT_EQ(0, rc);

  remove("test_pren1.tmp");
  remove("test_pren2.tmp");
  PASS();
}

TEST test_posix_mkstemp(void) {
  char tmpl[32];
  int fd;
  (void)fd;

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
