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

TEST test_openat(void) {
  int fd;

#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(-1, openat(AT_FDCWD, NULL, O_RDONLY));
#endif
  ASSERT_EQ(-1, openat(AT_FDCWD, "nonexistent_openat_xyz.tmp", O_RDONLY));

  fd = openat(AT_FDCWD, "test_openat.tmp", O_RDWR | O_CREAT, 0666);
  ASSERT(fd >= 0);
  close(fd);
  remove("test_openat.tmp");
  PASS();
}

TEST test_sync_file_range(void) {
  int fd;

  ASSERT_EQ(-1, sync_file_range(-1, 0, 0, 0));

  fd = open("test_sfr.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    (void)sync_file_range(fd, 0, 0, 0);
    close(fd);
    remove("test_sfr.tmp");
  }
  PASS();
}

TEST test_fdatasync(void) {
  int fd;

  ASSERT_EQ(-1, fdatasync(-1));

  fd = open("test_fds.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    ASSERT_EQ(0, fdatasync(fd));
    close(fd);
    remove("test_fds.tmp");
  }
  PASS();
}

TEST test_pipe(void) {
  int fds[2];
  char write_buf[16];
  char read_buf[16];
  int written;
  int nread;

#if defined(_MSC_VER)
  strcpy_s(write_buf, sizeof(write_buf), "pipe_test");
#else
  strcpy(write_buf, "pipe_test");
#endif
  memset(read_buf, 0, sizeof(read_buf));

  ASSERT_EQ(0, pipe(fds));
  ASSERT(fds[0] >= 0);
  ASSERT(fds[1] >= 0);

  written = (int)write(fds[1], write_buf, strlen(write_buf));
  ASSERT_EQ((int)strlen(write_buf), written);

  nread = (int)read(fds[0], read_buf, sizeof(read_buf) - 1);
  ASSERT_EQ(written, nread);
  ASSERT_STR_EQ("pipe_test", read_buf);

  close(fds[0]);
  close(fds[1]);

  PASS();
}

TEST test_pipe2(void) {
  int fds[2];

#if defined(__APPLE__)
  if (__builtin_available(macOS 27.0, *)) {
    ASSERT_EQ(-1, pipe2(NULL, 0));

    ASSERT_EQ(0, pipe2(fds, 0));
    ASSERT(fds[0] >= 0);
    ASSERT(fds[1] >= 0);

    close(fds[0]);
    close(fds[1]);
  }
#else
  ASSERT_EQ(-1, pipe2(NULL, 0));

  ASSERT_EQ(0, pipe2(fds, 0));
  ASSERT(fds[0] >= 0);
  ASSERT(fds[1] >= 0);

  close(fds[0]);
  close(fds[1]);
#endif

  PASS();
}

TEST test_pread(void) {
  int fd;
  char buf[16];
  ssize_t n;

  ASSERT_EQ(-1, pread(-1, NULL, 0, 0));

  fd = open("test_pread_unit.tmp", O_RDWR | O_CREAT, 0666);
  ASSERT(fd >= 0);
  write(fd, "teststring", 10);

  memset(buf, 0, sizeof(buf));
  n = pread(fd, buf, 6, 4);
  if (n >= 0) {
    ASSERT_STR_EQ("string", buf);
  }
  close(fd);
  remove("test_pread_unit.tmp");
  PASS();
}

TEST test_pwrite(void) {
  int fd;
  ssize_t n;

  ASSERT_EQ(-1, pwrite(-1, NULL, 0, 0));

  fd = open("test_pwrite_unit.tmp", O_RDWR | O_CREAT, 0666);
  ASSERT(fd >= 0);
  n = pwrite(fd, "abcdef", 6, 0);
  if (n >= 0) {
    ASSERT_EQ(6, n);
  }
  close(fd);
  remove("test_pwrite_unit.tmp");
  PASS();
}

TEST test_readlink(void) {
  char buf[64];

#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(-1, readlink(NULL, buf, sizeof(buf)));
#endif
  ASSERT_EQ(-1, readlink("nonexistent_link_xyz.tmp", buf, sizeof(buf)));
  PASS();
}

TEST test_readlinkat(void) {
  char buf[64];

#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(-1, readlinkat(AT_FDCWD, NULL, buf, sizeof(buf)));
#endif
  ASSERT_EQ(-1,
            readlinkat(AT_FDCWD, "nonexistent_link_xyz.tmp", buf, sizeof(buf)));
  PASS();
}

TEST test_sync(void) {
  sync();
  PASS();
}

SUITE(suite_posix_core_io) {
  RUN_TEST(test_openat);
  RUN_TEST(test_sync_file_range);
  RUN_TEST(test_fdatasync);
  RUN_TEST(test_pipe);
  RUN_TEST(test_pipe2);
  RUN_TEST(test_pread);
  RUN_TEST(test_pwrite);
  RUN_TEST(test_readlink);
  RUN_TEST(test_readlinkat);
  RUN_TEST(test_sync);
}
