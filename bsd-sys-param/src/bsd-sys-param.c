#include "bsd-sys-param.h"

#if defined(_MSC_VER) || defined(_WIN32)

int bsd_sys_param_dummy_link(void) {
    return MAXPATHLEN;
}

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_bsd_sys_param(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_sys_param;

