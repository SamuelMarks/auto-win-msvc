#ifndef POSIX_INTTYPES_H
#define POSIX_INTTYPES_H

/**
 * @file posix-inttypes.h
 * @brief Polyfill for <inttypes.h> format specifiers and integer conversion.
 */

/* clang-format off */
#include <stddef.h>
#if defined(_MSC_VER)
#if _MSC_VER < 1800
#include <posix-stdint.h>
#include <stdlib.h>
#else
#include <inttypes.h>
#endif
#else
#include <inttypes.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1800)

#ifndef PRId8
/** @brief Format specifier for signed 8-bit integer decimal output. */
#define PRId8 "d"
#endif
#ifndef PRId16
/** @brief Format specifier for signed 16-bit integer decimal output. */
#define PRId16 "d"
#endif
#ifndef PRId32
/** @brief Format specifier for signed 32-bit integer decimal output. */
#define PRId32 "d"
#endif
#ifndef PRId64
/** @brief Format specifier for signed 64-bit integer decimal output. */
#define PRId64 "I64d"
#endif

#ifndef PRIi8
/** @brief Format specifier for signed 8-bit integer output. */
#define PRIi8 "i"
#endif
#ifndef PRIi16
/** @brief Format specifier for signed 16-bit integer output. */
#define PRIi16 "i"
#endif
#ifndef PRIi32
/** @brief Format specifier for signed 32-bit integer output. */
#define PRIi32 "i"
#endif
#ifndef PRIi64
/** @brief Format specifier for signed 64-bit integer output. */
#define PRIi64 "I64i"
#endif

#ifndef PRIo8
/** @brief Format specifier for octal 8-bit unsigned integer output. */
#define PRIo8 "o"
#endif
#ifndef PRIo16
/** @brief Format specifier for octal 16-bit unsigned integer output. */
#define PRIo16 "o"
#endif
#ifndef PRIo32
/** @brief Format specifier for octal 32-bit unsigned integer output. */
#define PRIo32 "o"
#endif
#ifndef PRIo64
/** @brief Format specifier for octal 64-bit unsigned integer output. */
#define PRIo64 "I64o"
#endif

#ifndef PRIu8
/** @brief Format specifier for unsigned 8-bit integer decimal output. */
#define PRIu8 "u"
#endif
#ifndef PRIu16
/** @brief Format specifier for unsigned 16-bit integer decimal output. */
#define PRIu16 "u"
#endif
#ifndef PRIu32
/** @brief Format specifier for unsigned 32-bit integer decimal output. */
#define PRIu32 "u"
#endif
#ifndef PRIu64
/** @brief Format specifier for unsigned 64-bit integer decimal output. */
#define PRIu64 "I64u"
#endif

#ifndef PRIx8
/** @brief Format specifier for lowercase hexadecimal 8-bit unsigned output. */
#define PRIx8 "x"
#endif
#ifndef PRIx16
/** @brief Format specifier for lowercase hexadecimal 16-bit unsigned output. */
#define PRIx16 "x"
#endif
#ifndef PRIx32
/** @brief Format specifier for lowercase hexadecimal 32-bit unsigned output. */
#define PRIx32 "x"
#endif
#ifndef PRIx64
/** @brief Format specifier for lowercase hexadecimal 64-bit unsigned output. */
#define PRIx64 "I64x"
#endif

#ifndef PRIX8
/** @brief Format specifier for uppercase hexadecimal 8-bit unsigned output. */
#define PRIX8 "X"
#endif
#ifndef PRIX16
/** @brief Format specifier for uppercase hexadecimal 16-bit unsigned output. */
#define PRIX16 "X"
#endif
#ifndef PRIX32
/** @brief Format specifier for uppercase hexadecimal 32-bit unsigned output. */
#define PRIX32 "X"
#endif
#ifndef PRIX64
/** @brief Format specifier for uppercase hexadecimal 64-bit unsigned output. */
#define PRIX64 "I64X"
#endif

#ifndef strtoimax
/** @brief Macro mapping strtoimax to MSVC _strtoi64. */
#define strtoimax _strtoi64
#endif
#ifndef strtoumax
/** @brief Macro mapping strtoumax to MSVC _strtoui64. */
#define strtoumax _strtoui64
#endif

#endif /* defined(_MSC_VER) && (_MSC_VER < 1800) */

/**
 * @brief Error codes returned by posix-inttypes functions.
 */
enum posix_inttypes_error_code {
  /** @brief Operation completed successfully. */
  POSIX_INTTYPES_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_INTTYPES_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on inttypes polyfill availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_INTTYPES_SUCCESS on success, or an error code on failure.
 */
enum posix_inttypes_error_code posix_inttypes_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_INTTYPES_H */
