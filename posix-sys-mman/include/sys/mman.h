#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/mman.h>
#else
#include <sys/mman.h>
#endif
#else
#include "posix-mman.h"
#endif
