#include "auto_win_msvc_export.h"
/* posix-sys-select/include/sys/select.h - Strict C89 Implementation */
#ifndef POSIX_SYS_SELECT_STUB
#define POSIX_SYS_SELECT_STUB

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
/* clang-format off */
#include <winsock2.h>

/* MSVC uses fd_set from winsock2.h, select is available there */

#elif defined(__MSDOS__) || defined(__WATCOMC__)
/* DOS has no select.h */
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
#include_next <sys/select.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_SELECT_STUB */
