#ifndef POSIX_SYS_SELECT_H
#define POSIX_SYS_SELECT_H

/**
 * @file posix-sys-select.h
 * @brief POSIX sys/select.h compatibility layer.
 */

/* clang-format off */
#include <sys/select.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-sys-select functions.
 */
enum posix_sys_select_error_code {
  /** @brief Operation completed successfully. */
  POSIX_SYS_SELECT_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_SYS_SELECT_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-sys-select availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_SYS_SELECT_SUCCESS on success, or
 * POSIX_SYS_SELECT_ERROR_NULL_POINTER on NULL pointer.
 */
enum posix_sys_select_error_code posix_sys_select_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_SELECT_H */
