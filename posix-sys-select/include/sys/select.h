/* posix-sys-select/include/sys/select.h - Strict C89 Implementation */
#ifndef POSIX_SYS_SELECT_STUB
#define POSIX_SYS_SELECT_STUB

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MSVC uses fd_set from winsock2.h, select is available there */

#ifdef __cplusplus
}
#endif

#else
#include_next <sys/select.h>
#endif

#endif /* POSIX_SYS_SELECT_STUB */
