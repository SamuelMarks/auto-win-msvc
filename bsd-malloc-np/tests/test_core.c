/* clang-format off */
#include "greatest.h"
#include "bsd-malloc-np.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
/* clang-format on */

static size_t g_cb_calls = 0;

static void dummy_cb(void *opaque, const char *str) {
  (void)opaque;
  if (str != NULL) {
    g_cb_calls++;
  }
}

#if defined(_WIN32) || defined(_WIN64)
static int g_mock_k32_present = 1;
static int g_mock_k32_proc_present = 1;
static int g_mock_psapi_loaded = 0;
static int g_mock_psapi_load_ok = 1;
static int g_mock_psapi_proc_present = 1;
static int g_mock_pmi_ret = 1;
static int g_mock_memstatus_ret = 1;

struct HINSTANCE__;
typedef struct HINSTANCE__ *MOCK_HMOD;
typedef int(__stdcall *MOCK_FARPROC)(void);

static int __stdcall mock_GetProcessMemoryInfo_fn(void *hProc, void *pmc_ptr,
                                                  unsigned long cb) {
  (void)hProc;
  if (pmc_ptr != NULL && cb > 0) {
    memset(pmc_ptr, 0, (size_t)cb);
  }
  return g_mock_pmi_ret;
}

static void *__stdcall mock_GetCurrentProcess(void) { return (void *)1; }

static MOCK_HMOD __stdcall mock_GetModuleHandleA(const char *name) {
  if (strcmp(name, "kernel32.dll") == 0) {
    return g_mock_k32_present ? (MOCK_HMOD)1 : (MOCK_HMOD)0;
  }
  if (strcmp(name, "psapi.dll") == 0) {
    return g_mock_psapi_loaded ? (MOCK_HMOD)2 : (MOCK_HMOD)0;
  }
  return (MOCK_HMOD)0;
}

static MOCK_HMOD __stdcall mock_LoadLibraryA(const char *name) {
  (void)name;
  return g_mock_psapi_load_ok ? (MOCK_HMOD)2 : (MOCK_HMOD)0;
}

static MOCK_FARPROC __stdcall mock_GetProcAddress(MOCK_HMOD hMod,
                                                  const char *proc) {
  (void)hMod;
  if (strcmp(proc, "K32GetProcessMemoryInfo") == 0) {
    return g_mock_k32_proc_present
               ? (MOCK_FARPROC)(size_t)mock_GetProcessMemoryInfo_fn
               : (MOCK_FARPROC)0;
  }
  if (strcmp(proc, "GetProcessMemoryInfo") == 0) {
    return g_mock_psapi_proc_present
               ? (MOCK_FARPROC)(size_t)mock_GetProcessMemoryInfo_fn
               : (MOCK_FARPROC)0;
  }
  return (MOCK_FARPROC)0;
}

static int __stdcall mock_GlobalMemoryStatusEx(void *buf) {
  if (buf != NULL && g_mock_memstatus_ret) {
    memset(buf, 0, 64);
    return 1;
  }
  return 0;
}

#define BSD_MALLOC_NP_MOCK_GetCurrentProcess mock_GetCurrentProcess
#define BSD_MALLOC_NP_MOCK_GlobalMemoryStatusEx mock_GlobalMemoryStatusEx
#define BSD_MALLOC_NP_MOCK_GetModuleHandleA mock_GetModuleHandleA
#define BSD_MALLOC_NP_MOCK_LoadLibraryA mock_LoadLibraryA
#define BSD_MALLOC_NP_MOCK_GetProcAddress mock_GetProcAddress

#define je_malloc_stats_print mocked_je_malloc_stats_print
#define bsd_malloc_np_init mocked_bsd_malloc_np_init
#include "../src/bsd-malloc-np.c"
#undef je_malloc_stats_print
#undef bsd_malloc_np_init
#endif

TEST test_bsd_malloc_np_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = bsd_malloc_np_init(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = bsd_malloc_np_init(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, status);

#if defined(_WIN32) || defined(_WIN64)
  status = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, mocked_bsd_malloc_np_init(0));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_bsd_malloc_np_init(&status));
  ASSERT_EQ(1, status);
#endif

  PASS();
}

TEST test_je_malloc_stats_print(void) {
  char opaque_data[] = "opaque_data";
  error_type_t err;

  err = je_malloc_stats_print(NULL, NULL, NULL);
  ASSERT_EQ(ERR_NONE, err);

  g_cb_calls = 0;
  err = je_malloc_stats_print(dummy_cb, (void *)opaque_data, "opts");
  ASSERT_EQ(ERR_NONE, err);
  ASSERT(g_cb_calls >= 2);

#if defined(_WIN32) || defined(_WIN64)
  /* NULL callback on mocked TU */
  ASSERT_EQ(ERR_NONE, mocked_je_malloc_stats_print(NULL, NULL, NULL));

  /* 1. Kernel32 present + K32GetProcessMemoryInfo succeeds + GlobalMemory ok */
  g_mock_k32_present = 1;
  g_mock_k32_proc_present = 1;
  g_mock_pmi_ret = 1;
  g_mock_memstatus_ret = 1;
  ASSERT_EQ(ERR_NONE, mocked_je_malloc_stats_print(dummy_cb, NULL, NULL));

  /* 2. Kernel32 present, K32 missing, psapi already loaded, PMI fails,
   * GlobalMemory fails */
  g_mock_k32_present = 1;
  g_mock_k32_proc_present = 0;
  g_mock_psapi_loaded = 1;
  g_mock_psapi_proc_present = 1;
  g_mock_pmi_ret = 0;
  g_mock_memstatus_ret = 0;
  ASSERT_EQ(ERR_NONE, mocked_je_malloc_stats_print(dummy_cb, NULL, "g"));

  /* 3. Kernel32 missing, psapi not loaded -> LoadLibraryA succeeds, proc
   * missing */
  g_mock_k32_present = 0;
  g_mock_psapi_loaded = 0;
  g_mock_psapi_load_ok = 1;
  g_mock_psapi_proc_present = 0;
  ASSERT_EQ(ERR_NONE, mocked_je_malloc_stats_print(dummy_cb, NULL, NULL));

  /* 4. Kernel32 missing, psapi not loaded -> LoadLibraryA fails */
  g_mock_k32_present = 0;
  g_mock_psapi_loaded = 0;
  g_mock_psapi_load_ok = 0;
  ASSERT_EQ(ERR_NONE, mocked_je_malloc_stats_print(dummy_cb, NULL, NULL));
#endif

  PASS();
}

TEST test_auto_win_msvc_error_helpers(void) {
  ASSERT_STR_EQ("Success", auto_win_msvc_error_string(AUTO_WIN_MSVC_SUCCESS));
  ASSERT_STR_EQ("Invalid argument", auto_win_msvc_error_string(
                                        AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT));
  ASSERT_STR_EQ("Out of memory",
                auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_OUT_OF_MEMORY));
  ASSERT_STR_EQ("Not implemented", auto_win_msvc_error_string(
                                       AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED));
  ASSERT_STR_EQ("OS call failed",
                auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED));
  ASSERT_STR_EQ("I/O error",
                auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_IO));
  ASSERT_STR_EQ(
      "Permission denied",
      auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_PERMISSION_DENIED));
  ASSERT_STR_EQ("Not found",
                auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_NOT_FOUND));
  ASSERT_STR_EQ("Already exists",
                auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_ALREADY_EXISTS));
  ASSERT_STR_EQ("Timeout",
                auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_TIMEOUT));
  ASSERT_STR_EQ("Would block",
                auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_WOULD_BLOCK));
  ASSERT_STR_EQ("Unknown error",
                auto_win_msvc_error_string(AUTO_WIN_MSVC_ERROR_UNKNOWN));
  ASSERT_STR_EQ("Unknown error",
                auto_win_msvc_error_string((auto_win_msvc_error_t)999));

  ASSERT_EQ(0, auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_SUCCESS));
  ASSERT_EQ(EINVAL,
            auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT));
  ASSERT_EQ(ENOMEM,
            auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_OUT_OF_MEMORY));
  ASSERT_EQ(ENOSYS,
            auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED));
  ASSERT_EQ(EFAULT,
            auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED));
  ASSERT_EQ(EIO, auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_IO));
  ASSERT_EQ(EACCES, auto_win_msvc_error_to_errno(
                        AUTO_WIN_MSVC_ERROR_PERMISSION_DENIED));
  ASSERT_EQ(ENOENT,
            auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_NOT_FOUND));
  ASSERT_EQ(EEXIST,
            auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_ALREADY_EXISTS));
  ASSERT_EQ(ETIMEDOUT,
            auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_TIMEOUT));
  ASSERT_EQ(EWOULDBLOCK,
            auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_WOULD_BLOCK));
  ASSERT_EQ(EFAULT, auto_win_msvc_error_to_errno(AUTO_WIN_MSVC_ERROR_UNKNOWN));
  ASSERT_EQ(EINVAL, auto_win_msvc_error_to_errno((auto_win_msvc_error_t)999));

  PASS();
}

SUITE(suite_bsd_malloc_np_core) {
  RUN_TEST(test_bsd_malloc_np_init);
  RUN_TEST(test_je_malloc_stats_print);
  RUN_TEST(test_auto_win_msvc_error_helpers);
}
