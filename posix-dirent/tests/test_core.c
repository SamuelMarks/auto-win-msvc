#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-dirent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

static int filter_all(const struct dirent *entry) {
  (void)entry;
  return 1;
}

TEST test_posix_dirent_get_info(void) {
  enum posix_dirent_error_code rc;
  int info;

  info = 0;
  rc = posix_dirent_get_info(NULL);
  if (rc != POSIX_DIRENT_ERROR_NULL_POINTER) {
    printf("Expected POSIX_DIRENT_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_dirent_get_info(&info);
  if (rc != POSIX_DIRENT_SUCCESS) {
    printf("posix_dirent_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_opendir_and_closedir(void) {
  DIR *d;

  /* Null pointer should fail */
  d = opendir(NULL);
  ASSERT_EQ(NULL, d);

  /* Non-existent directory should fail */
  d = opendir("non_existent_directory_xyz_12345");
  ASSERT_EQ(NULL, d);

#if defined(_WIN32) || defined(_MSC_VER)
  /* Closedir on NULL */
  ASSERT_EQ(-1, closedir(NULL));
#endif

  /* Open current directory */
  d = opendir(".");
  ASSERT(d != NULL);
  ASSERT_EQ(0, closedir(d));

  PASS();
}

TEST test_readdir_and_rewind(void) {
  DIR *d;
  struct dirent *entry;
  int count;

  d = opendir(".");
  ASSERT(d != NULL);

  count = 0;
  while ((entry = readdir(d)) != NULL) {
    ASSERT(entry->d_name[0] != '\0');
    count++;
  }
  ASSERT(count > 0);

  /* Rewind and count again */
  rewinddir(d);
  count = 0;
  while ((entry = readdir(d)) != NULL) {
    count++;
  }
  ASSERT(count > 0);

  /* seekdir and telldir */
  rewinddir(d);
#if defined(_WIN32) || defined(_MSC_VER)
  {
    long loc;
    loc = telldir(d);
    ASSERT(loc >= 0);
    seekdir(d, loc);
  }
  rewinddir(NULL);
  seekdir(NULL, 0);
  ASSERT_EQ(-1, telldir(NULL));
  ASSERT_EQ(NULL, readdir(NULL));
#endif

  closedir(d);
  PASS();
}

TEST test_scandir_and_alphasort(void) {
  struct dirent **namelist;
  int n;
  int i;

  /* Invalid arguments */
  n = scandir(NULL, &namelist, NULL, alphasort);
  ASSERT_EQ(-1, n);

#if defined(_WIN32) || defined(_MSC_VER)
  n = scandir(".", NULL, NULL, alphasort);
  ASSERT_EQ(-1, n);
#endif

  n = scandir("non_existent_dir_998877", &namelist, NULL, alphasort);
  ASSERT_EQ(-1, n);

  /* Valid scandir */
  namelist = NULL;
  n = scandir(".", &namelist, filter_all, alphasort);
  ASSERT(n >= 0);
  ASSERT(namelist != NULL);

  for (i = 0; i < n; i++) {
    ASSERT(namelist[i] != NULL);
    free(namelist[i]);
  }
  free(namelist);

  PASS();
}

SUITE(suite_posix_dirent_core) {
  RUN_TEST(test_posix_dirent_get_info);
  RUN_TEST(test_opendir_and_closedir);
  RUN_TEST(test_readdir_and_rewind);
  RUN_TEST(test_scandir_and_alphasort);
}
