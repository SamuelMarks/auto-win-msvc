#ifndef POSIX_STRINGS_H
#define POSIX_STRINGS_H

/**
 * @file posix-strings.h
 * @brief Polyfill for <strings.h> POSIX functions and string manipulation.
 */

/* clang-format off */
#if !defined(_MSC_VER) || defined(__clang__)
#include <strings.h>
#endif

#if defined(_MSC_VER)
#include <string.h>
#endif
#include <stddef.h>
/* clang-format on */

#if defined(__GNUC__) || defined(__clang__)
#define POSIX_STRINGS_EXTENSION __extension__
#else
#define POSIX_STRINGS_EXTENSION
#endif

POSIX_STRINGS_EXTENSION typedef long long posix_strings_llong;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-strings functions.
 */
enum posix_strings_error_code {
  /** @brief Operation completed successfully. */
  POSIX_STRINGS_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_STRINGS_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-strings polyfill availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_STRINGS_SUCCESS on success, or POSIX_STRINGS_ERROR_NULL_POINTER
 * on NULL pointer.
 */
enum posix_strings_error_code posix_strings_get_info(int *out_available);

#if defined(_MSC_VER)
#ifndef strcasecmp
/** @brief Macro mapping strcasecmp to _stricmp on MSVC. */
#define strcasecmp _stricmp
#endif

#ifndef strncasecmp
/** @brief Macro mapping strncasecmp to _strnicmp on MSVC. */
#define strncasecmp _strnicmp
#endif

#ifndef strtok_r
/** @brief Thread-safe string tokenizer macro mapping to strtok_s. */
#define strtok_r strtok_s
#endif

#endif /* _MSC_VER */

#ifndef bzero
/** @brief Sets first len bytes of the area starting at b to zero. */
#define bzero(b, len) (memset((b), '\0', (len)), (void)0)
#endif

#ifndef bcopy
/** @brief Copies len bytes from src to dest. */
#define bcopy(src, dest, len) (memmove((dest), (src), (len)), (void)0)
#endif

#ifndef bcmp
/** @brief Compares byte string b1 against byte string b2. */
#define bcmp(b1, b2, len) memcmp((b1), (b2), (len))
#endif

#if defined(_MSC_VER)

/**
 * @brief Finds the first (least significant) bit set in an integer.
 * @param i Value to check.
 * @return 1-based index of least significant set bit, or 0 if no bits are set.
 */
int ffs(int i);

/**
 * @brief Finds the first (least significant) bit set in a long integer.
 * @param i Value to check.
 * @return 1-based index of least significant set bit, or 0 if no bits are set.
 */
int ffsl(long i);

/**
 * @brief Finds the first (least significant) bit set in a long long integer.
 * @param i Value to check.
 * @return 1-based index of least significant set bit, or 0 if no bits are set.
 */
int ffsll(posix_strings_llong i);

#else /* !_MSC_VER */

#if !defined(__USE_POSIX)
extern char *strtok_r(char *str, const char *delim, char **saveptr);
#endif

#if !defined(__USE_GNU)
extern int ffsl(long i);
extern int ffsll(posix_strings_llong i);
#endif

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STRINGS_H */
