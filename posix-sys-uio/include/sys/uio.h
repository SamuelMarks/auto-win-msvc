#if defined(__GNUC__)
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#pragma GCC system_header
#include_next <sys/uio.h>
#else
#include <sys/uio.h>
#endif
#else
#include "posix-sys-uio.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
