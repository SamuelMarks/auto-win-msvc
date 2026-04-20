/* pthread.h - shim for posix-pthread.h */
#if (!defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__)) ||      \
    defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <pthread.h>
#else
/* clang-format off */
#include <pthread.h>
#endif
#else
#ifndef PTHREAD_H_SHIM
#define PTHREAD_H_SHIM
#include "posix-pthread.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
