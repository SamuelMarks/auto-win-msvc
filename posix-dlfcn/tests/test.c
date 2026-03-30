#include <stdio.h>
#include <string.h>

#include "greatest.h"
#include "posix-dlfcn.h"

/* Helper to clear errors */
static void clear_dlerror(void) { dlerror(); }

TEST test_dlopen_null(void) {
  void *handle;

  clear_dlerror();
  handle = dlopen(NULL, RTLD_LAZY);
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  ASSERT(handle != NULL);
  ASSERT(dlerror() == NULL);
  dlclose(handle);
#else
  ASSERT(handle == NULL);
#endif
  PASS();
}

TEST test_dlopen_valid(void) {
  void *handle;

  clear_dlerror();
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  handle = dlopen("kernel32.dll", RTLD_LAZY);
  ASSERT(handle != NULL);
  ASSERT(dlerror() == NULL);
  dlclose(handle);
#else
  handle = dlopen("libm.so", RTLD_LAZY);
  ASSERT(handle == NULL);
#endif
  PASS();
}

TEST test_dlopen_invalid(void) {
  void *handle;
  char *err;

  clear_dlerror();
  handle = dlopen("nonexistent_library_name_for_test.dll", RTLD_LAZY);
  ASSERT(handle == NULL);

  err = dlerror();
  ASSERT(err != NULL);

  /* Second call should return NULL */
  ASSERT(dlerror() == NULL);

  PASS();
}

TEST test_dlsym_valid(void) {
  void *handle;
  void *sym;

  clear_dlerror();
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  handle = dlopen("kernel32.dll", RTLD_LAZY);
  ASSERT(handle != NULL);

  sym = dlsym(handle, "LoadLibraryA");
  ASSERT(sym != NULL);
  ASSERT(dlerror() == NULL);

  dlclose(handle);
#else
  handle = dlopen("libm.so", RTLD_LAZY);
  ASSERT(handle == NULL);
  sym = dlsym(handle, "cos");
  ASSERT(sym == NULL);
#endif
  PASS();
}

TEST test_dlsym_invalid(void) {
  void *handle;
  void *sym;

  clear_dlerror();
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  handle = dlopen("kernel32.dll", RTLD_LAZY);
  ASSERT(handle != NULL);

  sym = dlsym(handle, "NonExistentFunctionName");
  ASSERT(sym == NULL);
  ASSERT(dlerror() != NULL);

  dlclose(handle);
#else
  handle = NULL;
  sym = dlsym(handle, "NonExistentFunctionName");
  ASSERT(sym == NULL);
#endif
  PASS();
}

TEST test_dlsym_invalid_params(void) {
  void *handle;
  void *sym;

  clear_dlerror();
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  handle = dlopen("kernel32.dll", RTLD_LAZY);
  ASSERT(handle != NULL);

  sym = dlsym(handle, NULL);
  ASSERT(sym == NULL);
  ASSERT(dlerror() != NULL);

  dlclose(handle);
#else
  handle = NULL;
  sym = dlsym(handle, NULL);
  ASSERT(sym == NULL);
#endif
  PASS();
}

TEST test_dlsym_special_handles(void) {
  void *sym;

  clear_dlerror();
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  /* RTLD_DEFAULT maps to GetModuleHandle(NULL) */
  /* Since we don't know what's exported by the test runner itself reliably,
     we just test that dlsym doesn't crash and returns NULL for some dummy name.
   */
  sym = dlsym(RTLD_DEFAULT, "DummySymbolNotInMain");
  ASSERT(sym == NULL);
  ASSERT(dlerror() != NULL);

  /* RTLD_NEXT is unsupported on Windows */
  sym = dlsym(RTLD_NEXT, "AnySymbol");
  ASSERT(sym == NULL);
  ASSERT(dlerror() != NULL);
#else
  sym = dlsym(RTLD_DEFAULT, "main");
  ASSERT(sym == NULL);
#endif
  PASS();
}

TEST test_dlclose_valid(void) {
  void *handle;
  int res;

  clear_dlerror();
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  handle = dlopen("kernel32.dll", RTLD_LAZY);
  ASSERT(handle != NULL);

  res = dlclose(handle);
  ASSERT(res == 0);
  ASSERT(dlerror() == NULL);
#else
  handle = dlopen("libm.so", RTLD_LAZY);
  res = dlclose(handle);
  ASSERT(res == -1);
#endif
  PASS();
}

TEST test_dlclose_null(void) {
  int res;

  clear_dlerror();
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  res = dlclose(NULL);
  ASSERT(res == 0);
  ASSERT(dlerror() == NULL);
#else
  res = dlclose(NULL);
  ASSERT(res == -1);
#endif
  PASS();
}

TEST test_dladdr_valid(void) {
  Dl_info info;
  int res;
  void *func_ptr;
  void *(*func)(const char *, int) = dlopen;

  /* A known function in the process */
  func_ptr = NULL;
  memcpy(&func_ptr, &func, sizeof(func));

  memset(&info, 0, sizeof(info));

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  res = dladdr(func_ptr, &info);
  ASSERT(res != 0);
  ASSERT(info.dli_fbase != NULL);
  ASSERT(info.dli_fname != NULL);
  /* dli_sname and dli_saddr are expected to be NULL in this implementation */
  ASSERT(info.dli_sname == NULL);
  ASSERT(info.dli_saddr == NULL);
#else
  res = dladdr(func_ptr, &info);
  ASSERT(res == 0);
#endif
  PASS();
}

TEST test_dladdr_invalid(void) {
  Dl_info info;
  int res;
  void *invalid_ptr;
  void *func_ptr;
  void *(*func)(const char *, int) = dlopen;

  func_ptr = NULL;
  memcpy(&func_ptr, &func, sizeof(func));

  invalid_ptr = NULL;
  memset(&info, 0, sizeof(info));

  res = dladdr(invalid_ptr, &info);
  ASSERT(res == 0);

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)
  res = dladdr(func_ptr, NULL);
  ASSERT(res == 0);
#endif

  PASS();
}

SUITE(dlfcn_suite) {
  RUN_TEST(test_dlopen_null);
  RUN_TEST(test_dlopen_valid);
  RUN_TEST(test_dlopen_invalid);
  RUN_TEST(test_dlsym_valid);
  RUN_TEST(test_dlsym_invalid);
  RUN_TEST(test_dlsym_invalid_params);
  RUN_TEST(test_dlsym_special_handles);
  RUN_TEST(test_dlclose_valid);
  RUN_TEST(test_dlclose_null);
  RUN_TEST(test_dladdr_valid);
  RUN_TEST(test_dladdr_invalid);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(dlfcn_suite);
  GREATEST_MAIN_END();
}
