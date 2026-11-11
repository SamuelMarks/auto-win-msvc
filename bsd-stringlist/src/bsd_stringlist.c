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

StringList *sl_init(void) {
  StringList *sl = (StringList *)calloc(1, sizeof(*sl));
  if (sl == NULL) {
    return NULL;
  }
  sl->sl_max = 20;
  sl->sl_cur = 0;
  sl->sl_str = (char **)calloc(sl->sl_max, sizeof(char *));
  if (sl->sl_str == NULL) {
    free(sl);
    return NULL;
  }
  return sl;
}

int sl_add(StringList *sl, char *name) {
  if (sl == NULL) {
    return -1;
  }
  if (sl->sl_cur + 1 >= sl->sl_max) {
    size_t nmax = sl->sl_max * 2;
    char **nstr = (char **)realloc(sl->sl_str, nmax * sizeof(char *));
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

void sl_free(StringList *sl, int all) {
  size_t i;
  if (sl == NULL) {
    return;
  }
  if (sl->sl_str != NULL) {
    if (all) {
      for (i = 0; i < sl->sl_cur; i++) {
        free(sl->sl_str[i]);
      }
    }
    free(sl->sl_str);
  }
  free(sl);
}

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

int sl_delete(StringList *sl, const char *name, int all) {
  size_t i, j;
  if (sl == NULL || name == NULL) {
    return -1;
  }
  for (i = 0; i < sl->sl_cur; i++) {
    if (sl->sl_str[i] != NULL && strcmp(sl->sl_str[i], name) == 0) {
      if (all) {
        free(sl->sl_str[i]);
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
