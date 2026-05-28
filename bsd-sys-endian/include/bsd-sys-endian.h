#ifndef BSD_SYS_ENDIAN_H
#define BSD_SYS_ENDIAN_H
#include "auto_win_msvc_export.h"

/* Polyfill for <sys/endian.h> */

#if defined(_MSC_VER)
/* clang-format off */
#include <linux-endian.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_ENDIAN_H */
