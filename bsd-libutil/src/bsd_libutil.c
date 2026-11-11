#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_libutil.c
 * @brief Implementation of FreeBSD <libutil.h> and <login_cap.h> functions.
 */

/* clang-format off */
#include <libutil.h>
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
#else
#include <unistd.h>
#endif
/* clang-format on */

#if !defined(__FreeBSD__)

int humanize_number(char *buf, size_t len, int64_t bytes, const char *suffix,
                    int scale, int flags) {
  static const char prefixes[] = " KMGTPE";
  const char *sep = (flags & HN_NOSPACE) ? "" : " ";
  const char *sfx = (suffix != NULL) ? suffix : "";
  int divisor = (flags & HN_DIVISOR_1000) ? 1000 : 1024;
  int i = 0;
  int64_t val = bytes;
  int sign = 1;
  char pfx[4];
  int r;

  if (buf == NULL || len == 0) {
    errno = EINVAL;
    return -1;
  }
  if (val < 0) {
    sign = -1;
    val = -val;
  }
  if (scale == HN_AUTOSCALE || scale == HN_GETSCALE || scale <= 0) {
    while (val >= divisor && i < 6) {
      val /= divisor;
      i++;
    }
  } else {
    while (i < scale && i < 6) {
      val /= divisor;
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
#if defined(_MSC_VER)
  r = _snprintf_s(buf, len, _TRUNCATE, "%ld%s%s%s", (long)(val * sign), sep,
                  pfx, sfx);
#else
  r = snprintf(buf, len, "%ld%s%s%s", (long)(val * sign), sep, pfx, sfx);
#endif
  if (r < 0 || (size_t)r >= len) {
    return -1;
  }
  return r;
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
#if defined(_MSC_VER)
  number = _strtoui64(p, &endptr, 0);
#else
  number = (uint64_t)strtoul(p, &endptr, 0);
#endif
  if (endptr == p || errno == ERANGE) {
    if (errno == 0) {
      errno = EINVAL;
    }
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
  if (endptr[1] != '\0' &&
      !(tolower((unsigned char)endptr[1]) == 'b' && endptr[2] == '\0') &&
      !(tolower((unsigned char)endptr[1]) == 'i' &&
        tolower((unsigned char)endptr[2]) == 'b' && endptr[3] == '\0')) {
    errno = EINVAL;
    return -1;
  }
  if (shift > 0 && (number << shift) >> shift != number) {
    errno = ERANGE;
    return -1;
  }
  *num = number << shift;
  return 0;
}

void hexdump(const void *ptr, int length, const char *hdr, int flags) {
  const unsigned char *cp = (const unsigned char *)ptr;
  int cols = flags & HD_COLUMN_MASK;
  int delim = (flags & HD_DELIM_MASK) >> 8;
  int i, j;

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
      printf("%04x  ", i);
    }
    if ((flags & HD_OMIT_HEX) == 0) {
      for (j = 0; j < cols; j++) {
        if (i + j < length) {
          printf("%02x ", cp[i + j]);
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
    printf("\n");
  }
}

struct pidfh *pidfile_open(const char *path, mode_t mode, pid_t *pidptr) {
  struct pidfh *pfh;
  const char *p = (path != NULL) ? path : "pidfile.pid";
  if (pidptr != NULL) {
    *pidptr = 0;
  }
  pfh = (struct pidfh *)calloc(1, sizeof(*pfh));
  if (pfh == NULL) {
    return NULL;
  }
#if defined(_MSC_VER)
  strncpy_s(pfh->pf_path, sizeof(pfh->pf_path), p, _TRUNCATE);
  pfh->pf_fd = -1;
  (void)_sopen_s(&pfh->pf_fd, p, _O_WRONLY | _O_CREAT | _O_TRUNC,
                 0x40 /* _SH_DENYNO */, (int)((mode ? mode : 0600) & 0600));
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
  len = sprintf_s(buf, sizeof(buf), "%d\n", (int)_getpid());
  return (_write(pfh->pf_fd, buf, (unsigned int)len) == len) ? 0 : -1;
#else
  len = sprintf(buf, "%d\n", (int)getpid());
  return (write(pfh->pf_fd, buf, (size_t)len) == len) ? 0 : -1;
#endif
}

int pidfile_close(struct pidfh *pfh) {
  if (pfh == NULL) {
    return 0;
  }
  if (pfh->pf_fd >= 0) {
#if defined(_MSC_VER)
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
#if defined(_MSC_VER)
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
  if (flags & O_CREAT) {
    va_start(ap, flags);
    mode = (mode_t)va_arg(ap, int);
    va_end(ap);
  }
#if defined(_MSC_VER)
  {
    int fd = -1;
    if (_sopen_s(&fd, path, flags, 0x40 /* _SH_DENYNO */, (int)(mode & 0600)) !=
        0) {
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
  login_cap_t *lc = (login_cap_t *)calloc(1, sizeof(*lc));
  if (lc == NULL) {
    return NULL;
  }
#if defined(_MSC_VER)
  lc->lc_class = _strdup(name ? name : LOGIN_DEFCLASS);
#else
  lc->lc_class = strdup(name ? name : LOGIN_DEFCLASS);
#endif
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
    if (lc->lc_class) {
      free(lc->lc_class);
    }
    if (lc->lc_cap) {
      free(lc->lc_cap);
    }
    if (lc->lc_style) {
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

int64_t login_getcapnum(login_cap_t *lc, const char *cap, int64_t def,
                        int64_t err) {
  (void)lc;
  (void)cap;
  (void)err;
  return def;
}

int64_t login_getcapsize(login_cap_t *lc, const char *cap, int64_t def,
                         int64_t err) {
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

int64_t login_getcaptime(login_cap_t *lc, const char *cap, int64_t def,
                         int64_t err) {
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

#endif /* !__FreeBSD__ */

typedef int make_iso_compilers_happy_tu_bsd_libutil;

#endif
typedef int make_iso_compilers_happy_mega_bsd_libutil;
