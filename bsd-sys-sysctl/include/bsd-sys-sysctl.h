#ifndef BSD_SYS_SYSCTL_H
#define BSD_SYS_SYSCTL_H
#if defined(_MSC_VER) && !defined(__clang__)
/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** \brief sysctl function. */
int sysctl(const int *name, unsigned int namelen, void *oldp, size_t *oldlenp,
           const void *newp, size_t newlen);
#endif

#ifdef __cplusplus
}
#endif

#endif
