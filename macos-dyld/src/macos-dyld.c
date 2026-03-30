/* macos-dyld.c - Strict C89 Implementation */

/* clang-format off */
#include "mach-o/dyld.h"
/* clang-format on */

/** \brief Polyfill for _dyld_present
 * \return 0 or appropriate
 * default value
 */
bool _dyld_present(void) { return false; }

/** \brief Polyfill for _dyld_get_image_vmaddr_slide
 * \return 0 or
 * appropriate default value
 */
intptr_t _dyld_get_image_vmaddr_slide(uint32_t image_index) {
  (void)image_index;
  return 0;
}
