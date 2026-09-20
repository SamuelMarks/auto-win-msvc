#ifndef POSIX_STDINT_H
#define POSIX_STDINT_H

/**
 * @file posix-stdint.h
 * @brief Polyfill for C99 <stdint.h> integer types and limits.
 */

/* clang-format off */
#include <stddef.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <vcruntime.h>
#elif !defined(_WIN32)
#include <sys/types.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _STDINT
#define _STDINT
#endif

#if !defined(__int8_t_defined) && !defined(_INT8_T_DEFINED)
#define _INT8_T_DEFINED
#define __int8_t_defined
/** @brief Signed 8-bit integer type. */
typedef signed char int8_t;
#endif

#if !defined(__uint8_t_defined) && !defined(_UINT8_T_DEFINED)
#define _UINT8_T_DEFINED
#define __uint8_t_defined
/** @brief Unsigned 8-bit integer type. */
typedef unsigned char uint8_t;
#endif

#if !defined(__int16_t_defined) && !defined(_INT16_T_DEFINED)
#define _INT16_T_DEFINED
#define __int16_t_defined
/** @brief Signed 16-bit integer type. */
typedef short int16_t;
#endif

#if !defined(__uint16_t_defined) && !defined(_UINT16_T_DEFINED)
#define _UINT16_T_DEFINED
#define __uint16_t_defined
/** @brief Unsigned 16-bit integer type. */
typedef unsigned short uint16_t;
#endif

#if !defined(__int32_t_defined) && !defined(_INT32_T_DEFINED)
#define _INT32_T_DEFINED
#define __int32_t_defined
/** @brief Signed 32-bit integer type. */
typedef int int32_t;
#endif

#if !defined(__uint32_t_defined) && !defined(_UINT32_T_DEFINED)
#define _UINT32_T_DEFINED
#define __uint32_t_defined
/** @brief Unsigned 32-bit integer type. */
typedef unsigned int uint32_t;
#endif

#if !defined(__int64_t_defined) && !defined(_INT64_T) &&                       \
    !defined(_INT64_T_DEFINED)
#define _INT64_T_DEFINED
#define _INT64_T
#define __int64_t_defined
#if defined(_MSC_VER)
/** @brief Signed 64-bit integer type. */
typedef __int64 int64_t;
#elif defined(__APPLE__)
/** @brief Signed 64-bit integer type. */
__extension__ typedef long long int64_t;
#elif defined(__LP64__)
/** @brief Signed 64-bit integer type. */
typedef long int64_t;
#else
/** @brief Signed 64-bit integer type. */
__extension__ typedef long long int64_t;
#endif
#endif

#if !defined(__uint64_t_defined) && !defined(_UINT64_T) &&                     \
    !defined(_UINT64_T_DEFINED)
#define _UINT64_T_DEFINED
#define _UINT64_T
#define __uint64_t_defined
#if defined(_MSC_VER)
/** @brief Unsigned 64-bit integer type. */
typedef unsigned __int64 uint64_t;
#elif defined(__APPLE__)
/** @brief Unsigned 64-bit integer type. */
__extension__ typedef unsigned long long uint64_t;
#elif defined(__LP64__)
/** @brief Unsigned 64-bit integer type. */
typedef unsigned long uint64_t;
#else
/** @brief Unsigned 64-bit integer type. */
__extension__ typedef unsigned long long uint64_t;
#endif
#endif

/** @brief Least signed 8-bit integer type. */
typedef int8_t int_least8_t;
/** @brief Least unsigned 8-bit integer type. */
typedef uint8_t uint_least8_t;
/** @brief Least signed 16-bit integer type. */
typedef int16_t int_least16_t;
/** @brief Least unsigned 16-bit integer type. */
typedef uint16_t uint_least16_t;
/** @brief Least signed 32-bit integer type. */
typedef int32_t int_least32_t;
/** @brief Least unsigned 32-bit integer type. */
typedef uint32_t uint_least32_t;
/** @brief Least signed 64-bit integer type. */
typedef int64_t int_least64_t;
/** @brief Least unsigned 64-bit integer type. */
typedef uint64_t uint_least64_t;

/** @brief Fastest signed 8-bit integer type. */
typedef int8_t int_fast8_t;
/** @brief Fastest unsigned 8-bit integer type. */
typedef uint8_t uint_fast8_t;
/** @brief Fastest signed 16-bit integer type. */
typedef int32_t int_fast16_t;
/** @brief Fastest unsigned 16-bit integer type. */
typedef uint32_t uint_fast16_t;
/** @brief Fastest signed 32-bit integer type. */
typedef int32_t int_fast32_t;
/** @brief Fastest unsigned 32-bit integer type. */
typedef uint32_t uint_fast32_t;
/** @brief Fastest signed 64-bit integer type. */
typedef int64_t int_fast64_t;
/** @brief Fastest unsigned 64-bit integer type. */
typedef uint64_t uint_fast64_t;

#ifndef _INTPTR_T_DEFINED
#define _INTPTR_T_DEFINED
#if defined(__INTPTR_TYPE__)
/** @brief Pointer-sized signed integer type. */
typedef __INTPTR_TYPE__ intptr_t;
#elif defined(_MSC_VER)
#ifdef _WIN64
/** @brief Pointer-sized signed integer type. */
typedef __int64 intptr_t;
#else
/** @brief Pointer-sized signed integer type. */
typedef int intptr_t;
#endif
#elif defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) ||           \
    defined(__aarch64__)
/** @brief Pointer-sized signed integer type. */
typedef long intptr_t;
#else
/** @brief Pointer-sized signed integer type. */
typedef int intptr_t;
#endif
#endif

#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
#if defined(__UINTPTR_TYPE__)
/** @brief Pointer-sized unsigned integer type. */
typedef __UINTPTR_TYPE__ uintptr_t;
#elif defined(_MSC_VER)
#ifdef _WIN64
/** @brief Pointer-sized unsigned integer type. */
typedef unsigned __int64 uintptr_t;
#else
/** @brief Pointer-sized unsigned integer type. */
typedef unsigned int uintptr_t;
#endif
#elif defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) ||           \
    defined(__aarch64__)
/** @brief Pointer-sized unsigned integer type. */
typedef unsigned long uintptr_t;
#else
/** @brief Pointer-sized unsigned integer type. */
typedef unsigned int uintptr_t;
#endif
#endif

#ifndef _INTMAX_T_DEFINED
#define _INTMAX_T_DEFINED
/** @brief Greatest-width signed integer type. */
typedef int64_t intmax_t;
#endif

#ifndef _UINTMAX_T_DEFINED
#define _UINTMAX_T_DEFINED
/** @brief Greatest-width unsigned integer type. */
typedef uint64_t uintmax_t;
#endif

#ifndef INT8_MIN
/** @brief Minimum value of signed 8-bit integer. */
#define INT8_MIN (-128)
#endif
#ifndef INT8_MAX
/** @brief Maximum value of signed 8-bit integer. */
#define INT8_MAX 127
#endif
#ifndef UINT8_MAX
/** @brief Maximum value of unsigned 8-bit integer. */
#define UINT8_MAX 255
#endif
#ifndef INT16_MIN
/** @brief Minimum value of signed 16-bit integer. */
#define INT16_MIN (-32768)
#endif
#ifndef INT16_MAX
/** @brief Maximum value of signed 16-bit integer. */
#define INT16_MAX 32767
#endif
#ifndef UINT16_MAX
/** @brief Maximum value of unsigned 16-bit integer. */
#define UINT16_MAX 65535
#endif
#ifndef INT32_MIN
/** @brief Minimum value of signed 32-bit integer. */
#define INT32_MIN (-2147483647 - 1)
#endif
#ifndef INT32_MAX
/** @brief Maximum value of signed 32-bit integer. */
#define INT32_MAX 2147483647
#endif
#ifndef UINT32_MAX
/** @brief Maximum value of unsigned 32-bit integer. */
#define UINT32_MAX 0xffffffffU
#endif

#ifndef INT64_MIN
#if defined(_MSC_VER)
/** @brief Minimum value of signed 64-bit integer. */
#define INT64_MIN (-9223372036854775807i64 - 1)
#else
/** @brief Minimum value of signed 64-bit integer. */
#define INT64_MIN (__extension__(-9223372036854775807LL - 1))
#endif
#endif

#ifndef INT64_MAX
#if defined(_MSC_VER)
/** @brief Maximum value of signed 64-bit integer. */
#define INT64_MAX 9223372036854775807i64
#else
/** @brief Maximum value of signed 64-bit integer. */
#define INT64_MAX (__extension__ 9223372036854775807LL)
#endif
#endif

#ifndef UINT64_MAX
#if defined(_MSC_VER)
/** @brief Maximum value of unsigned 64-bit integer. */
#define UINT64_MAX 0xffffffffffffffffui64
#else
/** @brief Maximum value of unsigned 64-bit integer. */
#define UINT64_MAX (__extension__ 0xffffffffffffffffULL)
#endif
#endif

#ifndef INT_LEAST8_MIN
/** @brief Minimum value of least signed 8-bit integer. */
#define INT_LEAST8_MIN INT8_MIN
#endif
#ifndef INT_LEAST8_MAX
/** @brief Maximum value of least signed 8-bit integer. */
#define INT_LEAST8_MAX INT8_MAX
#endif
#ifndef UINT_LEAST8_MAX
/** @brief Maximum value of least unsigned 8-bit integer. */
#define UINT_LEAST8_MAX UINT8_MAX
#endif
#ifndef INT_LEAST16_MIN
/** @brief Minimum value of least signed 16-bit integer. */
#define INT_LEAST16_MIN INT16_MIN
#endif
#ifndef INT_LEAST16_MAX
/** @brief Maximum value of least signed 16-bit integer. */
#define INT_LEAST16_MAX INT16_MAX
#endif
#ifndef UINT_LEAST16_MAX
/** @brief Maximum value of least unsigned 16-bit integer. */
#define UINT_LEAST16_MAX UINT16_MAX
#endif
#ifndef INT_LEAST32_MIN
/** @brief Minimum value of least signed 32-bit integer. */
#define INT_LEAST32_MIN INT32_MIN
#endif
#ifndef INT_LEAST32_MAX
/** @brief Maximum value of least signed 32-bit integer. */
#define INT_LEAST32_MAX INT32_MAX
#endif
#ifndef UINT_LEAST32_MAX
/** @brief Maximum value of least unsigned 32-bit integer. */
#define UINT_LEAST32_MAX UINT32_MAX
#endif
#ifndef INT_LEAST64_MIN
/** @brief Minimum value of least signed 64-bit integer. */
#define INT_LEAST64_MIN INT64_MIN
#endif
#ifndef INT_LEAST64_MAX
/** @brief Maximum value of least signed 64-bit integer. */
#define INT_LEAST64_MAX INT64_MAX
#endif
#ifndef UINT_LEAST64_MAX
/** @brief Maximum value of least unsigned 64-bit integer. */
#define UINT_LEAST64_MAX UINT64_MAX
#endif

#ifndef INT_FAST8_MIN
/** @brief Minimum value of fastest signed 8-bit integer. */
#define INT_FAST8_MIN INT8_MIN
#endif
#ifndef INT_FAST8_MAX
/** @brief Maximum value of fastest signed 8-bit integer. */
#define INT_FAST8_MAX INT8_MAX
#endif
#ifndef UINT_FAST8_MAX
/** @brief Maximum value of fastest unsigned 8-bit integer. */
#define UINT_FAST8_MAX UINT8_MAX
#endif
#ifndef INT_FAST16_MIN
/** @brief Minimum value of fastest signed 16-bit integer. */
#define INT_FAST16_MIN INT32_MIN
#endif
#ifndef INT_FAST16_MAX
/** @brief Maximum value of fastest signed 16-bit integer. */
#define INT_FAST16_MAX INT32_MAX
#endif
#ifndef UINT_FAST16_MAX
/** @brief Maximum value of fastest unsigned 16-bit integer. */
#define UINT_FAST16_MAX UINT32_MAX
#endif
#ifndef INT_FAST32_MIN
/** @brief Minimum value of fastest signed 32-bit integer. */
#define INT_FAST32_MIN INT32_MIN
#endif
#ifndef INT_FAST32_MAX
/** @brief Maximum value of fastest signed 32-bit integer. */
#define INT_FAST32_MAX INT32_MAX
#endif
#ifndef UINT_FAST32_MAX
/** @brief Maximum value of fastest unsigned 32-bit integer. */
#define UINT_FAST32_MAX UINT32_MAX
#endif
#ifndef INT_FAST64_MIN
/** @brief Minimum value of fastest signed 64-bit integer. */
#define INT_FAST64_MIN INT64_MIN
#endif
#ifndef INT_FAST64_MAX
/** @brief Maximum value of fastest signed 64-bit integer. */
#define INT_FAST64_MAX INT64_MAX
#endif
#ifndef UINT_FAST64_MAX
/** @brief Maximum value of fastest unsigned 64-bit integer. */
#define UINT_FAST64_MAX UINT64_MAX
#endif

#ifndef INTPTR_MIN
#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) ||             \
    defined(__aarch64__)
/** @brief Minimum value of pointer-sized signed integer. */
#define INTPTR_MIN INT64_MIN
/** @brief Maximum value of pointer-sized signed integer. */
#define INTPTR_MAX INT64_MAX
/** @brief Maximum value of pointer-sized unsigned integer. */
#define UINTPTR_MAX UINT64_MAX
#else
/** @brief Minimum value of pointer-sized signed integer. */
#define INTPTR_MIN INT32_MIN
/** @brief Maximum value of pointer-sized signed integer. */
#define INTPTR_MAX INT32_MAX
/** @brief Maximum value of pointer-sized unsigned integer. */
#define UINTPTR_MAX UINT32_MAX
#endif
#endif

#ifndef INTMAX_MIN
/** @brief Minimum value of greatest-width signed integer. */
#define INTMAX_MIN INT64_MIN
#endif
#ifndef INTMAX_MAX
/** @brief Maximum value of greatest-width signed integer. */
#define INTMAX_MAX INT64_MAX
#endif
#ifndef UINTMAX_MAX
/** @brief Maximum value of greatest-width unsigned integer. */
#define UINTMAX_MAX UINT64_MAX
#endif

#ifndef PTRDIFF_MIN
#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) ||             \
    defined(__aarch64__)
/** @brief Minimum value of ptrdiff_t. */
#define PTRDIFF_MIN INT64_MIN
/** @brief Maximum value of ptrdiff_t. */
#define PTRDIFF_MAX INT64_MAX
#else
/** @brief Minimum value of ptrdiff_t. */
#define PTRDIFF_MIN INT32_MIN
/** @brief Maximum value of ptrdiff_t. */
#define PTRDIFF_MAX INT32_MAX
#endif
#endif

#ifndef SIZE_MAX
#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) ||             \
    defined(__aarch64__)
/** @brief Maximum value of size_t. */
#define SIZE_MAX UINT64_MAX
#else
/** @brief Maximum value of size_t. */
#define SIZE_MAX UINT32_MAX
#endif
#endif

#ifndef INT8_C
/** @brief Macro to expand signed 8-bit integer constant. */
#define INT8_C(val) val
/** @brief Macro to expand unsigned 8-bit integer constant. */
#define UINT8_C(val) val##U
/** @brief Macro to expand signed 16-bit integer constant. */
#define INT16_C(val) val
/** @brief Macro to expand unsigned 16-bit integer constant. */
#define UINT16_C(val) val##U
/** @brief Macro to expand signed 32-bit integer constant. */
#define INT32_C(val) val
/** @brief Macro to expand unsigned 32-bit integer constant. */
#define UINT32_C(val) val##U
#if defined(_MSC_VER)
/** @brief Macro to expand signed 64-bit integer constant. */
#define INT64_C(val) val##i64
/** @brief Macro to expand unsigned 64-bit integer constant. */
#define UINT64_C(val) val##ui64
/** @brief Macro to expand greatest-width signed integer constant. */
#define INTMAX_C(val) val##i64
/** @brief Macro to expand greatest-width unsigned integer constant. */
#define UINTMAX_C(val) val##ui64
#else
/** @brief Macro to expand signed 64-bit integer constant. */
#define INT64_C(val) (__extension__(val##LL))
/** @brief Macro to expand unsigned 64-bit integer constant. */
#define UINT64_C(val) (__extension__(val##ULL))
/** @brief Macro to expand greatest-width signed integer constant. */
#define INTMAX_C(val) (__extension__(val##LL))
/** @brief Macro to expand greatest-width unsigned integer constant. */
#define UINTMAX_C(val) (__extension__(val##ULL))
#endif
#endif

/**
 * @brief Error codes returned by posix-stdint functions.
 */
enum posix_stdint_error_code {
  /** @brief Operation completed successfully. */
  POSIX_STDINT_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_STDINT_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on stdint polyfill availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_STDINT_SUCCESS on success, or an error code on failure.
 */
enum posix_stdint_error_code posix_stdint_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STDINT_H */
