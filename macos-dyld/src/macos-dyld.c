/* clang-format off */
#include "mach-o/dyld.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#ifndef ENOSYS
#define ENOSYS 38
#endif

/**
 * @brief Initializes and validates the macos-dyld module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return MACOS_DYLD_SUCCESS on success, or an error code on failure.
 */
enum macos_dyld_error_code macos_dyld_init(int *out_status) {
  if (out_status == NULL) {
    return MACOS_DYLD_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return MACOS_DYLD_SUCCESS;
}

/**
 * @brief Checks whether dyld is present.
 * @return True if present, false otherwise.
 */
bool _dyld_present(void) { return false; }

/**
 * @brief Gets the virtual memory address slide for an image.
 * @param image_index Index of the loaded image.
 * @return Virtual memory address slide, or -1 on error.
 */
intptr_t _dyld_get_image_vmaddr_slide(uint32_t image_index) {
  (void)image_index;
  errno = ENOSYS;
  return -1;
}

typedef int make_iso_compilers_happy_tu_macos_dyld;
