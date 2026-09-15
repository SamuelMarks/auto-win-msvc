/**
 * @file posix-unwind.c
 * @brief Implementation of posix-unwind polyfills.
 */

/* clang-format off */
#include "posix-unwind.h"
#include <stddef.h>
/* clang-format on */

#if defined(_MSC_VER)

#ifdef __cplusplus
extern "C" {
#endif

typedef void *WIN_HMODULE;
typedef int(__stdcall *WIN_FARPROC)(void);

__declspec(dllimport)
WIN_HMODULE __stdcall GetModuleHandleA(const char *lpModuleName);
__declspec(dllimport)
WIN_FARPROC __stdcall GetProcAddress(WIN_HMODULE hModule,
                                     const char *lpProcName);

#ifdef __cplusplus
}
#endif

struct _Unwind_Context {
  void *ip;
};

size_t _Unwind_GetIP(struct _Unwind_Context *context) {
  if (context != NULL) {
    return (size_t)context->ip;
  }
  return 0;
}

_Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn trace,
                                      void *trace_argument) {
  void *frames[64];
  unsigned short count;
  unsigned short i;
  WIN_HMODULE hNtDll;
  typedef unsigned short(__stdcall * RtlCaptureStackBackTrace_t)(
      unsigned long, unsigned long, void **, unsigned long *);
  RtlCaptureStackBackTrace_t pRtlCaptureStackBackTrace = NULL;

  if (trace == NULL) {
    return _URC_FATAL_PHASE1_ERROR;
  }

  hNtDll = GetModuleHandleA("ntdll.dll");
  if (hNtDll != NULL) {
    pRtlCaptureStackBackTrace =
        (RtlCaptureStackBackTrace_t)(size_t)GetProcAddress(
            hNtDll, "RtlCaptureStackBackTrace");
  }

  if (pRtlCaptureStackBackTrace == NULL) {
    return _URC_END_OF_STACK;
  }

  count = pRtlCaptureStackBackTrace(0, 64, frames, NULL);
  for (i = 0; i < count; i++) {
    struct _Unwind_Context ctx;
    ctx.ip = frames[i];
    if (trace(&ctx, trace_argument) != _URC_NO_REASON) {
      return _URC_NORMAL_STOP;
    }
  }
  return _URC_END_OF_STACK;
}

#endif /* _MSC_VER */

/**
 * @brief Retrieves information on posix-unwind availability.
 */
enum posix_unwind_error_code posix_unwind_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_UNWIND_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_UNWIND_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_unwind;
