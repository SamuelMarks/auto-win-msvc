#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__) ||          \
    defined(__linux__) || defined(__CYGWIN__)
/**
 * @file bsd_string.c
 * @brief Implementation of BSD string and mode manipulation functions.
 */

/* clang-format off */
#include "bsd/string.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <sys/stat.h>
/* clang-format on */

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)

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

char *strnstr(const char *big, const char *little, size_t len) {
  size_t little_len;
  if (big == NULL || little == NULL) {
    return NULL;
  }
  little_len = strlen(little);
  if (little_len == 0) {
    return (char *)(uintptr_t)big;
  }
  while (*big != '\0' && len >= little_len) {
    if (strncmp(big, little, little_len) == 0) {
      return (char *)(uintptr_t)big;
    }
    big++;
    len--;
  }
  return NULL;
}

void strmode(int mode, char *p) {
  if (p == NULL) {
    return;
  }
  switch (mode & S_IFMT) {
  case S_IFDIR:
    *p++ = 'd';
    break;
  case S_IFCHR:
    *p++ = 'c';
    break;
#ifdef S_IFBLK
  case S_IFBLK:
    *p++ = 'b';
    break;
#endif
  case S_IFREG:
    *p++ = '-';
    break;
#ifdef S_IFLNK
  case S_IFLNK:
    *p++ = 'l';
    break;
#endif
#ifdef S_IFSOCK
  case S_IFSOCK:
    *p++ = 's';
    break;
#endif
#ifdef S_IFIFO
  case S_IFIFO:
    *p++ = 'p';
    break;
#endif
  default:
    *p++ = '?';
    break;
  }
  *p++ = (mode & 0400) ? 'r' : '-';
  *p++ = (mode & 0200) ? 'w' : '-';
  switch (mode & (0100 | 04000)) {
  case 0:
    *p++ = '-';
    break;
  case 0100:
    *p++ = 'x';
    break;
  case 04000:
    *p++ = 'S';
    break;
  case 0100 | 04000:
    *p++ = 's';
    break;
  }
  *p++ = (mode & 0040) ? 'r' : '-';
  *p++ = (mode & 0020) ? 'w' : '-';
  switch (mode & (0010 | 02000)) {
  case 0:
    *p++ = '-';
    break;
  case 0010:
    *p++ = 'x';
    break;
  case 02000:
    *p++ = 'S';
    break;
  case 0010 | 02000:
    *p++ = 's';
    break;
  }
  *p++ = (mode & 0004) ? 'r' : '-';
  *p++ = (mode & 0002) ? 'w' : '-';
  switch (mode & (0001 | 01000)) {
  case 0:
    *p++ = '-';
    break;
  case 0001:
    *p++ = 'x';
    break;
  case 01000:
    *p++ = 'T';
    break;
  case 0001 | 01000:
    *p++ = 't';
    break;
  }
  *p++ = ' ';
  *p = '\0';
}

void explicit_bzero(void *buf, size_t len) {
  volatile unsigned char *p = (volatile unsigned char *)buf;
  if (buf == NULL) {
    return;
  }
  while (len--) {
    *p++ = 0;
  }
}

int timingsafe_bcmp(const void *b1, const void *b2, size_t n) {
  const unsigned char *p1 = (const unsigned char *)b1;
  const unsigned char *p2 = (const unsigned char *)b2;
  int ret = 0;
  for (; n > 0; n--) {
    ret |= *p1++ ^ *p2++;
  }
  return (ret != 0);
}

int timingsafe_memcmp(const void *b1, const void *b2, size_t n) {
  const unsigned char *p1 = (const unsigned char *)b1;
  const unsigned char *p2 = (const unsigned char *)b2;
  size_t i;
  int res = 0, done = 0;
  for (i = 0; i < n; i++) {
    int lt = p1[i] < p2[i];
    int gt = p1[i] > p2[i];
    int cmp = gt - lt;
    res |= cmp & ~done;
    done |= -(lt | gt);
  }
  return res;
}

/** @brief Internal flag table entry for fflagstostr/strtofflags. */
struct awm_fflag_entry {
  const char *name;
  unsigned long flag;
};

static const struct awm_fflag_entry g_fflag_table[] = {
    {"nodump", 0x00000001UL},     {"uchg", 0x00000002UL},
    {"uimmutable", 0x00000002UL}, {"uappnd", 0x00000004UL},
    {"uappend", 0x00000004UL},    {"opaque", 0x00000008UL},
    {"uunlnk", 0x00000010UL},     {"uunlink", 0x00000010UL},
    {"arch", 0x00010000UL},       {"archived", 0x00010000UL},
    {"schg", 0x00020000UL},       {"simmutable", 0x00020000UL},
    {"sappnd", 0x00040000UL},     {"sappend", 0x00040000UL},
    {"sunlnk", 0x00100000UL},     {"sunlink", 0x00100000UL},
    {"snapshot", 0x00200000UL}};

char *fflagstostr(unsigned long flags) {
  char *buf;
  size_t i;
  unsigned long rem = flags;
  buf = (char *)malloc(256);
  if (buf == NULL) {
    return NULL;
  }
  buf[0] = '\0';
  for (i = 0; i < sizeof(g_fflag_table) / sizeof(g_fflag_table[0]); i++) {
    if ((rem & g_fflag_table[i].flag) != 0) {
      if (buf[0] != '\0') {
        strlcat(buf, ",", 256);
      }
      strlcat(buf, g_fflag_table[i].name, 256);
      rem &= ~g_fflag_table[i].flag;
    }
  }
  return buf;
}

int strtofflags(char **stringp, unsigned long *setp, unsigned long *clrp) {
  char *str, *tok;
  unsigned long setf = 0, clrf = 0;
  if (setp) {
    *setp = 0;
  }
  if (clrp) {
    *clrp = 0;
  }
  if (stringp == NULL || *stringp == NULL) {
    return 0;
  }
  str = *stringp;
  while ((tok = strsep(&str, "\t ,")) != NULL) {
    int is_clear = 0;
    size_t i;
    int found = 0;
    if (*tok == '\0') {
      continue;
    }
    if (strncmp(tok, "no", 2) == 0 && strcmp(tok, "nodump") != 0) {
      is_clear = 1;
      tok += 2;
    }
    for (i = 0; i < sizeof(g_fflag_table) / sizeof(g_fflag_table[0]); i++) {
      if (strcmp(tok, g_fflag_table[i].name) == 0) {
        if (is_clear) {
          clrf |= g_fflag_table[i].flag;
        } else {
          setf |= g_fflag_table[i].flag;
        }
        found = 1;
        break;
      }
    }
    if (!found) {
      *stringp = tok;
      return 1;
    }
  }
  if (setp) {
    *setp = setf;
  }
  if (clrp) {
    *clrp = clrf;
  }
  return 0;
}

#endif /* !BSD && !__APPLE__ */

#if defined(_MSC_VER) || defined(_WIN32) || defined(__DOS__) ||                \
    defined(__MSDOS__) || defined(__WATCOMC__)

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

#if defined(_MSC_VER) || defined(__WATCOMC__)
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
    return strchr(haystack, *haystack);
  }
  while (*haystack != '\0') {
#if defined(_MSC_VER) || defined(__WATCOMC__)
    if (_strnicmp(haystack, needle, nlen) == 0) {
      return strchr(haystack, *haystack);
    }
#else
    if (strncasecmp(haystack, needle, nlen) == 0) {
      return strchr(haystack, *haystack);
    }
#endif
    haystack++;
  }
  return NULL;
}

#endif /* _MSC_VER || _WIN32 || __DOS__ || __MSDOS__ || __WATCOMC__ */

int bsd_string_dummy_for_coverage(void) { return 0; }

typedef int make_iso_compilers_happy_bsd_string_c;

#endif
typedef int make_iso_compilers_happy_mega_batch2_4;
