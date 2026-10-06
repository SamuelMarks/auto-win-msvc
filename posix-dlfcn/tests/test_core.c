#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-dlfcn.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_dlfcn_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = posix_dlfcn_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_dlfcn_get_info(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_dlopen_dlclose(void) {
  void *handle;
  int rc;

  /* Unknown library */
  handle = dlopen("non_existent_library_999.dll", RTLD_NOW);
  ASSERT_EQ(NULL, handle);
  ASSERT(dlerror() != NULL);

#if defined(_WIN32) || defined(_MSC_VER)
  handle = dlopen(NULL, RTLD_LAZY); /* Should get self */
  ASSERT(handle != NULL);
  ASSERT_EQ(0, dlclose(handle));

  /* dlclose invalid */
  rc = dlclose(NULL);
  (void)rc;
  /* ASSERT(rc != 0); */
  /* The documentation for FreeLibrary doesn't guarantee GetLastError() will be
     set for invalid handles in all cases, especially NULL, so don't assert
     dlerror() != NULL here */
#endif

  (void)rc;
  PASS();
}

TEST test_dlsym(void) {
#if defined(_WIN32) || defined(_MSC_VER)
  void *handle;
  void *sym;

  handle = dlopen("kernel32.dll", RTLD_NOW);
  ASSERT(handle != NULL);

  sym = dlsym(handle, "GetProcAddress");
  ASSERT(sym != NULL);

  sym = dlsym(handle, "NonExistentFunctionXYZ");
  ASSERT_EQ(NULL, sym);
  ASSERT(dlerror() != NULL);

  dlclose(handle);

  /* dlsym with invalid handle */
  sym = dlsym(NULL, "GetProcAddress");
  ASSERT_EQ(NULL, sym);
  ASSERT(dlerror() != NULL);

  /* Null symbol */
  handle = dlopen("kernel32.dll", RTLD_NOW);
  sym = dlsym(handle, NULL);
  ASSERT_EQ(NULL, sym);
  dlclose(handle);
  PASS();
#else
  SKIP();
#endif
}

SUITE(suite_posix_dlfcn_core) {
  RUN_TEST(test_posix_dlfcn_init);
  RUN_TEST(test_dlopen_dlclose);
  RUN_TEST(test_dlsym);
}
