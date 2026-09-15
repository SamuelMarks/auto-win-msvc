#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#if defined(_MSC_VER)
#include <share.h>
#endif
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_mmap(void) {
  void *p1;
  void *p2;
  void *p3;
  void *p4;
  void *p5;
  void *p6;
  void *p7;
  void *pf;
  int fd;
  int rc;
  const char *tmp_filename = "test_mmap_tmp_file.dat";

  /* Error paths */
  p1 = mmap(NULL, 4096, PROT_READ, MAP_SHARED, -1, 0);
  ASSERT_EQ(MAP_FAILED, p1);
  ASSERT_EQ(EBADF, errno);

  p1 = mmap(NULL, (size_t)-1, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_EQ(MAP_FAILED, p1);

  /* Anonymous mapping with various protections */
  p1 = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, p1);
  rc = munmap(p1, 4096);
  ASSERT_EQ(0, rc);

  p2 = mmap(NULL, 4096, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, p2);
  rc = munmap(p2, 4096);
  ASSERT_EQ(0, rc);

  p3 = mmap(NULL, 4096, PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, p3);
  rc = munmap(p3, 4096);
  ASSERT_EQ(0, rc);

  p4 = mmap(NULL, 4096, PROT_EXEC | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1,
            0);
  ASSERT_NEQ(MAP_FAILED, p4);
  rc = munmap(p4, 4096);
  ASSERT_EQ(0, rc);

  p5 = mmap(NULL, 4096, PROT_EXEC | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,
            0);
#if defined(_WIN32)
  ASSERT_NEQ(MAP_FAILED, p5);
  rc = munmap(p5, 4096);
  ASSERT_EQ(0, rc);
#else
  if (p5 != MAP_FAILED) {
    munmap(p5, 4096);
  }
#endif

  p6 = mmap(NULL, 4096, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, p6);
  rc = munmap(p6, 4096);
  ASSERT_EQ(0, rc);

  p7 = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,
            0);
  ASSERT_NEQ(MAP_FAILED, p7);
  rc = munmap(p7, 4096);
  ASSERT_EQ(0, rc);

  /* File-backed mapping */
#if defined(_MSC_VER)
  _sopen_s(&fd, tmp_filename, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY,
           _SH_DENYNO, _S_IREAD | _S_IWRITE);
  ASSERT(fd >= 0);
  rc = _write(fd, "1234567890abcdef", 16);
  ASSERT_EQ(16, rc);
#elif defined(_WIN32)
  fd = _open(tmp_filename, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY,
             _S_IREAD | _S_IWRITE);
  ASSERT(fd >= 0);
  rc = _write(fd, "1234567890abcdef", 16);
  ASSERT_EQ(16, rc);
#else
  fd = open(tmp_filename, O_CREAT | O_TRUNC | O_RDWR, 0666);
  ASSERT(fd >= 0);
  rc = (int)write(fd, "1234567890abcdef", 16);
  ASSERT_EQ(16, rc);
#endif

  pf = mmap(NULL, 16, PROT_READ, MAP_SHARED, fd, 0);
  ASSERT_NEQ(MAP_FAILED, pf);
  ASSERT_EQ('1', ((char *)pf)[0]);
  munmap(pf, 16);

  pf = mmap(NULL, 16, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  ASSERT_NEQ(MAP_FAILED, pf);
  ((char *)pf)[0] = 'Z';
  munmap(pf, 16);

  pf = mmap(NULL, 16, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
  if (pf != MAP_FAILED) {
    munmap(pf, 16);
  }

#if defined(_WIN32)
  _close(fd);
  _unlink(tmp_filename);
#else
  close(fd);
  unlink(tmp_filename);
#endif

  PASS();
}

SUITE(suite_posix_mman_mmap) { RUN_TEST(test_mmap); }
