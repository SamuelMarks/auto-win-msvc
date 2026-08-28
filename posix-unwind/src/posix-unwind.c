/* posix-unwind.c - Strict C89 Implementation */

/* clang-format off */
#include "unwind.h"

#if defined(_MSC_VER)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
/* clang-format on */

#endif

#if defined(_MSC_VER)

/** \brief Context struct for MSVC */
struct _Unwind_Context {
  void *ip;
};

/**
 * @brief Get the instruction pointer from the context
 * @param context The unwind context
 * @return The instruction pointer
 */
size_t _Unwind_GetIP(struct _Unwind_Context *context) {
  if (context) {
    return (size_t)context->ip;
  }
  return 0;
}

/**
 * @brief Backtrace the stack
 * @param trace The trace function
 * @param trace_argument The trace argument
 * @return The reason code
 */
_Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn trace,
                                      void *trace_argument) {
  void *frames[64];
  unsigned short count;
  unsigned short i;
  void *hNtDll;
  typedef unsigned short(__stdcall * RtlCaptureStackBackTrace_t)(
      unsigned long, unsigned long, void **, unsigned long *);
  RtlCaptureStackBackTrace_t pRtlCaptureStackBackTrace = NULL;

  if (!trace) {
    return _URC_FATAL_PHASE1_ERROR;
  }

  hNtDll = GetModuleHandleA("ntdll.dll");
  if (hNtDll) {
    pRtlCaptureStackBackTrace =
        (RtlCaptureStackBackTrace_t)(size_t)GetProcAddress(
            (HMODULE)hNtDll, "RtlCaptureStackBackTrace");
  }

  if (!pRtlCaptureStackBackTrace) {
    return _URC_END_OF_STACK; /* Fallback if not available */
  }

  count = pRtlCaptureStackBackTrace(1, 64, frames, NULL);
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

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu_posix_unwind;
