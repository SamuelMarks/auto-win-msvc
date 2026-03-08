/* posix-sys-un.h - Strict C89 Header */
#ifndef POSIX_SYS_UN_H
#define POSIX_SYS_UN_H

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <afunix.h>

#else

#include <sys/un.h>

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#endif /* POSIX_SYS_UN_H */
