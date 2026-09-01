/* clang-format off */
#include "linux-sys-syscall.h"
#include <errno.h>
#ifndef ENOSYS
#define ENOSYS 38
#endif
#include <stdarg.h>
/* clang-format on */
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport) unsigned long __stdcall GetCurrentThreadId(void);
/** \brief syscall function. */
error_type_t syscall(long number, long *out_result, ...) {
  if (number == SYS_gettid) {
    if (out_result) {
      *out_result = (long)GetCurrentThreadId();
    }
    return ERR_NONE;
  }
  return ENOSYS;
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_linux_sys_syscall(void) { return 0; }

typedef int make_iso_compilers_happy_tu_linux_sys_syscall;
