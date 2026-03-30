#ifndef SOLARIS_SYS_BYTEORDER_H
#define SOLARIS_SYS_BYTEORDER_H

/* Polyfill for <sys/byteorder.h> */

#if defined(_MSC_VER)
#include <linux-endian.h>

#ifdef __cplusplus
extern "C" {
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* SOLARIS_SYS_BYTEORDER_H */
