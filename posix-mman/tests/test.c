/* test.c - 100% Test Coverage */
/* clang-format off */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#define OPEN _open
#define CLOSE _close
#ifndef O_RDWR
#define O_RDWR _O_RDWR
#endif
#ifndef O_CREAT
#define O_CREAT _O_CREAT
#endif
#else
#include <unistd.h>
#define OPEN open
#define CLOSE close
#endif

#include "greatest.h"
#include "posix-mman.h"
/* clang-format on */

/* C89 safe printf format for size_t */
#define NUM_FORMAT "%lu"

/* Helper for creating a temp file */
static void create_temp_file(const char *path, const char *content) {
  FILE *f = NULL;
#if defined(_MSC_VER)
  fopen_s(&f, path, "wb");
#else
  f = fopen(path, "wb");
#endif
  if (f) {
    if (content) {
      fputs(content, f);
    }
    fclose(f);
  }
}

TEST test_mmap_anon(void) {
  void *ptr;
  int res;

  ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
             -1, 0);
  ASSERT(ptr != MAP_FAILED);
  ASSERT(ptr != NULL);

  /* Write something */
#if defined(_MSC_VER)
  strcpy_s((char *)ptr, 4096, "hello");
#else
  strcpy((char *)ptr, "hello");
#endif
  ASSERT_EQ(0, strcmp((char *)ptr, "hello"));

  /* mprotect to read only */
  res = mprotect(ptr, 4096, PROT_READ);
  ASSERT_EQ(0, res);

  printf("Successfully mapped and protected " NUM_FORMAT " bytes.\n",
         (unsigned long)4096);

  /* msync */
  res = msync(ptr, 4096, MS_SYNC);
  ASSERT_EQ(0, res);

  /* mlock / munlock */
  mlock(
      ptr,
      4096); /* May fail depending on permissions/limits, we just exercise it */
  munlock(ptr, 4096);

  /* munmap */
  res = munmap(ptr, 4096);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_madvise(void) {
  void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT(ptr != MAP_FAILED);
  ASSERT_EQ(0, madvise(ptr, 4096, 3)); /* MADV_WILLNEED */
  ASSERT_EQ(0, madvise(ptr, 4096, 4)); /* MADV_DONTNEED */
#ifndef __CYGWIN__
  ASSERT_EQ(0, madvise(ptr, 4096, 8)); /* MADV_FREE */
#endif
  ASSERT_EQ(0, madvise(ptr, 4096, 0)); /* Other */
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, madvise(NULL, 4096, 3));
  ASSERT_EQ(EINVAL, errno);
  ASSERT_EQ(-1, madvise(ptr, 0, 3));
  ASSERT_EQ(EINVAL, errno);
#endif
  munmap(ptr, 4096);
  PASS();
}

TEST test_mlockall_munlockall(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(0, mlockall(1 | 2)); /* MCL_CURRENT | MCL_FUTURE */
  ASSERT_EQ(-1, mlockall(0xFF)); /* Invalid */
  ASSERT_EQ(EINVAL, errno);
  ASSERT_EQ(0, munlockall());
#endif
  PASS();
}

TEST test_mmap_file(void) {

  const char *tmp = "test_mmap_file.txt";
  int fd;
  void *ptr;
  int res;

  create_temp_file(tmp, "mmap_file_content123"); /* at least 16 bytes */

  fd = OPEN(tmp, O_RDWR);
  if (fd < 0) {
    SKIP();
  }

  ptr = mmap(NULL, 16, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    CLOSE(fd);
    remove(tmp);
    FAILm("mmap failed");
  }

  ASSERT_EQ('m', ((char *)ptr)[0]);

  /* Modify map */
  ((char *)ptr)[0] = 'z';

  res = msync(ptr, 16, MS_SYNC);
  ASSERT_EQ(0, res);

  res = munmap(ptr, 16);
  ASSERT_EQ(0, res);

  CLOSE(fd);
  remove(tmp);

  PASS();
}

TEST test_shm(void) {
  const char *name = "/my_test_shm";
  int fd;
  int res;

  fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  if (fd < 0) {
    SKIP();
  }

  CLOSE(fd);

  res = shm_unlink(name);
  ASSERT_EQ(0, res);

  PASS();
}

SUITE(mman_suite) {

  RUN_TEST(test_mmap_anon);
  RUN_TEST(test_madvise);
  RUN_TEST(test_mlockall_munlockall);

  RUN_TEST(test_mmap_file);
  RUN_TEST(test_shm);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(mman_suite);
  GREATEST_MAIN_END();
}
