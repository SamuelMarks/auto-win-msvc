#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_stringlist.c
 * @brief Implementation of BSD <stringlist.h> functions.
 */

/* clang-format off */
#include <stringlist.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

static void *default_sl_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

static void *default_sl_realloc(void *ptr, size_t size) {
  return realloc(ptr, size);
}

/** @brief Overrideable calloc hook for OOM testing in bsd-stringlist. */
void *(*g_bsd_sl_calloc)(size_t, size_t) = default_sl_calloc;
/** @brief Overrideable realloc hook for OOM testing in bsd-stringlist. */
void *(*g_bsd_sl_realloc)(void *, size_t) = default_sl_realloc;

/** @brief Resets all bsd-stringlist allocator hooks to defaults. */
void bsd_sl_reset_hooks(void);
void bsd_sl_reset_hooks(void) {
  g_bsd_sl_calloc = default_sl_calloc;
  g_bsd_sl_realloc = default_sl_realloc;
}

#ifndef BSD_STRINGLIST_CALLOC
#define BSD_STRINGLIST_CALLOC(nmemb, size) g_bsd_sl_calloc((nmemb), (size))
#endif
#ifndef BSD_STRINGLIST_REALLOC
#define BSD_STRINGLIST_REALLOC(ptr, size) g_bsd_sl_realloc((ptr), (size))
#endif
#ifndef BSD_STRINGLIST_FREE
#define BSD_STRINGLIST_FREE(ptr) free((ptr))
#endif

/**
 * @brief Allocate and initialize a new StringList.
 * @return Pointer to initialized StringList, or NULL on allocation failure.
 */
StringList *sl_init(void) {
  StringList *sl = (StringList *)BSD_STRINGLIST_CALLOC(1, sizeof(*sl));
  if (sl == NULL) {
    return NULL;
  }
  sl->sl_max = 20;
  sl->sl_cur = 0;
  sl->sl_str = (char **)BSD_STRINGLIST_CALLOC(sl->sl_max, sizeof(char *));
  if (sl->sl_str == NULL) {
    BSD_STRINGLIST_FREE(sl);
    return NULL;
  }
  return sl;
}

/**
 * @brief Append a string pointer to a StringList.
 * @param sl Target StringList.
 * @param name String pointer to append.
 * @return 0 on success, or -1 on failure.
 */
int sl_add(StringList *sl, char *name) {
  if (sl == NULL) {
    return -1;
  }
  if (sl->sl_cur + 1 >= sl->sl_max) {
    size_t nmax = sl->sl_max * 2;
    char **nstr =
        (char **)BSD_STRINGLIST_REALLOC(sl->sl_str, nmax * sizeof(char *));
    if (nstr == NULL) {
      return -1;
    }
    sl->sl_str = nstr;
    sl->sl_max = nmax;
  }
  sl->sl_str[sl->sl_cur++] = name;
  sl->sl_str[sl->sl_cur] = NULL;
  return 0;
}

/**
 * @brief Free a StringList and optionally its elements.
 */
void sl_free(StringList *sl, int all) {
  size_t i;
  if (sl == NULL) {
    return;
  }
  if (sl->sl_str != NULL) {
    if (all) {
      for (i = 0; i < sl->sl_cur; i++) {
        BSD_STRINGLIST_FREE(sl->sl_str[i]);
      }
    }
    BSD_STRINGLIST_FREE(sl->sl_str);
  }
  BSD_STRINGLIST_FREE(sl);
}

/**
 * @brief Find a string in a StringList by strcmp.
 * @param sl Target StringList.
 * @param name NUL-terminated string to search for.
 * @return Matching element pointer, or NULL if not found.
 */
char *sl_find(StringList *sl, const char *name) {
  size_t i;
  if (sl == NULL || name == NULL) {
    return NULL;
  }
  for (i = 0; i < sl->sl_cur; i++) {
    if (sl->sl_str[i] != NULL && strcmp(sl->sl_str[i], name) == 0) {
      return sl->sl_str[i];
    }
  }
  return NULL;
}

/**
 * @brief Delete a string from a StringList and optionally free it.
 * @return 0 if found and removed, or -1 if not found or invalid arguments.
 */
int sl_delete(StringList *sl, const char *name, int all) {
  size_t i;
  size_t j;
  if (sl == NULL || name == NULL) {
    return -1;
  }
  for (i = 0; i < sl->sl_cur; i++) {
    if (sl->sl_str[i] != NULL && strcmp(sl->sl_str[i], name) == 0) {
      if (all) {
        BSD_STRINGLIST_FREE(sl->sl_str[i]);
      }
      for (j = i + 1; j < sl->sl_cur; j++) {
        sl->sl_str[j - 1] = sl->sl_str[j];
      }
      sl->sl_cur--;
      sl->sl_str[sl->sl_cur] = NULL;
      return 0;
    }
  }
  return -1;
}

typedef int make_iso_compilers_happy_tu_bsd_stringlist;

#endif
typedef int make_iso_compilers_happy_mega_bsd_stringlist;
