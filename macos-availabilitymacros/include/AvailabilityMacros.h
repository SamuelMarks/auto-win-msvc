#ifndef AVAILABILITYMACROS_H
#define AVAILABILITYMACROS_H

/**
 * @file AvailabilityMacros.h
 * @brief Polyfill for macOS <AvailabilityMacros.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAC_OS_X_VERSION_10_0
/** @brief macOS 10.0 version constant. */
#define MAC_OS_X_VERSION_10_0 1000
#endif

#ifndef MAC_OS_X_VERSION_MAX_ALLOWED
/** @brief Maximum allowed macOS version constant. */
#define MAC_OS_X_VERSION_MAX_ALLOWED MAC_OS_X_VERSION_10_0
#endif

/**
 * @brief Error codes returned by macos-availabilitymacros functions.
 */
enum macos_availabilitymacros_error_code {
  /** @brief Successful operation. */
  MACOS_AVAILABILITYMACROS_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  MACOS_AVAILABILITYMACROS_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves the MAC_OS_X_VERSION_MAX_ALLOWED constant value.
 * @param[out] out_version Pointer to an int that receives the version.
 * @return MACOS_AVAILABILITYMACROS_SUCCESS on success, or an error code on
 * failure.
 */
enum macos_availabilitymacros_error_code
macos_availabilitymacros_get_version(int *out_version);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AVAILABILITYMACROS_H */
