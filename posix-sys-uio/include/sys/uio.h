#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/uio.h>
#else
#include <sys/uio.h>
#endif
#else
#include "posix-sys-uio.h"
#endif
