#if defined(__GNUC__)
#pragma GCC system_header
#endif
/* posix-sys-un.h - Strict C89 Header */
#ifndef POSIX_SYS_UN_H
#define POSIX_SYS_UN_H

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#if defined(__has_include)
#if __has_include(<afunix.h>)
#include <afunix.h>
#define HAVE_AFUNIX_H 1
#endif
#endif
#else
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/un.h>
#else
#include <sys/un.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(_WIN32)

#ifdef DISABLE_UNIX_SOCKET
/* Unix domain sockets disabled by configuration */
#else
#ifndef AF_UNIX
#define AF_UNIX 1
#endif

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif

#if !defined(HAVE_AFUNIX_H) && !defined(_SOCKADDR_UN_DEFINED)
#define _SOCKADDR_UN_DEFINED
/** \brief UNIX domain socket address structure for Windows. */
struct sockaddr_un {
  ADDRESS_FAMILY sun_family;
  char sun_path[108];
};
#endif
#endif /* DISABLE_UNIX_SOCKET */

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_UN_H */
