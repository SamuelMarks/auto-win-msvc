#ifndef MACH_O_DYLD_H
#define MACH_O_DYLD_H

/**
 * @file mach-o/dyld.h
 * @brief Polyfill for macOS <mach-o/dyld.h>.
 */

/* clang-format off */
#include <stddef.h>
#include <posix-stdbool.h>
#include <posix-stdint.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by macos-dyld functions.
 */
enum macos_dyld_error_code {
  /** @brief Successful operation. */
  MACOS_DYLD_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  MACOS_DYLD_ERROR_NULL_POINTER = 1,
  /** @brief Operation not supported on this platform. */
  MACOS_DYLD_ERROR_NOT_SUPPORTED = 2
};

/**
 * @brief Initializes and validates the macos-dyld module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return MACOS_DYLD_SUCCESS on success, or an error code on failure.
 */
enum macos_dyld_error_code macos_dyld_init(int *out_status);

/**
 * @brief Checks whether dyld is present.
 * @return True if present, false otherwise.
 */
bool _dyld_present(void);

/**
 * @brief Gets the virtual memory address slide for an image.
 * @param image_index Index of the loaded image.
 * @return Virtual memory address slide, or -1 on error.
 */
intptr_t _dyld_get_image_vmaddr_slide(uint32_t image_index);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACH_O_DYLD_H */
