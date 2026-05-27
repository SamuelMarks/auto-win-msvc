#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/resource.h>
#else
#include <sys/resource.h>
#endif
#else
#include "posix-sys-resource.h"
#endif
