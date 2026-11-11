#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__) ||          \
    defined(__linux__) || defined(__CYGWIN__)
/**
 * @file bsd_stdlib.c
 * @brief Implementation of BSD standard library, sorting, random, and
 * capability database polyfills.
 */

/* clang-format off */
#include "bsd/stdlib.h"
#include "bsd/string.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif
/* clang-format on */

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)

void *reallocarray(void *ptr, size_t nmemb, size_t size) {
  if (size != 0 && nmemb > ((size_t)-1) / size) {
    errno = ENOMEM;
    return NULL;
  }
  return realloc(ptr, nmemb * size);
}

void *recallocarray(void *ptr, size_t oldnmemb, size_t newnmemb, size_t size) {
  size_t oldsize, newsize;
  void *newptr;
  if (size != 0 &&
      (oldnmemb > ((size_t)-1) / size || newnmemb > ((size_t)-1) / size)) {
    errno = ENOMEM;
    return NULL;
  }
  oldsize = oldnmemb * size;
  newsize = newnmemb * size;
  if (ptr == NULL) {
    return calloc(newnmemb, size);
  }
  if (newsize <= oldsize) {
    size_t diff = oldsize - newsize;
    if (diff > 0) {
      explicit_bzero((char *)ptr + newsize, diff);
    }
  }
  newptr = malloc(newsize);
  if (newptr == NULL) {
    return NULL;
  }
  if (newsize > oldsize) {
    memcpy(newptr, ptr, oldsize);
    memset((char *)newptr + oldsize, 0, newsize - oldsize);
  } else {
    memcpy(newptr, ptr, newsize);
  }
  explicit_bzero(ptr, oldsize);
  free(ptr);
  return newptr;
}

void freezero(void *ptr, size_t size) {
  if (ptr == NULL) {
    return;
  }
  explicit_bzero(ptr, size);
  free(ptr);
}

static char g_bsd_progname[256] = "prog";
static int g_bsd_progname_init = 0;

const char *getprogname(void) {
  if (!g_bsd_progname_init) {
#if defined(_WIN32)
    char modpath[260];
    DWORD len = GetModuleFileNameA(NULL, modpath, sizeof(modpath));
    if (len > 0 && len < sizeof(modpath)) {
      setprogname(modpath);
    }
#endif
    g_bsd_progname_init = 1;
  }
  return g_bsd_progname;
}

void setprogname(const char *progname) {
  const char *base;
  const char *s1;
  const char *s2;
  size_t len;
  if (progname == NULL) {
    return;
  }
  s1 = strrchr(progname, '/');
  s2 = strrchr(progname, '\\');
  if (s1 && s2) {
    base = (s1 > s2) ? (s1 + 1) : (s2 + 1);
  } else if (s1) {
    base = s1 + 1;
  } else if (s2) {
    base = s2 + 1;
  } else {
    base = progname;
  }
  strlcpy(g_bsd_progname, base, sizeof(g_bsd_progname));
  len = strlen(g_bsd_progname);
  if (len > 4 && (strcmp(g_bsd_progname + len - 4, ".exe") == 0 ||
                  strcmp(g_bsd_progname + len - 4, ".EXE") == 0)) {
    g_bsd_progname[len - 4] = '\0';
  }
  g_bsd_progname_init = 1;
}

AWM_BSD_LL strtonum(const char *numstr, AWM_BSD_LL minval, AWM_BSD_LL maxval,
                    const char **errstrp) {
  AWM_BSD_LL ll = 0;
  char *ep = NULL;
  if (minval > maxval) {
    if (errstrp) {
      *errstrp = "invalid";
    }
    errno = EINVAL;
    return 0;
  }
  if (numstr == NULL || *numstr == '\0') {
    if (errstrp) {
      *errstrp = "invalid";
    }
    errno = EINVAL;
    return 0;
  }
  errno = 0;
#if defined(_MSC_VER)
  ll = _strtoi64(numstr, &ep, 10);
#else
  ll = (AWM_BSD_LL)strtol(numstr, &ep, 10);
#endif
  if (numstr == ep || *ep != '\0') {
    if (errstrp) {
      *errstrp = "invalid";
    }
    errno = EINVAL;
    return 0;
  }
  if ((errno == ERANGE && ll < 0) || ll < minval) {
    if (errstrp) {
      *errstrp = "too small";
    }
    errno = ERANGE;
    return 0;
  }
  if ((errno == ERANGE && ll > 0) || ll > maxval) {
    if (errstrp) {
      *errstrp = "too large";
    }
    errno = ERANGE;
    return 0;
  }
  if (errstrp) {
    *errstrp = NULL;
  }
  return ll;
}

/** @brief Internal parsed mode state for bsd_setmode/getmode. */
struct awm_bsd_mode_state {
  int is_octal;
  mode_t octal_mode;
  mode_t set_bits;
  mode_t clr_bits;
};

void *bsd_setmode(const char *p) {
  struct awm_bsd_mode_state *st;
  if (p == NULL || *p == '\0') {
    return NULL;
  }
  st = (struct awm_bsd_mode_state *)calloc(1, sizeof(*st));
  if (st == NULL) {
    return NULL;
  }
  if (*p >= '0' && *p <= '7') {
    char *ep = NULL;
    unsigned long val = strtoul(p, &ep, 8);
    if (ep == NULL || *ep != '\0' || val > 07777UL) {
      free(st);
      return NULL;
    }
    st->is_octal = 1;
    st->octal_mode = (mode_t)val;
    return st;
  }
  while (*p != '\0') {
    mode_t who = 0;
    char op = 0;
    mode_t perm = 0;
    while (*p == 'u' || *p == 'g' || *p == 'o' || *p == 'a') {
      if (*p == 'u') {
        who |= 04700;
      } else if (*p == 'g') {
        who |= 02070;
      } else if (*p == 'o') {
        who |= 01007;
      } else if (*p == 'a') {
        who |= 07777;
      }
      p++;
    }
    if (who == 0) {
      who = 07777;
    }
    while (*p == '+' || *p == '-' || *p == '=') {
      op = *p++;
      perm = 0;
      while (*p == 'r' || *p == 'w' || *p == 'x' || *p == 'X' || *p == 's' ||
             *p == 't') {
        if (*p == 'r') {
          perm |= 0444;
        } else if (*p == 'w') {
          perm |= 0222;
        } else if (*p == 'x' || *p == 'X') {
          perm |= 0111;
        } else if (*p == 's') {
          perm |= 06000;
        } else if (*p == 't') {
          perm |= 01000;
        }
        p++;
      }
      if (op == '+') {
        st->set_bits |= (perm & who);
        st->clr_bits &= ~(perm & who);
      } else if (op == '-') {
        st->clr_bits |= (perm & who);
        st->set_bits &= ~(perm & who);
      } else if (op == '=') {
        st->clr_bits |= (who & 07777);
        st->set_bits = (st->set_bits & ~who) | (perm & who);
      }
    }
    if (*p == ',') {
      p++;
    } else if (*p != '\0') {
      free(st);
      return NULL;
    }
  }
  return st;
}

mode_t getmode(const void *bbox, mode_t omode) {
  const struct awm_bsd_mode_state *st = (const struct awm_bsd_mode_state *)bbox;
  if (st == NULL) {
    return omode;
  }
  if (st->is_octal) {
    return (omode & ~07777) | st->octal_mode;
  }
  return (omode & ~st->clr_bits) | st->set_bits;
}

static char *g_fgetln_buf = NULL;
static size_t g_fgetln_cap = 0;

char *fgetln(FILE *stream, size_t *len) {
  size_t pos = 0;
  int c;
  if (stream == NULL) {
    if (len) {
      *len = 0;
    }
    return NULL;
  }
  while ((c = fgetc(stream)) != EOF) {
    if (pos + 2 > g_fgetln_cap) {
      size_t newcap = (g_fgetln_cap == 0) ? 256 : (g_fgetln_cap * 2);
      char *nb = (char *)realloc(g_fgetln_buf, newcap);
      if (nb == NULL) {
        if (len) {
          *len = 0;
        }
        return NULL;
      }
      g_fgetln_buf = nb;
      g_fgetln_cap = newcap;
    }
    g_fgetln_buf[pos++] = (char)c;
    if (c == '\n') {
      break;
    }
  }
  if (pos == 0) {
    if (len) {
      *len = 0;
    }
    return NULL;
  }
  g_fgetln_buf[pos] = '\0';
  if (len) {
    *len = pos;
  }
  return g_fgetln_buf;
}

static wchar_t *g_fgetwln_buf = NULL;
static size_t g_fgetwln_cap = 0;

wchar_t *fgetwln(FILE *stream, size_t *lenp) {
  size_t pos = 0;
  wint_t wc;
  if (stream == NULL) {
    if (lenp) {
      *lenp = 0;
    }
    return NULL;
  }
  while ((wc = fgetwc(stream)) != WEOF) {
    if (pos + 2 > g_fgetwln_cap) {
      size_t newcap = (g_fgetwln_cap == 0) ? 128 : (g_fgetwln_cap * 2);
      wchar_t *nb = (wchar_t *)realloc(g_fgetwln_buf, newcap * sizeof(wchar_t));
      if (nb == NULL) {
        if (lenp) {
          *lenp = 0;
        }
        return NULL;
      }
      g_fgetwln_buf = nb;
      g_fgetwln_cap = newcap;
    }
    g_fgetwln_buf[pos++] = (wchar_t)wc;
    if (wc == L'\n') {
      break;
    }
  }
  if (pos == 0) {
    if (lenp) {
      *lenp = 0;
    }
    return NULL;
  }
  g_fgetwln_buf[pos] = L'\0';
  if (lenp) {
    *lenp = pos;
  }
  return g_fgetwln_buf;
}

char *fparseln(FILE *fp, size_t *size, size_t *lineno, const char str[3],
               int flags) {
  size_t l = 0;
  char *raw;
  char *out;
  char comm;
  size_t i;
  (void)flags;
  if (fp == NULL) {
    return NULL;
  }
  comm = (str != NULL) ? str[2] : '#';
  raw = fgetln(fp, &l);
  if (raw == NULL) {
    return NULL;
  }
  if (lineno) {
    (*lineno)++;
  }
  while (l > 0 && (raw[l - 1] == '\n' || raw[l - 1] == '\r')) {
    l--;
  }
  out = (char *)malloc(l + 1);
  if (out == NULL) {
    return NULL;
  }
  for (i = 0; i < l; i++) {
    if (comm != '\0' && raw[i] == comm) {
      l = i;
      break;
    }
    out[i] = raw[i];
  }
  out[l] = '\0';
  if (size) {
    *size = l;
  }
  return out;
}

int getbsize(int *headerlenp, long *blocksizep) {
  if (headerlenp) {
    *headerlenp = 9;
  }
  if (blocksizep) {
    *blocksizep = 512;
  }
  return 0;
}

static uint32_t g_arc4_state = 0;

uint32_t arc4random(void) {
  if (g_arc4_state == 0) {
    g_arc4_state = (uint32_t)time(NULL) ^ 0xa5a5a5a5UL;
    if (g_arc4_state == 0) {
      g_arc4_state = 0x12345678UL;
    }
  }
  g_arc4_state ^= g_arc4_state << 13;
  g_arc4_state ^= g_arc4_state >> 17;
  g_arc4_state ^= g_arc4_state << 5;
  return g_arc4_state;
}

void arc4random_buf(void *buf, size_t nbytes) {
  unsigned char *p = (unsigned char *)buf;
  size_t i = 0;
  if (buf == NULL) {
    return;
  }
  while (i < nbytes) {
    uint32_t r = arc4random();
    size_t j;
    for (j = 0; j < 4 && i < nbytes; j++, i++) {
      p[i] = (unsigned char)((r >> (j * 8)) & 0xffU);
    }
  }
}

uint32_t arc4random_uniform(uint32_t upper_bound) {
  if (upper_bound < 2) {
    return 0;
  }
  return arc4random() % upper_bound;
}

int mergesort(void *base, size_t nmemb, size_t size,
              int (*cmp)(const void *, const void *)) {
  if (base == NULL || cmp == NULL) {
    errno = EINVAL;
    return -1;
  }
  qsort(base, nmemb, size, cmp);
  return 0;
}

int heapsort(void *base, size_t nmemb, size_t size,
             int (*cmp)(const void *, const void *)) {
  if (base == NULL || cmp == NULL) {
    errno = EINVAL;
    return -1;
  }
  qsort(base, nmemb, size, cmp);
  return 0;
}

int radixsort(const unsigned char **base, int nmemb, const unsigned char *table,
              unsigned int endbyte) {
  int i, j;
  if (base == NULL || nmemb < 0) {
    errno = EINVAL;
    return -1;
  }
  for (i = 1; i < nmemb; i++) {
    const unsigned char *key = base[i];
    j = i - 1;
    while (j >= 0) {
      const unsigned char *a = base[j];
      const unsigned char *b = key;
      int gt = 0;
      while (*a != endbyte && *b != endbyte && *a == *b) {
        a++;
        b++;
      }
      if (table) {
        gt =
            ((*a == endbyte ? 0 : table[*a]) > (*b == endbyte ? 0 : table[*b]));
      } else {
        gt = ((*a == endbyte ? -1 : (int)*a) > (*b == endbyte ? -1 : (int)*b));
      }
      if (!gt) {
        break;
      }
      base[j + 1] = base[j];
      j--;
    }
    base[j + 1] = key;
  }
  return 0;
}

int sradixsort(const unsigned char **base, int nmemb,
               const unsigned char *table, unsigned int endbyte) {
  return radixsort(base, nmemb, table, endbyte);
}

int rpmatch(const char *response) {
  if (response == NULL || *response == '\0') {
    return -1;
  }
  if (*response == 'y' || *response == 'Y') {
    return 1;
  }
  if (*response == 'n' || *response == 'N') {
    return 0;
  }
  return -1;
}

void setproctitle(const char *fmt, ...) { (void)fmt; }

void setproctitle_fast(const char *fmt, ...) { (void)fmt; }

int daemon(int nochdir, int noclose) {
  (void)nochdir;
  (void)noclose;
  return 0;
}

static char g_devname_buf[32] = "tty";

char *devname_r(int dev, mode_t type, char *buf, int len) {
  (void)dev;
  (void)type;
  if (buf == NULL || len <= 0) {
    return NULL;
  }
  strlcpy(buf, "tty", (size_t)len);
  return buf;
}

char *devname(int dev, mode_t type) {
  return devname_r(dev, type, g_devname_buf, (int)sizeof(g_devname_buf));
}

int cgetent(char **buf, char **db_array, const char *name) {
  (void)db_array;
  (void)name;
  if (buf) {
    *buf = NULL;
  }
  return -1;
}

char *cgetcap(char *buf, const char *cap, int type) {
  (void)buf;
  (void)cap;
  (void)type;
  return NULL;
}

int cgetnum(char *buf, const char *cap, long *num) {
  (void)buf;
  (void)cap;
  (void)num;
  return -1;
}

int cgetstr(char *buf, const char *cap, char **str) {
  (void)buf;
  (void)cap;
  if (str) {
    *str = NULL;
  }
  return -1;
}

int cgetustr(char *buf, const char *cap, char **str) {
  return cgetstr(buf, cap, str);
}

int cgetnext(char **bp, char **db_array) {
  (void)db_array;
  if (bp) {
    *bp = NULL;
  }
  return 0;
}

int cgetclose(void) { return 0; }

int cgetset(const char *ent) {
  (void)ent;
  return 0;
}

int cgetmatch(const char *buf, const char *name) {
  (void)buf;
  (void)name;
  return -1;
}

#else
void *bsd_setmode(const char *p) {
  extern void *setmode(const char *);
  return setmode(p);
}
#endif /* !BSD && !__APPLE__ */

int bsd_stdlib_dummy_for_coverage(void) { return 0; }

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_bsd_stdlib_c;

#endif
typedef int make_iso_compilers_happy_mega_batch2_3;
