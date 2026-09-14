#if defined(__GNUC__)
#pragma GCC system_header
#endif
/* posix-stdint.h - Strict C89 Header */
#ifndef POSIX_STDINT_H
#define POSIX_STDINT_H

#if defined(_MSC_VER)

#ifndef _STDINT
#define _STDINT
#endif

#if _MSC_VER >= 1900
/* clang-format off */
#include <vcruntime.h>
/* clang-format on */
#endif

#ifndef _INT8_T_DEFINED
#define _INT8_T_DEFINED
typedef signed char int8_t;
#endif

#ifndef _UINT8_T_DEFINED
#define _UINT8_T_DEFINED
typedef unsigned char uint8_t;
#endif

#ifndef _INT16_T_DEFINED
#define _INT16_T_DEFINED
typedef short int16_t;
#endif

#ifndef _UINT16_T_DEFINED
#define _UINT16_T_DEFINED
typedef unsigned short uint16_t;
#endif

#ifndef _INT32_T_DEFINED
#define _INT32_T_DEFINED
typedef int int32_t;
#endif

#ifndef _UINT32_T_DEFINED
#define _UINT32_T_DEFINED
typedef unsigned int uint32_t;
#endif

#ifndef _INT64_T_DEFINED
#define _INT64_T_DEFINED
typedef __int64 int64_t;
#endif

#ifndef _UINT64_T_DEFINED
#define _UINT64_T_DEFINED
typedef unsigned __int64 uint64_t;
#endif

typedef int8_t int_least8_t;
typedef uint8_t uint_least8_t;
typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;
typedef int32_t int_least32_t;
typedef uint32_t uint_least32_t;
typedef int64_t int_least64_t;
typedef uint64_t uint_least64_t;

typedef int8_t int_fast8_t;
typedef uint8_t uint_fast8_t;
typedef int32_t int_fast16_t;
typedef uint32_t uint_fast16_t;
typedef int32_t int_fast32_t;
typedef uint32_t uint_fast32_t;
typedef int64_t int_fast64_t;
typedef uint64_t uint_fast64_t;

#ifndef _INTPTR_T_DEFINED
#define _INTPTR_T_DEFINED
#ifdef _WIN64
typedef __int64 intptr_t;
#else
typedef int intptr_t;
#endif
#endif

#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
#ifdef _WIN64
typedef unsigned __int64 uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif
#endif

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

#ifndef INT8_MIN
#define INT8_MIN (-128)
#endif
#ifndef INT8_MAX
#define INT8_MAX 127
#endif
#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif
#ifndef INT16_MIN
#define INT16_MIN (-32768)
#endif
#ifndef INT16_MAX
#define INT16_MAX 32767
#endif
#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif
#ifndef INT32_MIN
#define INT32_MIN (-2147483647 - 1)
#endif
#ifndef INT32_MAX
#define INT32_MAX 2147483647
#endif
#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffffU
#endif

#else /* !_MSC_VER */

#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#include_next <stdint.h>
/* clang-format on */
#else
/* clang-format off */
#include <stdint.h>
/* clang-format on */
#endif

#endif /* _MSC_VER */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STDINT_H */
