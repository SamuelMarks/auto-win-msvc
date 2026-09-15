#if defined(__GNUC__)
#pragma GCC system_header
#endif
#ifndef POSIX_STDINT_H
#define POSIX_STDINT_H

/**
 * @file posix-stdint.h
 * @brief Polyfill for C99 <stdint.h> integer types and limits.
 */

/* clang-format off */
#if defined(_MSC_VER)
#if _MSC_VER >= 1900
#include <vcruntime.h>
#endif
#else
#if defined(__GNUC__) || defined(__clang__)
#include_next <stdint.h>
#else
#include <stdint.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)

#ifndef _STDINT
#define _STDINT
#endif

#ifndef _INT8_T_DEFINED
#define _INT8_T_DEFINED
/** @brief Signed 8-bit integer type. */
typedef signed char int8_t;
#endif

#ifndef _UINT8_T_DEFINED
#define _UINT8_T_DEFINED
/** @brief Unsigned 8-bit integer type. */
typedef unsigned char uint8_t;
#endif

#ifndef _INT16_T_DEFINED
#define _INT16_T_DEFINED
/** @brief Signed 16-bit integer type. */
typedef short int16_t;
#endif

#ifndef _UINT16_T_DEFINED
#define _UINT16_T_DEFINED
/** @brief Unsigned 16-bit integer type. */
typedef unsigned short uint16_t;
#endif

#ifndef _INT32_T_DEFINED
#define _INT32_T_DEFINED
/** @brief Signed 32-bit integer type. */
typedef int int32_t;
#endif

#ifndef _UINT32_T_DEFINED
#define _UINT32_T_DEFINED
/** @brief Unsigned 32-bit integer type. */
typedef unsigned int uint32_t;
#endif

#ifndef _INT64_T_DEFINED
#define _INT64_T_DEFINED
/** @brief Signed 64-bit integer type. */
typedef __int64 int64_t;
#endif

#ifndef _UINT64_T_DEFINED
#define _UINT64_T_DEFINED
/** @brief Unsigned 64-bit integer type. */
typedef unsigned __int64 uint64_t;
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
#ifdef _WIN64
/** @brief Pointer-sized signed integer type. */
typedef __int64 intptr_t;
#else
/** @brief Pointer-sized signed integer type. */
typedef int intptr_t;
#endif
#endif

#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
#ifdef _WIN64
/** @brief Pointer-sized unsigned integer type. */
typedef unsigned __int64 uintptr_t;
#else
/** @brief Pointer-sized unsigned integer type. */
typedef unsigned int uintptr_t;
#endif
#endif

/** @brief Maximum-width signed integer type. */
typedef int64_t intmax_t;
/** @brief Maximum-width unsigned integer type. */
typedef uint64_t uintmax_t;

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

#endif /* defined(_MSC_VER) */

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
