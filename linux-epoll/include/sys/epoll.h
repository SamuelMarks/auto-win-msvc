/* sys/epoll.h - Strict C89 Header */
#ifndef AUTO_WIN_MSVC_SYS_EPOLL_H
#define AUTO_WIN_MSVC_SYS_EPOLL_H

/**
 * @file epoll.h
 * @brief Standard <sys/epoll.h> forwarding header to linux-epoll.h.
 */

/* clang-format off */
#if defined(__linux__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
#include_next <sys/epoll.h>
#else
#include "linux-epoll.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUTO_WIN_MSVC_SYS_EPOLL_H */
/* API Contract symbols: epoll_create, epoll_create1, epoll_ctl, epoll_wait,
 * epoll_pwait, epoll_pwait2, __dependencies__ */
