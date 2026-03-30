#include "linux-sys-syscall.h"
#include <stdarg.h>
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport) unsigned long __stdcall GetCurrentThreadId(void);
/** \brief syscall function. */
long syscall(long number, ...) {
  if (number == SYS_gettid)
    return (long)GetCurrentThreadId();
  return -1;
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_linux_sys_syscall(void) { return 0; }

typedef int make_iso_compilers_happy_tu_linux_sys_syscall;
