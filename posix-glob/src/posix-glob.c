/**
 * @file posix-glob.c
 * @brief Strict C89 implementation of fnmatch, glob, and wordexp.
 */
#if !defined(_MSC_VER)
#ifndef _POSIX_C_SOURCE
/** @brief Request POSIX.1-2008 interfaces. */
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _XOPEN_SOURCE
/** @brief Request X/Open 7 interfaces. */
#define _XOPEN_SOURCE 700
#endif
#endif

/* clang-format off */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "posix-glob.h"
#if defined(_WIN32) || defined(__WATCOMC__) || defined(__MSDOS__) || defined(_MSC_VER)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#else
#if defined(__WATCOMC__)
#include <direct.h>
#else
#include <dirent.h>
#endif
#include <sys/stat.h>
#endif
/* clang-format on */

#if defined(POSIX_GLOB_MOCK_MALLOC)
#if defined(_WIN32)
/** @brief Export modifier for glob OOM mock pointers on Windows. */
#define POSIX_GLOB_HOOK_EXPORT __declspec(dllexport)
#else
/** @brief Export modifier for glob OOM mock pointers. */
#define POSIX_GLOB_HOOK_EXPORT
#endif
/** @brief Optional test hook overriding malloc in posix-glob. */
POSIX_GLOB_HOOK_EXPORT void *(*posix_glob_mock_malloc_ptr)(size_t) = NULL;
/** @brief Optional test hook overriding realloc in posix-glob. */
POSIX_GLOB_HOOK_EXPORT void *(*posix_glob_mock_realloc_ptr)(void *,
                                                            size_t) = NULL;
static void *my_malloc(size_t size) {
  return posix_glob_mock_malloc_ptr ? posix_glob_mock_malloc_ptr(size)
                                    : malloc(size);
}
static void *my_realloc(void *ptr, size_t size) {
  return posix_glob_mock_realloc_ptr ? posix_glob_mock_realloc_ptr(ptr, size)
                                     : realloc(ptr, size);
}
/** @brief Redirect malloc to OOM-mockable wrapper. */
#define malloc my_malloc
/** @brief Redirect realloc to OOM-mockable wrapper. */
#define realloc my_realloc
#endif

#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/* helper to append to glob_t */
static int glob_append(glob_t *pglob, const char *path) {
  size_t new_count = pglob->gl_pathc + 1;
  size_t alloc_count = pglob->gl_offs + new_count + 1;
  char **new_pathv =
      (char **)realloc(pglob->gl_pathv, alloc_count * sizeof(char *));
  char *new_str;
  size_t len;

  if (!new_pathv)
    return GLOB_NOSPACE;
  pglob->gl_pathv = new_pathv;

  len = strlen(path) + 1;
  new_str = (char *)malloc(len);
  if (!new_str)
    return GLOB_NOSPACE;

#if defined(_MSC_VER)
  strcpy_s(new_str, len, path);
#else
  strcpy(new_str, path);
#endif

  pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = new_str;
  pglob->gl_pathc++;
  pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;

  return 0;
}

/** \brief glob_compare function. */
static int glob_compare(const void *a, const void *b) {
  const char *sa = *(const char *const *)a;
  const char *sb = *(const char *const *)b;
  if (!sa && !sb)
    return 0;
  if (!sa)
    return 1;
  if (!sb)
    return -1;
  return strcmp(sa, sb);
}

/** \brief fnmatch function. */
int fnmatch(const char *pattern, const char *string, int flags) {
  const char *p = pattern;
  const char *s = string;
  int c;

  if (!pattern || !string)
    return FNM_NOMATCH;

  for (;;) {
    c = *p++;
    switch (c) {
    case '\0':
      return (*s == '\0' ? 0 : FNM_NOMATCH);
    case '?':
      if (*s == '\0')
        return FNM_NOMATCH;
      if ((flags & FNM_PATHNAME) && *s == '/')
        return FNM_NOMATCH;
      if ((flags & FNM_PERIOD) && s == string && *s == '.')
        return FNM_NOMATCH;
      s++;
      break;
    case '*':
      c = *p;
      while (c == '*') {
        c = *++p;
      } /* collapse multiple stars */
      if ((flags & FNM_PERIOD) && s == string && *s == '.')
        return FNM_NOMATCH;
      if (c == '\0') {
        if (flags & FNM_PATHNAME) {
          while (*s != '\0') {
            if (*s == '/')
              return FNM_NOMATCH;
            s++;
          }
          return 0;
        }
        return 0;
      }
      while (*s != '\0') {
        if ((flags & FNM_PATHNAME) && *s == '/')
          break;
        if (fnmatch(p, s, flags) == 0)
          return 0;
        s++;
      }
      return FNM_NOMATCH;
    case '\\':
      if (!(flags & FNM_NOESCAPE)) {
        if (*p != '\0')
          c = *p++;
      }
      /* fall through */
    default:
      if (c != *s)
        return FNM_NOMATCH;
      s++;
      break;
    }
  }
}

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__MSDOS__) ||           \
    defined(_MSC_VER)
/** \brief glob function. */
int glob(const char *pattern, int flags,
         int (*errfunc)(const char *epath, int eerrno), glob_t *pglob) {
  struct _finddata_t fileinfo;
  intptr_t handle;
  char dir_prefix[1024];
  const char *last_slash;
  size_t prefix_len = 0;
  size_t initial_pathc;
  int ret = 0;
  int match_found = 0;

  if (!pattern || !pglob)
    return GLOB_ABORTED;

  pglob->gl_flags = flags;

  if (!(flags & GLOB_APPEND)) {
    pglob->gl_pathc = 0;
    pglob->gl_pathv = NULL;
    if (flags & GLOB_DOOFFS) {
      pglob->gl_pathv = (char **)malloc((pglob->gl_offs + 1) * sizeof(char *));
      if (!pglob->gl_pathv)
        return GLOB_NOSPACE;
      memset(pglob->gl_pathv, 0, (pglob->gl_offs + 1) * sizeof(char *));
    } else {
      pglob->gl_offs = 0;
    }
  }

  initial_pathc = pglob->gl_pathc;

  last_slash = strrchr(pattern, '/');
  if (!last_slash) {
    const char *bslash = strrchr(pattern, '\\');
    if (bslash)
      last_slash = bslash;
  }

  if (last_slash) {
    prefix_len = (size_t)(last_slash - pattern) + 1;
    if (prefix_len > sizeof(dir_prefix) - 1)
      prefix_len = sizeof(dir_prefix) - 1;
#if defined(_MSC_VER)
    strncpy_s(dir_prefix, sizeof(dir_prefix), pattern, prefix_len);
#else
    strncpy(dir_prefix, pattern, prefix_len);
    dir_prefix[prefix_len] = '\0';
#endif
  } else {
    dir_prefix[0] = '\0';
  }

  {
#if defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
    char dos_pat[1032];
    size_t plen = strlen(pattern);
    size_t i_p;
    const char *base_p;
    if (plen > sizeof(dos_pat) - 4) {
      plen = sizeof(dos_pat) - 4;
    }
    for (i_p = 0; i_p < plen; ++i_p) {
      dos_pat[i_p] = (pattern[i_p] == '/') ? '\\' : pattern[i_p];
    }
    dos_pat[plen] = '\0';
    base_p = strrchr(dos_pat, '\\');
    base_p = base_p ? (base_p + 1) : dos_pat;
    if (plen > 0 && dos_pat[plen - 1] == '*' && strchr(base_p, '.') == NULL) {
      dos_pat[plen] = '.';
      dos_pat[plen + 1] = '*';
      dos_pat[plen + 2] = '\0';
    }
#if defined(__WATCOMC__)
    if (_osmajor < 2) {
      handle = -1;
      errno = ENOENT;
    } else {
      handle = _findfirst(dos_pat, &fileinfo);
    }
#else
    handle = _findfirst(dos_pat, &fileinfo);
#endif
#else
    handle = _findfirst(pattern, &fileinfo);
#endif
  }
  if (handle == -1) {
    if (errfunc) {
      if (errfunc(pattern, errno))
        return GLOB_ABORTED;
    }
    if (flags & GLOB_ERR) {
      return GLOB_ABORTED;
    }
  } else {
    do {
      char full_path[2048];
      if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0)
        continue;

      if (prefix_len > 0) {
#if defined(_MSC_VER)
        sprintf_s(full_path, sizeof(full_path), "%s%s", dir_prefix,
                  fileinfo.name);
#else
        sprintf(full_path, "%s%s", dir_prefix, fileinfo.name);
#endif
      } else {
#if defined(_MSC_VER)
        strcpy_s(full_path, sizeof(full_path), fileinfo.name);
#else
        strcpy(full_path, fileinfo.name);
#endif
      }

      ret = glob_append(pglob, full_path);
      if (ret != 0)
        break;
      match_found = 1;
    } while (_findnext(handle, &fileinfo) == 0);
    _findclose(handle);
  }

  if (ret != 0) {
    if (!(flags & GLOB_APPEND)) {
      globfree(pglob);
    }
    return ret;
  }

  if (!match_found) {
    int has_magic = (strpbrk(pattern, "*?[") != NULL);
    if ((flags & GLOB_NOCHECK) || ((flags & GLOB_NOMAGIC) && !has_magic)) {
      pglob->gl_flags |= GLOB_NOMAGIC;
      ret = glob_append(pglob, pattern);
      if (ret != 0 && !(flags & GLOB_APPEND)) {
        globfree(pglob);
      }
      return ret;
    }
    if (!(flags & GLOB_APPEND)) {
      globfree(pglob);
    }
    return GLOB_NOMATCH;
  }

  if (!(flags & GLOB_NOSORT)) {
    qsort(pglob->gl_pathv + pglob->gl_offs + initial_pathc,
          pglob->gl_pathc - initial_pathc, sizeof(char *), glob_compare);
  }

  return 0;
}
#else
/** \brief glob function. */
int glob(const char *pattern, int flags,
         int (*errfunc)(const char *epath, int eerrno), glob_t *pglob) {
  DIR *dir;
  struct dirent *ent;
  char dir_prefix[1024];
  const char *last_slash;
  size_t prefix_len = 0;
  size_t initial_pathc;
  int ret = 0;
  int match_found = 0;
  char search_dir[1024];
  const char *file_pattern;

  if (!pattern || !pglob)
    return GLOB_ABORTED;

  pglob->gl_flags = flags;

  if (!(flags & GLOB_APPEND)) {
    pglob->gl_pathc = 0;
    pglob->gl_pathv = NULL;
    if (flags & GLOB_DOOFFS) {
      pglob->gl_pathv = (char **)malloc((pglob->gl_offs + 1) * sizeof(char *));
      if (!pglob->gl_pathv)
        return GLOB_NOSPACE;
      memset(pglob->gl_pathv, 0, (pglob->gl_offs + 1) * sizeof(char *));
    } else {
      pglob->gl_offs = 0;
    }
  }

  initial_pathc = pglob->gl_pathc;

  last_slash = strrchr(pattern, '/');
  if (last_slash) {
    prefix_len = (size_t)(last_slash - pattern) + 1;
    if (prefix_len > sizeof(dir_prefix) - 1)
      prefix_len = sizeof(dir_prefix) - 1;
    strncpy(dir_prefix, pattern, prefix_len);
    dir_prefix[prefix_len] = '\0';

    if (prefix_len > 1) {
      strncpy(search_dir, pattern, prefix_len - 1);
      search_dir[prefix_len - 1] = '\0';
    } else {
#if defined(_MSC_VER)
      strcpy_s(search_dir, (strlen("/") + 1), "/");
#else
      strcpy(search_dir, "/");
#endif
    }
    file_pattern = last_slash + 1;
  } else {
    dir_prefix[0] = '\0';
#if defined(_MSC_VER)
    strcpy_s(search_dir, (strlen(".") + 1), ".");
#else
    strcpy(search_dir, ".");
#endif
    file_pattern = pattern;
  }

  dir = opendir(search_dir);
  if (!dir) {
    if (errfunc) {
      if (errfunc(search_dir, errno))
        return GLOB_ABORTED;
    }
    if (flags & GLOB_ERR) {
      return GLOB_ABORTED;
    }
  } else {
    while ((ent = readdir(dir)) != NULL) {
      if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        continue;

      if (fnmatch(file_pattern, ent->d_name, 0) == 0) {
        char full_path[2048];
        if (prefix_len > 0) {
#if defined(_MSC_VER)
          sprintf_s(full_path, MAX_PATH, "%s%s", dir_prefix, ent->d_name);
#else
#if defined(_MSC_VER)
          sprintf_s(full_path, sizeof(full_path), "%s%s", dir_prefix,
                    ent->d_name);
#else
          sprintf(full_path, "%s%s", dir_prefix, ent->d_name);
#endif
#endif
        } else {
#if defined(_MSC_VER)
          strcpy_s(full_path, MAX_PATH, ent->d_name);
#else
          strcpy(full_path, ent->d_name);
#endif
        }

        ret = glob_append(pglob, full_path);
        if (ret != 0)
          break;
        match_found = 1;
      }
    }
    closedir(dir);
  }

  if (ret != 0) {
    if (!(flags & GLOB_APPEND)) {
      globfree(pglob);
    }
    return ret;
  }

  if (!match_found) {
    int has_magic = (strpbrk(pattern, "*?[") != NULL);
    if ((flags & GLOB_NOCHECK) || ((flags & GLOB_NOMAGIC) && !has_magic)) {
      pglob->gl_flags |= GLOB_NOMAGIC;
      ret = glob_append(pglob, pattern);
      if (ret != 0 && !(flags & GLOB_APPEND)) {
        globfree(pglob);
      }
      return ret;
    }
    if (!(flags & GLOB_APPEND)) {
      globfree(pglob);
    }
    return GLOB_NOMATCH;
  }

  if (!(flags & GLOB_NOSORT)) {
    qsort(pglob->gl_pathv + pglob->gl_offs + initial_pathc,
          pglob->gl_pathc - initial_pathc, sizeof(char *), glob_compare);
  }

  return 0;
}
#endif

/** \brief globfree function. */
void globfree(glob_t *pglob) {
  size_t i;
  if (!pglob || !pglob->gl_pathv)
    return;
  for (i = 0; i < pglob->gl_pathc; i++) {
    free(pglob->gl_pathv[pglob->gl_offs + i]);
  }
  free(pglob->gl_pathv);
  pglob->gl_pathv = NULL;
  pglob->gl_pathc = 0;
}

/** \brief wordexp function. */
int wordexp(const char *words, wordexp_t *pwordexp, int flags) {
  char *copy;
  char *token;
#if defined(_MSC_VER) ||                                                       \
    (!defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__))
  char *context = NULL;
#endif
  char **new_we_wordv;

  if (!words || !pwordexp)
    return WRDE_SYNTAX;

  if (!(flags & WRDE_APPEND)) {
    pwordexp->we_wordc = 0;
    pwordexp->we_wordv = NULL;
    if (flags & WRDE_DOOFFS) {
      pwordexp->we_wordv =
          (char **)malloc((pwordexp->we_offs + 1) * sizeof(char *));
      if (!pwordexp->we_wordv)
        return WRDE_NOSPACE;
      memset(pwordexp->we_wordv, 0, (pwordexp->we_offs + 1) * sizeof(char *));
    } else {
      pwordexp->we_offs = 0;
    }
  }

  copy = (char *)malloc(strlen(words) + 1);
  if (!copy)
    return WRDE_NOSPACE;

#if defined(_MSC_VER)
  strcpy_s(copy, strlen(words) + 1, words);
  token = strtok_s(copy, " \t\n", &context);
#else
  strcpy(copy, words);
#if defined(_WIN32) || defined(__WATCOMC__) || defined(__MSDOS__)
  token = strtok(copy, " \t\n");
#else
  token = strtok_r(copy, " \t\n", &context);
#endif
#endif

  while (token) {
    size_t len = strlen(token) + 1;
    char *word = (char *)malloc(len);
    if (!word) {
      free(copy);
      return WRDE_NOSPACE;
    }
#if defined(_MSC_VER)
    strcpy_s(word, len, token);
#else
    strcpy(word, token);
#endif

    new_we_wordv = (char **)realloc(
        pwordexp->we_wordv,
        (pwordexp->we_offs + pwordexp->we_wordc + 2) * sizeof(char *));
    if (!new_we_wordv) {
      free(word);
      free(copy);
      return WRDE_NOSPACE;
    }
    pwordexp->we_wordv = new_we_wordv;
    pwordexp->we_wordv[pwordexp->we_offs + pwordexp->we_wordc] = word;
    pwordexp->we_wordc++;
    pwordexp->we_wordv[pwordexp->we_offs + pwordexp->we_wordc] = NULL;

#if defined(_MSC_VER)
    token = strtok_s(NULL, " \t\n", &context);
#else
#if defined(_WIN32) || defined(__WATCOMC__) || defined(__MSDOS__)
    token = strtok(NULL, " \t\n");
#else
    token = strtok_r(NULL, " \t\n", &context);
#endif
#endif
  }
  free(copy);

  return 0;
}

/** \brief wordfree function. */
void wordfree(wordexp_t *pwordexp) {
  size_t i;
  if (!pwordexp || !pwordexp->we_wordv)
    return;
  for (i = 0; i < pwordexp->we_wordc; i++) {
    free(pwordexp->we_wordv[pwordexp->we_offs + i]);
  }
  free(pwordexp->we_wordv);
  pwordexp->we_wordv = NULL;
  pwordexp->we_wordc = 0;
}

/**
 * @brief Retrieves information on posix-glob module availability.
 */
auto_win_msvc_error_t posix_glob_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

/** @brief Dummy typedef preventing empty translation unit warnings. */
typedef int make_iso_compilers_happy_tu;

/** @brief Dummy anchor symbol ensuring non-empty posix-glob archive. */
int dummy_posix_glob(void);
int dummy_posix_glob(void) { return 0; }

/** @brief Internal qsort comparator exposed for unit testing. */
int posix_glob_compare_test_internal(const void *a, const void *b);
/** @brief Internal qsort comparator exposed for unit testing. */
int posix_glob_compare_test_internal(const void *a, const void *b) {
  return glob_compare(a, b);
}

#endif
/** @brief Dummy typedef preventing empty mega translation unit warnings. */
typedef int make_iso_compilers_happy_tu_posix_glob;
