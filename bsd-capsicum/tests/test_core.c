/* clang-format off */
#include "greatest.h"
#include <sys/capsicum.h>
#include <capsicum_helpers.h>
#include <libcasper.h>
#include <casper/cap_fileargs.h>
#include <casper/cap_pwd.h>
#include <casper/cap_grp.h>
#include <casper/cap_net.h>
#include <casper/cap_netdb.h>
#include <casper/cap_sysctl.h>
#include <casper/cap_syslog.h>
#include <sys/procdesc.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
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

extern void *(*g_bsd_capsicum_calloc)(size_t, size_t);
extern void bsd_capsicum_reset_calloc_hook(void);

static int g_mock_capsicum_oom = 0;

static void *mock_capsicum_calloc(size_t nmemb, size_t size) {
  if (g_mock_capsicum_oom) {
    return NULL;
  }
  return calloc(nmemb, size);
}

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
extern int (*g_bsd_capsicum_pipe)(int[2]);
extern pid_t (*g_bsd_capsicum_fork)(void);
extern pid_t (*g_bsd_capsicum_waitpid)(pid_t, int *, int);
extern int (*g_bsd_capsicum_kill)(pid_t, int);
extern int (*g_bsd_capsicum_close)(int);
extern void bsd_capsicum_reset_posix_hooks(void);

static int g_mock_pipe_fail = 0;
static int g_mock_next_pipe_fd = 200;
static pid_t g_mock_fork_ret = 1234;
static pid_t g_mock_waitpid_ret = 1234;

static int mock_capsicum_pipe(int pfd[2]) {
  if (g_mock_pipe_fail) {
    return -1;
  }
  pfd[0] = g_mock_next_pipe_fd++;
  pfd[1] = g_mock_next_pipe_fd++;
  return 0;
}

static pid_t mock_capsicum_fork(void) { return g_mock_fork_ret; }

static pid_t mock_capsicum_waitpid(pid_t pid, int *status, int options) {
  (void)pid;
  (void)options;
  if (status != NULL) {
    *status = 0;
  }
  return g_mock_waitpid_ret;
}

static int mock_capsicum_kill(pid_t pid, int sig) {
  (void)pid;
  (void)sig;
  return 0;
}

static int mock_capsicum_close(int fd) {
  (void)fd;
  return 0;
}
#endif

static void call_cap_vsyslog_helper(cap_channel_t *chan, int pri,
                                    const char *fmt, ...)
    CAP_SYSLOG_PRINTF(3, 4);

static void call_cap_vsyslog_helper(cap_channel_t *chan, int pri,
                                    const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  cap_vsyslog(chan, pri, fmt, ap);
  va_end(ap);
}

TEST test_bsd_capsicum_rights_and_helpers(void) {
  cap_rights_t rights;
  cap_rights_t r2;
  unsigned int mode = 1;
  uint32_t fcntls = 0;

  ASSERT_EQ(0, cap_enter());
  ASSERT_EQ(-1, cap_getmode(NULL));
  ASSERT_EQ(0, cap_getmode(&mode));
  ASSERT_EQ(0U, mode);
  ASSERT_EQ(0, cap_sandboxed());

  /* Direct __cap_rights_* functions (NULL + valid + missing bit) */
  ASSERT(__cap_rights_init(CAP_RIGHTS_VERSION, NULL, (uint64_t)0) == NULL);
  ASSERT(__cap_rights_init(CAP_RIGHTS_VERSION, &rights, CAP_READ,
                           (uint64_t)0) == &rights);
  ASSERT(__cap_rights_set(NULL, (uint64_t)0) == NULL);
  ASSERT(__cap_rights_set(&rights, CAP_WRITE, (uint64_t)0) == &rights);
  ASSERT_EQ(0, __cap_rights_is_set(NULL, (uint64_t)0));
  ASSERT_EQ(1, __cap_rights_is_set(&rights, CAP_READ, CAP_WRITE, (uint64_t)0));
  ASSERT_EQ(0, __cap_rights_is_set(&rights, CAP_FSYNC, (uint64_t)0));
  ASSERT(__cap_rights_clear(NULL, (uint64_t)0) == NULL);
  ASSERT(__cap_rights_clear(&rights, CAP_WRITE, (uint64_t)0) == &rights);
  ASSERT_EQ(0, __cap_rights_is_set(&rights, CAP_WRITE, (uint64_t)0));

  /* C89 fallback cap_rights_* functions (NULL + valid) */
  ASSERT(cap_rights_init(NULL) == NULL);
  ASSERT(cap_rights_init(&rights, CAP_READ, CAP_WRITE) == &rights);
  ASSERT_EQ(0, cap_rights_is_valid(NULL));
  ASSERT_EQ(1, cap_rights_is_valid(&rights));
  ASSERT_EQ(0, cap_rights_is_set(NULL));
  ASSERT_EQ(1, cap_rights_is_set(&rights, CAP_READ));
  ASSERT(cap_rights_set(&rights, CAP_SEEK) == &rights);
  ASSERT(cap_rights_clear(NULL) == NULL);
  ASSERT(cap_rights_clear(&rights, CAP_WRITE) == &rights);
  ASSERT_EQ(0, cap_rights_is_set(&rights, CAP_WRITE));

  /* merge, remove, contains */
  __cap_rights_init(CAP_RIGHTS_VERSION, &rights, CAP_READ, CAP_WRITE,
                    (uint64_t)0);
  __cap_rights_init(CAP_RIGHTS_VERSION, &r2, CAP_READ, (uint64_t)0);
  ASSERT_EQ(0, cap_rights_contains(NULL, &r2));
  ASSERT_EQ(0, cap_rights_contains(&rights, NULL));
  ASSERT_EQ(1, cap_rights_contains(&rights, &r2));
  ASSERT_EQ(0, cap_rights_contains(&r2, &rights));

  ASSERT(cap_rights_merge(NULL, &r2) == NULL);
  ASSERT(cap_rights_merge(&r2, NULL) == &r2);
  ASSERT(cap_rights_merge(&r2, &rights) == &r2);

  ASSERT(cap_rights_remove(NULL, &r2) == NULL);
  ASSERT(cap_rights_remove(&rights, NULL) == &rights);
  ASSERT(cap_rights_remove(&rights, &r2) == &rights);

  /* Descriptor limits & getters */
  ASSERT_EQ(0, cap_rights_limit(0, &r2));
  ASSERT_EQ(0, __cap_rights_get(CAP_RIGHTS_VERSION, 0, NULL));
  ASSERT_EQ(0, cap_rights_get(0, &rights));
  ASSERT_EQ(0, cap_ioctls_limit(0, NULL, 0));
  ASSERT_EQ((ssize_t)CAP_IOCTLS_ALL, cap_ioctls_get(0, NULL, 0));
  ASSERT_EQ(0, cap_fcntls_limit(0, CAP_FCNTL_ALL));
  ASSERT_EQ(0, cap_fcntls_get(0, NULL));
  ASSERT_EQ(0, cap_fcntls_get(0, &fcntls));

  /* All 12 capsicum_helpers.h inline functions */
  ASSERT_EQ(0, caph_enter());
  ASSERT_EQ(0, caph_enter_casper());
  ASSERT_EQ(0, caph_rights_limit(0, &rights));
  ASSERT_EQ(0, caph_ioctls_limit(0, NULL, 0));
  ASSERT_EQ(0, caph_fcntls_limit(0, CAP_FCNTL_ALL));
  ASSERT_EQ(0, caph_limit_stream(0, CAPH_READ));
  ASSERT_EQ(0, caph_limit_stderr());
  ASSERT_EQ(0, caph_limit_stdin());
  ASSERT_EQ(0, caph_limit_stdout());
  ASSERT_EQ(0, caph_limit_stdio());
  caph_cache_tzdata();
  caph_cache_catpages();

  PASS();
}

TEST test_bsd_casper_fileargs_and_procdesc(void) {
  cap_channel_t *ch;
  cap_channel_t *s;
  nvlist_t *nvl = (nvlist_t *)(size_t)1;
  fileargs_t *fa;
  int unwrap_flags = -1;
  int fd = -1;
  FILE *fp;
  struct stat sb;
#ifdef PATH_MAX
  char resolved[PATH_MAX];
#else
  char resolved[4096];
#endif

  /* Casper channel lifecycle & NULL branches */
  ch = cap_init();
  ASSERT(ch != NULL);
  ASSERT_EQ(-1, cap_sock(ch));
  ASSERT_EQ(-1, cap_sock(NULL));

  s = cap_service_open(ch, "system.fileargs");
  ASSERT(s != NULL);
  cap_close(s);

  s = cap_clone(NULL);
  ASSERT(s != NULL);
  ASSERT_EQ(-1, cap_unwrap(s, &unwrap_flags));
  ASSERT_EQ(0, unwrap_flags);
  ASSERT_EQ(-1, cap_unwrap(NULL, NULL));

  s = cap_wrap(7, 0);
  ASSERT(s != NULL);
  ASSERT_EQ(7, cap_unwrap(s, NULL));

  ASSERT_EQ(0, cap_limit_set(ch, NULL));
  ASSERT_EQ(0, cap_limit_get(ch, NULL));
  ASSERT_EQ(0, cap_limit_get(ch, &nvl));
  ASSERT(nvl == NULL);
  ASSERT_EQ(0, cap_send_nvlist(ch, NULL));
  ASSERT(cap_recv_nvlist(ch) == NULL);
  ASSERT(cap_xfer_nvlist(ch, NULL) == NULL);
  cap_close(NULL);
  cap_close(ch);

  /* OOM branches via g_bsd_capsicum_calloc */
  g_bsd_capsicum_calloc = mock_capsicum_calloc;
  g_mock_capsicum_oom = 1;
  ASSERT(cap_init() == NULL);
  ASSERT(cap_wrap(3, 0) == NULL);
  ASSERT(fileargs_init(0, NULL, 0, 0644, NULL, FA_OPEN) == NULL);
  g_mock_capsicum_oom = 0;
  bsd_capsicum_reset_calloc_hook();

  /* fileargs operations */
  fa = fileargs_initnv(NULL);
  ASSERT(fa != NULL);
  fileargs_free(fa);
  fileargs_free(NULL);

  /* Create temporary file for fileargs_open/fopen/lstat/realpath */
#if defined(_MSC_VER)
  if (fopen_s(&fp, "test_capsicum_tmp.txt", "wb+") != 0) {
    fp = NULL;
  }
#else
  fp = fopen("test_capsicum_tmp.txt", "wb+");
#endif
  ASSERT(fp != NULL);
  fclose(fp);

  fa = fileargs_init(0, NULL, O_RDONLY, 0644, NULL,
                     FA_OPEN | FA_LSTAT | FA_REALPATH);
  ASSERT(fa != NULL);
  ASSERT_EQ(0, fileargs_caph_enter(fa));

  ASSERT_EQ(-1, fileargs_open(fa, NULL));
  ASSERT_EQ(-1, fileargs_open(NULL, "nonexistent_capsicum_9999.txt"));
  fd = fileargs_open(fa, "test_capsicum_tmp.txt");
  ASSERT(fd >= 0);
#if defined(_WIN32)
  _close(fd);
#else
  close(fd);
#endif

  ASSERT(fileargs_fopen(fa, NULL, "rb") == NULL);
  ASSERT(fileargs_fopen(fa, "test_capsicum_tmp.txt", NULL) == NULL);
  ASSERT(fileargs_fopen(fa, "nonexistent_capsicum_9999.txt", "rb") == NULL);
  fp = fileargs_fopen(fa, "test_capsicum_tmp.txt", "rb");
  ASSERT(fp != NULL);
  fclose(fp);

  ASSERT_EQ(-1, fileargs_lstat(fa, NULL, &sb));
  ASSERT_EQ(-1, fileargs_lstat(fa, "test_capsicum_tmp.txt", NULL));
  ASSERT_EQ(0, fileargs_lstat(fa, "test_capsicum_tmp.txt", &sb));

  ASSERT(fileargs_realpath(fa, NULL, resolved) == NULL);
  ASSERT(fileargs_realpath(fa, "test_capsicum_tmp.txt", resolved) != NULL);

  fileargs_free(fa);
  remove("test_capsicum_tmp.txt");

  /* Process descriptors: real + mocked branches */
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  (void)0;
  {
    pid_t cpid;
    pid_t got_pid = 0;
    int wstatus = 0;
    size_t idx;

    ASSERT_EQ(-1, pdfork(NULL, 0));
    ASSERT_EQ(-1, pdgetpid(0, &got_pid));
    ASSERT_EQ(-1, pdgetpid(9999, NULL));
    ASSERT_EQ(-1, pdgetpid(9999, &got_pid));
    ASSERT_EQ(-1, pdkill(9999, 0));
    ASSERT_EQ(-1, pdwait4(0, &wstatus, 0, NULL));
    ASSERT_EQ(-1, pdwait4(9999, &wstatus, 0, NULL));
    ASSERT_EQ(-1, pdwait(9999, &wstatus, 0, NULL, NULL));

    cpid = pdfork(&fd, 0);
    if (cpid == 0) {
      _exit(0);
    }
    ASSERT(cpid > 0);
    ASSERT(fd > 0);
    ASSERT_EQ(0, pdgetpid(fd, &got_pid));
    ASSERT_EQ(cpid, got_pid);
    ASSERT_EQ(0, pdkill(fd, 0));
    ASSERT_EQ(0, pdwait(fd, &wstatus, WEXITED, NULL, NULL));
    close(fd);

    /* Mocked POSIX pdfork/pdgetpid/pdkill/pdwait4 edge cases */
    g_bsd_capsicum_pipe = mock_capsicum_pipe;
    g_bsd_capsicum_fork = mock_capsicum_fork;
    g_bsd_capsicum_waitpid = mock_capsicum_waitpid;
    g_bsd_capsicum_kill = mock_capsicum_kill;
    g_bsd_capsicum_close = mock_capsicum_close;

    g_mock_pipe_fail = 1;
    ASSERT_EQ(-1, pdfork(&fd, 0));
    g_mock_pipe_fail = 0;

    g_mock_fork_ret = -1;
    ASSERT_EQ(-1, pdfork(&fd, 0));

    g_mock_fork_ret = 0;
    ASSERT_EQ(0, pdfork(&fd, 0));

    /* Fill all 64 slots + 1 overflow call to test table-full loop completion */
    g_mock_fork_ret = 4321;
    g_mock_next_pipe_fd = 200;
    for (idx = 0; idx < 65; ++idx) {
      ASSERT_EQ(4321, pdfork(&fd, 0));
    }

    /* waitpid <= 0 (e.g. WNOHANG not ready) keeps table entry */
    g_mock_waitpid_ret = 0;
    ASSERT_EQ(-1, pdwait(200, &wstatus, 0, NULL, NULL));

    /* Reap all 64 slots */
    g_mock_waitpid_ret = 4321;
    for (idx = 0; idx < 64; ++idx) {
      ASSERT_EQ(4321, pdwait4(200 + (int)(idx * 2), &wstatus, WEXITED, NULL));
    }

    bsd_capsicum_reset_posix_hooks();
  }
#else
  {
    pid_t got_pid = 99;
    int wstatus = 0;
    ASSERT_EQ(-1, pdfork(NULL, 0));
    ASSERT_EQ(-1, pdfork(&fd, 0));
    ASSERT_EQ(-1, fd);
    ASSERT_EQ(-1, pdgetpid(0, NULL));
    ASSERT_EQ(-1, pdgetpid(0, &got_pid));
    ASSERT_EQ(0, got_pid);
    ASSERT_EQ(-1, pdkill(0, 0));
    ASSERT_EQ(-1, pdwait4(0, &wstatus, 0, NULL));
    ASSERT_EQ(-1, pdwait(0, &wstatus, 0, NULL, NULL));
  }
#endif

  PASS();
}

TEST test_bsd_casper_service_headers(void) {
  cap_channel_t *ch = cap_init();
  struct group grp;
  struct group *grp_res = NULL;
  struct passwd pwd;
  struct passwd *pwd_res = NULL;
  char buf[512];
  struct sockaddr sa;
  struct addrinfo *ai = NULL;
  cap_net_limit_t *nlim;
  cap_sysctl_limit_t *slim;
  int fam = AF_INET;
  int mib[4];
  size_t miblen = 4;
  int pagesz = 0;
  size_t len = sizeof(pagesz);
#if defined(_WIN32)
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

  memset(&sa, 0, sizeof(sa));
  sa.sa_family = AF_INET;

  /* 1. casper/cap_grp.h (all 11 functions) */
  cap_setgrent(ch);
  (void)cap_getgrent(ch);
  (void)cap_getgrnam(ch, "wheel");
  (void)cap_getgrgid(ch, (gid_t)0);
  (void)cap_getgrnam_r(ch, "wheel", &grp, buf, sizeof(buf), &grp_res);
  (void)cap_getgrgid_r(ch, (gid_t)0, &grp, buf, sizeof(buf), &grp_res);
  ASSERT_EQ(1, cap_setgroupent(ch, 1));
  cap_endgrent(ch);
  ASSERT_EQ(0, cap_grp_limit_cmds(ch, NULL, 0));
  ASSERT_EQ(0, cap_grp_limit_fields(ch, NULL, 0));
  ASSERT_EQ(0, cap_grp_limit_groups(ch, NULL, 0, NULL, 0));

  /* 2. casper/cap_pwd.h (all 11 functions) */
  cap_setpwent(ch);
  (void)cap_getpwent(ch);
  (void)cap_getpwnam(ch, "root");
  (void)cap_getpwuid(ch, (uid_t)0);
  (void)cap_getpwnam_r(ch, "root", &pwd, buf, sizeof(buf), &pwd_res);
  (void)cap_getpwuid_r(ch, (uid_t)0, &pwd, buf, sizeof(buf), &pwd_res);
  ASSERT_EQ(1, cap_setpassent(ch, 1));
  cap_endpwent(ch);
  ASSERT_EQ(0, cap_pwd_limit_cmds(ch, NULL, 0));
  ASSERT_EQ(0, cap_pwd_limit_fields(ch, NULL, 0));
  ASSERT_EQ(0, cap_pwd_limit_users(ch, NULL, 0, NULL, 0));

  /* 3. casper/cap_net.h (all 16 functions) */
  (void)cap_bind(ch, -1, &sa, (socklen_t)sizeof(sa));
  (void)cap_connect(ch, -1, &sa, (socklen_t)sizeof(sa));
  if (cap_getaddrinfo(ch, "127.0.0.1", NULL, NULL, &ai) == 0 && ai != NULL) {
    freeaddrinfo(ai);
  }
  (void)cap_getnameinfo(ch, &sa, (socklen_t)sizeof(sa), buf, 32, buf + 64, 32,
                        NI_NUMERICHOST | NI_NUMERICSERV);
  (void)cap_gethostbyname(ch, "127.0.0.1");
  (void)cap_gethostbyname2(ch, "127.0.0.1", AF_INET);
  {
    unsigned char loopback[4] = {127, 0, 0, 1};
    (void)cap_gethostbyaddr(ch, loopback, 4, AF_INET);
  }
  nlim = cap_net_limit_init(ch, CAPNET_NAME2ADDR);
  nlim = cap_net_limit_addr2name_family(nlim, &fam, 1);
  nlim = cap_net_limit_addr2name(nlim, &sa, (socklen_t)sizeof(sa));
  nlim = cap_net_limit_name2addr_family(nlim, &fam, 1);
  nlim = cap_net_limit_name2addr(nlim, "localhost", "80");
  nlim = cap_net_limit_connect(nlim, &sa, (socklen_t)sizeof(sa));
  nlim = cap_net_limit_bind(nlim, &sa, (socklen_t)sizeof(sa));
  ASSERT_EQ(0, cap_net_limit(nlim));
  cap_net_free(nlim);

  /* 4. casper/cap_netdb.h (1 function) */
  (void)cap_getprotobyname(ch, "tcp");

  /* 5. casper/cap_sysctl.h (all 7 functions) */
  (void)cap_sysctlnametomib(ch, "hw.pagesize", mib, &miblen);
  (void)cap_sysctlbyname(ch, "hw.pagesize", &pagesz, &len, NULL, 0);
  len = sizeof(pagesz);
  (void)cap_sysctl(ch, mib, 2, &pagesz, &len, NULL, 0);
  slim = cap_sysctl_limit_init(ch);
  slim = cap_sysctl_limit_name(slim, "hw.pagesize", CAP_SYSCTL_READ);
  slim = cap_sysctl_limit_mib(slim, mib, 2, CAP_SYSCTL_READ);
  ASSERT_EQ(0, cap_sysctl_limit(slim));

  /* 6. casper/cap_syslog.h (all 5 functions) */
  cap_openlog(ch, "test_capsicum", 0, LOG_USER);
  (void)cap_setlogmask(ch, LOG_UPTO(LOG_INFO));
  cap_syslog(ch, LOG_INFO, "hello %d", 1);
  call_cap_vsyslog_helper(ch, LOG_INFO, "vhello %d", 2);
  cap_closelog(ch);

#if defined(_WIN32)
  WSACleanup();
#endif
  cap_close(ch);
  PASS();
}

SUITE(suite_bsd_capsicum_core) {
  RUN_TEST(test_bsd_capsicum_rights_and_helpers);
  RUN_TEST(test_bsd_casper_fileargs_and_procdesc);
  RUN_TEST(test_bsd_casper_service_headers);
}
