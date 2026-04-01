#ifndef BSD_MACHINE_ENDIAN_H
#define BSD_MACHINE_ENDIAN_H

/* Polyfill for <machine/endian.h> */

#if defined(_MSC_VER)
/* We defer to the standard endian.h polyfill which provides the necessary
 * macros */
/* clang-format off */
#include <linux-endian.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif

#endif /* BSD_MACHINE_ENDIAN_H */
