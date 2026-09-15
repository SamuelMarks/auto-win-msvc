#ifndef HAIKU_OS_H
#define HAIKU_OS_H

/**
 * @file haiku-os.h
 * @brief Polyfill for Haiku OS APIs and types.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by haiku-os functions.
 */
enum haiku_os_error_code {
  /** @brief Successful operation. */
  HAIKU_OS_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  HAIKU_OS_ERROR_NULL_POINTER = 1
};

/**
 * @brief Initializes and validates the haiku-os module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return HAIKU_OS_SUCCESS on success, or an error code on failure.
 */
enum haiku_os_error_code haiku_os_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAIKU_OS_H */
