#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <unistd.h>
#else
#include <unistd.h>
#endif
#else
#include "posix-core.h"
#endif
