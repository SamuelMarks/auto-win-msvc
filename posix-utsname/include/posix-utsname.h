
#ifndef POSIX_UTSNAME_H
#define POSIX_UTSNAME_H

/**
 * @file posix-utsname.h
 * @brief POSIX sys/utsname.h compatibility layer for MSVC.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def _UTSNAME_LENGTH
 * @brief Length of the strings in struct utsname.
 *
 * Defines the maximum length of the character arrays in struct utsname,
 * including the terminating null byte.
 */
#ifndef _UTSNAME_LENGTH
#define _UTSNAME_LENGTH 256
#endif

/**
 * @struct utsname
 * @brief Structure describing the system and machine.
 *
 * This structure is populated by the uname() function with information
 * about the current operating system and hardware.
 */
struct utsname {
  /** @brief Name of the operating system implementation. */
  char sysname[_UTSNAME_LENGTH];
  /** @brief Network name of this machine. */
  char nodename[_UTSNAME_LENGTH];
  /** @brief Current release level of the operating system. */
  char release[_UTSNAME_LENGTH];
  /** @brief Current version level of the operating system. */
  char version[_UTSNAME_LENGTH];
  /** @brief Hardware type/architecture. */
  char machine[_UTSNAME_LENGTH];
};

/**
 * @brief Retrieves information on posix-utsname availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_utsname_get_info(int *out_available);

/**
 * @brief Get system identification.
 *
 * Populates the provided utsname structure with system information.
 *
 * @param name Pointer to a utsname structure to be filled.
 * @return 0 on success, or -1 on error (with errno set appropriately).
 */
int uname(struct utsname *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_UTSNAME_H */
