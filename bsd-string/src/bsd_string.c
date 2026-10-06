/**
 * @file bsd_string.c
 * @brief Implementation of BSD string functions.
 */

/* clang-format off */
#include "bsd/string.h"
#include <string.h>
#include <stddef.h>
#include <errno.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

size_t strlcpy(char *dst, const char *src, size_t size) {
  size_t srclen;
  if (src == NULL) {
    return 0;
  }
  srclen = strlen(src);
  if (dst != NULL && size > 0) {
    size_t copylen = (srclen >= size) ? size - 1 : srclen;
    memcpy(dst, src, copylen);
    dst[copylen] = '\0';
  }
  return srclen;
}

size_t strlcat(char *dst, const char *src, size_t size) {
  size_t dstlen, srclen;
  if (dst == NULL || src == NULL) {
    return 0;
  }
  dstlen = strlen(dst);
  srclen = strlen(src);
  if (dstlen >= size) {
    return size + srclen;
  }
  if (dstlen + srclen < size) {
    memcpy(dst + dstlen, src, srclen + 1);
  } else {
    memcpy(dst + dstlen, src, size - dstlen - 1);
    dst[size - 1] = '\0';
  }
  return dstlen + srclen;
}

char *strsep(char **stringp, const char *delim) {
  char *begin, *end;
  if (stringp == NULL || delim == NULL) {
    return NULL;
  }
  begin = *stringp;
  if (begin == NULL) {
    return NULL;
  }
  end = begin + strcspn(begin, delim);
  if (*end != '\0') {
    *end++ = '\0';
    *stringp = end;
  } else {
    *stringp = NULL;
  }
  return begin;
}

#if defined(_MSC_VER)
int strcasecmp(const char *s1, const char *s2) {
  if (s1 == NULL || s2 == NULL) {
    return (s1 == s2) ? 0 : (s1 ? 1 : -1);
  }
  return _stricmp(s1, s2);
}

int strncasecmp(const char *s1, const char *s2, size_t n) {
  if (s1 == NULL || s2 == NULL) {
    return (s1 == s2) ? 0 : (s1 ? 1 : -1);
  }
  return _strnicmp(s1, s2, n);
}
#endif

int strverscmp(const char *s1, const char *s2) {
  if (s1 == NULL || s2 == NULL) {
    return (s1 == s2) ? 0 : (s1 ? 1 : -1);
  }
  return strcmp(s1, s2);
}

char *strcasestr(const char *haystack, const char *needle) {
  size_t nlen;
  if (haystack == NULL || needle == NULL) {
    return NULL;
  }
  nlen = strlen(needle);
  if (nlen == 0) {
    return (char *)haystack;
  }
  while (*haystack != '\0') {
#if defined(_MSC_VER)
    if (_strnicmp(haystack, needle, nlen) == 0) {
      return (char *)haystack;
    }
#else
    if (strncasecmp(haystack, needle, nlen) == 0) {
      return (char *)haystack;
    }
#endif
    haystack++;
  }
  return NULL;
}

#endif /* _MSC_VER || _WIN32 */

int bsd_string_dummy_for_coverage(void) { return 0; }

typedef int make_iso_compilers_happy_bsd_string_c;
