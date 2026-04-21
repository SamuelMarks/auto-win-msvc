#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-dirent.h"

TEST test_dirent(void) {
  DIR *d = opendir(".");
  if (d) {
    closedir(d);
    PASS();
  }
  SKIP();
}

TEST test_opendir(void) { SKIP(); /* Generated stub for opendir */ }

TEST test_readdir(void) { SKIP(); /* Generated stub for readdir */ }

TEST test_closedir(void) { SKIP(); /* Generated stub for closedir */ }

TEST test_rewinddir(void) { SKIP(); /* Generated stub for rewinddir */ }

TEST test_seekdir(void) { SKIP(); /* Generated stub for seekdir */ }

TEST test_telldir(void) { SKIP(); /* Generated stub for telldir */ }

TEST test_scandir(void) { SKIP(); /* Generated stub for scandir */ }

TEST test_alphasort(void) { SKIP(); /* Generated stub for alphasort */ }

SUITE(suite_posix_dirent_core) {
  RUN_TEST(test_dirent);
  RUN_TEST(test_opendir);
  RUN_TEST(test_readdir);
  RUN_TEST(test_closedir);
  RUN_TEST(test_rewinddir);
  RUN_TEST(test_seekdir);
  RUN_TEST(test_telldir);
  RUN_TEST(test_scandir);
  RUN_TEST(test_alphasort);
}
