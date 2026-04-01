/* posix-poll/include/poll.h - Strict C89 Implementation */
#ifndef POSIX_POLL_STUB
#define POSIX_POLL_STUB

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
/* clang-format off */
#include <winsock2.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/* Provide macro to map poll to WSAPoll */
#define poll WSAPoll
#define nfds_t ULONG

#ifdef __cplusplus
}
#endif

#else
#include_next <poll.h>
#endif

#endif /* POSIX_POLL_STUB */
