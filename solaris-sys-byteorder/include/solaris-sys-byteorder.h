#ifndef SOLARIS_SYS_BYTEORDER_H
#define SOLARIS_SYS_BYTEORDER_H

/* Polyfill for <sys/byteorder.h> */

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

#endif /* SOLARIS_SYS_BYTEORDER_H */
