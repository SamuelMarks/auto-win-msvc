/* clang-format off */
#include "greatest.h"
#include "posix-dlfcn.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

#if defined(_WIN32)
typedef struct HINSTANCE__ *TEST_DLFCN_HMOD;
typedef int(__stdcall *TEST_DLFCN_FARPROC)(void);
typedef struct _TEST_DLFCN_MBI {
  void *BaseAddress;
  void *AllocationBase;
  unsigned long AllocationProtect;
  size_t RegionSize;
  unsigned long State;
  unsigned long Protect;
  unsigned long Type;
} TEST_DLFCN_MBI;

extern void posix_dlfcn_set_win32_hooks(
    TEST_DLFCN_HMOD (*load_lib_fn)(const char *),
    TEST_DLFCN_HMOD (*get_mod_fn)(const char *),
    int (*free_lib_fn)(TEST_DLFCN_HMOD),
    TEST_DLFCN_FARPROC (*get_proc_fn)(TEST_DLFCN_HMOD, const char *),
    unsigned long (*last_err_fn)(void),
    unsigned long (*mod_fname_fn)(TEST_DLFCN_HMOD, char *, unsigned long),
    unsigned long (*fmt_msg_fn)(unsigned long, const void *, unsigned long,
                                unsigned long, char *, unsigned long, void *),
    size_t (*vquery_fn)(const void *, TEST_DLFCN_MBI *, size_t));
extern void posix_dlfcn_reset_win32_hooks(void);

static TEST_DLFCN_HMOD g_mock_load_ret = NULL;
static TEST_DLFCN_HMOD g_mock_mod_ret = NULL;
static int g_mock_free_ret = 0;
static TEST_DLFCN_FARPROC g_mock_proc_ret = NULL;
static unsigned long g_mock_err_ret = 0UL;
static unsigned long g_mock_fname_ret = 0UL;
static int g_mock_fmt_mode = 0;
static size_t g_mock_vq_ret = 0;

static TEST_DLFCN_HMOD mock_load_lib(const char *n) {
  (void)n;
  return g_mock_load_ret;
}
static TEST_DLFCN_HMOD mock_get_mod(const char *n) {
  (void)n;
  return g_mock_mod_ret;
}
static int mock_free_lib(TEST_DLFCN_HMOD m) {
  (void)m;
  return g_mock_free_ret;
}
static TEST_DLFCN_FARPROC mock_get_proc(TEST_DLFCN_HMOD m, const char *n) {
  (void)m;
  (void)n;
  return g_mock_proc_ret;
}
static unsigned long mock_last_err(void) { return g_mock_err_ret; }
static unsigned long mock_mod_fname(TEST_DLFCN_HMOD m, char *b,
                                    unsigned long sz) {
  (void)m;
  if (g_mock_fname_ret > 0 && g_mock_fname_ret < sz) {
    b[0] = 'x';
    b[1] = '\0';
  }
  return g_mock_fname_ret;
}
static unsigned long mock_fmt_msg(unsigned long fl, const void *src,
                                  unsigned long id, unsigned long lang,
                                  char *buf, unsigned long sz, void *args) {
  (void)fl;
  (void)src;
  (void)id;
  (void)lang;
  (void)sz;
  (void)args;
  if (g_mock_fmt_mode == 0) {
    return 0UL;
  }
  if (g_mock_fmt_mode == 1) {
    buf[0] = '\0';
    return 1UL;
  }
  if (g_mock_fmt_mode == 2) {
    buf[0] = 'm';
    buf[1] = 's';
    buf[2] = 'g';
    buf[3] = (char)13;
    buf[4] = (char)10;
    buf[5] = '\0';
    return 5UL;
  }
#if defined(_MSC_VER)
  strcpy_s(buf, sz, "plain");
#else
  strcpy(buf, "plain");
#endif
  return 5UL;
}
static size_t mock_vquery(const void *addr, TEST_DLFCN_MBI *mbi, size_t len) {
  (void)addr;
  (void)len;
  memset(mbi, 0, sizeof(*mbi));
  mbi->AllocationBase = (void *)(size_t)0x1000;
  return g_mock_vq_ret;
}
#endif

TEST test_posix_dlfcn_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = posix_dlfcn_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_dlfcn_get_info(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_dlopen_dlclose(void) {
  void *handle;
  char *err;

  /* Clear any prior dlerror */
  (void)dlerror();
  ASSERT_EQ(NULL, dlerror());

  handle = dlopen("nonexistent_library_xyz_12345.so", RTLD_LAZY);
  ASSERT_EQ(NULL, handle);

  err = dlerror();
  ASSERT(err != NULL);
  ASSERT_EQ(NULL, dlerror());

  handle = dlopen(NULL, RTLD_NOW);
  ASSERT(handle != NULL);
  ASSERT_EQ(0, dlclose(handle));

#if defined(_WIN32)
  /* mode < 0 */
  ASSERT_EQ(NULL, dlopen("kernel32.dll", -1));

  /* Path normalization: libvalkeylua stripping, .dylib/.so -> .dll, '/' -> ''
   */
  ASSERT_EQ(NULL, dlopen("dir/libvalkeylua.dylib", RTLD_NOW));
  (void)dlerror();
  ASSERT_EQ(NULL, dlopen("no_extension_nonexistent_xyz", RTLD_NOW));
  (void)dlerror();

  /* dlclose(NULL) and dlclose on invalid handle */
  ASSERT_EQ(0, dlclose(NULL));
  ASSERT_EQ(-1, dlclose((void *)(size_t)0x1));
  ASSERT(dlerror() != NULL);
#endif

  PASS();
}

TEST test_dlsym_and_dladdr(void) {
#if defined(_WIN32)
  void *handle;
  void *sym;
  Dl_info info;

  handle = dlopen("kernel32.dll", RTLD_NOW);
  ASSERT(handle != NULL);

  sym = dlsym(handle, "GetCurrentProcessId");
  ASSERT(sym != NULL);

  sym = dlsym(handle, "NonExistentFunctionXYZ123");
  ASSERT_EQ(NULL, sym);
  ASSERT(dlerror() != NULL);

  sym = dlsym(handle, NULL);
  ASSERT_EQ(NULL, sym);
  ASSERT(dlerror() != NULL);

  /* RTLD_DEFAULT and RTLD_NEXT */
  (void)dlsym(RTLD_DEFAULT, "NonExistentMainSymXYZ");
  (void)dlerror();
  ASSERT_EQ(NULL, dlsym(RTLD_NEXT, "anything"));
  ASSERT_STR_EQ("RTLD_NEXT is not supported", dlerror());

  /* Real dladdr calls using address inside our PE image */
  ASSERT_EQ(0, dladdr(NULL, &info));
  ASSERT_EQ(0, dladdr((const void *)&g_mock_vq_ret, NULL));
  ASSERT_EQ(1, dladdr((const void *)&g_mock_vq_ret, &info));
  ASSERT(info.dli_fbase != NULL);
  ASSERT(info.dli_fname != NULL);

  ASSERT_EQ(0, dlclose(handle));

  /* Hooked Win32 edge cases: GetModuleHandleA(NULL)==NULL, err_code==0,
   * FormatMessageA==0 / empty / crlf / plain, VirtualQuery==0,
   * GetModuleFileNameA==0 and ==MAX_PATH */
  posix_dlfcn_set_win32_hooks(mock_load_lib, mock_get_mod, mock_free_lib,
                              mock_get_proc, mock_last_err, mock_mod_fname,
                              mock_fmt_msg, mock_vquery);

  g_mock_mod_ret = NULL;
  g_mock_err_ret = 0UL;
  ASSERT_EQ(NULL, dlopen(NULL, RTLD_NOW));
  ASSERT_EQ(NULL, dlerror());

  /* Also cover dlopen success and dlclose success/failure via hooks */
  g_mock_load_ret = (TEST_DLFCN_HMOD)(size_t)0x2000;
  ASSERT_NEQ(NULL, dlopen("foo.so", RTLD_NOW));
  g_mock_mod_ret = (TEST_DLFCN_HMOD)(size_t)0x1000;
  g_mock_free_ret = 1;
  ASSERT_EQ(0, dlclose((void *)(size_t)0x2000));

  g_mock_load_ret = NULL;
  g_mock_err_ret = 12345UL;
  g_mock_fmt_mode = 0; /* FormatMessageA returns 0 -> Unknown error code */
  ASSERT_EQ(NULL, dlopen("foo.dll", RTLD_NOW));
  ASSERT(dlerror() != NULL);

  g_mock_fmt_mode = 1; /* Empty message (len == 0) */
  ASSERT_EQ(NULL, dlopen("foo.dll", RTLD_NOW));
  ASSERT(dlerror() != NULL);

  g_mock_fmt_mode = 2; /* CRLF message */
  ASSERT_EQ(NULL, dlopen("foo.dll", RTLD_NOW));
  ASSERT_STR_EQ("msg", dlerror());

  g_mock_fmt_mode = 3; /* No trailing CR/LF */
  ASSERT_EQ(NULL, dlopen("foo.dll", RTLD_NOW));
  ASSERT_STR_EQ("plain", dlerror());

  /* dladdr with VirtualQuery == 0, mod_fname == 0, mod_fname == 260, valid */
  g_mock_vq_ret = 0;
  ASSERT_EQ(0, dladdr((const void *)(size_t)0x1000, &info));

  g_mock_vq_ret = sizeof(TEST_DLFCN_MBI);
  g_mock_fname_ret = 0UL;
  ASSERT_EQ(1, dladdr((const void *)(size_t)0x1000, &info));
  ASSERT_EQ(NULL, info.dli_fname);

  g_mock_fname_ret = 260UL;
  ASSERT_EQ(1, dladdr((const void *)(size_t)0x1000, &info));
  ASSERT_EQ(NULL, info.dli_fname);

  g_mock_fname_ret = 1UL;
  ASSERT_EQ(1, dladdr((const void *)(size_t)0x1000, &info));
  ASSERT(info.dli_fname != NULL);

  posix_dlfcn_reset_win32_hooks();
#else
  void *handle = dlopen(NULL, RTLD_NOW);
  ASSERT(handle != NULL);
  (void)dlsym(handle, "nonexistent_symbol_xyz");
  (void)dlerror();
  dlclose(handle);
#endif
  PASS();
}

SUITE(suite_posix_dlfcn_core) {
  RUN_TEST(test_posix_dlfcn_init);
  RUN_TEST(test_dlopen_dlclose);
  RUN_TEST(test_dlsym_and_dladdr);
}
