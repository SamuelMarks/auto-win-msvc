/**
 * @file posix-libproc.c
 * @brief Implementation of posix-libproc polyfills.
 */

/* clang-format off */
#include "posix-libproc.h"
#include <stddef.h>
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32) || defined(_WIN64)

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long WIN_DWORD;
__declspec(dllimport) WIN_DWORD __stdcall GetCurrentProcessId(void);
__declspec(dllimport) WIN_DWORD __stdcall GetModuleFileNameA(void *hModule,
                                                             char *lpFilename,
                                                             WIN_DWORD nSize);

#ifdef __cplusplus
}
#endif

int proc_pidpath(int pid, void *buffer, unsigned int buffersize) {
  WIN_DWORD len;
  WIN_DWORD cur_pid;

  if (buffer == NULL || buffersize == 0) {
    return 0;
  }

  cur_pid = GetCurrentProcessId();
  if (pid != 0 && (WIN_DWORD)pid != cur_pid) {
    return 0;
  }

  len = GetModuleFileNameA(NULL, (char *)buffer, (WIN_DWORD)buffersize);
  if (len == 0 || len >= (WIN_DWORD)buffersize) {
    return 0;
  }

  return (int)len;
}

#elif !defined(__APPLE__)

int proc_pidpath(int pid, void *buffer, unsigned int buffersize) {
  if (pid < 0 || buffer == NULL || buffersize == 0) {
    return 0;
  }
  return 0;
}

#endif

/**
 * @brief Retrieves information on posix-libproc availability.
 */
auto_win_msvc_error_t posix_libproc_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_libproc;
