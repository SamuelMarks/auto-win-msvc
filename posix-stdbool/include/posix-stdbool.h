#if defined(__GNUC__)
#pragma GCC system_header
#endif
/* posix-stdbool.h - Strict C89 Header */
#ifndef POSIX_STDBOOL_H
#define POSIX_STDBOOL_H

#ifndef _STDBOOL
#define _STDBOOL

#define __bool_true_false_are_defined 1

#ifndef __cplusplus

#if defined(_MSC_VER) && _MSC_VER < 1800
#define bool unsigned char
#elif defined(_MSC_VER)
#define bool _Bool
#elif defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#include_next <stdbool.h>
/* clang-format on */
#else
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#endif /* !__cplusplus */

#endif /* _STDBOOL */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STDBOOL_H */
