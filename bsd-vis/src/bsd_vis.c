#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_vis.c
 * @brief Implementation of BSD <vis.h> string encoding and decoding.
 */

/* clang-format off */
#include <vis.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

char *snvis(char *dst, size_t dlen, int c, int flag, int nextc,
            const char *extra) {
  unsigned char uc = (unsigned char)c;
  int encode = 0;
  (void)nextc;
  if (dst == NULL || dlen < 5) {
    return NULL;
  }
  if (extra != NULL && strchr(extra, c) != NULL) {
    encode = 1;
  } else if (!isascii(uc) || !isgraph(uc)) {
    if (uc == ' ' && (flag & VIS_SP) == 0) {
      encode = 0;
    } else if (uc == '\t' && (flag & VIS_TAB) == 0 && (flag & VIS_SAFE)) {
      encode = 0;
    } else if (uc == '\n' && (flag & VIS_NL) == 0 && (flag & VIS_SAFE)) {
      encode = 0;
    } else {
      encode = 1;
    }
  }
  if (uc == '\\' && (flag & (VIS_NOSLASH | VIS_NOESCAPE)) == 0) {
    *dst++ = '\\';
    *dst++ = '\\';
    *dst = '\0';
    return dst;
  }
  if (!encode) {
    *dst++ = (char)uc;
    *dst = '\0';
    return dst;
  }
  if (flag & VIS_CSTYLE) {
    switch (uc) {
    case '\n':
      *dst++ = '\\';
      *dst++ = 'n';
      *dst = '\0';
      return dst;
    case '\r':
      *dst++ = '\\';
      *dst++ = 'r';
      *dst = '\0';
      return dst;
    case '\b':
      *dst++ = '\\';
      *dst++ = 'b';
      *dst = '\0';
      return dst;
    case '\a':
      *dst++ = '\\';
      *dst++ = 'a';
      *dst = '\0';
      return dst;
    case '\v':
      *dst++ = '\\';
      *dst++ = 'v';
      *dst = '\0';
      return dst;
    case '\t':
      *dst++ = '\\';
      *dst++ = 't';
      *dst = '\0';
      return dst;
    case '\f':
      *dst++ = '\\';
      *dst++ = 'f';
      *dst = '\0';
      return dst;
    case ' ':
      *dst++ = '\\';
      *dst++ = 's';
      *dst = '\0';
      return dst;
    case '\0':
      *dst++ = '\\';
      *dst++ = '0';
      *dst = '\0';
      return dst;
    default:
      break;
    }
  }
  *dst++ = '\\';
  *dst++ = (char)(((uc >> 6) & 03) + '0');
  *dst++ = (char)(((uc >> 3) & 07) + '0');
  *dst++ = (char)((uc & 07) + '0');
  *dst = '\0';
  return dst;
}

char *svis(char *dst, int c, int flag, int nextc, const char *extra) {
  return snvis(dst, 5, c, flag, nextc, extra);
}

char *nvis(char *dst, size_t dlen, int c, int flag, int nextc) {
  return snvis(dst, dlen, c, flag, nextc, "");
}

char *vis(char *dst, int c, int flag, int nextc) {
  return snvis(dst, 5, c, flag, nextc, "");
}

int strsnvisx(char *dst, size_t dlen, const char *src, size_t len, int flag,
              const char *extra) {
  char *start = dst;
  char tbuf[8];
  size_t i;
  if (dst == NULL || dlen == 0) {
    errno = ENOSPC;
    return -1;
  }
  for (i = 0; i < len; i++) {
    int nextc = (i + 1 < len) ? (unsigned char)src[i + 1] : 0;
    char *end = snvis(tbuf, sizeof(tbuf), (unsigned char)src[i], flag, nextc,
                      extra ? extra : "");
    size_t clen = (size_t)(end - tbuf);
    if ((size_t)(dst - start) + clen + 1 > dlen) {
      *dst = '\0';
      errno = ENOSPC;
      return -1;
    }
    memcpy(dst, tbuf, clen);
    dst += clen;
  }
  *dst = '\0';
  return (int)(dst - start);
}

int strsvisx(char *dst, const char *src, size_t len, int flag,
             const char *extra) {
  return strsnvisx(dst, len * 4 + 1, src, len, flag, extra);
}

int strnvisx(char *dst, size_t dlen, const char *src, size_t len, int flag) {
  return strsnvisx(dst, dlen, src, len, flag, "");
}

int strenvisx(char *dst, size_t dlen, const char *src, size_t len, int flag,
              int *cerr_ptr) {
  if (cerr_ptr != NULL) {
    *cerr_ptr = 0;
  }
  return strsnvisx(dst, dlen, src, len, flag, "");
}

int strvisx(char *dst, const char *src, size_t len, int flag) {
  return strsnvisx(dst, len * 4 + 1, src, len, flag, "");
}

int strsnvis(char *dst, size_t dlen, const char *src, int flag,
             const char *extra) {
  return strsnvisx(dst, dlen, src, src ? strlen(src) : 0, flag, extra);
}

int strsvis(char *dst, const char *src, int flag, const char *extra) {
  size_t len = src ? strlen(src) : 0;
  return strsnvisx(dst, len * 4 + 1, src, len, flag, extra);
}

int strnvis(char *dst, const char *src, size_t dlen, int flag) {
  return strsnvisx(dst, dlen, src, src ? strlen(src) : 0, flag, "");
}

int strvis(char *dst, const char *src, int flag) {
  size_t len = src ? strlen(src) : 0;
  return strsnvisx(dst, len * 4 + 1, src, len, flag, "");
}

int stravis(char **outp, const char *src, int flag) {
  size_t len = src ? strlen(src) : 0;
  size_t cap = len * 4 + 1;
  char *buf;
  if (outp == NULL) {
    errno = EINVAL;
    return -1;
  }
  buf = (char *)malloc(cap);
  if (buf == NULL) {
    *outp = NULL;
    return -1;
  }
  *outp = buf;
  return strsnvisx(buf, cap, src, len, flag, "");
}

int unvis(char *cp, int c, int *astate, int flag) {
  (void)flag;
  if (cp == NULL || astate == NULL) {
    return UNVIS_SYNBAD;
  }
  if (*astate == 0) {
    if (c == '\\') {
      *astate = 1;
      return 0;
    }
    *cp = (char)c;
    return UNVIS_VALID;
  }
  *astate = 0;
  switch (c) {
  case 'n':
    *cp = '\n';
    break;
  case 'r':
    *cp = '\r';
    break;
  case 't':
    *cp = '\t';
    break;
  case 's':
    *cp = ' ';
    break;
  case '\\':
    *cp = '\\';
    break;
  default:
    *cp = (char)c;
    break;
  }
  return UNVIS_VALID;
}

int strnunvisx(char *dst, size_t dlen, const char *src, int flag) {
  char *start = dst;
  int state = 0;
  char c;
  if (dst == NULL || src == NULL || dlen == 0) {
    return -1;
  }
  while ((c = *src++) != '\0') {
    char outc = 0;
    int r = unvis(&outc, (unsigned char)c, &state, flag);
    if (r == UNVIS_VALID) {
      if ((size_t)(dst - start) + 1 >= dlen) {
        *dst = '\0';
        return -1;
      }
      *dst++ = outc;
    } else if (r < 0) {
      return -1;
    }
  }
  *dst = '\0';
  return (int)(dst - start);
}

int strunvisx(char *dst, const char *src, int flag) {
  size_t len = src ? strlen(src) : 0;
  return strnunvisx(dst, len + 1, src, flag);
}

int strnunvis(char *dst, const char *src, size_t dlen) {
  return strnunvisx(dst, dlen, src, 0);
}

int strunvis(char *dst, const char *src) { return strunvisx(dst, src, 0); }

typedef int make_iso_compilers_happy_tu_bsd_vis;

#endif
typedef int make_iso_compilers_happy_mega_bsd_vis;
