#ifndef BSD_STRINGLIST_H
#define BSD_STRINGLIST_H

/**
 * @file stringlist.h
 * @brief BSD dynamic string list (<stringlist.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Dynamic array of string pointers. */
typedef struct _stringlist {
  char **sl_str;
  size_t sl_max;
  size_t sl_cur;
} StringList;

/** @brief Allocate and initialize a new StringList. */
StringList *sl_init(void);
/** @brief Append a string pointer to a StringList. */
int sl_add(StringList *sl, char *name);
/** @brief Free a StringList and optionally its elements. */
void sl_free(StringList *sl, int all);
/** @brief Find a string in a StringList by strcmp. */
char *sl_find(StringList *sl, const char *name);
/** @brief Delete a string from a StringList and optionally free it. */
int sl_delete(StringList *sl, const char *name, int all);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_STRINGLIST_H */
