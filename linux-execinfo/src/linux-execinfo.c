#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file linux-execinfo.c
 * @brief Polyfill implementation for GNU <execinfo.h> backtrace utilities.
 */

/* clang-format off */
#include "linux-execinfo.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

/* Minimal Windows types and forward declarations to avoid <windows.h> */
typedef unsigned short WORD;
typedef unsigned long DWORD;
#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 execinfo_u64;
#elif defined(__GNUC__) || defined(__clang__)
__extension__ typedef unsigned long long execinfo_u64;
#else
typedef unsigned long long execinfo_u64;
#endif
typedef execinfo_u64 DWORD64;
typedef void *PVOID;
typedef void *HANDLE;
typedef int BOOL;
typedef char CHAR;
typedef const CHAR *PCSTR;
typedef unsigned long ULONG;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MAX_SYM_NAME
#define MAX_SYM_NAME 2000
#endif

#if defined(_MSC_VER) || defined(_WIN32)
#define WINBASEAPI __declspec(dllimport)
#define WINAPI __stdcall
#else
#define WINBASEAPI
#define WINAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

WINBASEAPI WORD WINAPI RtlCaptureStackBackTrace(DWORD FramesToSkip,
                                                DWORD FramesToCapture,
                                                PVOID *BackTrace,
                                                DWORD *BackTraceHash);

WINBASEAPI HANDLE WINAPI GetCurrentProcess(void);
WINBASEAPI HANDLE WINAPI GetStdHandle(DWORD nStdHandle);
WINBASEAPI BOOL WINAPI WriteFile(HANDLE hFile, const void *lpBuffer,
                                 DWORD nNumberOfBytesToWrite,
                                 DWORD *lpNumberOfBytesWritten,
                                 void *lpOverlapped);

#define STD_OUTPUT_HANDLE ((DWORD) - 11)
#define STD_ERROR_HANDLE ((DWORD) - 12)
#define INVALID_HANDLE_VALUE ((HANDLE)(ptrdiff_t)-1)

/* DbgHelp types and declarations */
#pragma pack(push, 8)
typedef struct _SYMBOL_INFO {
  ULONG SizeOfStruct;
  ULONG TypeIndex;
  DWORD64 Reserved[2];
  ULONG Index;
  ULONG Size;
  DWORD64 ModBase;
  ULONG Flags;
  DWORD64 Value;
  DWORD64 Address;
  ULONG Register;
  ULONG Scope;
  ULONG Tag;
  ULONG NameLen;
  ULONG MaxNameLen;
  CHAR Name[1];
} SYMBOL_INFO, *PSYMBOL_INFO;
#pragma pack(pop)

#define SYMOPT_UNDNAME 0x00000002
#define SYMOPT_DEFERRED_LOADS 0x00000004

#ifndef PDWORD64
typedef DWORD64 *PDWORD64;
#endif

DWORD WINAPI SymSetOptions(DWORD SymOptions);
DWORD WINAPI SymGetOptions(void);
BOOL WINAPI SymInitialize(HANDLE hProcess, PCSTR UserSearchPath,
                          BOOL fInvadeProcess);
BOOL WINAPI SymFromAddr(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement,
                        PSYMBOL_INFO Symbol);

#ifdef __cplusplus
}
#endif

#ifndef EXECINFO_MOCK_RtlCaptureStackBackTrace
#define EXECINFO_MOCK_RtlCaptureStackBackTrace RtlCaptureStackBackTrace
#endif
#ifndef EXECINFO_MOCK_GetCurrentProcess
#define EXECINFO_MOCK_GetCurrentProcess GetCurrentProcess
#endif
#ifndef EXECINFO_MOCK_GetStdHandle
#define EXECINFO_MOCK_GetStdHandle GetStdHandle
#endif
#ifndef EXECINFO_MOCK_WriteFile
#define EXECINFO_MOCK_WriteFile WriteFile
#endif
#ifndef EXECINFO_MOCK_SymSetOptions
#define EXECINFO_MOCK_SymSetOptions SymSetOptions
#endif
#ifndef EXECINFO_MOCK_SymGetOptions
#define EXECINFO_MOCK_SymGetOptions SymGetOptions
#endif
#ifndef EXECINFO_MOCK_SymInitialize
#define EXECINFO_MOCK_SymInitialize SymInitialize
#endif
#ifndef EXECINFO_MOCK_SymFromAddr
#define EXECINFO_MOCK_SymFromAddr SymFromAddr
#endif
#ifndef EXECINFO_MOCK_malloc
static void *default_execinfo_malloc(size_t sz) { return malloc(sz); }
#define EXECINFO_MOCK_malloc default_execinfo_malloc
#endif
#ifndef EXECINFO_MOCK_free
#define EXECINFO_MOCK_free free
#endif

static error_type_t backtrace_win_impl(void **buffer, int size, int *captured) {
  WORD frames;
  if (size <= 0 || buffer == NULL || captured == NULL) {
    return -1;
  }
  frames = EXECINFO_MOCK_RtlCaptureStackBackTrace(1, (DWORD)size, buffer, NULL);
  *captured = (int)frames;
  return ERR_NONE;
}

static char **backtrace_symbols_win_impl(void *const *buffer, int size) {
  HANDLE process;
  char **strings;
  char *p;
  size_t ptrs_size;
  size_t strs_size;
  size_t total_size;
  int i;
  const size_t line_size = 256;
  static int init_sym_done = 0;
  char buffer_sym[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(char)];
  SYMBOL_INFO *symbol;

  if (size <= 0 || buffer == NULL) {
    return NULL;
  }

  process = EXECINFO_MOCK_GetCurrentProcess();

  if (!init_sym_done) {
    DWORD old_options = EXECINFO_MOCK_SymGetOptions();
    (void)EXECINFO_MOCK_SymSetOptions(old_options | SYMOPT_UNDNAME |
                                      SYMOPT_DEFERRED_LOADS);
    (void)EXECINFO_MOCK_SymInitialize(process, NULL, TRUE);
    init_sym_done = 1;
  }

  ptrs_size = (size_t)size * sizeof(char *);
  strs_size = (size_t)size * line_size;
  total_size = ptrs_size + strs_size;

  strings = (char **)EXECINFO_MOCK_malloc(total_size);
  if (strings == NULL) {
    return NULL;
  }

  p = (char *)(strings + size);
  memset(buffer_sym, 0, sizeof(buffer_sym));
  symbol = (SYMBOL_INFO *)buffer_sym;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  symbol->MaxNameLen = MAX_SYM_NAME;

  for (i = 0; i < size; ++i) {
    DWORD64 displacement = 0;
    strings[i] = p;
    if (EXECINFO_MOCK_SymFromAddr(process, (DWORD64)(size_t)buffer[i],
                                  &displacement, symbol)) {
#if defined(_MSC_VER)
      (void)sprintf_s(p, line_size, "%.200s [%p]", symbol->Name, buffer[i]);
#else
      (void)sprintf(p, "%.200s [%p]", symbol->Name, buffer[i]);
#endif
    } else {
#if defined(_MSC_VER)
      (void)sprintf_s(p, line_size, "[%p]", buffer[i]);
#else
      (void)sprintf(p, "[%p]", buffer[i]);
#endif
    }
    p += strlen(p) + 1;
  }

  return strings;
}

static error_type_t backtrace_symbols_fd_win_impl(void *const *buffer, int size,
                                                  int fd) {
  char **strings;
  int i;
  HANDLE hFile;

  if (fd < 0 || size <= 0 || buffer == NULL) {
    return -1;
  }

  if (fd == 2) {
    hFile = EXECINFO_MOCK_GetStdHandle(STD_ERROR_HANDLE);
  } else {
    hFile = EXECINFO_MOCK_GetStdHandle(STD_OUTPUT_HANDLE);
  }

  if (hFile == INVALID_HANDLE_VALUE) {
    return -1;
  }

  strings = backtrace_symbols_win_impl(buffer, size);
  if (strings == NULL) {
    return -1;
  }

  for (i = 0; i < size; ++i) {
    DWORD written = 0;
    DWORD len = (DWORD)strlen(strings[i]);
    if (!EXECINFO_MOCK_WriteFile(hFile, strings[i], len, &written, NULL)) {
      EXECINFO_MOCK_free(strings);
      return -1;
    }
    if (!EXECINFO_MOCK_WriteFile(hFile, "\n", 1, &written, NULL)) {
      EXECINFO_MOCK_free(strings);
      return -1;
    }
  }

  EXECINFO_MOCK_free(strings);
  return ERR_NONE;
}

/**
 * @brief Captures a backtrace for the calling program.
 * @return ERR_NONE on success, or -1 on error.
 */
error_type_t backtrace(void **buffer, int size, int *captured) {
  return backtrace_win_impl(buffer, size, captured);
}

/**
 * @brief Translates return addresses into an allocated array of symbol strings.
 * @return Newly allocated array of strings (caller frees single block), or NULL
 * on failure.
 */
char **backtrace_symbols(void *const *buffer, int size) {
  return backtrace_symbols_win_impl(buffer, size);
}

/**
 * @brief Writes translated backtrace symbols directly to a file descriptor.
 * @return ERR_NONE on success, or -1 on error.
 */
error_type_t backtrace_symbols_fd(void *const *buffer, int size, int fd) {
  return backtrace_symbols_fd_win_impl(buffer, size, fd);
}

#else

/**
 * @brief Captures a backtrace (non-Windows stub).
 * @return ERR_NONE on success, or -1 on invalid arguments.
 */
error_type_t backtrace(void **buffer, int size, int *captured) {
  if (size <= 0 || buffer == NULL || captured == NULL) {
    return -1;
  }
  *captured = 0;
  return ERR_NONE;
}

/**
 * @brief Translates return addresses to symbol strings (non-Windows stub).
 * @return Always NULL on non-Windows stub.
 */
char **backtrace_symbols(void *const *buffer, int size) {
  if (size <= 0 || buffer == NULL) {
    return NULL;
  }
  return NULL;
}

/**
 * @brief Writes translated backtrace symbols to fd (non-Windows stub).
 * @return ERR_NONE on success, or -1 on invalid arguments.
 */
error_type_t backtrace_symbols_fd(void *const *buffer, int size, int fd) {
  if (fd < 0 || size <= 0 || buffer == NULL) {
    return -1;
  }
  return ERR_NONE;
}

#endif

/**
 * @brief Initializes and validates the linux-execinfo module.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT if out_status is NULL.
 */
auto_win_msvc_error_t linux_execinfo_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_execinfo;

#endif
typedef int make_iso_compilers_happy_mega_batch2_17;
