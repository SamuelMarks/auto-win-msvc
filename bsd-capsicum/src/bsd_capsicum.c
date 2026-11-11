#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_capsicum.c
 * @brief Implementation of FreeBSD Capsicum, Casper, and process descriptor
 * shims.
 */

/* clang-format off */
#include <sys/capsicum.h>
#include <capsicum_helpers.h>
#include <libcasper.h>
#include <casper/cap_fileargs.h>
#include <sys/procdesc.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

#if !defined(__FreeBSD__)

int cap_enter(void) { return 0; }

int cap_getmode(unsigned int *modep) {
  if (modep == NULL) {
    errno = EFAULT;
    return -1;
  }
  *modep = 0;
  return 0;
}

int cap_sandboxed(void) { return 0; }

cap_rights_t *__cap_rights_init(int version, cap_rights_t *rights, ...) {
  va_list ap;
  uint64_t r;
  (void)version;
  if (rights == NULL) {
    return NULL;
  }
  rights->cr_rights[0] = 0;
  rights->cr_rights[1] = 0;
  va_start(ap, rights);
  while ((r = va_arg(ap, uint64_t)) != ((uint64_t)0UL)) {
    rights->cr_rights[0] |= r;
  }
  va_end(ap);
  return rights;
}

cap_rights_t *__cap_rights_set(cap_rights_t *rights, ...) {
  va_list ap;
  uint64_t r;
  if (rights == NULL) {
    return NULL;
  }
  va_start(ap, rights);
  while ((r = va_arg(ap, uint64_t)) != ((uint64_t)0UL)) {
    rights->cr_rights[0] |= r;
  }
  va_end(ap);
  return rights;
}

cap_rights_t *__cap_rights_clear(cap_rights_t *rights, ...) {
  va_list ap;
  uint64_t r;
  if (rights == NULL) {
    return NULL;
  }
  va_start(ap, rights);
  while ((r = va_arg(ap, uint64_t)) != ((uint64_t)0UL)) {
    rights->cr_rights[0] &= ~r;
  }
  va_end(ap);
  return rights;
}

int __cap_rights_is_set(const cap_rights_t *rights, ...) {
  va_list ap;
  uint64_t r;
  if (rights == NULL) {
    return 0;
  }
  va_start(ap, rights);
  while ((r = va_arg(ap, uint64_t)) != ((uint64_t)0UL)) {
    if ((rights->cr_rights[0] & r) != r) {
      va_end(ap);
      return 0;
    }
  }
  va_end(ap);
  return 1;
}

cap_rights_t *cap_rights_init(cap_rights_t *rights, ...) {
  if (rights != NULL) {
    rights->cr_rights[0] = CAP_ALL0;
    rights->cr_rights[1] = CAP_ALL1;
  }
  return rights;
}

cap_rights_t *cap_rights_set(cap_rights_t *rights, ...) { return rights; }

cap_rights_t *cap_rights_clear(cap_rights_t *rights, ...) {
  if (rights != NULL) {
    rights->cr_rights[0] = 0;
    rights->cr_rights[1] = 0;
  }
  return rights;
}

int cap_rights_is_set(const cap_rights_t *rights, ...) {
  return (rights != NULL && rights->cr_rights[0] != 0) ? 1 : 0;
}

int cap_rights_is_valid(const cap_rights_t *rights) {
  return (rights != NULL) ? 1 : 0;
}

cap_rights_t *cap_rights_merge(cap_rights_t *dst, const cap_rights_t *src) {
  if (dst != NULL && src != NULL) {
    dst->cr_rights[0] |= src->cr_rights[0];
    dst->cr_rights[1] |= src->cr_rights[1];
  }
  return dst;
}

cap_rights_t *cap_rights_remove(cap_rights_t *dst, const cap_rights_t *src) {
  if (dst != NULL && src != NULL) {
    dst->cr_rights[0] &= ~src->cr_rights[0];
    dst->cr_rights[1] &= ~src->cr_rights[1];
  }
  return dst;
}

int cap_rights_contains(const cap_rights_t *big, const cap_rights_t *little) {
  if (big == NULL || little == NULL) {
    return 0;
  }
  return ((big->cr_rights[0] & little->cr_rights[0]) == little->cr_rights[0])
             ? 1
             : 0;
}

int cap_rights_limit(int fd, const cap_rights_t *rights) {
  (void)fd;
  (void)rights;
  return 0;
}

int __cap_rights_get(int version, int fd, cap_rights_t *rights) {
  (void)version;
  (void)fd;
  if (rights != NULL) {
    rights->cr_rights[0] = CAP_ALL0;
    rights->cr_rights[1] = CAP_ALL1;
  }
  return 0;
}

int cap_ioctls_limit(int fd, const unsigned long *cmds, size_t ncmds) {
  (void)fd;
  (void)cmds;
  (void)ncmds;
  return 0;
}

ssize_t cap_ioctls_get(int fd, unsigned long *cmds, size_t maxcmds) {
  (void)fd;
  (void)cmds;
  (void)maxcmds;
  return CAP_IOCTLS_ALL;
}

int cap_fcntls_limit(int fd, uint32_t fcntlrights) {
  (void)fd;
  (void)fcntlrights;
  return 0;
}

int cap_fcntls_get(int fd, uint32_t *fcntlrightsp) {
  (void)fd;
  if (fcntlrightsp != NULL) {
    *fcntlrightsp = CAP_FCNTL_ALL;
  }
  return 0;
}

cap_channel_t *cap_init(void) {
  cap_channel_t *ch = (cap_channel_t *)calloc(1, sizeof(*ch));
  if (ch != NULL) {
    ch->fd = -1;
  }
  return ch;
}

cap_channel_t *cap_wrap(int sock, int flags) {
  cap_channel_t *ch = (cap_channel_t *)calloc(1, sizeof(*ch));
  (void)flags;
  if (ch != NULL) {
    ch->fd = sock;
  }
  return ch;
}

int cap_unwrap(cap_channel_t *chan, int *flags) {
  int fd = -1;
  if (flags != NULL) {
    *flags = 0;
  }
  if (chan != NULL) {
    fd = chan->fd;
    free(chan);
  }
  return fd;
}

int cap_sock(const cap_channel_t *chan) {
  return (chan != NULL) ? chan->fd : -1;
}

cap_channel_t *cap_clone(const cap_channel_t *chan) {
  return cap_wrap((chan != NULL) ? chan->fd : -1, 0);
}

void cap_close(cap_channel_t *chan) {
  if (chan != NULL) {
    free(chan);
  }
}

cap_channel_t *cap_service_open(const cap_channel_t *chan, const char *name) {
  (void)name;
  return cap_clone(chan);
}

int cap_limit_set(const cap_channel_t *chan, nvlist_t *limits) {
  (void)chan;
  (void)limits;
  return 0;
}

int cap_limit_get(const cap_channel_t *chan, nvlist_t **limitsp) {
  (void)chan;
  if (limitsp != NULL) {
    *limitsp = NULL;
  }
  return 0;
}

int cap_send_nvlist(const cap_channel_t *chan, const nvlist_t *nvl) {
  (void)chan;
  (void)nvl;
  return 0;
}

nvlist_t *cap_recv_nvlist(const cap_channel_t *chan) {
  (void)chan;
  return NULL;
}

nvlist_t *cap_xfer_nvlist(const cap_channel_t *chan, nvlist_t *nvl) {
  (void)chan;
  (void)nvl;
  return NULL;
}

fileargs_t *fileargs_init(int argc, char *argv[], int flags, mode_t mode,
                          cap_rights_t *rightsp, int operations) {
  fileargs_t *fa;
  (void)argc;
  (void)argv;
  (void)rightsp;
  fa = (fileargs_t *)calloc(1, sizeof(*fa));
  if (fa != NULL) {
    fa->flags = flags;
    fa->mode = mode;
    fa->operations = operations;
  }
  return fa;
}

fileargs_t *fileargs_initnv(nvlist_t *limits) {
  (void)limits;
  return fileargs_init(0, NULL, 0, 0666, NULL,
                       FA_OPEN | FA_LSTAT | FA_REALPATH);
}

int fileargs_open(fileargs_t *fa, const char *name) {
  int flags = (fa != NULL) ? fa->flags : 0;
  mode_t mode = (fa != NULL) ? fa->mode : 0666;
  if (name == NULL) {
    errno = EINVAL;
    return -1;
  }
#if defined(_MSC_VER)
  {
    int fd = -1;
    if (_sopen_s(&fd, name, flags, 0x40 /* _SH_DENYNO */, (int)(mode & 0600)) !=
        0) {
      return -1;
    }
    return fd;
  }
#else
  return open(name, flags, mode);
#endif
}

FILE *fileargs_fopen(fileargs_t *fa, const char *name, const char *mode) {
  (void)fa;
  if (name == NULL || mode == NULL) {
    errno = EINVAL;
    return NULL;
  }
#if defined(_MSC_VER)
  {
    FILE *fp = NULL;
    if (fopen_s(&fp, name, mode) != 0) {
      return NULL;
    }
    return fp;
  }
#else
  return fopen(name, mode);
#endif
}

int fileargs_lstat(fileargs_t *fa, const char *name, struct stat *sb) {
  (void)fa;
  if (name == NULL || sb == NULL) {
    errno = EINVAL;
    return -1;
  }
#if defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__) ||               \
    defined(__WATCOMC__)
  return stat(name, sb);
#else
  return lstat(name, sb);
#endif
}

char *fileargs_realpath(fileargs_t *fa, const char *pathname,
                        char *reserved_path) {
  (void)fa;
  if (pathname == NULL) {
    errno = EINVAL;
    return NULL;
  }
#if defined(_WIN32)
  return _fullpath(reserved_path, pathname, 1024);
#elif defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
  if (reserved_path != NULL) {
    strcpy(reserved_path, pathname);
    return reserved_path;
  }
  return strdup(pathname);
#else
  return realpath(pathname, reserved_path);
#endif
}

int fileargs_caph_enter(fileargs_t *fa) {
  (void)fa;
  return 0;
}

void fileargs_free(fileargs_t *fa) {
  if (fa != NULL) {
    free(fa);
  }
}

pid_t pdfork(int *fdp, int flags) {
  (void)flags;
  if (fdp != NULL) {
    *fdp = -1;
  }
  errno = ENOSYS;
  return -1;
}

int pdgetpid(int fd, pid_t *pidp) {
  (void)fd;
  if (pidp != NULL) {
    *pidp = 0;
  }
  errno = ENOSYS;
  return -1;
}

int pdkill(int fd, int signum) {
  (void)fd;
  (void)signum;
  errno = ENOSYS;
  return -1;
}

int pdwait4(int fd, int *status, int options, struct rusage *rusage) {
  (void)fd;
  (void)status;
  (void)options;
  (void)rusage;
  errno = ENOSYS;
  return -1;
}

#endif /* !__FreeBSD__ */

typedef int make_iso_compilers_happy_tu_bsd_capsicum;

#endif
typedef int make_iso_compilers_happy_mega_bsd_capsicum;
