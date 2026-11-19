#ifndef BSD_SYS_CONF_H
#define BSD_SYS_CONF_H

/**
 * @file conf.h
 * @brief BSD <sys/conf.h> device flag constants polyfill.
 */

/* clang-format off */
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define D_TAPE 0x0001
#define D_DISK 0x0002
#define D_TTY 0x0004
#define D_MEM 0x0008

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_CONF_H */
