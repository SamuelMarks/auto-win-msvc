#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <netinet/tcp.h>
#else
#include <netinet/tcp.h>
#endif
#else
#include "posix-netinet-tcp.h"
#endif
