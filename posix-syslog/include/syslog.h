#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <syslog.h>
#else
#include <syslog.h>
#endif
#else
#include "posix-syslog.h"
#endif
