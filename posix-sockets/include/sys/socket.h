#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/socket.h>
#else
#include <sys/socket.h>
#endif
#else
#include "posix-sockets.h"
#endif
