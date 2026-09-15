#ifndef LINUX_SYS_USER_H
#define LINUX_SYS_USER_H

/**
 * @file linux-sys-user.h
 * @brief Polyfill for Linux <sys/user.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by linux-sys-user functions.
 */
enum linux_sys_user_error_code {
  /** @brief Successful operation. */
  LINUX_SYS_USER_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_SYS_USER_ERROR_NULL_POINTER = 1,
  /** @brief Operation not supported on this platform. */
  LINUX_SYS_USER_ERROR_NOT_SUPPORTED = 2
};

/**
 * @brief Initializes and validates the linux-sys-user module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_SYS_USER_SUCCESS on success, or an error code on failure.
 */
enum linux_sys_user_error_code linux_sys_user_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_SYS_USER_H */
