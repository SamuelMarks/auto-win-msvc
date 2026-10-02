#if defined(__GNUC__)
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#pragma GCC system_header
#include_next <sys/socket.h>
#else
#include <sys/socket.h>
#endif
#else
#include "posix-sockets.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* API Contract symbols: bind, accept, __dependencies__, __include_next__,
 * connect, getpeername, getsockname, getsockopt, listen, recv, recvmsg, send,
 * sendmsg, setsockopt, shutdown, sockatmark, socketpair */
