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
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#elif defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
#include <io.h>
#else
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
/* clang-format on */

#if !defined(__FreeBSD__)

static void *default_capsicum_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

/** @brief Overrideable calloc hook for OOM testing in bsd-capsicum. */
void *(*g_bsd_capsicum_calloc)(size_t, size_t) = default_capsicum_calloc;

/** @brief Resets g_bsd_capsicum_calloc back to the default allocator. */
void bsd_capsicum_reset_calloc_hook(void);
void bsd_capsicum_reset_calloc_hook(void) {
  g_bsd_capsicum_calloc = default_capsicum_calloc;
}

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

/** @brief Initialize Casper daemon client channel (passthrough shim). */
cap_channel_t *cap_init(void) {
  cap_channel_t *ch = (cap_channel_t *)g_bsd_capsicum_calloc(1, sizeof(*ch));
  if (ch != NULL) {
    ch->fd = -1;
  }
  return ch;
}

/** @brief Wrap existing socket descriptor in Casper channel handle. */
cap_channel_t *cap_wrap(int sock, int flags) {
  cap_channel_t *ch = (cap_channel_t *)g_bsd_capsicum_calloc(1, sizeof(*ch));
  (void)flags;
  if (ch != NULL) {
    ch->fd = sock;
  }
  return ch;
}

/** @brief Unwrap Casper channel handle and return underlying descriptor. */
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

/** @brief Return underlying socket descriptor of Casper channel. */
int cap_sock(const cap_channel_t *chan) {
  return (chan != NULL) ? chan->fd : -1;
}

/** @brief Clone Casper channel handle. */
cap_channel_t *cap_clone(const cap_channel_t *chan) {
  return cap_wrap((chan != NULL) ? chan->fd : -1, 0);
}

/** @brief Close and free Casper channel handle. */
void cap_close(cap_channel_t *chan) {
  if (chan != NULL) {
    free(chan);
  }
}

/** @brief Open named Casper service channel. */
cap_channel_t *cap_service_open(const cap_channel_t *chan, const char *name) {
  (void)name;
  return cap_clone(chan);
}

/** @brief Apply nvlist service limits to Casper channel. */
int cap_limit_set(const cap_channel_t *chan, nvlist_t *limits) {
  (void)chan;
  (void)limits;
  return 0;
}

/** @brief Retrieve active nvlist service limits from Casper channel. */
int cap_limit_get(const cap_channel_t *chan, nvlist_t **limitsp) {
  (void)chan;
  if (limitsp != NULL) {
    *limitsp = NULL;
  }
  return 0;
}

/** @brief Send nvlist message over Casper channel. */
int cap_send_nvlist(const cap_channel_t *chan, const nvlist_t *nvl) {
  (void)chan;
  (void)nvl;
  return 0;
}

/** @brief Receive nvlist message from Casper channel. */
nvlist_t *cap_recv_nvlist(const cap_channel_t *chan) {
  (void)chan;
  return NULL;
}

/** @brief Send nvlist request and receive nvlist reply over Casper channel. */
nvlist_t *cap_xfer_nvlist(const cap_channel_t *chan, nvlist_t *nvl) {
  (void)chan;
  (void)nvl;
  return NULL;
}

/** @brief Initialize Casper fileargs service handle. */
fileargs_t *fileargs_init(int argc, char *argv[], int flags, mode_t mode,
                          cap_rights_t *rightsp, int operations) {
  fileargs_t *fa;
  (void)argc;
  (void)argv;
  (void)rightsp;
  fa = (fileargs_t *)g_bsd_capsicum_calloc(1, sizeof(*fa));
  if (fa != NULL) {
    fa->flags = flags;
    fa->mode = mode;
    fa->operations = operations;
  }
  return fa;
}

/** @brief Initialize Casper fileargs handle from nvlist configuration. */
fileargs_t *fileargs_initnv(nvlist_t *limits) {
  (void)limits;
  return fileargs_init(0, NULL, 0, 0666, NULL,
                       FA_OPEN | FA_LSTAT | FA_REALPATH);
}

/** @brief Open file path via Casper fileargs service. */
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
#elif defined(_WIN32)
  return _open(name, flags, (int)(mode & 0600));
#else
  return open(name, flags, mode);
#endif
}

/** @brief Open stdio FILE stream via Casper fileargs service. */
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

/** @brief Stat file path via Casper fileargs service. */
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

/** @brief Resolve canonical path via Casper fileargs service. */
char *fileargs_realpath(fileargs_t *fa, const char *pathname,
                        char *reserved_path) {
  (void)fa;
  if (pathname == NULL) {
    errno = EINVAL;
    return NULL;
  }
#if defined(_WIN32)
  return _fullpath(reserved_path, pathname, _MAX_PATH);
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

/** @brief Enter capability mode after fileargs setup. */
int fileargs_caph_enter(fileargs_t *fa) {
  (void)fa;
  return 0;
}

/** @brief Free Casper fileargs handle. */
void fileargs_free(fileargs_t *fa) {
  if (fa != NULL) {
    free(fa);
  }
}

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
static int default_capsicum_pipe(int pfd[2]) { return pipe(pfd); }
static pid_t default_capsicum_fork(void) { return fork(); }
static pid_t default_capsicum_waitpid(pid_t pid, int *status, int options) {
  return waitpid(pid, status, options);
}
static int default_capsicum_kill(pid_t pid, int sig) { return kill(pid, sig); }
static int default_capsicum_close(int fd) { return close(fd); }

/** @brief Overrideable pipe() hook for POSIX pdfork testing. */
int (*g_bsd_capsicum_pipe)(int[2]) = default_capsicum_pipe;
/** @brief Overrideable fork() hook for POSIX pdfork testing. */
pid_t (*g_bsd_capsicum_fork)(void) = default_capsicum_fork;
/** @brief Overrideable waitpid() hook for POSIX pdwait4 testing. */
pid_t (*g_bsd_capsicum_waitpid)(pid_t, int *, int) = default_capsicum_waitpid;
/** @brief Overrideable kill() hook for POSIX pdkill testing. */
int (*g_bsd_capsicum_kill)(pid_t, int) = default_capsicum_kill;
/** @brief Overrideable close() hook for POSIX pdfork testing. */
int (*g_bsd_capsicum_close)(int) = default_capsicum_close;

/** @brief Resets all POSIX process-descriptor hooks to OS defaults. */
void bsd_capsicum_reset_posix_hooks(void);
/** @brief Resets all POSIX process-descriptor hooks to OS defaults. */
void bsd_capsicum_reset_posix_hooks(void) {
  g_bsd_capsicum_pipe = default_capsicum_pipe;
  g_bsd_capsicum_fork = default_capsicum_fork;
  g_bsd_capsicum_waitpid = default_capsicum_waitpid;
  g_bsd_capsicum_kill = default_capsicum_kill;
  g_bsd_capsicum_close = default_capsicum_close;
}

static struct {
  /** @brief Pipe descriptor representing the process descriptor. */
  int fd;
  /** @brief Child process identifier. */
  pid_t pid;
} s_awm_pd_table[64];
#endif

/** @brief Fork child process and return process descriptor. */
pid_t pdfork(int *fdp, int flags) {
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  int pfd[2];
  pid_t pid;
  size_t i;
  (void)flags;
  if (fdp == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (g_bsd_capsicum_pipe(pfd) != 0) {
    return -1;
  }
  pid = g_bsd_capsicum_fork();
  if (pid < 0) {
    g_bsd_capsicum_close(pfd[0]);
    g_bsd_capsicum_close(pfd[1]);
    return -1;
  }
  if (pid == 0) {
    g_bsd_capsicum_close(pfd[0]);
    g_bsd_capsicum_close(pfd[1]);
    return 0;
  }
  g_bsd_capsicum_close(pfd[1]);
  *fdp = pfd[0];
  for (i = 0; i < sizeof(s_awm_pd_table) / sizeof(s_awm_pd_table[0]); ++i) {
    if (s_awm_pd_table[i].fd <= 0) {
      s_awm_pd_table[i].fd = pfd[0];
      s_awm_pd_table[i].pid = pid;
      break;
    }
  }
  return pid;
#else
  (void)flags;
  if (fdp != NULL) {
    *fdp = -1;
  }
  errno = ENOSYS;
  return -1;
#endif
}

/** @brief Query PID associated with process descriptor. */
int pdgetpid(int fd, pid_t *pidp) {
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  size_t i;
  if (pidp == NULL || fd <= 0) {
    errno = EINVAL;
    return -1;
  }
  for (i = 0; i < sizeof(s_awm_pd_table) / sizeof(s_awm_pd_table[0]); ++i) {
    if (s_awm_pd_table[i].fd == fd) {
      *pidp = s_awm_pd_table[i].pid;
      return 0;
    }
  }
#else
  (void)fd;
  if (pidp != NULL) {
    *pidp = 0;
  }
#endif
  errno = EBADF;
  return -1;
}

/** @brief Send signal to process referenced by process descriptor. */
int pdkill(int fd, int signum) {
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  pid_t pid = 0;
  if (pdgetpid(fd, &pid) == 0) {
    return g_bsd_capsicum_kill(pid, signum);
  }
#else
  (void)fd;
  (void)signum;
#endif
  errno = EBADF;
  return -1;
}

/** @brief Wait for process referenced by process descriptor (rusage form). */
int pdwait4(int fd, int *status, int options, struct rusage *rusage) {
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  size_t i;
  (void)rusage;
  if (fd <= 0) {
    errno = EBADF;
    return -1;
  }
  for (i = 0; i < sizeof(s_awm_pd_table) / sizeof(s_awm_pd_table[0]); ++i) {
    if (s_awm_pd_table[i].fd == fd) {
      pid_t rc = g_bsd_capsicum_waitpid(s_awm_pd_table[i].pid, status,
                                        options & ~WEXITED);
      if (rc > 0) {
        s_awm_pd_table[i].pid = 0;
        s_awm_pd_table[i].fd = 0;
      }
      return (int)rc;
    }
  }
#else
  (void)fd;
  (void)status;
  (void)options;
  (void)rusage;
#endif
  errno = EBADF;
  return -1;
}

/** @brief Wait for process referenced by process descriptor. */
int pdwait(int fd, int *status, int options, struct __wrusage *wrusage,
           struct __siginfo *infop) {
  (void)wrusage;
  (void)infop;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  return (pdwait4(fd, status, options, NULL) > 0) ? 0 : -1;
#else
  return pdwait4(fd, status, options, NULL);
#endif
}

#endif /* !__FreeBSD__ */

/** @brief Dummy typedef preventing empty translation unit warnings. */
typedef int make_iso_compilers_happy_tu_bsd_capsicum;

#endif
/** @brief Dummy typedef preventing empty mega translation unit warnings. */
typedef int make_iso_compilers_happy_mega_bsd_capsicum;
