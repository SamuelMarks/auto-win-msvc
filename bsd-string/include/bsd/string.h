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

#ifdef __cplusplus
}
#endif

#endif /* BSD_STRING_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
