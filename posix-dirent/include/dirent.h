#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <dirent.h>
#else
#include <dirent.h>
#endif
#else
#include "posix-dirent.h"
#endif
