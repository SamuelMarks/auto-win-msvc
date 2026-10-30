/* clang-format off */
#include "sys/cdefs.h"
#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>
#endif /* _MSC_VER || _WIN32 */
/* clang-format on */

int sys_cdefs_dummy_for_coverage(void) { return 0; }

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_cdefs_c;
