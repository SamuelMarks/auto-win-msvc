#ifndef POSIX_SYS_SELECT_SYS_SELECT_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_SYS_SELECT_SYS_SELECT_H

/**
 * @file select.h
 * @brief POSIX sys/select.h header and MSVC fd_set helpers.
 */

/* clang-format off */
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#include <stddef.h>
#else
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/select.h>
#else
#include <sys/select.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)

#if defined(_MSC_VER)
#undef FD_ZERO
#define FD_ZERO(set) (((fd_set *)(set))->fd_count = 0)
#undef FD_SET
#define FD_SET(fd, set) auto_win_msvc_fd_set((SOCKET)(fd), (fd_set *)(set))
#undef FD_CLR
#define FD_CLR(fd, set) auto_win_msvc_fd_clr((SOCKET)(fd), (fd_set *)(set))
#endif

/**
 * @brief Adds a socket descriptor to an fd_set without exceeding FD_SETSIZE.
 * @param fd Socket descriptor to add.
 * @param set Pointer to fd_set.
 */
void auto_win_msvc_fd_set(SOCKET fd, fd_set *set);

/**
 * @brief Removes a socket descriptor from an fd_set.
 * @param fd Socket descriptor to remove.
 * @param set Pointer to fd_set.
 */
void auto_win_msvc_fd_clr(SOCKET fd, fd_set *set);

#endif /* defined(_WIN32) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_SELECT_SYS_SELECT_H */
