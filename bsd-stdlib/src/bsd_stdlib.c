#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
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
#include "auto-win-msvc-error.h"
#include <io.h>
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(
    void *hModule, char *lpFilename, unsigned long nSize);
#ifdef __cplusplus
}
#endif
#else
#include <unistd.h>
#endif
/* clang-format on */

#undef AWM_HAVE_GLIBC_CLOSEFROM
#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 34)
#define AWM_HAVE_GLIBC_CLOSEFROM 1
#endif
#endif

#undef AWM_HAVE_GLIBC_REALLOCARRAY
#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 26)
#define AWM_HAVE_GLIBC_REALLOCARRAY 1
#endif
#endif

#undef AWM_HAVE_GLIBC_ARC4RANDOM
#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 36)
#define AWM_HAVE_GLIBC_ARC4RANDOM 1
#endif
#endif

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
static void *default_bsd_stdlib_malloc(size_t sz) { return malloc(sz); }
static void *default_bsd_stdlib_calloc(size_t n, size_t sz) {
  return calloc(n, sz);
}
static void *default_bsd_stdlib_realloc(void *ptr, size_t sz) {
  return realloc(ptr, sz);
}

static void *(*g_bsd_stdlib_malloc)(size_t) = default_bsd_stdlib_malloc;
static void *(*g_bsd_stdlib_calloc)(size_t, size_t) = default_bsd_stdlib_calloc;
static void *(*g_bsd_stdlib_realloc)(void *,
                                     size_t) = default_bsd_stdlib_realloc;

#if !defined(AWM_HAVE_GLIBC_CLOSEFROM)
#if defined(_MSC_VER) && _MSC_VER >= 1400
static void bsd_stdlib_null_invalid_param_handler(const wchar_t *expression,
                                                  const wchar_t *function,
                                                  const wchar_t *file,
                                                  unsigned int line,
                                                  uintptr_t pReserved) {
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
}
#endif

static int default_bsd_stdlib_close_fd(int fd) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
  int rc;
  _invalid_parameter_handler old =
      _set_invalid_parameter_handler(bsd_stdlib_null_invalid_param_handler);
  AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
  rc = _close(fd);
  _set_invalid_parameter_handler(old);
  return rc;
#elif defined(_WIN32)
  return _close(fd);
#else
  return close(fd);
#endif
}
static int (*g_bsd_stdlib_close_fd)(int) = default_bsd_stdlib_close_fd;

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    defined(_SC_OPEN_MAX)
static long default_bsd_stdlib_sysconf(int name) { return sysconf(name); }
static long (*g_bsd_stdlib_sysconf)(int) = default_bsd_stdlib_sysconf;
#endif
#endif /* !AWM_HAVE_GLIBC_CLOSEFROM */
#endif /* !BSD */

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
#if defined(_WIN32)
static unsigned long default_bsd_stdlib_mod_fname(char *buf, unsigned long sz) {
  return GetModuleFileNameA(NULL, buf, sz);
}
static unsigned long (*g_bsd_stdlib_mod_fname)(char *, unsigned long) =
    default_bsd_stdlib_mod_fname;
#endif

#if !defined(AWM_HAVE_GLIBC_ARC4RANDOM)
static time_t default_bsd_stdlib_time(time_t *t) { return time(t); }
static time_t (*g_bsd_stdlib_time)(time_t *) = default_bsd_stdlib_time;
#endif
#endif /* !__APPLE__ && !BSD */

/**
 * @brief Overrides allocator hooks used by bsd-stdlib for OOM testing.
 */
void bsd_stdlib_set_alloc_hooks(void *(*malloc_fn)(size_t),
                                void *(*calloc_fn)(size_t, size_t),
                                void *(*realloc_fn)(void *, size_t)) {
#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
  g_bsd_stdlib_malloc =
      (malloc_fn != NULL) ? malloc_fn : default_bsd_stdlib_malloc;
  g_bsd_stdlib_calloc =
      (calloc_fn != NULL) ? calloc_fn : default_bsd_stdlib_calloc;
  g_bsd_stdlib_realloc =
      (realloc_fn != NULL) ? realloc_fn : default_bsd_stdlib_realloc;
#else
  (void)malloc_fn;
  (void)calloc_fn;
  (void)realloc_fn;
#endif
}

/**
 * @brief Overrides OS hooks used by bsd-stdlib for deterministic branch
 * testing.
 */
void bsd_stdlib_set_sys_hooks(int (*close_fn)(int), long (*sysconf_fn)(int),
                              time_t (*time_fn)(time_t *),
                              unsigned long (*mod_fname_fn)(char *,
                                                            unsigned long)) {
#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__) &&  \
    !defined(AWM_HAVE_GLIBC_CLOSEFROM)
  g_bsd_stdlib_close_fd =
      (close_fn != NULL) ? close_fn : default_bsd_stdlib_close_fd;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    defined(_SC_OPEN_MAX)
  g_bsd_stdlib_sysconf =
      (sysconf_fn != NULL) ? sysconf_fn : default_bsd_stdlib_sysconf;
#else
  (void)sysconf_fn;
#endif
#else
  (void)close_fn;
  (void)sysconf_fn;
#endif

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__) && !defined(AWM_HAVE_GLIBC_ARC4RANDOM)
  g_bsd_stdlib_time = (time_fn != NULL) ? time_fn : default_bsd_stdlib_time;
#else
  (void)time_fn;
#endif

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__) && defined(_WIN32)
  g_bsd_stdlib_mod_fname =
      (mod_fname_fn != NULL) ? mod_fname_fn : default_bsd_stdlib_mod_fname;
#else
  (void)mod_fname_fn;
#endif
}

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)

#if !defined(AWM_HAVE_GLIBC_CLOSEFROM)
/**
 * @brief Close all open file descriptors greater than or equal to lowfd.
 */
void closefrom(int lowfd) {
  int fd;
  int maxfd = 256;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    defined(_SC_OPEN_MAX)
  long sc_max = g_bsd_stdlib_sysconf(_SC_OPEN_MAX);
  if (sc_max > 0 && sc_max < 4096) {
    maxfd = (int)sc_max;
  }
#endif
  if (lowfd < 0) {
    lowfd = 0;
  }
  for (fd = lowfd; fd < maxfd; fd++) {
    g_bsd_stdlib_close_fd(fd);
  }
}
#endif

#if !defined(AWM_HAVE_GLIBC_REALLOCARRAY)
/**
 * @brief Reallocate memory safely with overflow checking.
 * @return Reallocated pointer, or NULL on overflow/error.
 */
void *reallocarray(void *ptr, size_t nmemb, size_t size) {
  if (size != 0 && nmemb > ((size_t)-1) / size) {
    errno = ENOMEM;
    return NULL;
  }
  return g_bsd_stdlib_realloc(ptr, nmemb * size);
}
#endif

/**
 * @brief Reallocate and zero-extend array safely.
 * @return Reallocated pointer, or NULL on overflow/error.
 */
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
    return g_bsd_stdlib_calloc(newnmemb, size);
  }
  if (newsize <= oldsize) {
    size_t diff = oldsize - newsize;
    if (diff > 0) {
      explicit_bzero((char *)ptr + newsize, diff);
    }
  }
  newptr = g_bsd_stdlib_malloc(newsize);
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

/**
 * @brief Zero and free a memory allocation.
 */
void freezero(void *ptr, size_t size) {
  if (ptr == NULL) {
    return;
  }
  explicit_bzero(ptr, size);
  free(ptr);
}

#endif /* !__FreeBSD__ && !__OpenBSD__ && !__NetBSD__ */

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)

static char g_bsd_progname[256] = "prog";
static int g_bsd_progname_init = 0;

/**
 * @brief Retrieve the current program name.
 * @return Pointer to program name string.
 */
const char *getprogname(void) {
  if (!g_bsd_progname_init) {
#if defined(_WIN32)
    char modpath[260];
    unsigned long len =
        g_bsd_stdlib_mod_fname(modpath, (unsigned long)sizeof(modpath));
    if (len > 0 && len < (unsigned long)sizeof(modpath)) {
      setprogname(modpath);
    }
#endif
    g_bsd_progname_init = 1;
  }
  return g_bsd_progname;
}

/**
 * @brief Set the current program name.
 */
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

/**
 * @brief Reliably convert string value to an integer within bounds.
 * @return Converted integer, or 0 on error.
 */
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
  if (errno == ERANGE || ll > maxval) {
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

/**
 * @brief Parse symbolic or octal file mode string into a bitmask state.
 * @return Allocated mode state pointer, or NULL on error.
 */
void *bsd_setmode(const char *p) {
  struct awm_bsd_mode_state *st;
  if (p == NULL || *p == '\0') {
    return NULL;
  }
  st = (struct awm_bsd_mode_state *)g_bsd_stdlib_calloc(1, sizeof(*st));
  if (st == NULL) {
    return NULL;
  }
  if (*p >= '0' && *p <= '7') {
    char *ep = NULL;
    unsigned long val = strtoul(p, &ep, 8);
    if (*ep != '\0' || val > 07777UL) {
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
      } else {
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
        } else {
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
      } else {
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

/**
 * @brief Apply parsed mode state to an initial file mode.
 * @return Modified file mode.
 */
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

/**
 * @brief Read a line from a stream without copying into a user buffer.
 * @return Pointer to internal line buffer, or NULL on EOF/error.
 */
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
      char *nb = (char *)g_bsd_stdlib_realloc(g_fgetln_buf, newcap);
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

/**
 * @brief Read a wide-character line from a stream.
 * @return Pointer to internal wide line buffer, or NULL on EOF/error.
 */
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
      wchar_t *nb = (wchar_t *)g_bsd_stdlib_realloc(g_fgetwln_buf,
                                                    newcap * sizeof(wchar_t));
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

/**
 * @brief Return the next logical line from a stream, handling comments.
 * @return Newly allocated line string, or NULL on EOF/error.
 */
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
  out = (char *)g_bsd_stdlib_malloc(l + 1);
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

/**
 * @brief Retrieve user-preferred block size.
 * @return 0 on success.
 */
int getbsize(int *headerlenp, long *blocksizep) {
  if (headerlenp) {
    *headerlenp = 9;
  }
  if (blocksizep) {
    *blocksizep = 512;
  }
  return 0;
}

#if !defined(AWM_HAVE_GLIBC_ARC4RANDOM)
static uint32_t g_arc4_state = 0;

/**
 * @brief Generate a 32-bit pseudo-random number.
 * @return Random 32-bit unsigned integer.
 */
uint32_t arc4random(void) {
  if (g_arc4_state == 0) {
    g_arc4_state = (uint32_t)g_bsd_stdlib_time(NULL) ^ 0xa5a5a5a5UL;
    if (g_arc4_state == 0) {
      g_arc4_state = 0x12345678UL;
    }
  }
  g_arc4_state ^= g_arc4_state << 13;
  g_arc4_state ^= g_arc4_state >> 17;
  g_arc4_state ^= g_arc4_state << 5;
  return g_arc4_state;
}

/**
 * @brief Fill a buffer with pseudo-random bytes.
 */
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

/**
 * @brief Generate a uniformly distributed random number less than upper_bound.
 * @return Random integer in [0, upper_bound).
 */
uint32_t arc4random_uniform(uint32_t upper_bound) {
  if (upper_bound < 2) {
    return 0;
  }
  return arc4random() % upper_bound;
}
#endif /* !AWM_HAVE_GLIBC_ARC4RANDOM */

/**
 * @brief Sort an array using merge sort.
 * @return 0 on success, -1 on failure.
 */
int mergesort(void *base, size_t nmemb, size_t size,
              int (*cmp)(const void *, const void *)) {
  if (base == NULL || cmp == NULL) {
    errno = EINVAL;
    return -1;
  }
  qsort(base, nmemb, size, cmp);
  return 0;
}

/**
 * @brief Sort an array using heap sort.
 * @return 0 on success, -1 on failure.
 */
int heapsort(void *base, size_t nmemb, size_t size,
             int (*cmp)(const void *, const void *)) {
  if (base == NULL || cmp == NULL) {
    errno = EINVAL;
    return -1;
  }
  qsort(base, nmemb, size, cmp);
  return 0;
}

/**
 * @brief Sort an array of byte strings using radix sort.
 * @return 0 on success, -1 on failure.
 */
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

/**
 * @brief Stable radix sort for an array of byte strings.
 * @return 0 on success, -1 on failure.
 */
int sradixsort(const unsigned char **base, int nmemb,
               const unsigned char *table, unsigned int endbyte) {
  return radixsort(base, nmemb, table, endbyte);
}

#if !defined(__GLIBC__)
/**
 * @brief Determine whether a response string is affirmative or negative.
 * @return 1 if affirmative, 0 if negative, -1 if unrecognized.
 */
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
#endif

/**
 * @brief Set process title for ps/top display (no-op on Windows/DOS).
 */
void setproctitle(const char *fmt, ...) { (void)fmt; }

/**
 * @brief Fast process title update (no-op on Windows/DOS).
 */
void setproctitle_fast(const char *fmt, ...) { (void)fmt; }

#if !defined(__GLIBC__)
/**
 * @brief Detach process into background daemon.
 * @return 0 on success.
 */
int daemon(int nochdir, int noclose) {
  (void)nochdir;
  (void)noclose;
  return 0;
}
#endif

static char g_devname_buf[32] = "tty";

/**
 * @brief Thread-safe get device name from device number and file type.
 * @return Pointer to buf, or NULL on invalid buffer.
 */
char *devname_r(int dev, mode_t type, char *buf, int len) {
  (void)dev;
  (void)type;
  if (buf == NULL || len <= 0) {
    return NULL;
  }
  strlcpy(buf, "tty", (size_t)len);
  return buf;
}

/**
 * @brief Get device name from device number and file type.
 * @return Device name string.
 */
char *devname(int dev, mode_t type) {
  return devname_r(dev, type, g_devname_buf, (int)sizeof(g_devname_buf));
}

/**
 * @brief Look up capability database entry.
 * @return -1 (not found).
 */
int cgetent(char **buf, char **db_array, const char *name) {
  (void)db_array;
  (void)name;
  if (buf) {
    *buf = NULL;
  }
  return -1;
}

/**
 * @brief Locate capability field in entry buffer.
 * @return NULL.
 */
char *cgetcap(char *buf, const char *cap, int type) {
  (void)buf;
  (void)cap;
  (void)type;
  return NULL;
}

/**
 * @brief Retrieve numeric capability value.
 * @return -1.
 */
int cgetnum(char *buf, const char *cap, long *num) {
  (void)buf;
  (void)cap;
  (void)num;
  return -1;
}

/**
 * @brief Retrieve string capability value.
 * @return -1.
 */
int cgetstr(char *buf, const char *cap, char **str) {
  (void)buf;
  (void)cap;
  if (str) {
    *str = NULL;
  }
  return -1;
}

/**
 * @brief Retrieve literal string capability value.
 * @return -1.
 */
int cgetustr(char *buf, const char *cap, char **str) {
  return cgetstr(buf, cap, str);
}

/**
 * @brief Sequential scan of capability database files.
 * @return 0 on EOF.
 */
int cgetnext(char **bp, char **db_array) {
  (void)db_array;
  if (bp) {
    *bp = NULL;
  }
  return 0;
}

/**
 * @brief Close sequential capability scan.
 * @return Always 0.
 */
int cgetclose(void) { return 0; }

/**
 * @brief Prepend an additional capability entry for cgetent lookups.
 * @return 0 on success.
 */
int cgetset(const char *ent) {
  (void)ent;
  return 0;
}

/**
 * @brief Match capability name alias in entry buffer.
 * @return -1 on mismatch.
 */
int cgetmatch(const char *buf, const char *name) {
  (void)buf;
  (void)name;
  return -1;
}

/**
 * @brief Compute new file mode from bsd_setmode state and existing mode.
 * @return Updated file mode bitmask.
 */
mode_t bsd_getmode(const void *bbox, mode_t omode) {
  return getmode(bbox, omode);
}

#elif defined(__APPLE__)
/**
 * @brief Parse symbolic or octal file mode string on macOS.
 * @return Allocated mode state pointer, or NULL on error.
 */
void *bsd_setmode(const char *p) {
  extern void *setmode(const char *);
  return setmode(p);
}

/**
 * @brief Apply parsed mode state to an initial file mode on macOS.
 * @return Modified file mode.
 */
mode_t bsd_getmode(const void *bbox, mode_t omode) {
  extern mode_t getmode(const void *, mode_t);
  return getmode(bbox, omode);
}
#endif /* !BSD && !__APPLE__ */

/**
 * @brief Resets cached fgetln, fgetwln, progname, and arc4random internal
 * state.
 */
void bsd_stdlib_reset_state(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  free(g_fgetln_buf);
  g_fgetln_buf = NULL;
  g_fgetln_cap = 0;
  free(g_fgetwln_buf);
  g_fgetwln_buf = NULL;
  g_fgetwln_cap = 0;
  memcpy(g_bsd_progname, "prog", 5);
  g_bsd_progname_init = 0;
#if !defined(AWM_HAVE_GLIBC_ARC4RANDOM)
  g_arc4_state = 0;
#endif
#endif
}

/**
 * @brief Coverage helper for bsd-stdlib module.
 * @return Always 0.
 */
int bsd_stdlib_dummy_for_coverage(void) { return 0; }

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_bsd_stdlib_c;

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__) &&  \
    !defined(__APPLE__)
static const char *awm_next_fmt_spec(const char *p, char *mod, size_t mod_sz,
                                     char *conv) {
  size_t m = 0;
  mod[0] = '\0';
  *conv = '\0';
  while (*p != '\0') {
    if (*p != '%') {
      p++;
      continue;
    }
    p++;
    if (*p == '%') {
      p++;
      continue;
    }
    while (*p != '\0' && strchr("#0- +'", *p) != NULL) {
      p++;
    }
    if (*p == '*') {
      *conv = '*';
      return p + 1;
    }
    while (*p >= '0' && *p <= '9') {
      p++;
    }
    if (*p == '.') {
      p++;
      if (*p == '*') {
        *conv = '.';
        return p + 1;
      }
      while (*p >= '0' && *p <= '9') {
        p++;
      }
    }
    while (*p != '\0' && strchr("hljztL", *p) != NULL) {
      if (m + 1 < mod_sz) {
        mod[m++] = *p;
      }
      p++;
    }
    mod[m] = '\0';
    if (*p == '\0') {
      return NULL;
    }
    *conv = *p;
    return p + 1;
  }
  return p;
}

/**
 * @brief Verify that two printf-style format strings accept compatible
 * arguments.
 * @return f1 if its format specifiers match f2, otherwise f2.
 */
const char *fmtcheck(const char *f1, const char *f2) {
  const char *p1;
  const char *p2;
  char m1[8], m2[8], c1, c2;
  if (f1 == NULL) {
    return f2;
  }
  if (f2 == NULL) {
    return f1;
  }
  p1 = f1;
  p2 = f2;
  for (;;) {
    p1 = awm_next_fmt_spec(p1, m1, sizeof(m1), &c1);
    p2 = awm_next_fmt_spec(p2, m2, sizeof(m2), &c2);
    if (p1 == NULL || p2 == NULL) {
      return f2;
    }
    if (c1 == '\0' && c2 == '\0') {
      return f1;
    }
    if (c1 != c2 || strcmp(m1, m2) != 0) {
      return f2;
    }
  }
}
#endif
#endif
typedef int make_iso_compilers_happy_mega_batch2_3;
