#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_libutil.c
 * @brief Implementation of FreeBSD <libutil.h> and <login_cap.h> functions.
 */

/* clang-format off */
#include <libutil.h>
#include <pwd.h>
#include <login_cap.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <io.h>
#include <process.h>
#if defined(_MSC_VER)
#include <share.h>
#endif
#else
#include <unistd.h>
#endif
/* clang-format on */

#ifndef NUM_FORMAT_CAST
#if defined(_MSC_VER)
#define NUM_FORMAT_CAST __int64
#else
#define NUM_FORMAT_CAST long
#endif
#endif

#ifndef NUM_FORMAT
#if defined(_MSC_VER)
#define NUM_FORMAT "%I64d"
#else
#define NUM_FORMAT "%ld"
#endif
#endif

static void *default_libutil_calloc(size_t n, size_t sz) {
  return calloc(n, sz);
}

static char *default_libutil_strdup(const char *s) {
#if defined(_MSC_VER)
  return _strdup(s);
#else
  return strdup(s);
#endif
}

static int default_libutil_write(int fd, const void *buf, unsigned int len) {
#if defined(_MSC_VER)
  return _write(fd, buf, len);
#else
  return (int)write(fd, buf, (size_t)len);
#endif
}

static void *(*g_libutil_calloc)(size_t, size_t) = default_libutil_calloc;
static char *(*g_libutil_strdup)(const char *) = default_libutil_strdup;
static int (*g_libutil_write)(int, const void *,
                              unsigned int) = default_libutil_write;

void bsd_libutil_set_alloc_hooks(void *(*calloc_fn)(size_t, size_t),
                                 char *(*strdup_fn)(const char *)) {
  g_libutil_calloc = (calloc_fn != NULL) ? calloc_fn : default_libutil_calloc;
  g_libutil_strdup = (strdup_fn != NULL) ? strdup_fn : default_libutil_strdup;
}

void bsd_libutil_set_write_hook(int (*write_fn)(int, const void *,
                                                unsigned int)) {
  g_libutil_write = (write_fn != NULL) ? write_fn : default_libutil_write;
}

#if !defined(__FreeBSD__)

int humanize_number(char *buf, size_t len, int64_t bytes, const char *suffix,
                    int scale, int flags) {
  static const char prefixes[] = " KMGTPE";
  const char *sep = (flags & HN_NOSPACE) ? "" : " ";
  const char *sfx = (suffix != NULL) ? suffix : "";
  int divisor = (flags & HN_DIVISOR_1000) ? 1000 : 1024;
  int i = 0;
  uint64_t uval;
  char pfx[4];
  char numbuf[32];
  size_t n_num;
  size_t n_sep;
  size_t n_pfx;
  size_t n_sfx;
  size_t total;

  if (buf == NULL || len == 0) {
    errno = EINVAL;
    return -1;
  }
  if (bytes < 0) {
    uval = (uint64_t)(-(bytes + 1)) + 1U;
  } else {
    uval = (uint64_t)bytes;
  }
  if (scale == HN_AUTOSCALE || scale == HN_GETSCALE || scale <= 0) {
    while (uval >= (uint64_t)divisor) {
      uval /= (uint64_t)divisor;
      i++;
    }
  } else {
    while (i < scale && i < 6) {
      uval /= (uint64_t)divisor;
      i++;
    }
  }
  if ((flags & HN_GETSCALE) && scale == HN_GETSCALE) {
    return i;
  }
  if (i == 0) {
    if (flags & HN_B) {
      pfx[0] = 'B';
      pfx[1] = '\0';
    } else {
      pfx[0] = '\0';
    }
  } else {
    pfx[0] = prefixes[i];
    if (flags & HN_IEC_PREFIXES) {
      pfx[1] = 'i';
      pfx[2] = '\0';
    } else {
      pfx[1] = '\0';
    }
  }
  if (bytes < 0) {
#if defined(_MSC_VER)
    sprintf_s(numbuf, sizeof(numbuf), "-" NUM_FORMAT, (NUM_FORMAT_CAST)uval);
#else
    sprintf(numbuf, "-" NUM_FORMAT, (NUM_FORMAT_CAST)uval);
#endif
  } else {
#if defined(_MSC_VER)
    sprintf_s(numbuf, sizeof(numbuf), NUM_FORMAT, (NUM_FORMAT_CAST)uval);
#else
    sprintf(numbuf, NUM_FORMAT, (NUM_FORMAT_CAST)uval);
#endif
  }
  n_num = strlen(numbuf);
  n_sep = strlen(sep);
  n_pfx = strlen(pfx);
  n_sfx = strlen(sfx);
  total = n_num + n_sep + n_pfx + n_sfx;
  if (total >= len) {
    return -1;
  }
  memcpy(buf, numbuf, n_num);
  memcpy(buf + n_num, sep, n_sep);
  memcpy(buf + n_num + n_sep, pfx, n_pfx);
  memcpy(buf + n_num + n_sep + n_pfx, sfx, n_sfx + 1);
  return (int)total;
}

int expand_number(const char *buf, uint64_t *num) {
  char *endptr = NULL;
  uint64_t number;
  unsigned int shift = 0;
  const char *p;

  if (buf == NULL || num == NULL) {
    errno = EINVAL;
    return -1;
  }
  p = buf;
  while (isspace((unsigned char)*p)) {
    p++;
  }
  if (*p == '-' || *p == '\0') {
    errno = EINVAL;
    return -1;
  }
  errno = 0;
#if defined(_MSC_VER) || defined(_WIN32)
  number = _strtoui64(p, &endptr, 0);
#else
  number = (uint64_t)strtoul(p, &endptr, 0);
#endif
  if (endptr == p) {
    errno = EINVAL;
    return -1;
  }
  if (errno == ERANGE) {
    return -1;
  }
  if (*endptr == '\0') {
    *num = number;
    return 0;
  }
  switch (tolower((unsigned char)endptr[0])) {
  case 'b':
    shift = 0;
    break;
  case 'k':
    shift = 10;
    break;
  case 'm':
    shift = 20;
    break;
  case 'g':
    shift = 30;
    break;
  case 't':
    shift = 40;
    break;
  case 'p':
    shift = 50;
    break;
  case 'e':
    shift = 60;
    break;
  default:
    errno = EINVAL;
    return -1;
  }
  if (endptr[1] != '\0') {
    int c1 = tolower((unsigned char)endptr[1]);
    if (c1 == 'b') {
      if (endptr[2] != '\0') {
        errno = EINVAL;
        return -1;
      }
    } else if (c1 == 'i' && tolower((unsigned char)endptr[2]) == 'b' &&
               endptr[3] == '\0') {
      /* Valid IEC suffix (e.g. KiB) */
    } else {
      errno = EINVAL;
      return -1;
    }
  }
  if (shift > 0 && (number << shift) >> shift != number) {
    errno = ERANGE;
    return -1;
  }
  *num = number << shift;
  return 0;
}

void hexdump(const void *ptr, int length, const char *hdr,
             awm_hexdump_flags_t flags) {
  const unsigned char *cp = (const unsigned char *)ptr;
  int cols = (int)(flags & HD_COLUMN_MASK);
  int delim = (int)((flags & HD_DELIM_MASK) >> 8);
  int i;
  int j;

  if (cols == 0) {
    cols = 16;
  }
  if (delim == 0) {
    delim = '|';
  }
  for (i = 0; i < length; i += cols) {
    if (hdr != NULL) {
      printf("%s", hdr);
    }
    if ((flags & HD_OMIT_COUNT) == 0) {
      printf("%04x  ", (unsigned int)i);
    }
    if ((flags & HD_OMIT_HEX) == 0) {
      for (j = 0; j < cols; j++) {
        if (i + j < length) {
          printf("%02x ", (unsigned int)cp[i + j]);
        } else {
          printf("   ");
        }
      }
      printf(" ");
    }
    if ((flags & HD_OMIT_CHARS) == 0) {
      printf("%c", delim);
      for (j = 0; j < cols && i + j < length; j++) {
        unsigned char ch = cp[i + j];
        printf("%c", isprint(ch) ? ch : '.');
      }
      printf("%c", delim);
    }
    putchar(10);
  }
}

#if defined(__WATCOMC__) && (defined(__DOS__) || defined(__MSDOS__))
static int libutil_dos1_has_subdir(const char *path) {
  const char *p = path;
  if (_osmajor >= 2 || p == NULL) {
    return 0;
  }
  if (((p[0] >= 'A' && p[0] <= 'Z') || (p[0] >= 'a' && p[0] <= 'z')) &&
      p[1] == ':') {
    p += 2;
  }
  if (p[0] == '/' || p[0] == '\\') {
    p++;
  }
  if (p[0] == '.' && (p[1] == '/' || p[1] == '\\')) {
    p += 2;
  }
  return (strchr(p, '/') != NULL || strchr(p, '\\') != NULL) ? 1 : 0;
}
#endif

struct pidfh *pidfile_open(const char *path, mode_t mode, pid_t *pidptr) {
  struct pidfh *pfh;
  const char *p = (path != NULL) ? path : "pidfile.pid";
  if (pidptr != NULL) {
    *pidptr = 0;
  }
#if defined(__WATCOMC__) && (defined(__DOS__) || defined(__MSDOS__))
  if (libutil_dos1_has_subdir(p)) {
    errno = ENOENT;
    return NULL;
  }
#endif
  pfh = (struct pidfh *)g_libutil_calloc(1, sizeof(*pfh));
  if (pfh == NULL) {
    return NULL;
  }
#if defined(_MSC_VER)
  strncpy_s(pfh->pf_path, sizeof(pfh->pf_path), p, _TRUNCATE);
  pfh->pf_fd = -1;
  (void)_sopen_s(&pfh->pf_fd, p, _O_WRONLY | _O_CREAT | _O_TRUNC, _SH_DENYNO,
                 (int)((mode ? mode : 0600) & 0600));
#else
  strncpy(pfh->pf_path, p, sizeof(pfh->pf_path) - 1);
  pfh->pf_path[sizeof(pfh->pf_path) - 1] = '\0';
  pfh->pf_fd = open(p, O_WRONLY | O_CREAT | O_TRUNC, mode ? mode : 0600);
#endif
  if (pfh->pf_fd < 0) {
    free(pfh);
    return NULL;
  }
  return pfh;
}

int pidfile_write(struct pidfh *pfh) {
  char buf[32];
  int len;
  if (pfh == NULL || pfh->pf_fd < 0) {
    errno = EINVAL;
    return -1;
  }
#if defined(_MSC_VER)
  len = sprintf_s(buf, sizeof(buf), "%d\012", (int)_getpid());
#elif defined(_WIN32)
  len = sprintf(buf, "%d\012", (int)_getpid());
#else
  len = sprintf(buf, "%d\012", (int)getpid());
#endif
  return (g_libutil_write(pfh->pf_fd, buf, (unsigned int)len) == len) ? 0 : -1;
}

int pidfile_close(struct pidfh *pfh) {
  if (pfh == NULL) {
    return 0;
  }
  if (pfh->pf_fd >= 0) {
#if defined(_MSC_VER) || defined(_WIN32)
    _close(pfh->pf_fd);
#else
    close(pfh->pf_fd);
#endif
  }
  free(pfh);
  return 0;
}

int pidfile_remove(struct pidfh *pfh) {
  if (pfh == NULL) {
    return 0;
  }
  if (pfh->pf_fd >= 0) {
#if defined(_MSC_VER) || defined(_WIN32)
    _close(pfh->pf_fd);
#else
    close(pfh->pf_fd);
#endif
  }
  (void)remove(pfh->pf_path);
  free(pfh);
  return 0;
}

int pidfile_fileno(const struct pidfh *pfh) {
  if (pfh == NULL) {
    errno = EINVAL;
    return -1;
  }
  return pfh->pf_fd;
}

int flopen(const char *path, int flags, ...) {
  mode_t mode = 0666;
  va_list ap;
  if (path == NULL) {
    errno = EINVAL;
    return -1;
  }
#if defined(__WATCOMC__) && (defined(__DOS__) || defined(__MSDOS__))
  if (libutil_dos1_has_subdir(path)) {
    errno = ENOENT;
    return -1;
  }
#endif
  if (flags & O_CREAT) {
    va_start(ap, flags);
    mode = (mode_t)va_arg(ap, int);
    va_end(ap);
  }
#if defined(_MSC_VER)
  {
    int fd = -1;
    if (_sopen_s(&fd, path, flags, _SH_DENYNO, (int)(mode & 0600)) != 0) {
      return -1;
    }
    return fd;
  }
#else
  return open(path, flags, mode);
#endif
}

int flopenat(int dirfd, const char *path, int flags, ...) {
  mode_t mode = 0666;
  va_list ap;
  (void)dirfd;
  if (flags & O_CREAT) {
    va_start(ap, flags);
    mode = (mode_t)va_arg(ap, int);
    va_end(ap);
  }
  return flopen(path, flags, (int)mode);
}

int uu_lock(const char *ttyname) {
  (void)ttyname;
  return UU_LOCK_OK;
}

int uu_unlock(const char *ttyname) {
  (void)ttyname;
  return 0;
}

int uu_lock_txfr(const char *ttyname, pid_t pid) {
  (void)ttyname;
  (void)pid;
  return UU_LOCK_OK;
}

const char *uu_lockerr(int uu_lockresult) {
  (void)uu_lockresult;
  return "No error";
}

void trimdomain(char *fullhost, int hostsize) {
  char *dot;
  if (fullhost == NULL || hostsize <= 0) {
    return;
  }
  dot = strchr(fullhost, '.');
  if (dot != NULL && (dot - fullhost) < hostsize) {
    *dot = '\0';
  }
}

int realhostname(char *host, size_t hsize, const void *ip) {
  (void)ip;
  if (host != NULL && hsize > 0) {
#if defined(_MSC_VER)
    strncpy_s(host, hsize, "localhost", _TRUNCATE);
#else
    strncpy(host, "localhost", hsize - 1);
    host[hsize - 1] = '\0';
#endif
  }
  return HOSTNAME_FOUND;
}

int realhostname_sa(char *host, size_t hsize, struct sockaddr *addr,
                    int addrlen) {
  (void)addr;
  (void)addrlen;
  return realhostname(host, hsize, NULL);
}

login_cap_t *login_getclass(const char *name) {
  login_cap_t *lc = (login_cap_t *)g_libutil_calloc(1, sizeof(*lc));
  if (lc == NULL) {
    return NULL;
  }
  lc->lc_class = g_libutil_strdup((name != NULL) ? name : LOGIN_DEFCLASS);
  if (lc->lc_class == NULL) {
    free(lc);
    return NULL;
  }
  return lc;
}

login_cap_t *login_getpwclass(const struct passwd *pwd) {
  (void)pwd;
  return login_getclass(LOGIN_DEFCLASS);
}

login_cap_t *login_getuserclass(const struct passwd *pwd) {
  (void)pwd;
  return login_getclass(LOGIN_DEFCLASS);
}

void login_close(login_cap_t *lc) {
  if (lc != NULL) {
    if (lc->lc_class != NULL) {
      free(lc->lc_class);
    }
    if (lc->lc_cap != NULL) {
      free(lc->lc_cap);
    }
    if (lc->lc_style != NULL) {
      free(lc->lc_style);
    }
    free(lc);
  }
}

int login_getcapbool(login_cap_t *lc, const char *cap, int def) {
  (void)lc;
  (void)cap;
  return def;
}

rlim_t login_getcapnum(login_cap_t *lc, const char *cap, rlim_t def,
                       rlim_t err) {
  (void)lc;
  (void)cap;
  (void)err;
  return def;
}

rlim_t login_getcapsize(login_cap_t *lc, const char *cap, rlim_t def,
                        rlim_t err) {
  (void)lc;
  (void)cap;
  (void)err;
  return def;
}

const char *login_getcapstr(login_cap_t *lc, const char *cap, const char *def,
                            const char *err) {
  (void)lc;
  (void)cap;
  (void)err;
  return def;
}

rlim_t login_getcaptime(login_cap_t *lc, const char *cap, rlim_t def,
                        rlim_t err) {
  (void)lc;
  (void)cap;
  (void)err;
  return def;
}

const char *login_getpath(login_cap_t *lc, const char *cap, const char *def,
                          const char *err) {
  (void)lc;
  (void)cap;
  (void)err;
  return def;
}

int setclasscontext(const char *lcname, unsigned int flags) {
  (void)lcname;
  (void)flags;
  return 0;
}

int setusercontext(login_cap_t *lc, const struct passwd *pwd, uid_t uid,
                   unsigned int flags) {
  (void)lc;
  (void)pwd;
  (void)uid;
  (void)flags;
  return 0;
}

const char *getlocalbase(void) {
#if defined(_MSC_VER)
  static char s_localbase[512];
  size_t req = 0;
  if (getenv_s(&req, s_localbase, sizeof(s_localbase), "LOCALBASE") == 0 &&
      req > 1) {
    return s_localbase;
  }
#elif defined(_WIN32)
  const char *env = getenv("LOCALBASE");
  if (env != NULL) {
    return env;
  }
#else
  const char *env = getenv("LOCALBASE");
  if (env != NULL && env[0] != '\0') {
    return env;
  }
#endif
#if defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__)
  return "C:\134usr\134local";
#else
  return "/usr/local";
#endif
}

int setclassenvironment(login_cap_t *lc, const struct passwd *pwd, int paths) {
  (void)lc;
  (void)pwd;
  (void)paths;
  return 0;
}

#endif /* !__FreeBSD__ */

/** @brief Opaque quota file state placeholder. */
struct quotafile {
  int dummy; /**< Placeholder field for ISO C89 compliance. */
};

struct quotafile *quota_open(struct fstab *fs, int quotatype, int openflags) {
  (void)fs;
  (void)quotatype;
  (void)openflags;
  return NULL;
}

int quota_read(struct quotafile *qf, struct dqblk *dqb, int id) {
  (void)qf;
  (void)dqb;
  (void)id;
  return -1;
}

void quota_close(struct quotafile *qf) { (void)qf; }

int pw_copy(int ffd, int tfd, const struct passwd *pw, struct passwd *old_pw) {
  (void)ffd;
  (void)tfd;
  (void)pw;
  (void)old_pw;
  return -1;
}

struct passwd *pw_dup(const struct passwd *pw) {
  struct passwd *copy;
  if (pw == NULL) {
    return NULL;
  }
  copy = (struct passwd *)g_libutil_calloc(1, sizeof(struct passwd));
  if (copy != NULL) {
    *copy = *pw;
  }
  return copy;
}

int pw_edit(int notsetuid) {
  (void)notsetuid;
  return -1;
}

int pw_equal(const struct passwd *pw1, const struct passwd *pw2) {
  if (pw1 == NULL || pw2 == NULL) {
    return 0;
  }
  return pw1->pw_uid == pw2->pw_uid && pw1->pw_gid == pw2->pw_gid;
}

void pw_fini(void) {}

int pw_init(const char *dir, const char *master) {
  (void)dir;
  (void)master;
  return 0;
}

void pw_initpwd(struct passwd *pw) {
  if (pw != NULL) {
    memset(pw, 0, sizeof(*pw));
  }
}

char *pw_make(const struct passwd *pw) {
  (void)pw;
  return NULL;
}

char *pw_make_v7(const struct passwd *pw) {
  (void)pw;
  return NULL;
}

int pw_mkdb(const char *user) {
  (void)user;
  return -1;
}

int pw_lock(void) { return -1; }

int __pw_scan(char *bp, struct passwd *pw, int flags) {
  (void)bp;
  (void)pw;
  (void)flags;
  return 0;
}

struct passwd *pw_scan(const char *line, int flags) {
  (void)line;
  (void)flags;
  return NULL;
}

const char *pw_tempname(void) { return "/etc/master.passwd.tmp"; }

int pw_tmp(int mfd) {
  (void)mfd;
  return -1;
}

login_cap_t *login_getclassbyname(const char *name, const struct passwd *pwd) {
  (void)pwd;
  return login_getclass(name);
}

typedef int make_iso_compilers_happy_tu_bsd_libutil;
#endif
typedef int make_iso_compilers_happy_mega_bsd_libutil;
