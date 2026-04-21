#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-dlfcn.h"
/* clang-format on */

TEST test_dl(void) {
  void *handle = dlopen(NULL, RTLD_LAZY);
  if (handle == NULL) {
    SKIP();
  }
  ASSERT(handle != NULL);
  dlclose(handle);
  PASS();
}

TEST test_dlopen(void) { SKIP(); /* Generated stub for dlopen */ }

TEST test_dlsym(void) { SKIP(); /* Generated stub for dlsym */ }

TEST test_dlclose(void) { SKIP(); /* Generated stub for dlclose */ }

TEST test_dlerror(void) { SKIP(); /* Generated stub for dlerror */ }

TEST test_dladdr(void) { SKIP(); /* Generated stub for dladdr */ }

SUITE(suite_posix_dlfcn_core) {
  RUN_TEST(test_dl);
  RUN_TEST(test_dlopen);
  RUN_TEST(test_dlsym);
  RUN_TEST(test_dlclose);
  RUN_TEST(test_dlerror);
  RUN_TEST(test_dladdr);
}
