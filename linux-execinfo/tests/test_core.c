/* clang-format off */
#include "greatest.h"
#include "linux-execinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
static int g_mock_execinfo_oom = 0;
static int g_mock_sym_from_addr_ok = 1;
static int g_mock_std_handle_invalid = 0;
static int g_mock_write_fail_at = 0;
static int g_mock_write_calls = 0;

static unsigned short __stdcall
mock_RtlCaptureStackBackTrace(unsigned long skip, unsigned long cap, void **bt,
                              unsigned long *hash) {
  (void)skip;
  (void)hash;
  if (cap >= 1) {
    bt[0] = (void *)(size_t)0x12345678UL;
    return 1;
  }
  return 0;
}

static void *__stdcall mock_GetCurrentProcess(void) {
  return (void *)(size_t)1;
}

static void *__stdcall mock_GetStdHandle(unsigned long nStdHandle) {
  (void)nStdHandle;
  if (g_mock_std_handle_invalid) {
    return (void *)(ptrdiff_t)-1;
  }
  return (void *)(size_t)2;
}

static int __stdcall mock_WriteFile(void *hFile, const void *buf,
                                    unsigned long len, unsigned long *written,
                                    void *overlapped) {
  (void)hFile;
  (void)buf;
  (void)overlapped;
  g_mock_write_calls++;
  if (g_mock_write_fail_at > 0 && g_mock_write_calls == g_mock_write_fail_at) {
    return 0;
  }
  if (written != NULL) {
    *written = len;
  }
  return 1;
}

static unsigned long __stdcall mock_SymSetOptions(unsigned long opts) {
  return opts;
}

static unsigned long __stdcall mock_SymGetOptions(void) { return 0; }

static int __stdcall mock_SymInitialize(void *hProcess, const char *path,
                                        int invade) {
  (void)hProcess;
  (void)path;
  (void)invade;
  return 1;
}

#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 mock_execinfo_u64;
#elif defined(__GNUC__) || defined(__clang__)
__extension__ typedef unsigned long long mock_execinfo_u64;
#else
typedef unsigned long long mock_execinfo_u64;
#endif

static int __stdcall mock_SymFromAddr(void *hProcess, mock_execinfo_u64 addr,
                                      mock_execinfo_u64 *disp, void *sym_ptr) {
  (void)hProcess;
  (void)addr;
  if (!g_mock_sym_from_addr_ok) {
    return 0;
  }
  if (disp != NULL) {
    *disp = 0;
  }
  if (sym_ptr != NULL) {
    /* Name field is at offset 84 on 32/64-bit packed(8) SYMBOL_INFO; copy
     * safely via struct layout inside the included TU below, or leave "sym" */
    char *raw = (char *)sym_ptr;
    raw[84] = 'f';
    raw[85] = 'n';
    raw[86] = '\0';
  }
  return 1;
}

static void *mock_execinfo_malloc(size_t sz) {
  if (g_mock_execinfo_oom) {
    return NULL;
  }
  return malloc(sz);
}

#define EXECINFO_MOCK_RtlCaptureStackBackTrace mock_RtlCaptureStackBackTrace
#define EXECINFO_MOCK_GetCurrentProcess mock_GetCurrentProcess
#define EXECINFO_MOCK_GetStdHandle mock_GetStdHandle
#define EXECINFO_MOCK_WriteFile mock_WriteFile
#define EXECINFO_MOCK_SymSetOptions mock_SymSetOptions
#define EXECINFO_MOCK_SymGetOptions mock_SymGetOptions
#define EXECINFO_MOCK_SymInitialize mock_SymInitialize
#define EXECINFO_MOCK_SymFromAddr mock_SymFromAddr
#define EXECINFO_MOCK_malloc mock_execinfo_malloc
#define EXECINFO_MOCK_free free

error_type_t mocked_backtrace(void **buffer, int size, int *captured);
char **mocked_backtrace_symbols(void *const *buffer, int size);
error_type_t mocked_backtrace_symbols_fd(void *const *buffer, int size, int fd);
auto_win_msvc_error_t mocked_linux_execinfo_init(int *out_status);
#define backtrace mocked_backtrace
#define backtrace_symbols mocked_backtrace_symbols
#define backtrace_symbols_fd mocked_backtrace_symbols_fd
#define linux_execinfo_init mocked_linux_execinfo_init

#include "../src/linux-execinfo.c"

#undef backtrace
#undef backtrace_symbols
#undef backtrace_symbols_fd
#undef linux_execinfo_init
#endif

TEST test_linux_execinfo_init(void) {
  int status = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, linux_execinfo_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, linux_execinfo_init(&status));
  ASSERT_EQ(1, status);

#if defined(_MSC_VER) || defined(_WIN32)
  status = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            mocked_linux_execinfo_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_linux_execinfo_init(&status));
  ASSERT_EQ(1, status);
#endif

  PASS();
}

TEST test_backtrace_basic(void) {
  void *buffer[10];
  int captured = 0;

  ASSERT_EQ(-1, backtrace(NULL, 10, &captured));
  ASSERT_EQ(-1, backtrace(buffer, 0, &captured));
  ASSERT_EQ(-1, backtrace(buffer, -1, &captured));
  ASSERT_EQ(-1, backtrace(buffer, 10, NULL));
  ASSERT_EQ(ERR_NONE, backtrace(buffer, 10, &captured));

#if defined(_MSC_VER) || defined(_WIN32)
  ASSERT_EQ(-1, mocked_backtrace(NULL, 10, &captured));
  ASSERT_EQ(-1, mocked_backtrace(buffer, 0, &captured));
  ASSERT_EQ(-1, mocked_backtrace(buffer, 10, NULL));
  captured = 0;
  ASSERT_EQ(ERR_NONE, mocked_backtrace(buffer, 10, &captured));
  ASSERT_EQ(1, captured);
#endif

  PASS();
}

TEST test_backtrace_symbols_basic(void) {
  void *buffer[2];
  char **symbols;

  buffer[0] = (void *)(size_t)0x12345678UL;
  buffer[1] = (void *)(size_t)0x87654321UL;

  ASSERT(backtrace_symbols(NULL, 2) == NULL);
  ASSERT(backtrace_symbols(buffer, 0) == NULL);
  ASSERT(backtrace_symbols(buffer, -1) == NULL);

  symbols = backtrace_symbols(buffer, 1);
  if (symbols != NULL) {
    free(symbols);
  }

  ASSERT_EQ(-1, backtrace_symbols_fd(NULL, 2, 1));
  ASSERT_EQ(-1, backtrace_symbols_fd(buffer, 0, 1));
  ASSERT_EQ(-1, backtrace_symbols_fd(buffer, -1, 1));
  ASSERT_EQ(-1, backtrace_symbols_fd(buffer, 2, -1));

#if !defined(_WIN32)
  ASSERT_EQ(ERR_NONE, backtrace_symbols_fd(buffer, 2, 1));
  ASSERT_EQ(ERR_NONE, backtrace_symbols_fd(buffer, 2, 2));
#endif

  PASS();
}

TEST test_backtrace_mocked_oom_and_os_branches(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  void *buffer[2];
  char **symbols;

  buffer[0] = (void *)(size_t)0x12345678UL;
  buffer[1] = (void *)(size_t)0x87654321UL;

  /* Validation branches on mocked functions */
  ASSERT(mocked_backtrace_symbols(NULL, 2) == NULL);
  ASSERT(mocked_backtrace_symbols(buffer, 0) == NULL);
  ASSERT_EQ(-1, mocked_backtrace_symbols_fd(buffer, 2, -1));
  ASSERT_EQ(-1, mocked_backtrace_symbols_fd(buffer, 0, 1));
  ASSERT_EQ(-1, mocked_backtrace_symbols_fd(NULL, 2, 1));

  /* 1st call: init_sym_done == 0, OOM -> returns NULL */
  g_mock_execinfo_oom = 1;
  ASSERT(mocked_backtrace_symbols(buffer, 1) == NULL);
  g_mock_execinfo_oom = 0;

  /* 2nd call: init_sym_done == 1, SymFromAddr succeeds */
  g_mock_sym_from_addr_ok = 1;
  symbols = mocked_backtrace_symbols(buffer, 2);
  ASSERT(symbols != NULL);
  free(symbols);

  /* SymFromAddr fails -> fallback "[%p]" formatting */
  g_mock_sym_from_addr_ok = 0;
  symbols = mocked_backtrace_symbols(buffer, 1);
  ASSERT(symbols != NULL);
  ASSERT(symbols[0][0] == '[');
  free(symbols);
  g_mock_sym_from_addr_ok = 1;

  /* backtrace_symbols_fd: INVALID_HANDLE_VALUE on stdout (fd=1) & stderr (fd=2)
   */
  g_mock_std_handle_invalid = 1;
  ASSERT_EQ(-1, mocked_backtrace_symbols_fd(buffer, 1, 1));
  ASSERT_EQ(-1, mocked_backtrace_symbols_fd(buffer, 1, 2));
  g_mock_std_handle_invalid = 0;

  /* backtrace_symbols_fd: OOM in backtrace_symbols */
  g_mock_execinfo_oom = 1;
  ASSERT_EQ(-1, mocked_backtrace_symbols_fd(buffer, 1, 1));
  g_mock_execinfo_oom = 0;

  /* backtrace_symbols_fd: 1st WriteFile (symbol text) fails */
  g_mock_write_calls = 0;
  g_mock_write_fail_at = 1;
  ASSERT_EQ(-1, mocked_backtrace_symbols_fd(buffer, 1, 1));

  /* backtrace_symbols_fd: 2nd WriteFile (newline) fails */
  g_mock_write_calls = 0;
  g_mock_write_fail_at = 2;
  ASSERT_EQ(-1, mocked_backtrace_symbols_fd(buffer, 1, 2));

  /* backtrace_symbols_fd: full success for both fd=1 and fd=2 */
  g_mock_write_calls = 0;
  g_mock_write_fail_at = 0;
  ASSERT_EQ(ERR_NONE, mocked_backtrace_symbols_fd(buffer, 2, 1));
  ASSERT_EQ(ERR_NONE, mocked_backtrace_symbols_fd(buffer, 1, 2));
#endif
  PASS();
}

SUITE(suite_linux_execinfo_core) {
  RUN_TEST(test_linux_execinfo_init);
  RUN_TEST(test_backtrace_basic);
  RUN_TEST(test_backtrace_symbols_basic);
}

SUITE(suite_linux_execinfo_extra) {
  RUN_TEST(test_backtrace_mocked_oom_and_os_branches);
}
