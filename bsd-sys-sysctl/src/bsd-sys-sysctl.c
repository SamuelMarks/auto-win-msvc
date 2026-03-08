#include "bsd-sys-sysctl.h"
#include <errno.h>
#if defined(_MSC_VER) && !defined(__clang__)
/** \brief sysctl function. */
int sysctl(const int *name, unsigned int namelen, void *oldp, size_t *oldlenp, const void *newp, size_t newlen) {
    (void)name; (void)namelen; (void)oldp; (void)oldlenp; (void)newp; (void)newlen;
    errno = ENOSYS; return -1;
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_bsd_sys_sysctl(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_sys_sysctl;

