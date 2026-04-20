#ifndef POSIX_STDBOOL_H
#define POSIX_STDBOOL_H

/* Polyfill for <stdbool.h> */

#if defined(_MSC_VER)

#if _MSC_VER < 1800
#ifndef __cplusplus
#define bool unsigned char
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#endif /* !__cplusplus */
#else
/* For newer MSVC */
/* clang-format off */
#include <stdbool.h>
#endif

#else /* !_MSC_VER */
/* For GCC/Clang */
#ifndef __cplusplus
#include <stdbool.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#endif
#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif

#endif /* POSIX_STDBOOL_H */
