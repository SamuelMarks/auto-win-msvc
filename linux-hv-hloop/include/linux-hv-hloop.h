#ifndef LINUX_HV_HLOOP_H
#define LINUX_HV_HLOOP_H

/**
 * @file linux-hv-hloop.h
 * @brief Compatibility header for libhv hloop event loop.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by linux-hv-hloop functions.
 */
enum linux_hv_hloop_error_code {
  /** @brief Successful operation. */
  LINUX_HV_HLOOP_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_HV_HLOOP_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed to function. */
  LINUX_HV_HLOOP_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Initializes and validates the linux-hv-hloop module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_HV_HLOOP_SUCCESS on success, or an error code on failure.
 */
enum linux_hv_hloop_error_code linux_hv_hloop_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_HV_HLOOP_H */
