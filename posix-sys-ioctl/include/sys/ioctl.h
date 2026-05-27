#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/ioctl.h>
#else
#include <sys/ioctl.h>
#endif
#else
#include "posix-sys-ioctl.h"
#endif
