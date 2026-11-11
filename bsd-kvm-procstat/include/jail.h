#ifndef BSD_JAIL_H
#define BSD_JAIL_H

/**
 * @file jail.h
 * @brief FreeBSD userland jail library (<jail.h>) polyfill.
 */

/* clang-format off */
#include <sys/jail.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

extern char jail_errmsg[];

/** @brief Look up jail ID by name or numeric string. */
int jail_getid(const char *name);
/** @brief Look up jail name by jail ID. */
char *jail_getname(int jid);
/** @brief Retrieve jail parameters by variadic key/value pairs. */
int jail_getv(int flags, ...);
/** @brief Set jail parameters by variadic key/value pairs. */
int jail_setv(int flags, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_JAIL_H */
