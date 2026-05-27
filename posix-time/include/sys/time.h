#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/time.h>
#else
#include <sys/time.h>
#endif
#else
#include "posix-time.h"
#endif
