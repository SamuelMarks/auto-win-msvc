#ifndef BSD_SYS_ENDIAN_H
#define BSD_SYS_ENDIAN_H

/* Polyfill for <sys/endian.h> */

#if defined(_MSC_VER)
#include <linux-endian.h>
#endif

#endif /* BSD_SYS_ENDIAN_H */