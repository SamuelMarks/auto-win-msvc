/* clang-format off */
#include "linux-sys-syscall.h"
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
/* clang-format on */

#ifndef ENOSYS
#define ENOSYS 38
#endif

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport) unsigned long __stdcall GetCurrentThreadId(void);

/** @brief syscall function. */
error_type_t syscall(long number, long *out_result, ...) {
  if (number == SYS_gettid) {
    if (out_result != NULL) {
      *out_result = (long)GetCurrentThreadId();
    }
    return ERR_NONE;
  }
  return ENOSYS;
}
#endif

/**
 * @brief Initializes and validates the linux-sys-syscall module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_SYS_SYSCALL_SUCCESS on success, or an error code on failure.
 */
enum linux_sys_syscall_error_code linux_sys_syscall_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_SYS_SYSCALL_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return LINUX_SYS_SYSCALL_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_sys_syscall;
