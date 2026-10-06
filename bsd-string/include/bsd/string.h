/* clang-format off */
#ifndef BSD_STRING_H_
#define BSD_STRING_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(_WIN32)
/**
 * @brief Copy strings safely.
 */
size_t strlcpy(char *dst, const char *src, size_t size);

/**
 * @brief Concatenate strings safely.
 */
size_t strlcat(char *dst, const char *src, size_t size);

/**
 * @brief Locate a substring ignoring case.
 * @param[in] haystack String to search in.
 * @param[in] needle Substring to look for.
 * @return Pointer to the beginning of the substring, or NULL if not found.
 */
char *strcasestr(const char *haystack, const char *needle);

/**
 * @brief Separate strings.
 */
char *strsep(char **stringp, const char *delim);

#if defined(_MSC_VER)
/**
 * @brief Compare strings ignoring case.
 */
int strcasecmp(const char *s1, const char *s2);

/**
 * @brief Compare length-limited strings ignoring case.
 */
int strncasecmp(const char *s1, const char *s2, size_t n);
#endif /* _MSC_VER */

/**
 * @brief Compare version strings.
 */
int strverscmp(const char *s1, const char *s2);

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#ifdef __cplusplus
}
#endif

#endif /* BSD_STRING_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
