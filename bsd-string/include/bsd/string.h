/* clang-format off */
#ifndef BSD_STRING_H_
#define BSD_STRING_H_

/**
 * @file string.h
 * @brief BSD string and mode manipulation polyfills.
 */

#include <sys/types.h>
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_MODE_T_DEFINED) && !defined(_MODE_T_DEFINED_) && \
    !defined(_MODE_T_) && !defined(HAVE_MODE_T) && \
    (defined(_MSC_VER) || defined(_WIN32))
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
#define HAVE_MODE_T 1
/** @brief POSIX file mode type. */
typedef unsigned short mode_t;
#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(__DOS__) || \
    defined(__MSDOS__) || defined(__WATCOMC__) || defined(__linux__) || \
    defined(__CYGWIN__)

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) && \
    !defined(__NetBSD__)
/**
 * @brief Copy strings safely.
 * @param[out] dst Destination buffer.
 * @param[in] src Source string.
 * @param[in] size Size of destination buffer.
 * @return Total length of the string tried to create (strlen(src)).
 */
size_t strlcpy(char *dst, const char *src, size_t size);

/**
 * @brief Concatenate strings safely.
 * @param[in,out] dst Destination buffer.
 * @param[in] src Source string.
 * @param[in] size Size of destination buffer.
 * @return Total length of the string tried to create.
 */
size_t strlcat(char *dst, const char *src, size_t size);

/**
 * @brief Locate a substring in a length-limited string.
 * @param[in] big String to search in.
 * @param[in] little Substring to look for.
 * @param[in] len Maximum number of characters to search.
 * @return Pointer to beginning of substring, or NULL if not found.
 */
char *strnstr(const char *big, const char *little, size_t len);

/**
 * @brief Convert a file mode to a symbolic string (e.g. "-rwxr-xr-x ").
 * @param[in] mode File mode bitmask.
 * @param[out] p Output buffer of at least 12 bytes.
 */
void strmode(int mode, char *p);

/**
 * @brief Zero a memory buffer without being optimized away by the compiler.
 * @param[out] buf Buffer to zero.
 * @param[in] len Number of bytes to zero.
 */
void explicit_bzero(void *buf, size_t len);

/**
 * @brief Constant-time memory comparison (BSD bcmp semantics).
 * @param[in] b1 First buffer.
 * @param[in] b2 Second buffer.
 * @param[in] n Number of bytes to compare.
 * @return 0 if equal, non-zero otherwise.
 */
int timingsafe_bcmp(const void *b1, const void *b2, size_t n);

/**
 * @brief Constant-time lexicographical memory comparison.
 * @param[in] b1 First buffer.
 * @param[in] b2 Second buffer.
 * @param[in] n Number of bytes to compare.
 * @return <0, 0, or >0.
 */
int timingsafe_memcmp(const void *b1, const void *b2, size_t n);

/**
 * @brief Convert file flags bitmask into a comma-separated string.
 * @param[in] flags File flags bitmask.
 * @return Newly allocated string describing flags, or NULL on failure.
 */
char *fflagstostr(unsigned long flags);

/**
 * @brief Parse comma-separated file flags string into set/clear bitmasks.
 * @param[in,out] stringp Pointer to string pointer.
 * @param[out] setp Pointer receiving flags to set.
 * @param[out] clrp Pointer receiving flags to clear.
 * @return 0 on success, non-zero if an unknown flag was encountered.
 */
int strtofflags(char **stringp, unsigned long *setp, unsigned long *clrp);
#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(__DOS__) || \
    defined(__MSDOS__) || defined(__WATCOMC__)
/**
 * @brief Locate a substring ignoring case.
 * @param[in] haystack String to search in.
 * @param[in] needle Substring to look for.
 * @return Pointer to the beginning of the substring, or NULL if not found.
 */
char *strcasestr(const char *haystack, const char *needle);

/**
 * @brief Separate strings.
 * @param[in,out] stringp Pointer to string pointer.
 * @param[in] delim Delimiter character set.
 * @return Next token or NULL.
 */
char *strsep(char **stringp, const char *delim);

#if defined(_MSC_VER) || defined(__WATCOMC__)
#ifndef strcasecmp
/**
 * @brief Compare strings ignoring case.
 * @param[in] s1 First string.
 * @param[in] s2 Second string.
 * @return <0, 0, or >0.
 */
int strcasecmp(const char *s1, const char *s2);
#endif

#ifndef strncasecmp
/**
 * @brief Compare length-limited strings ignoring case.
 * @param[in] s1 First string.
 * @param[in] s2 Second string.
 * @param[in] n Maximum characters to compare.
 * @return <0, 0, or >0.
 */
int strncasecmp(const char *s1, const char *s2, size_t n);
#endif
#endif /* _MSC_VER || __WATCOMC__ */

/**
 * @brief Compare version strings.
 * @param[in] s1 First string.
 * @param[in] s2 Second string.
 * @return <0, 0, or >0.
 */
int strverscmp(const char *s1, const char *s2);

#endif /* _MSC_VER || _WIN32 || __DOS__ || __MSDOS__ || __WATCOMC__ */

#endif /* target guard */

/**
 * @brief Coverage helper for bsd-string module.
 * @return Always 0.
 */
int bsd_string_dummy_for_coverage(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_STRING_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
