#include "auto_win_msvc_export.h"
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#include_next <dlfcn.h>
#else
#include <dlfcn.h>
#endif
#else
#include "posix-dlfcn.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
