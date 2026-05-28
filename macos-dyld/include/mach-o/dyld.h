/* mach-o/dyld.h - Strict C89 Header */
#ifndef MACH_O_DYLD_H
#define MACH_O_DYLD_H
#include "auto_win_msvc_export.h"

/* clang-format off */
#include <posix-stdbool.h>
#include <posix-stdint.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

AUTO_WIN_MSVC_EXPORT bool _dyld_present(void);
AUTO_WIN_MSVC_EXPORT intptr_t AUTO_WIN_MSVC_EXPORT
_dyld_get_image_vmaddr_slide(uint32_t image_index);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACH_O_DYLD_H */
