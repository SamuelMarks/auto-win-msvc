/* clang-format off */
#ifndef BSD_STDLIB_H_
#define BSD_STDLIB_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reallocate memory safely.
 */
void *reallocarray(void *ptr, size_t nmemb, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* BSD_STDLIB_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
