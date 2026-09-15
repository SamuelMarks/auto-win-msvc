#ifndef LINUX_SYS_SYSCALL_H
#define LINUX_SYS_SYSCALL_H

/**
 * @file linux-sys-syscall.h
 * @brief Polyfill for Linux <sys/syscall.h> system call interface.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
/** @brief Generic error type code. */
typedef int error_type_t;
/** @brief Success indicator for error_type_t. */
#define ERR_NONE 0
#endif

/**
 * @brief Error codes returned by linux-sys-syscall functions.
 */
enum linux_sys_syscall_error_code {
  /** @brief Successful operation. */
  LINUX_SYS_SYSCALL_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_SYS_SYSCALL_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument or unknown syscall number. */
  LINUX_SYS_SYSCALL_ERROR_INVALID_ARGUMENT = 2,
  /** @brief System call not implemented. */
  LINUX_SYS_SYSCALL_ERROR_NOT_IMPLEMENTED = 3
};

/**
 * @brief Initializes and validates the linux-sys-syscall module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_SYS_SYSCALL_SUCCESS on success, or an error code on failure.
 */
enum linux_sys_syscall_error_code linux_sys_syscall_init(int *out_status);

#if defined(_MSC_VER) && !defined(__clang__)

/** @brief System call number for gettid. */
#define SYS_gettid 178

/**
 * @brief Invokes a system call by number.
 * @param number System call number.
 * @param[out] out_result Output pointer receiving the result of syscall.
 * @param ... Additional arguments depending on system call.
 * @return ERR_NONE on success, or an error code on failure.
 */
error_type_t syscall(long number, long *out_result, ...);

#endif /* _MSC_VER && !__clang__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_SYS_SYSCALL_H */
