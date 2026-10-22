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

#endif

  printf("test_openat 1\n");
  fflush(stdout);
  fd = openat(AT_FDCWD, "test_openat.tmp", O_RDWR | O_CREAT, 0666);
  printf("test_openat 2\n");
  fflush(stdout);

  if (fd >= 0) {
    close(fd);
    printf("test_openat 3\n");
    fflush(stdout);
    remove("test_openat.tmp");
  }
  PASS();
}

TEST test_sync_file_range(void) {
  int fd;

  fd = open("test_sfr.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    sync_file_range(fd, 0, 0, 0);
    close(fd);
    remove("test_sfr.tmp");
  }
  sync_file_range(-1, -1, 0, 0);
  sync_file_range(-1, 0, -1, 0);
  sync_file_range(-1, 0, 0, 1);
  PASS();
}

TEST test_fdatasync(void) {
  int fd;

  fd = open("test_fds.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    fdatasync(fd);
    close(fd);
    remove("test_fds.tmp");
  }
  fdatasync(-1);
  PASS();
}

TEST test_pipe(void) {
  int fds[2];
  char write_buf[16];
  char read_buf[16];
  int written;
  int nread;
  (void)written;
  (void)nread;

#if defined(_MSC_VER)
  strcpy_s(write_buf, sizeof(write_buf), "pipe_test");
#else
  strcpy(write_buf, "pipe_test");
#endif
  memset(read_buf, 0, sizeof(read_buf));

  if (pipe(fds) == 0) {
    written = (int)write(fds[1], write_buf, strlen(write_buf));
    nread = (int)read(fds[0], read_buf, sizeof(read_buf) - 1);
    close(fds[0]);
    close(fds[1]);
  }

  PASS();
}

TEST test_pipe2(void) {
  int fds[2];

  pipe2(NULL, 0);
  pipe2(fds, -1);

#if defined(__APPLE__)
  if (__builtin_available(macOS 27.0, *)) {
    if (pipe2(fds, 0) == 0) {
      close(fds[0]);
      close(fds[1]);
    }
  } else {
    if (pipe2(fds, 0) == 0) {
      close(fds[0]);
      close(fds[1]);
    }
  }
#else
  if (pipe2(fds, 0) == 0) {
    close(fds[0]);
    close(fds[1]);
  }
#endif

  PASS();
}

TEST test_pread(void) {
  int fd;
  char buf[16];
  ssize_t n;
  (void)n;

  fd = open("test_pread_unit.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    n = write(fd, "teststring", 10);
    memset(buf, 0, sizeof(buf));
    n = pread(fd, buf, 6, 4);
    close(fd);
    remove("test_pread_unit.tmp");
  }
  n = pread(-1, buf, 1, 0);
  PASS();
}

TEST test_pwrite(void) {
  int fd;
  ssize_t n;
  (void)n;

  fd = open("test_pwrite_unit.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    n = pwrite(fd, "abcdef", 6, 0);
    close(fd);
    remove("test_pwrite_unit.tmp");
  }
  n = pwrite(-1, "a", 1, 0);
  PASS();
}

TEST test_readlink(void) {
  char buf[64];
  (void)buf;

#if defined(_WIN32) || defined(_MSC_VER)

#endif

  PASS();
}

TEST test_readlinkat(void) {
  char buf[64];
  (void)buf;

#if defined(_WIN32) || defined(_MSC_VER)

#endif
  ASSERT_EQ(-1,
            readlinkat(AT_FDCWD, "nonexistent_link_xyz.tmp", buf, sizeof(buf)));
  PASS();
}

TEST test_sync(void) {
  sync();
  PASS();
}

TEST test_posix_fopen(void) {
  FILE *f;
  f = posix_fopen(NULL, "r");

  printf("errno = %d\n", errno);

  f = posix_fopen("nonexistent_test_posix_fopen.txt", "r");

  posix_fopen(NULL, "r");
  posix_fopen("test", NULL);

  f = posix_fopen("/proc/meminfo", "r");
  if (f) {
    char buf[256];
    if (fgets(buf, sizeof(buf), f)) {
    }
    fclose(f);
  }
  f = posix_fopen("/proc/meminfo", "r");
  if (f) {
    char buf[256];
    if (fgets(buf, sizeof(buf), f)) {
      /* Just reading to see it works */
    }
    fclose(f);
  }
#if defined(_WIN32)
  {
    const char *vpaths[] = {
        "/proc/uptime",     "/proc/loadavg",   "/proc/version",
        "/proc/stat",       "/proc/mounts",    "/proc/filesystems",
        "/proc/swaps",      "/proc/cpuinfo",   "/proc/1/stat",
        "/proc/1/cmdline",  "/proc/1/comm",    "/proc/1/status",
        "/proc/self/stat",  "/proc/self/comm", "/proc/self/cmdline",
        "/proc/self/status"};
    size_t idx;
    for (idx = 0; idx < sizeof(vpaths) / sizeof(vpaths[0]); idx++) {
      f = posix_fopen(vpaths[idx], "r");
      ASSERT(f != NULL);
      fclose(f);
    }
  }
#elif !defined(__linux__) && !defined(__CYGWIN__)
  {
    const char *vpaths[] = {"/proc/uptime",    "/proc/version",
                            "/proc/1/stat",    "/proc/1/cmdline",
                            "/proc/1/comm",    "/proc/1/status",
                            "/proc/self/stat", "/proc/self/cmdline",
                            "/proc/self/comm", "/proc/self/status"};
    size_t idx;
    for (idx = 0; idx < sizeof(vpaths) / sizeof(vpaths[0]); idx++) {
      f = posix_fopen(vpaths[idx], "r");
      ASSERT(f != NULL);
      fclose(f);
    }
  }
#endif
  PASS();
}

TEST test_posix_open(void) {
  int fd;
  printf("test_posix_open 1\n");
  fflush(stdout);
  fd = posix_open(NULL, O_RDONLY);

  printf("errno = %d\n", errno);
  fflush(stdout);

  fd = posix_open("nonexistent_test_posix_open.txt", O_RDONLY);
  printf("test_posix_open 2\n");
  fflush(stdout);

#if defined(_WIN32)
  fd = posix_open("/proc/version", O_RDONLY);
  ASSERT(fd >= 0);
  close(fd);
  fd = open("version", O_RDONLY);
  ASSERT(fd >= 0);
  close(fd);
#elif !defined(__linux__) && !defined(__CYGWIN__)
  fd = posix_open("/proc/uptime", O_RDONLY);
  ASSERT(fd >= 0);
  close(fd);
  fd = posix_open("/proc/self/stat", O_RDONLY);
  ASSERT(fd >= 0);
  close(fd);
  fd = posix_open("/proc/self/cmdline", O_RDONLY);
  ASSERT(fd >= 0);
  close(fd);
#endif

  fd = posix_open("test_posix_open.tmp", O_WRONLY | O_CREAT, 0666);
  printf("test_posix_open 3\n");
  fflush(stdout);

  if (fd >= 0) {
    close(fd);
    printf("test_posix_open 4\n");
    fflush(stdout);
    remove("test_posix_open.tmp");
    printf("test_posix_open 5\n");
    fflush(stdout);
  }

  PASS();
}

TEST test_posix_creat(void) {
  int fd;
  printf("test_posix_creat 1\n");
  fflush(stdout);
  fd = posix_creat("test_posix_creat.tmp", 0666);
  printf("test_posix_creat 2\n");
  fflush(stdout);

  if (fd >= 0) {
    close(fd);
    printf("test_posix_creat 3\n");
    fflush(stdout);
    remove("test_posix_creat.tmp");
  }
  PASS();
}

TEST test_posix_read_write_close(void) {
  int fd;
  char buf[16];
  ssize_t n;
  (void)n;

  posix_close(-1);
  posix_read(-1, buf, 1);
  posix_write(-1, "a", 1);
  ASSERT_EQ(-1, posix_lseek(-1, 0, SEEK_SET));

  fd = posix_open("test_posix_rw.tmp", O_RDWR | O_CREAT, 0666);
  n = posix_write(fd, "hello", 5);
  ASSERT_EQ(0, posix_lseek(fd, 0, SEEK_SET));
  posix_close(fd);

  fd = posix_open("test_posix_rw.tmp", O_RDONLY);
  memset(buf, 0, sizeof(buf));
  n = posix_read(fd, buf, 5);

  posix_close(fd);
  remove("test_posix_rw.tmp");

  PASS();
}

TEST test_posix_dup2(void) {
  int fd1, fd2;

  posix_dup2(-1, 0);
  posix_dup2(0, -1);

  fd1 = posix_open("test_posix_dup2.tmp", O_RDWR | O_CREAT, 0666);
  fd2 = posix_open("test_posix_dup2_2.tmp", O_RDWR | O_CREAT, 0666);

  if (posix_dup2(fd1, fd2) >= 0) {
    /* now fd2 is a copy of fd1 */
  }
  posix_close(fd1);
  posix_close(fd2);
  remove("test_posix_dup2_2.tmp");
  remove("test_posix_dup2.tmp");

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
  RUN_TEST(test_posix_fopen);
  RUN_TEST(test_posix_open);
  RUN_TEST(test_posix_creat);
  RUN_TEST(test_posix_read_write_close);
  RUN_TEST(test_posix_dup2);
}
