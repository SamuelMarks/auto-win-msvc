/* mach-o/dyld.h - Strict C89 Header */
#ifndef MACH_O_DYLD_H
#define MACH_O_DYLD_H

/* clang-format off */
#include <posix-stdbool.h>
#include <posix-stdint.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

bool _dyld_present(void);
intptr_t _dyld_get_image_vmaddr_slide(uint32_t image_index);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACH_O_DYLD_H */
