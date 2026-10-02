#if defined(__GNUC__)
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#pragma GCC system_header
#include_next <sys/time.h>
#else
#include <sys/time.h>
#endif
#else
#include "posix-time.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* API Contract symbols: getitimer, gettimeofday, utimes, __dependencies__,
 * __include_next__ */
