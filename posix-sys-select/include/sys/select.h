/* posix-sys-select/include/sys/select.h - Strict C89 Implementation */
#ifndef POSIX_SYS_SELECT_STUB
#define POSIX_SYS_SELECT_STUB

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
/* clang-format off */
#include <winsock2.h>
/* MSVC uses fd_set from winsock2.h, select is available there */
#ifdef _MSC_VER
#undef FD_ZERO
#define FD_ZERO(set) (((fd_set *)(set))->fd_count = 0)
#undef FD_SET
#define FD_SET(fd, set) auto_win_msvc_fd_set((SOCKET)(fd), (fd_set *)(set))
#undef FD_CLR
#define FD_CLR(fd, set) auto_win_msvc_fd_clr((SOCKET)(fd), (fd_set *)(set))

extern void auto_win_msvc_fd_set(SOCKET fd, fd_set *set);
extern void auto_win_msvc_fd_clr(SOCKET fd, fd_set *set);
#endif

#elif defined(__MSDOS__) || defined(__WATCOMC__)
/* DOS has no select.h */
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
#include_next <sys/select.h>
/* clang-format on */
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_SELECT_STUB */
