#ifndef LINUX_SYS_PRCTL_H
#define LINUX_SYS_PRCTL_H

/**
 * @file linux-sys-prctl.h
 * @brief Polyfill for Linux <sys/prctl.h> process control operations.
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
 * @brief Error codes returned by linux-sys-prctl functions.
 */
enum linux_sys_prctl_error_code {
  /** @brief Successful operation. */
  LINUX_SYS_PRCTL_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_SYS_PRCTL_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument or option passed to function. */
  LINUX_SYS_PRCTL_ERROR_INVALID_ARGUMENT = 2,
  /** @brief Operation not supported on this platform. */
  LINUX_SYS_PRCTL_ERROR_NOT_SUPPORTED = 3
};

/**
 * @brief Initializes and validates the linux-sys-prctl module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_SYS_PRCTL_SUCCESS on success, or an error code on failure.
 */
enum linux_sys_prctl_error_code linux_sys_prctl_init(int *out_status);

/** @brief Option to set the name of the calling thread. */
#define PR_SET_NAME 15

/** @brief Option to receive a signal when the parent process dies. */
#define PR_SET_PDEATHSIG 1

/** @brief Option to get the current parent death signal. */
#define PR_GET_PDEATHSIG 2

#if defined(_WIN32) || defined(_MSC_VER)

/**
 * @brief Implements process control operations.
 *
 * Currently supports PR_SET_NAME to set the calling thread's name,
 * PR_SET_PDEATHSIG to terminate the child when the parent dies,
 * and PR_GET_PDEATHSIG to retrieve the parent death signal.
 *
 * @param option The operation to perform.
 * @param ... Variable arguments based on the option.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t prctl(int option, ...);

#endif /* _WIN32 || _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_SYS_PRCTL_H */
