#ifndef POSIX_NL_TYPES_H
#define POSIX_NL_TYPES_H

/**
 * @file nl_types.h
 * @brief POSIX NLS message catalog (<nl_types.h>) polyfill.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define NL_SETD 1
#define NL_CAT_LOCALE 1

#ifndef _NL_ITEM_DECLARED
#define _NL_ITEM_DECLARED
typedef int nl_item;
#endif

typedef void *nl_catd;

/** @brief Open a message catalog. */
nl_catd catopen(const char *name, int oflag);
/** @brief Read a program message from a message catalog (returns s fallback).
 */
char *catgets(nl_catd catd, int set_id, int msg_id, const char *s);
/** @brief Close a message catalog. */
int catclose(nl_catd catd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NL_TYPES_H */
