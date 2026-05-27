#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <poll.h>
#else
#include <poll.h>
#endif
#else
#include "posix-poll.h"
#endif
