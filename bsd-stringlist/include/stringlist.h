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
  /** @brief Array of string pointers. */
  char **sl_str;
  /** @brief Allocated capacity of sl_str. */
  size_t sl_max;
  /** @brief Current number of elements in sl_str. */
  size_t sl_cur;
} StringList;

/**
 * @brief Allocate and initialize a new StringList.
 * @return Pointer to initialized StringList, or NULL on allocation failure.
 */
StringList *sl_init(void);

/**
 * @brief Append a string pointer to a StringList.
 * @param sl Target StringList.
 * @param name String pointer to append.
 * @return 0 on success, or -1 on failure.
 */
int sl_add(StringList *sl, char *name);

/**
 * @brief Free a StringList and optionally its elements.
 * @param sl Target StringList (may be NULL).
 * @param all Non-zero to free each element pointer via free().
 */
void sl_free(StringList *sl, int all);

/**
 * @brief Find a string in a StringList by strcmp.
 * @param sl Target StringList.
 * @param name NUL-terminated string to search for.
 * @return Matching element pointer, or NULL if not found.
 */
char *sl_find(StringList *sl, const char *name);

/**
 * @brief Delete a string from a StringList and optionally free it.
 * @param sl Target StringList.
 * @param name NUL-terminated string to remove.
 * @param all Non-zero to free the removed element pointer via free().
 * @return 0 if found and removed, or -1 if not found or invalid arguments.
 */
int sl_delete(StringList *sl, const char *name, int all);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_STRINGLIST_H */
