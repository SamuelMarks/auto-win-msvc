#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/* clang-format off */
#include "bsd/stdlib.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>

/* Could not parse: void *reallocarray(void *ptr, size_t nmemb, size_t size); */
#endif /* _MSC_VER || _WIN32 */

int bsd_stdlib_dummy_for_coverage(void) { return 0; }

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_bsd_stdlib_c;

#endif
typedef int make_iso_compilers_happy_mega_batch2_3;
