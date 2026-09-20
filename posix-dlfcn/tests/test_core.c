#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-dlfcn.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_dlfcn_get_info(void) {
  enum posix_dlfcn_error_code rc;
  int info;

  info = 0;
  rc = posix_dlfcn_get_info(NULL);
  if (rc != POSIX_DLFCN_ERROR_NULL_POINTER) {
    printf("Expected POSIX_DLFCN_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_dlfcn_get_info(&info);
  if (rc != POSIX_DLFCN_SUCCESS) {
    printf("posix_dlfcn_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_dlopen_null_and_close(void) {
  void *handle;
  int rc;

  handle = dlopen(NULL, RTLD_LAZY);
  ASSERT(handle != NULL);

  rc = dlclose(handle);
  ASSERT_EQ(0, rc);

#if defined(_WIN32)
  rc = dlclose(NULL);
  ASSERT_EQ(0, rc);
#endif

  PASS();
}

TEST test_dlopen_nonexistent(void) {
  void *handle;
  char *err;
  char *err2;

  handle = dlopen("definitely_nonexistent_lib_987654321.so", RTLD_NOW);
  ASSERT(handle == NULL);

  err = dlerror();
  ASSERT(err != NULL);
  ASSERT(strlen(err) > 0);

  err2 = dlerror();
  ASSERT(err2 == NULL);

  PASS();
}

TEST test_dlopen_transform_paths(void) {
  void *h1;
  void *h2;
  char *err;

  h1 = dlopen("libvalkeylua.so", RTLD_LAZY);
  if (h1 != NULL) {
    dlclose(h1);
  }
  err = dlerror();
  (void)err;

  h2 = dlopen("some/sub/dir/test.dylib", RTLD_LAZY);
  if (h2 != NULL) {
    dlclose(h2);
  }
  err = dlerror();
  (void)err;

  PASS();
}

TEST test_dlsym(void) {
  void *handle;
  void *sym;
  char *err;

  handle = dlopen(NULL, RTLD_LAZY);
  ASSERT(handle != NULL);

#if defined(_WIN32)
  sym = dlsym(handle, NULL);
  ASSERT(sym == NULL);
  err = dlerror();
  (void)err;
#endif

  sym = dlsym(RTLD_DEFAULT, "test_posix_dlfcn_get_info");
  (void)sym;

  sym = dlsym(RTLD_NEXT, "test_posix_dlfcn_get_info");
  (void)sym;

  sym = dlsym(handle, "definitely_nonexistent_symbol_12345");
  ASSERT(sym == NULL);
  err = dlerror();
  (void)err;

  dlclose(handle);
  PASS();
}

static const int s_dummy_obj = 42;

TEST test_dladdr(void) {
  Dl_info info;
  int rc;

  rc = dladdr(NULL, &info);
  ASSERT_EQ(0, rc);

#if defined(_WIN32)
  rc = dladdr((const void *)&s_dummy_obj, NULL);
  ASSERT_EQ(0, rc);
#endif

  rc = dladdr((const void *)&s_dummy_obj, &info);
  ASSERT_NEQ(0, rc);
  ASSERT(info.dli_fbase != NULL);
  ASSERT(info.dli_fname != NULL);

  PASS();
}

SUITE(suite_posix_dlfcn_core) {
  RUN_TEST(test_posix_dlfcn_get_info);
  RUN_TEST(test_dlopen_null_and_close);
  RUN_TEST(test_dlopen_nonexistent);
  RUN_TEST(test_dlopen_transform_paths);
  RUN_TEST(test_dlsym);
  RUN_TEST(test_dladdr);
}
