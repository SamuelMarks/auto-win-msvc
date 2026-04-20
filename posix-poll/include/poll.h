/* posix-poll/include/poll.h - Strict C89 Implementation */
#ifndef POSIX_POLL_STUB
#define POSIX_POLL_STUB

#if defined(_WIN32)
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

#elif defined(__MSDOS__) || defined(__WATCOMC__)
/* DOS has no poll.h */
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
#include_next <poll.h>
#endif

#endif /* POSIX_POLL_STUB */
