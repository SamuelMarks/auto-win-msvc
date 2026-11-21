/* clang-format off */
#include "greatest.h"
#include "posix-utsname.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
/* clang-format on */

static int uname_fail_mock = 0;
static int mock_uname(struct utsname *name) {
  if (uname_fail_mock) {
    return -1;
  }
#if defined(_WIN32) || defined(__CYGWIN__)
  return uname(name);
#else
  strcpy(name->sysname, "MockOS");
  return 0;
#endif
}

int uname_main(int argc, char **argv);
#define main uname_main
#define uname mock_uname
#include "../src/uname_main.c"
#undef main
#undef uname

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#if defined(_MSC_VER)
#define MOCK_UTS_WINAPI __stdcall
#else
#define MOCK_UTS_WINAPI __attribute__((stdcall))
#endif

static int g_mock_uts_comp_ok = 1;
static int g_mock_uts_ntdll_ok = 1;
static int g_mock_uts_rtl_present = 1;
static long g_mock_uts_rtl_status = 0;
static unsigned short g_mock_uts_arch = 9;

static int MOCK_UTS_WINAPI mock_uts_GetComputerNameA(char *buf,
                                                     unsigned long *sz) {
  if (!g_mock_uts_comp_ok) {
    return 0;
  }
  if (buf != NULL && sz != NULL && *sz > 4) {
    memcpy(buf, "host", 5);
    *sz = 4;
  }
  return 1;
}

static void *MOCK_UTS_WINAPI mock_uts_GetModuleHandleA(const char *mod) {
  (void)mod;
  return g_mock_uts_ntdll_ok ? (void *)(size_t)1 : NULL;
}

static long MOCK_UTS_WINAPI mock_uts_RtlGetVersion_fn(void *rosvi_ptr) {
  unsigned long *fields = (unsigned long *)rosvi_ptr;
  if (g_mock_uts_rtl_status == 0 && fields != NULL) {
    fields[1] = 10;
    fields[2] = 0;
    fields[3] = 19045;
  }
  return g_mock_uts_rtl_status;
}

typedef long(MOCK_UTS_WINAPI *mock_uts_farproc_t)(void);

static mock_uts_farproc_t MOCK_UTS_WINAPI
mock_uts_GetProcAddress(void *hMod, const char *proc) {
  (void)hMod;
  (void)proc;
  return g_mock_uts_rtl_present
             ? (mock_uts_farproc_t)(size_t)mock_uts_RtlGetVersion_fn
             : (mock_uts_farproc_t)0;
}

static int MOCK_UTS_WINAPI mock_uts_GetVersionExA(void *osvi_ptr) {
  unsigned long *fields = (unsigned long *)osvi_ptr;
  if (fields != NULL) {
    fields[1] = 6;
    fields[2] = 1;
    fields[3] = 7601;
  }
  return 1;
}

static void MOCK_UTS_WINAPI mock_uts_GetSystemInfo(void *si_ptr) {
  unsigned short *words = (unsigned short *)si_ptr;
  if (words != NULL) {
    memset(si_ptr, 0, sizeof(void *) == 8 ? 48 : 36);
    words[0] = g_mock_uts_arch;
  }
}

#define UTS_MOCK_GetComputerNameA mock_uts_GetComputerNameA
#define UTS_MOCK_GetModuleHandleA mock_uts_GetModuleHandleA
#define UTS_MOCK_GetProcAddress mock_uts_GetProcAddress
#define UTS_MOCK_GetSystemInfo mock_uts_GetSystemInfo
#define UTS_MOCK_GetVersionExA mock_uts_GetVersionExA

int mocked_win_uname(struct utsname *name);
auto_win_msvc_error_t mocked_posix_utsname_get_info(int *out_available);
#define uname mocked_win_uname
#define posix_utsname_get_info mocked_posix_utsname_get_info

#include "../src/posix-utsname.c"

#undef uname
#undef posix_utsname_get_info
#endif

TEST test_posix_utsname_get_info(void) {
  int info = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_utsname_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_utsname_get_info(&info));
  ASSERT_EQ(1, info);

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
  info = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            mocked_posix_utsname_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_posix_utsname_get_info(&info));
  ASSERT_EQ(1, info);
#endif

  PASS();
}

TEST test_uname_null(void) {
  errno = 0;
  ASSERT_EQ(-1, uname(NULL));
  ASSERT_EQ(EFAULT, errno);

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
  errno = 0;
  ASSERT_EQ(-1, mocked_win_uname(NULL));
  ASSERT_EQ(EFAULT, errno);
#endif

  PASS();
}

TEST test_uname(void) {
  struct utsname name;
  int res;

  memset(&name, 0, sizeof(name));
  res = uname(&name);
#if defined(_WIN32) || defined(__CYGWIN__) || defined(__DOS__) ||              \
    defined(__MSDOS__) || defined(__WATCOMC__)
  ASSERT_EQ(0, res);
  ASSERT(strlen(name.sysname) > 0);
  ASSERT(strlen(name.nodename) > 0);
  ASSERT(strlen(name.release) > 0);
  ASSERT(strlen(name.version) > 0);
  ASSERT(strlen(name.machine) > 0);
#else
  ASSERT_EQ(-1, res);
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
  /* 1. RtlGetVersion success + INTEL */
  g_mock_uts_comp_ok = 1;
  g_mock_uts_ntdll_ok = 1;
  g_mock_uts_rtl_present = 1;
  g_mock_uts_rtl_status = 0;
  g_mock_uts_arch = UTS_PROCESSOR_ARCHITECTURE_INTEL;
  ASSERT_EQ(0, mocked_win_uname(&name));
  ASSERT_STR_EQ("i686", name.machine);
  ASSERT_STR_EQ("10.0", name.release);

  /* 2. GetComputerNameA fails + ntdll NULL -> GetVersionExA + AMD64 */
  g_mock_uts_comp_ok = 0;
  g_mock_uts_ntdll_ok = 0;
  g_mock_uts_arch = UTS_PROCESSOR_ARCHITECTURE_AMD64;
  ASSERT_EQ(0, mocked_win_uname(&name));
  ASSERT_STR_EQ("unknown", name.nodename);
  ASSERT_STR_EQ("x86_64", name.machine);
  ASSERT_STR_EQ("6.1", name.release);

  /* 3. ntdll present, RtlGetVersion missing + ARM */
  g_mock_uts_comp_ok = 1;
  g_mock_uts_ntdll_ok = 1;
  g_mock_uts_rtl_present = 0;
  g_mock_uts_arch = UTS_PROCESSOR_ARCHITECTURE_ARM;
  ASSERT_EQ(0, mocked_win_uname(&name));
  ASSERT_STR_EQ("arm", name.machine);

  /* 4. RtlGetVersion returns error -> fallback GetVersionExA + ARM64 */
  g_mock_uts_rtl_present = 1;
  g_mock_uts_rtl_status = -1;
  g_mock_uts_arch = UTS_PROCESSOR_ARCHITECTURE_ARM64;
  ASSERT_EQ(0, mocked_win_uname(&name));
  ASSERT_STR_EQ("aarch64", name.machine);

  /* 5. IA64 and unknown default architecture */
  g_mock_uts_rtl_status = 0;
  g_mock_uts_arch = UTS_PROCESSOR_ARCHITECTURE_IA64;
  ASSERT_EQ(0, mocked_win_uname(&name));
  ASSERT_STR_EQ("ia64", name.machine);

  g_mock_uts_arch = 999;
  ASSERT_EQ(0, mocked_win_uname(&name));
  ASSERT_STR_EQ("unknown", name.machine);
#endif

  PASS();
}

TEST test_uname_main_func(void) {
  char arg0[] = "uname";
  char *argv[2];
  argv[0] = arg0;
  argv[1] = NULL;

  uname_fail_mock = 0;
  ASSERT_EQ(0, uname_main(1, argv));

  uname_fail_mock = 1;
  ASSERT_EQ(0, uname_main(1, argv));

  uname_fail_mock = 0;
  PASS();
}

SUITE(suite_posix_utsname_core) {
  RUN_TEST(test_posix_utsname_get_info);
  RUN_TEST(test_uname_null);
  RUN_TEST(test_uname);
  RUN_TEST(test_uname_main_func);
}
