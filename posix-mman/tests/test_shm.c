#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#else
#include <unistd.h>
#if !defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)
extern int ftruncate(int fd, off_t length);
#endif
#endif
/* clang-format on */

TEST test_shm_open(void) {
  int fd;
  char buf[16];
  void *ptr;

#if defined(_WIN32) || defined(_MSC_VER)
  /* Null arguments should fail */
  fd = shm_open(NULL, 0, 0);
  ASSERT_EQ(-1, fd);
#endif

  /* Open and create */
  fd = shm_open("/test_auto_win_msvc_shm", O_CREAT | O_RDWR, 0666);
  ASSERT(fd >= 0);

#if !defined(_WIN32)
  ftruncate(fd, 4096);
#else
  _chsize(fd, 4096);
#endif

  ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);
  memcpy(ptr, "hello", 5);
  munmap(ptr, 4096);

#if defined(_WIN32)
  _close(fd);
#else
  close(fd);
#endif

  /* Reopen and read */
  fd = shm_open("/test_auto_win_msvc_shm", O_RDWR, 0);
  ASSERT(fd >= 0);

  ptr = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);
  memcpy(buf, ptr, 5);
  buf[5] = '\0';
  ASSERT_STR_EQ("hello", buf);
  munmap(ptr, 4096);

#if defined(_WIN32)
  _close(fd);
#else
  close(fd);
#endif

#if defined(_MSC_VER)
  PASS();
#else
  PASS();
  return GREATEST_TEST_RES_PASS;
#endif
}

TEST test_shm_unlink(void) {
  int rc;

#if defined(_WIN32) || defined(_MSC_VER)
  /* Null argument should fail */
  rc = shm_unlink(NULL);
  ASSERT_EQ(-1, rc);
#endif

  /* Non-existent shared memory object */
  rc = shm_unlink("non_existent_shm_obj_auto_win_msvc_12345");
  ASSERT_EQ(-1, rc);

  /* Unlink existing object created in test_shm_open */
  rc = shm_unlink("/test_auto_win_msvc_shm");
  ASSERT_EQ(0, rc);

  PASS();
}

SUITE(suite_posix_mman_shm) {
  RUN_TEST(test_shm_open);
  RUN_TEST(test_shm_unlink);
}
