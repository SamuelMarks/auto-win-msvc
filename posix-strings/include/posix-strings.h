#ifndef POSIX_STRINGS_H
#define POSIX_STRINGS_H

/**
 * @file posix-strings.h
 * @brief Polyfill for <strings.h> POSIX functions and string manipulation.
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(_MSC_VER) && !defined(__APPLE__)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#endif
#include <stddef.h>
#include <string.h>
#include "auto-win-msvc-error.h"
#if !defined(_MSC_VER) || defined(__clang__)
#include <strings.h>
#endif
/* clang-format on */

#if defined(__GNUC__) || defined(__clang__)
/** @brief Compiler extension prefix suppressing C89 long long pedantic
 * warnings. */
#define POSIX_STRINGS_EXTENSION __extension__
#else
/** @brief Compiler extension prefix suppressing C89 long long pedantic
 * warnings. */
#define POSIX_STRINGS_EXTENSION
#endif

/** @brief 64-bit signed integer type used by ffsll. */
POSIX_STRINGS_EXTENSION typedef long long posix_strings_llong;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-strings polyfill availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_strings_get_info(int *out_available);

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

#if !defined(__WATCOMC__)
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
#endif

#if defined(_MSC_VER) || defined(_WIN32)

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

#else /* !_MSC_VER && !_WIN32 */

#if !defined(__USE_POSIX) && !defined(strtok_r)
extern char *strtok_r(char *str, const char *delim, char **saveptr);
#endif

#if !defined(__USE_GNU)
extern int ffsl(long i);
extern int ffsll(posix_strings_llong i);
#endif

#endif /* _MSC_VER || _WIN32 */

/**
 * @brief Copies the string src to dest including '\0', returning pointer to
 * '\0'.
 * @param dest Destination buffer.
 * @param src Source null-terminated string.
 * @return Pointer to the terminating '\0' in dest.
 */
char *posix_stpcpy(char *dest, const char *src);

/**
 * @brief Copies at most n bytes of string src to dest, returning pointer to
 * '\0' or dest + n.
 * @param dest Destination buffer.
 * @param src Source null-terminated string.
 * @param n Maximum number of bytes to write.
 * @return Pointer to the terminating '\0' in dest if src is shorter than n,
 * else dest + n.
 */
char *posix_stpncpy(char *dest, const char *src, size_t n);

/**
 * @brief Locates the first occurrence of c in s, or pointer to terminating
 * '\0'.
 * @param s String to search.
 * @param c Character to search for.
 * @return Pointer to the matched character, or pointer to the null terminator
 * if not found.
 */
char *posix_strchrnul(const char *s, int c);

/**
 * @brief Copies n bytes from src to dest, returning pointer to byte following
 * last written byte.
 * @param dest Destination buffer.
 * @param src Source memory area.
 * @param n Number of bytes to copy.
 * @return Pointer to dest + n.
 */
void *posix_mempcpy(void *dest, const void *src, size_t n);

/**
 * @brief Finds last occurrence of c in initial n bytes of s.
 * @param s Memory area.
 * @param c Character byte to search for.
 * @param n Length of memory area.
 * @return Pointer to matched byte or NULL if not found.
 */
void *posix_memrchr(const void *s, int c, size_t n);

/**
 * @brief Returns description string for signal number.
 * @param sig Signal number.
 * @return Description string for the signal.
 */
char *posix_strsignal(int sig);

#ifndef stpcpy
/** @brief POSIX stpcpy macro alias. */
#define stpcpy posix_stpcpy
#endif
#ifndef stpncpy
/** @brief POSIX stpncpy macro alias. */
#define stpncpy posix_stpncpy
#endif
#if !defined(__APPLE__) || !defined(AUTO_WIN_MSVC_HAS_AUTOCONF_H)
#ifndef strchrnul
/** @brief GNU strchrnul macro alias. */
#define strchrnul posix_strchrnul
#endif
#endif
#ifndef mempcpy
/** @brief GNU mempcpy macro alias. */
#define mempcpy posix_mempcpy
#endif
#ifndef memrchr
/** @brief GNU memrchr macro alias. */
#define memrchr posix_memrchr
#endif
#ifndef strsignal
/** @brief POSIX strsignal macro alias. */
#define strsignal posix_strsignal
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STRINGS_H */
