/* posix-libgen.c - Strict C89 Implementation */
#undef _GNU_SOURCE
/* clang-format off */
#include "posix-libgen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Carefully guard every header that is not available on MSVC throughout the
 * codebase; to prevent errors. */
#if !defined(_MSC_VER) && !defined(__WATCOMC__) && !defined(__MSDOS__)
#include <sys/param.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif
#endif
/* clang-format on */

/* For printf formatting things you can  * later guarded for msvc and c89 etc.
 */

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

/* Helper function returning int (exit code) */
static int get_path_len(const char *path, size_t *out_len) {
  if (path == NULL) {
    *out_len = 0;
    return 1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  *out_len = strnlen_s(path, _MAX_PATH);
#else
  *out_len = strlen(path);
#endif
  return 0;
}

char *basename(char *path) {
  static char result[_MAX_PATH];
  char *p;
  size_t len = 0;
  size_t i;
  int is_all_slashes = 1;

  if (path == NULL || *path == '\0') {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(result, sizeof(result), ".");
#else
    strcpy(result, ".");
#endif
    return result;
  }

  if (get_path_len(path, &len) != 0) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(result, sizeof(result), ".");
#else
    strcpy(result, ".");
#endif
    return result;
  }

  for (i = 0; i < len; ++i) {
    if (path[i] != '/' && path[i] != '\\') {
      is_all_slashes = 0;
      break;
    }
  }

  if (is_all_slashes) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(result, sizeof(result), "/");
#else
    strcpy(result, "/");
#endif
    return result;
  }

  /* Windows: strip trailing slashes */
  while (len > 0 && (path[len - 1] == '/' || path[len - 1] == '\\')) {
    path[len - 1] = '\0';
    len--;
  }

  /* Windows: Check if it's just a drive letter */
  if (len == 2 && path[1] == ':') {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(result, sizeof(result), ".");
#else
    strcpy(result, ".");
#endif
    return result;
  }

  /* Find last slash */
  for (p = path + len - 1; p >= path; p--) {
    if (*p == '/' || *p == '\\' || *p == ':') {
#if defined(_MSC_VER) && _MSC_VER >= 1400
      strcpy_s(result, sizeof(result), p + 1);
#else
      strcpy(result, p + 1);
#endif
      return result;
    }
  }

#if defined(_MSC_VER) && _MSC_VER >= 1400
  strcpy_s(result, sizeof(result), path);
#else
  strcpy(result, path);
#endif
  return result;
}

char *dirname(char *path) {
  static char result[_MAX_PATH];
  char *p;
  size_t len = 0;
  size_t i;
  int is_all_slashes = 1;

  if (path == NULL || *path == '\0') {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(result, sizeof(result), ".");
#else
    strcpy(result, ".");
#endif
    return result;
  }

  if (get_path_len(path, &len) != 0) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(result, sizeof(result), ".");
#else
    strcpy(result, ".");
#endif
    return result;
  }

  for (i = 0; i < len; ++i) {
    if (path[i] != '/' && path[i] != '\\') {
      is_all_slashes = 0;
      break;
    }
  }

  if (is_all_slashes) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(result, sizeof(result), "/");
#else
    strcpy(result, "/");
#endif
    return result;
  }

  /* Strip trailing slashes */
  while (len > 1 && (path[len - 1] == '/' || path[len - 1] == '\\')) {
    path[len - 1] = '\0';
    len--;
  }

  for (p = path + len - 1; p >= path; p--) {
    if (*p == '/' || *p == '\\') {
      while (p > path && (*p == '/' || *p == '\\')) {
        if (p - path == 2 && path[1] == ':') {
          *(p + 1) = '\0';
#if defined(_MSC_VER) && _MSC_VER >= 1400
          strcpy_s(result, sizeof(result), path);
#else
          strcpy(result, path);
#endif
          return result;
        }
        *p = '\0';
        p--;
      }
      if (p == path && (*p == '/' || *p == '\\')) {
        *(p + 1) = '\0';
      } else {
        if (p >= path && (*p == '/' || *p == '\\'))
          *p = '\0';
        else
          *(p + 1) = '\0';
      }
      if (path[0] == '\0') {
#if defined(_MSC_VER) && _MSC_VER >= 1400
        strcpy_s(result, sizeof(result), ".");
#else
        strcpy(result, ".");
#endif
        return result;
      }
#if defined(_MSC_VER) && _MSC_VER >= 1400
      strcpy_s(result, sizeof(result), path);
#else
      strcpy(result, path);
#endif
      return result;
    } else if (*p == ':') {
      if (p == path + 1) {
        *(p + 1) = '\0';
#if defined(_MSC_VER) && _MSC_VER >= 1400
        strcpy_s(result, sizeof(result), path);
#else
        strcpy(result, path);
#endif
        return result;
      }
    }
  }

#if defined(_MSC_VER) && _MSC_VER >= 1400
  strcpy_s(result, sizeof(result), ".");
#else
  strcpy(result, ".");
#endif
  return result;
}

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_libgen(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_libgen;
