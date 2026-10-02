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

#ifdef __cplusplus
}
#endif

#endif /* BSD_STRING_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
