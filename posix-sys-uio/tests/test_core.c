#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sys-uio.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#if defined(_MSC_VER) || defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <share.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_posix_sys_uio_get_info(void) {
  enum posix_sys_uio_error_code rc;
  int info;

  info = 0;
  rc = posix_sys_uio_get_info(NULL);
  if (rc != POSIX_SYS_UIO_ERROR_NULL_POINTER) {
    printf("Expected POSIX_SYS_UIO_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_sys_uio_get_info(&info);
  if (rc != POSIX_SYS_UIO_SUCCESS) {
    printf("posix_sys_uio_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_uio_readv_writev_file(void) {
  int fd;
  long res;
  struct iovec iov[2];
  char buf1[8];
  char buf2[8];
  char rbuf1[8];
  char rbuf2[8];
  const char *test_filename = "test_uio_tmp.bin";

#if defined(_MSC_VER)
  if (_sopen_s(&fd, test_filename, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY,
               _SH_DENYNO, _S_IREAD | _S_IWRITE) != 0) {
    fd = -1;
  }
#elif defined(_WIN32)
  fd = _open(test_filename, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, 0666);
#else
  fd = open(test_filename, O_CREAT | O_TRUNC | O_RDWR, 0666);
#endif
  ASSERT(fd >= 0);

  /* Test invalid iovcnt */
  res = posix_writev(fd, iov, -1);
  ASSERT_EQ(-1, res);
  res = posix_writev(fd, iov, 1025);
  ASSERT_EQ(-1, res);
  res = posix_readv(fd, iov, -1);
  ASSERT_EQ(-1, res);
  res = posix_readv(fd, iov, 1025);
  ASSERT_EQ(-1, res);

  /* Test valid writev */
  memcpy(buf1, "HELLO_", 6);
  memcpy(buf2, "WORLD!", 6);
  iov[0].iov_base = buf1;
  iov[0].iov_len = 6;
  iov[1].iov_base = buf2;
  iov[1].iov_len = 6;

  res = posix_writev(fd, iov, 2);
  ASSERT_EQ(12, res);

  /* Seek back to 0 */
#if defined(_MSC_VER) || defined(_WIN32)
  _lseek(fd, 0, 0 /* SEEK_SET */);
#else
  lseek(fd, 0, SEEK_SET);
#endif

  /* Test valid readv */
  memset(rbuf1, 0, sizeof(rbuf1));
  memset(rbuf2, 0, sizeof(rbuf2));
  iov[0].iov_base = rbuf1;
  iov[0].iov_len = 6;
  iov[1].iov_base = rbuf2;
  iov[1].iov_len = 6;

  res = posix_readv(fd, iov, 2);
  ASSERT_EQ(12, res);
  ASSERT_MEM_EQ("HELLO_", rbuf1, 6);
  ASSERT_MEM_EQ("WORLD!", rbuf2, 6);

  /* Test preadv and pwritev */
  memcpy(buf1, "1234", 4);
  iov[0].iov_base = buf1;
  iov[0].iov_len = 4;
  res = posix_pwritev(fd, iov, 1, 0);
  ASSERT_EQ(4, res);

  memset(rbuf1, 0, sizeof(rbuf1));
  iov[0].iov_base = rbuf1;
  iov[0].iov_len = 4;
  res = posix_preadv(fd, iov, 1, 0);
  ASSERT_EQ(4, res);
  ASSERT_MEM_EQ("1234", rbuf1, 4);

#if defined(_MSC_VER) || defined(_WIN32)
  _close(fd);
  _unlink(test_filename);
#else
  close(fd);
  unlink(test_filename);
#endif

  PASS();
}

SUITE(suite_posix_sys_uio_core) {
  RUN_TEST(test_posix_sys_uio_get_info);
  RUN_TEST(test_posix_uio_readv_writev_file);
}
