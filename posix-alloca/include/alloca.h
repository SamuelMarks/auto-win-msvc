/* alloca.h - Strict C89 Implementation */
#ifndef POSIX_ALLOCA_H
#define POSIX_ALLOCA_H

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */
#if defined(_MSC_VER) || defined(__WATCOMC__) || defined(__MSDOS__) || defined(__MINGW32__)
#include <malloc.h>
#if defined(_MSC_VER) && !defined(alloca)
#define alloca _alloca
#endif
#else
#include <stdlib.h>
#endif
/* clang-format on */

#ifdef __cplusplus
}
#endif

#endif /* POSIX_ALLOCA_H */
