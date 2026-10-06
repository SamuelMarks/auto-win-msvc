/* clang-format off */
#include "greatest.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#define DEFINED_WIN32_FOR_TEST
#define _WIN32 1
#endif
#ifndef _MSC_VER
#define DEFINED_MSC_VER_FOR_TEST
#define _MSC_VER 1
#endif

/* Prevent inclusion of native <io.h> */
#define MOCK_IO_FOR_TEST 1

static int mock_read_calls = 0;
static int mock_write_calls = 0;
static int mock_lseek_calls = 0;

static int mock_read_ret[10] = {0};
static int mock_write_ret[10] = {0};
static long mock_lseek_ret[10] = {0};

/* Provide exactly the signatures expected */
int mock_read(int fd, void *buf, unsigned int count) {
    int ret = mock_read_ret[mock_read_calls++];
    (void)fd; (void)buf; (void)count;
    return ret;
}

int mock_write(int fd, const void *buf, unsigned int count) {
    int ret = mock_write_ret[mock_write_calls++];
    (void)fd; (void)buf; (void)count;
    return ret;
}

long mock_lseek(int fd, long offset, int origin) {
    long ret = mock_lseek_ret[mock_lseek_calls++];
    (void)fd; (void)offset; (void)origin;
    return ret;
}

static void reset_mocks(void) {
    mock_read_calls = 0;
    mock_write_calls = 0;
    mock_lseek_calls = 0;
    memset(mock_read_ret, 0, sizeof(mock_read_ret));
    memset(mock_write_ret, 0, sizeof(mock_write_ret));
    memset(mock_lseek_ret, 0, sizeof(mock_lseek_ret));
}

#define _read mock_read
#define _write mock_write
#define _lseek mock_lseek

#include "../src/sys_sendfile.c"

#ifdef DEFINED_WIN32_FOR_TEST
#undef _WIN32
#endif
#ifdef DEFINED_MSC_VER_FOR_TEST
#undef _MSC_VER
#endif
/* clang-format on */

TEST test_sendfile_basic(void) {
  off_t off = 0;
  (void)off;

  reset_mocks();
  ASSERT_EQ(-1, sendfile(-1, 0, NULL, 10));
  ASSERT_EQ(EBADF, errno);

  reset_mocks();
  ASSERT_EQ(0, sendfile(1, 0, NULL, 0));

  reset_mocks();
  off = -1;
  ASSERT_EQ(-1, sendfile(1, 0, &off, 10));
  ASSERT_EQ(EINVAL, errno);

  reset_mocks();
  off = 0;
  mock_lseek_ret[0] = 0;
  mock_lseek_ret[1] = -1;
  ASSERT_EQ(-1, sendfile(1, 0, &off, 10));

  reset_mocks();
  off = 0;
  mock_lseek_ret[0] = 0;
  mock_lseek_ret[1] = 0;
  mock_lseek_ret[2] = 0;
  mock_read_ret[0] = 10;
  mock_write_ret[0] = 10;
  ASSERT_EQ(10, sendfile(1, 0, &off, 10));
  ASSERT_EQ(10, off);

  reset_mocks();
  mock_read_ret[0] = 8192;
  mock_read_ret[1] = 1808;
  mock_write_ret[0] = 8192;
  mock_write_ret[1] = 1808;
  ASSERT_EQ(10000, sendfile(1, 0, NULL, 10000));

  reset_mocks();
  off = 0;
  mock_read_ret[0] = -1;
  ASSERT_EQ(-1, sendfile(1, 0, &off, 10));

  reset_mocks();
  off = 0;
  mock_read_ret[0] = 0;
  ASSERT_EQ(0, sendfile(1, 0, &off, 10));

  reset_mocks();
  mock_read_ret[0] = 5;
  mock_read_ret[1] = -1;
  mock_write_ret[0] = 5;
  ASSERT_EQ(5, sendfile(1, 0, NULL, 10));

  reset_mocks();
  mock_read_ret[0] = 10;
  mock_write_ret[0] = -1;
  ASSERT_EQ(-1, sendfile(1, 0, NULL, 10));

  reset_mocks();
  off = 0;
  mock_read_ret[0] = 10;
  mock_write_ret[0] = -1;
  ASSERT_EQ(-1, sendfile(1, 0, &off, 10));

  reset_mocks();
  mock_read_ret[0] = 10;
  mock_read_ret[1] = 10;
  mock_write_ret[0] = 10;
  mock_write_ret[1] = -1;
  ASSERT_EQ(10, sendfile(1, 0, NULL, 20));

  reset_mocks();
  mock_read_ret[0] = 10;
  mock_write_ret[0] = 5;
  ASSERT_EQ(5, sendfile(1, 0, NULL, 10));

  PASS();
}

TEST test_sendfile_branches(void) {
  off_t off = 0;
  (void)off;

  reset_mocks();
  /* Hit in_fd < 0 after out_fd >= 0 */
  ASSERT_EQ(-1, sendfile(1, -1, NULL, 10));
  ASSERT_EQ(EBADF, errno);

  reset_mocks();
  /* Hit restore_pos == 0 in read fail when total == 0 */
  off = 0;
  mock_read_ret[0] = -1;
  ASSERT_EQ(-1, sendfile(1, 0, NULL, 10));

  PASS();
}

SUITE(suite_sys_sendfile) {
  RUN_TEST(test_sendfile_basic);
  RUN_TEST(test_sendfile_branches);
}
