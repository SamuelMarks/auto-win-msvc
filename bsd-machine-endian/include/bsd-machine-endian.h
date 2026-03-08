#ifndef BSD_MACHINE_ENDIAN_H
#define BSD_MACHINE_ENDIAN_H

/* Polyfill for <machine/endian.h> */

#if defined(_MSC_VER)
/* We defer to the standard endian.h polyfill which provides the necessary macros */
#include <linux-endian.h>
#endif /* _MSC_VER */

#endif /* BSD_MACHINE_ENDIAN_H */