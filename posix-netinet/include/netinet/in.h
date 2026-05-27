#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <netinet/in.h>
#else
#include <netinet/in.h>
#endif
#else
#include "posix-netinet-in.h"
#endif
