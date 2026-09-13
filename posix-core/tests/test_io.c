#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#if defined(_MSC_VER) || defined(_WIN32)
#include <process.h> /* For _getpid if needed */
#endif
/* clang-format on */

TEST test_openat(void) { SKIP(); /* Generated stub for openat */ }

TEST test_sync_file_range(void) {
  SKIP(); /* Generated stub for sync_file_range */
}

TEST test_fdatasync(void) { SKIP(); /* Generated stub for fdatasync */ }

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

#if defined(_WIN32)
  written = _write(fds[1], write_buf, (unsigned int)strlen(write_buf));
#else
  written = write(fds[1], write_buf, strlen(write_buf));
#endif
  ASSERT_EQ((int)strlen(write_buf), written);

#if defined(_WIN32)
  nread = _read(fds[0], read_buf, (unsigned int)sizeof(read_buf) - 1);
#else
  nread = read(fds[0], read_buf, sizeof(read_buf) - 1);
#endif
  ASSERT_EQ(written, nread);
  ASSERT_STR_EQ("pipe_test", read_buf);

#if defined(_WIN32)
  _close(fds[0]);
  _close(fds[1]);
#else
  close(fds[0]);
  close(fds[1]);
#endif

  PASS();
}

TEST test_pipe2(void) { SKIP(); /* Generated stub for pipe2 */ }

TEST test_pread(void) { SKIP(); /* Generated stub for pread */ }

TEST test_pwrite(void) { SKIP(); /* Generated stub for pwrite */ }

TEST test_readlink(void) { SKIP(); /* Generated stub for readlink */ }

TEST test_readlinkat(void) { SKIP(); /* Generated stub for readlinkat */ }

TEST test_sync(void) { SKIP(); /* Generated stub for sync */ }

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
