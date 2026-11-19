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

static void *default_vis_malloc(size_t size) { return malloc(size); }

/** @brief Overrideable malloc hook for OOM testing in bsd-vis. */
void *(*g_bsd_vis_malloc)(size_t) = default_vis_malloc;

/** @brief Resets g_bsd_vis_malloc back to the standard allocator. */
void bsd_vis_reset_malloc_hook(void);
void bsd_vis_reset_malloc_hook(void) { g_bsd_vis_malloc = default_vis_malloc; }

/**
 * @brief Encode a single character with extra characters into bounded buffer.
 * @param[out] dst Destination buffer (must have room for at least 5 bytes).
 * @param[in] dlen Size of destination buffer in bytes.
 * @param[in] c Character to encode.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] nextc Lookahead next character.
 * @param[in] extra Optional string of additional characters to escape.
 * @return Pointer to NUL terminator in dst, or NULL if buffer too small.
 */
char *snvis(char *dst, size_t dlen, int c, int flag, int nextc,
            const char *extra) {
  unsigned char uc = (unsigned char)c;
  int encode = 0;
  (void)nextc;
  if (dst == NULL || dlen < 5) {
    return NULL;
  }
  if (extra != NULL && uc != '\0' && strchr(extra, (int)uc) != NULL) {
    encode = 1;
  } else if (uc >= 0x80U || !isgraph(uc)) {
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

/**
 * @brief Encode a single character with extra characters.
 * @param[out] dst Destination buffer (at least 5 bytes).
 * @param[in] c Character to encode.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] nextc Lookahead next character.
 * @param[in] extra Additional characters to escape.
 * @return Pointer to NUL terminator in dst, or NULL on error.
 */
char *svis(char *dst, int c, int flag, int nextc, const char *extra) {
  return snvis(dst, 5, c, flag, nextc, extra);
}

/**
 * @brief Encode a single character into a bounded buffer.
 * @param[out] dst Destination buffer.
 * @param[in] dlen Size of destination buffer.
 * @param[in] c Character to encode.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] nextc Lookahead next character.
 * @return Pointer to NUL terminator in dst, or NULL on error.
 */
char *nvis(char *dst, size_t dlen, int c, int flag, int nextc) {
  return snvis(dst, dlen, c, flag, nextc, "");
}

/**
 * @brief Encode a single character visually.
 * @param[out] dst Destination buffer (at least 5 bytes).
 * @param[in] c Character to encode.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] nextc Lookahead next character.
 * @return Pointer to NUL terminator in dst, or NULL on error.
 */
char *vis(char *dst, int c, int flag, int nextc) {
  return snvis(dst, 5, c, flag, nextc, "");
}

/**
 * @brief Visually encode a fixed-length buffer with extra chars into bounded
 * dst.
 * @param[out] dst Destination buffer.
 * @param[in] dlen Size of destination buffer.
 * @param[in] src Source byte buffer.
 * @param[in] len Number of source bytes to encode.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] extra Optional string of extra characters to escape.
 * @return Encoded string length, or -1 if destination buffer is too small.
 */
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

/**
 * @brief Visually encode a fixed-length buffer with extra characters.
 * @param[out] dst Destination buffer.
 * @param[in] src Source byte buffer.
 * @param[in] len Number of source bytes.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] extra Additional characters to escape.
 * @return Encoded string length, or -1 on error.
 */
int strsvisx(char *dst, const char *src, size_t len, int flag,
             const char *extra) {
  return strsnvisx(dst, len * 4 + 1, src, len, flag, extra);
}

/**
 * @brief Visually encode a fixed-length buffer into bounded destination.
 * @param[out] dst Destination buffer.
 * @param[in] dlen Size of destination buffer.
 * @param[in] src Source byte buffer.
 * @param[in] len Number of source bytes.
 * @param[in] flag VIS_* encoding flags.
 * @return Encoded string length, or -1 on error.
 */
int strnvisx(char *dst, size_t dlen, const char *src, size_t len, int flag) {
  return strsnvisx(dst, dlen, src, len, flag, "");
}

/**
 * @brief Visually encode a fixed-length buffer with extra chars and error ptr.
 * @param[out] dst Destination buffer.
 * @param[in] dlen Size of destination buffer.
 * @param[in] src Source byte buffer.
 * @param[in] len Number of source bytes.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] extra Additional characters to escape.
 * @param[out] cerr_ptr Optional conversion error status output.
 * @return Encoded string length, or -1 on error.
 */
int strsenvisx(char *dst, size_t dlen, const char *src, size_t len, int flag,
               const char *extra, int *cerr_ptr) {
  if (cerr_ptr != NULL) {
    *cerr_ptr = 0;
  }
  return strsnvisx(dst, dlen, src, len, flag, extra);
}

/**
 * @brief Visually encode a fixed-length buffer with error pointer.
 * @param[out] dst Destination buffer.
 * @param[in] dlen Size of destination buffer.
 * @param[in] src Source byte buffer.
 * @param[in] len Number of source bytes.
 * @param[in] flag VIS_* encoding flags.
 * @param[out] cerr_ptr Optional conversion error status output.
 * @return Encoded string length, or -1 on error.
 */
int strenvisx(char *dst, size_t dlen, const char *src, size_t len, int flag,
              int *cerr_ptr) {
  if (cerr_ptr != NULL) {
    *cerr_ptr = 0;
  }
  return strsnvisx(dst, dlen, src, len, flag, "");
}

/**
 * @brief Visually encode a fixed-length buffer.
 * @param[out] dst Destination buffer.
 * @param[in] src Source byte buffer.
 * @param[in] len Number of source bytes.
 * @param[in] flag VIS_* encoding flags.
 * @return Encoded string length, or -1 on error.
 */
int strvisx(char *dst, const char *src, size_t len, int flag) {
  return strsnvisx(dst, len * 4 + 1, src, len, flag, "");
}

/**
 * @brief Visually encode a NUL-terminated string with extra characters into
 * bounded buffer.
 * @param[out] dst Destination buffer.
 * @param[in] dlen Size of destination buffer.
 * @param[in] src Source NUL-terminated string.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] extra Additional characters to escape.
 * @return Encoded string length, or -1 on error.
 */
int strsnvis(char *dst, size_t dlen, const char *src, int flag,
             const char *extra) {
  return strsnvisx(dst, dlen, src, src ? strlen(src) : 0, flag, extra);
}

/**
 * @brief Visually encode a NUL-terminated string with extra characters.
 * @param[out] dst Destination buffer.
 * @param[in] src Source NUL-terminated string.
 * @param[in] flag VIS_* encoding flags.
 * @param[in] extra Additional characters to escape.
 * @return Encoded string length, or -1 on error.
 */
int strsvis(char *dst, const char *src, int flag, const char *extra) {
  size_t len = src ? strlen(src) : 0;
  return strsnvisx(dst, len * 4 + 1, src, len, flag, extra);
}

/**
 * @brief Visually encode a NUL-terminated string into bounded buffer.
 * @param[out] dst Destination buffer.
 * @param[in] src Source NUL-terminated string.
 * @param[in] dlen Size of destination buffer.
 * @param[in] flag VIS_* encoding flags.
 * @return Encoded string length, or -1 on error.
 */
int strnvis(char *dst, const char *src, size_t dlen, int flag) {
  return strsnvisx(dst, dlen, src, src ? strlen(src) : 0, flag, "");
}

/**
 * @brief Visually encode a NUL-terminated string.
 * @param[out] dst Destination buffer.
 * @param[in] src Source NUL-terminated string.
 * @param[in] flag VIS_* encoding flags.
 * @return Encoded string length, or -1 on error.
 */
int strvis(char *dst, const char *src, int flag) {
  size_t len = src ? strlen(src) : 0;
  return strsnvisx(dst, len * 4 + 1, src, len, flag, "");
}

/**
 * @brief Visually encode a string into a dynamically allocated buffer.
 * @param[out] outp Pointer receiving allocated NUL-terminated buffer.
 * @param[in] src Source NUL-terminated string.
 * @param[in] flag VIS_* encoding flags.
 * @return Encoded string length, or -1 on error.
 */
int stravis(char **outp, const char *src, int flag) {
  size_t len = src ? strlen(src) : 0;
  size_t cap = len * 4 + 1;
  char *buf;
  if (outp == NULL) {
    errno = EINVAL;
    return -1;
  }
  buf = (char *)g_bsd_vis_malloc(cap);
  if (buf == NULL) {
    *outp = NULL;
    return -1;
  }
  *outp = buf;
  return strsnvisx(buf, cap, src, len, flag, "");
}

/**
 * @brief Decode a visual representation of a character step-by-step.
 * @param[out] cp Output decoded character.
 * @param[in] c Next input byte.
 * @param[in,out] astate Decoder state machine variable.
 * @param[in] flag UNVIS_* / VIS_* decoding flags.
 * @return UNVIS_VALID, UNVIS_NOCHAR, 0, or UNVIS_SYNBAD on syntax error.
 */
int unvis(char *cp, int c, int *astate, int flag) {
  if (cp == NULL || astate == NULL || flag < 0) {
    return UNVIS_SYNBAD;
  }
  if (flag & UNVIS_END) {
    if (*astate == 0) {
      return UNVIS_NOCHAR;
    }
    *astate = 0;
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

/**
 * @brief Decode a vis-encoded string with flags into a bounded buffer.
 * @param[out] dst Destination buffer.
 * @param[in] dlen Size of destination buffer.
 * @param[in] src Source vis-encoded NUL-terminated string.
 * @param[in] flag Decoding flags.
 * @return Decoded byte length, or -1 on error/overflow.
 */
int strnunvisx(char *dst, size_t dlen, const char *src, int flag) {
  char *start = dst;
  int state = 0;
  char outc = 0;
  char c;
  if (dst == NULL || src == NULL || dlen == 0) {
    return -1;
  }
  while ((c = *src++) != '\0') {
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
  if (unvis(&outc, 0, &state, UNVIS_END) == UNVIS_SYNBAD) {
    *dst = '\0';
    return -1;
  }
  *dst = '\0';
  return (int)(dst - start);
}

/**
 * @brief Decode a vis-encoded string with flags.
 * @param[out] dst Destination buffer.
 * @param[in] src Source vis-encoded NUL-terminated string.
 * @param[in] flag Decoding flags.
 * @return Decoded byte length, or -1 on error.
 */
int strunvisx(char *dst, const char *src, int flag) {
  size_t len = src ? strlen(src) : 0;
  return strnunvisx(dst, len + 1, src, flag);
}

/**
 * @brief Decode a vis-encoded string into a bounded buffer.
 * @param[out] dst Destination buffer.
 * @param[in] src Source vis-encoded NUL-terminated string.
 * @param[in] dlen Size of destination buffer.
 * @return Decoded byte length, or -1 on error.
 */
int strnunvis(char *dst, const char *src, size_t dlen) {
  return strnunvisx(dst, dlen, src, 0);
}

/**
 * @brief Decode a vis-encoded string.
 * @param[out] dst Destination buffer.
 * @param[in] src Source vis-encoded NUL-terminated string.
 * @return Decoded byte length, or -1 on error.
 */
int strunvis(char *dst, const char *src) { return strunvisx(dst, src, 0); }

/** @brief Dummy typedef preventing empty translation unit warnings. */
typedef int make_iso_compilers_happy_tu_bsd_vis;

#endif
/** @brief Dummy typedef preventing empty mega translation unit warnings. */
typedef int make_iso_compilers_happy_mega_bsd_vis;
