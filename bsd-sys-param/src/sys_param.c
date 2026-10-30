#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/* clang-format off */
#include "sys/param.h"
#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>
#endif /* _MSC_VER || _WIN32 */
/* clang-format on */

int sys_param_dummy_for_coverage(void) { return 0; }

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_param_c;

#endif
typedef int make_iso_compilers_happy_mega_batch2_10;
