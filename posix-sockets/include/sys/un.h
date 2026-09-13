#if defined(__GNUC__)
#pragma GCC system_header
#endif

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/un.h>
#else
#include <sys/un.h>
#endif
#else
#include "../posix-sys-un.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
