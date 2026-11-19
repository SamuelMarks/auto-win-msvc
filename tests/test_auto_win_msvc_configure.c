#define AUTO_WIN_MSVC_NO_EVENT2 1
/* clang-format off */
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#else
#ifdef _WIN64
typedef unsigned __int64 uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif
#endif

#if defined(_MSC_VER) || defined(_WIN32)
static int mock_cfg_setvbuf(void *stream, char *buf, int mode, size_t size);
static int mock_cfg_wsastartup(unsigned short ver, void *data);
static uintptr_t mock_cfg_socket(int af, int type, int proto);
static int mock_cfg_bind(uintptr_t s, const void *addr, int len);
static int mock_cfg_getsockname(uintptr_t s, void *addr, int *len);
static int mock_cfg_listen(uintptr_t s, int backlog);
static int mock_cfg_connect(uintptr_t s, const void *addr, int len);
static uintptr_t mock_cfg_accept(uintptr_t s, void *addr, int *len);
static int mock_cfg_closesocket(uintptr_t s);
static int mock_cfg_ioctlsocket(uintptr_t s, long cmd, unsigned long *argp);
static void mock_cfg_sleep(unsigned long ms);
static int mock_cfg_dup2(int oldfd, int newfd);
static void *mock_cfg_openprocess(unsigned long access, int inherit, unsigned long pid);
static int mock_cfg_terminateprocess(void *h, unsigned int code);
static int mock_cfg_closehandle(void *h);
static unsigned long mock_cfg_waitforsingleobject(void *h, unsigned long ms);
static int mock_cfg_getexitcodeprocess(void *h, unsigned long *code);
static void *mock_cfg_malloc(size_t sz);
static void *mock_cfg_realloc(void *ptr, size_t sz);

#define AWM_CFG_SETVBUF(st, buf, mode, sz) mock_cfg_setvbuf((void *)(st), (char *)(buf), (int)(mode), (size_t)(sz))
#define AWM_CFG_WSASTARTUP(ver, data) mock_cfg_wsastartup((unsigned short)(ver), (void *)(data))
#define AWM_CFG_SOCKET(af, type, proto) ((SOCKET)mock_cfg_socket((int)(af), (int)(type), (int)(proto)))
#define AWM_CFG_BIND(s, addr, len) mock_cfg_bind((uintptr_t)(s), (const void *)(addr), (int)(len))
#define AWM_CFG_GETSOCKNAME(s, addr, len) mock_cfg_getsockname((uintptr_t)(s), (void *)(addr), (int *)(len))
#define AWM_CFG_LISTEN(s, bl) mock_cfg_listen((uintptr_t)(s), (int)(bl))
#define AWM_CFG_CONNECT(s, addr, len) mock_cfg_connect((uintptr_t)(s), (const void *)(addr), (int)(len))
#define AWM_CFG_ACCEPT(s, addr, len) ((SOCKET)mock_cfg_accept((uintptr_t)(s), (void *)(addr), (int *)(len)))
#define AWM_CFG_CLOSESOCKET(s) mock_cfg_closesocket((uintptr_t)(s))
#define AWM_CFG_IOCTLSOCKET(s, cmd, argp) mock_cfg_ioctlsocket((uintptr_t)(s), (long)(cmd), (unsigned long *)(argp))
#define AWM_CFG_SLEEP(ms) mock_cfg_sleep((unsigned long)(ms))
#define AWM_CFG_DUP2(oldfd, newfd) mock_cfg_dup2((int)(oldfd), (int)(newfd))
#define AWM_CFG_OPENPROCESS(acc, inh, pid) mock_cfg_openprocess((unsigned long)(acc), (int)(inh), (unsigned long)(pid))
#define AWM_CFG_TERMINATEPROCESS(h, code) mock_cfg_terminateprocess((void *)(h), (unsigned int)(code))
#define AWM_CFG_CLOSEHANDLE(h) mock_cfg_closehandle((void *)(h))
#define AWM_CFG_WAITFORSINGLEOBJECT(h, ms) mock_cfg_waitforsingleobject((void *)(h), (unsigned long)(ms))
#define AWM_CFG_GETEXITCODEPROCESS(h, code) mock_cfg_getexitcodeprocess((void *)(h), (unsigned long *)(code))
#define AWM_CFG_MALLOC(sz) mock_cfg_malloc((size_t)(sz))
#define AWM_CFG_REALLOC(ptr, sz) mock_cfg_realloc((void *)(ptr), (size_t)(sz))
#endif

#include "auto-win-msvc-configure.h"
#include "greatest.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
extern int awm_vfork_push(void **globals_slot);
extern void *awm_vfork_jb(void);
extern int awm_vfork_parent_pid(void);
extern void awm_vfork_exit(int status);

static int g_fail_setvbuf_call = 0;
static int g_setvbuf_calls = 0;
static int g_fail_wsastartup = 0;
static int g_fail_socket_call = 0;
static int g_socket_calls = 0;
static int g_fail_bind = 0;
static int g_fail_getsockname = 0;
static int g_fail_listen = 0;
static int g_fail_connect = 0;
static int g_fail_accept = 0;
static int g_fail_dup2 = 0;
static int g_openproc_ok = 0;
static int g_termproc_ok = 1;
static int g_closehandle_ok = 1;
static unsigned long g_wait_result = 0;
static int g_getexitcode_ok = 1;
static unsigned long g_exitcode_val = 0;
static int g_fail_malloc = 0;
static int g_fail_realloc = 0;
static pthread_once_t *g_spin_once_target = NULL;
static int g_once_counter = 0;

static void reset_cfg_mocks(void) {
  g_fail_setvbuf_call = 0;
  g_setvbuf_calls = 0;
  g_fail_wsastartup = 0;
  g_fail_socket_call = 0;
  g_socket_calls = 0;
  g_fail_bind = 0;
  g_fail_getsockname = 0;
  g_fail_listen = 0;
  g_fail_connect = 0;
  g_fail_accept = 0;
  g_fail_dup2 = 0;
  g_openproc_ok = 0;
  g_termproc_ok = 1;
  g_closehandle_ok = 1;
  g_wait_result = 0;
  g_getexitcode_ok = 1;
  g_exitcode_val = 0;
  g_fail_malloc = 0;
  g_fail_realloc = 0;
  g_spin_once_target = NULL;
}

static int mock_cfg_setvbuf(void *stream, char *buf, int mode, size_t size) {
  g_setvbuf_calls++;
  if (g_fail_setvbuf_call > 0 && g_setvbuf_calls == g_fail_setvbuf_call) {
    return -1;
  }
  return setvbuf((FILE *)stream, buf, mode, size);
}

static int mock_cfg_wsastartup(unsigned short ver, void *data) {
  if (g_fail_wsastartup) {
    return -1;
  }
  return WSAStartup(ver, (LPWSADATA)data);
}

static uintptr_t mock_cfg_socket(int af, int type, int proto) {
  g_socket_calls++;
  if (g_fail_socket_call > 0 && g_socket_calls == g_fail_socket_call) {
    return (uintptr_t)INVALID_SOCKET;
  }
  return (uintptr_t)WSASocketW(af, type, proto, NULL, 0, 0);
}

static int mock_cfg_bind(uintptr_t s, const void *addr, int len) {
  if (g_fail_bind) {
    return SOCKET_ERROR;
  }
  return posix_bind((intptr_t)s, (const struct sockaddr *)addr, len);
}

static int mock_cfg_getsockname(uintptr_t s, void *addr, int *len) {
  if (g_fail_getsockname) {
    return SOCKET_ERROR;
  }
  return posix_getsockname((intptr_t)s, (struct sockaddr *)addr, len);
}

static int mock_cfg_listen(uintptr_t s, int backlog) {
  if (g_fail_listen) {
    return SOCKET_ERROR;
  }
  return posix_listen((intptr_t)s, backlog);
}

static int mock_cfg_connect(uintptr_t s, const void *addr, int len) {
  if (g_fail_connect) {
    return SOCKET_ERROR;
  }
  return posix_connect((intptr_t)s, (const struct sockaddr *)addr, len);
}

static uintptr_t mock_cfg_accept(uintptr_t s, void *addr, int *len) {
  if (g_fail_accept) {
    return (uintptr_t)INVALID_SOCKET;
  }
  return (uintptr_t)posix_accept((intptr_t)s, (struct sockaddr *)addr, len);
}

static int mock_cfg_closesocket(uintptr_t s) { return closesocket((SOCKET)s); }

static int mock_cfg_ioctlsocket(uintptr_t s, long cmd, unsigned long *argp) {
  return ioctlsocket((SOCKET)s, cmd, argp);
}

static void mock_cfg_sleep(unsigned long ms) {
  (void)ms;
  if (g_spin_once_target != NULL) {
    g_spin_once_target->p = (void *)2;
  }
}

static int mock_cfg_dup2(int oldfd, int newfd) {
  if (g_fail_dup2) {
    return -1;
  }
  return _dup2(oldfd, newfd);
}

static void *mock_cfg_openprocess(unsigned long access, int inherit,
                                  unsigned long pid) {
  (void)access;
  (void)inherit;
  (void)pid;
  if (!g_openproc_ok) {
    return NULL;
  }
  return (void *)(uintptr_t)0x1234U;
}

static int mock_cfg_terminateprocess(void *h, unsigned int code) {
  (void)h;
  (void)code;
  return g_termproc_ok;
}

static int mock_cfg_closehandle(void *h) {
  (void)h;
  return g_closehandle_ok;
}

static unsigned long mock_cfg_waitforsingleobject(void *h, unsigned long ms) {
  (void)h;
  (void)ms;
  return g_wait_result;
}

static int mock_cfg_getexitcodeprocess(void *h, unsigned long *code) {
  (void)h;
  if (!g_getexitcode_ok) {
    return 0;
  }
  *code = g_exitcode_val;
  return 1;
}

static void *mock_cfg_malloc(size_t sz) {
  if (g_fail_malloc) {
    return NULL;
  }
  return malloc(sz);
}

static void *mock_cfg_realloc(void *ptr, size_t sz) {
  if (g_fail_realloc) {
    return NULL;
  }
  return realloc(ptr, sz);
}

static void sample_once_init(void) { g_once_counter++; }

TEST test_cfg_wsastartup_and_pipe(void) {
  int pfd[2];
  char rbuf[16];
  struct iovec iov;
  struct msghdr msg;
  char msg_payload[6];

  /* 1. Failure paths before initialized becomes 1 */
  reset_cfg_mocks();
  g_fail_setvbuf_call = 1;
  ASSERT_EQ(-1, auto_win_msvc_ensure_wsastartup());

  reset_cfg_mocks();
  g_fail_setvbuf_call = 2;
  ASSERT_EQ(-1, auto_win_msvc_ensure_wsastartup());

  reset_cfg_mocks();
  g_fail_wsastartup = 1;
  ASSERT_EQ(-1, auto_win_msvc_ensure_wsastartup());
  ASSERT_EQ(-1, auto_win_msvc_native_pipe(pfd));

  /* 2. Succeed once, then hit already-initialized branch */
  reset_cfg_mocks();
  ASSERT_EQ(0, auto_win_msvc_ensure_wsastartup());
  ASSERT_EQ(0, auto_win_msvc_ensure_wsastartup());

  /* 3. Every error path in auto_win_msvc_native_pipe */
  reset_cfg_mocks();
  g_fail_socket_call = 1;
  ASSERT_EQ(-1, auto_win_msvc_native_pipe(pfd));

  reset_cfg_mocks();
  g_fail_bind = 1;
  ASSERT_EQ(-1, auto_win_msvc_native_pipe(pfd));

  reset_cfg_mocks();
  g_fail_getsockname = 1;
  ASSERT_EQ(-1, auto_win_msvc_native_pipe(pfd));

  reset_cfg_mocks();
  g_fail_listen = 1;
  ASSERT_EQ(-1, auto_win_msvc_native_pipe(pfd));

  reset_cfg_mocks();
  g_fail_socket_call = 2;
  ASSERT_EQ(-1, auto_win_msvc_native_pipe(pfd));

  reset_cfg_mocks();
  g_fail_connect = 1;
  ASSERT_EQ(-1, auto_win_msvc_native_pipe(pfd));

  reset_cfg_mocks();
  g_fail_accept = 1;
  ASSERT_EQ(-1, auto_win_msvc_native_pipe(pfd));

  /* 4. Real native loopback pipe + fcntl + write + sendmsg + read + close */
  reset_cfg_mocks();
  ASSERT_EQ(0, auto_win_msvc_native_pipe(pfd));

  ASSERT_EQ(0, auto_win_msvc_fcntl((intptr_t)pfd[0], F_GETFD));
  ASSERT_EQ(0, auto_win_msvc_fcntl((intptr_t)pfd[0], F_SETFD, 1));
  ASSERT_EQ(0, auto_win_msvc_fcntl((intptr_t)pfd[0], F_GETFL));
  ASSERT_EQ(0, auto_win_msvc_fcntl((intptr_t)pfd[0], F_SETFL, O_NONBLOCK));
  ASSERT_EQ(0, auto_win_msvc_fcntl((intptr_t)pfd[0], F_SETFL, 0));
  ASSERT_EQ(-1, auto_win_msvc_fcntl((intptr_t)INVALID_SOCKET, F_SETFL, 0));
  ASSERT_EQ(0, auto_win_msvc_fcntl((intptr_t)pfd[0], 999));

  ASSERT_EQ(4, (int)auto_win_msvc_write(pfd[1], "ping", 4));
  memset(rbuf, 0, sizeof(rbuf));
  ASSERT_EQ(4, (int)auto_win_msvc_read(pfd[0], rbuf, 4));
  ASSERT_STR_EQ("ping", rbuf);

  memcpy(msg_payload, "world", 6);
  iov.iov_base = msg_payload;
  iov.iov_len = 5;
  memset(&msg, 0, sizeof(msg));
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  ASSERT_EQ(5, (int)auto_win_msvc_sendmsg(pfd[1], &msg, 0));
  memset(rbuf, 0, sizeof(rbuf));
  ASSERT_EQ(5, (int)auto_win_msvc_read(pfd[0], rbuf, 5));
  ASSERT_STR_EQ("world", rbuf);

  ASSERT_EQ(0, auto_win_msvc_close(pfd[0]));
  ASSERT_EQ(0, auto_win_msvc_close(pfd[1]));
  PASS();
}

TEST test_cfg_pthread_once_and_dup2(void) {
  pthread_once_t once_ctl = PTHREAD_ONCE_INIT;
  FILE *tmpf;
  int dup_fd;

  ASSERT_EQ(-1, auto_win_msvc_pthread_once(NULL, sample_once_init));
  ASSERT_EQ(-1, auto_win_msvc_pthread_once(&once_ctl, NULL));

  g_once_counter = 0;
  ASSERT_EQ(0, auto_win_msvc_pthread_once(&once_ctl, sample_once_init));
  ASSERT_EQ(1, g_once_counter);

  /* Already initialized (once_ctl.p == 2) */
  ASSERT_EQ(0, auto_win_msvc_pthread_once(&once_ctl, sample_once_init));
  ASSERT_EQ(1, g_once_counter);

  /* Contended spin path (once_ctl.p == 1 -> mock_cfg_sleep transitions to 2) */
  once_ctl.p = (void *)1;
  g_spin_once_target = &once_ctl;
  ASSERT_EQ(0, auto_win_msvc_pthread_once(&once_ctl, sample_once_init));
  g_spin_once_target = NULL;
  ASSERT_EQ(1, g_once_counter);

  /* auto_win_msvc_dup2 failure and success */
  reset_cfg_mocks();
  g_fail_dup2 = 1;
  ASSERT_EQ(-1, auto_win_msvc_dup2(0, 10));

  reset_cfg_mocks();
#if defined(_MSC_VER) && _MSC_VER >= 1400
  tmpf = NULL;
  (void)tmpfile_s(&tmpf);
#else
  tmpf = tmpfile();
#endif
  if (tmpf != NULL) {
    int orig_fd = _fileno(tmpf);
    dup_fd = _dup(orig_fd);
    if (dup_fd >= 0) {
      ASSERT_EQ(dup_fd, auto_win_msvc_dup2(orig_fd, dup_fd));
      _close(dup_fd);
    }
    fclose(tmpf);
  }
  PASS();
}

TEST test_cfg_kill_and_waitpid(void) {
  pid_t self_pid = (pid_t)GetCurrentProcessId();
  int status = 0;

  /* auto_win_msvc_kill branches */
  reset_cfg_mocks();
  ASSERT_EQ(0, auto_win_msvc_kill(self_pid, SIGHUP));
  ASSERT_EQ(0, auto_win_msvc_kill(self_pid, 0));
  ASSERT_EQ(0, auto_win_msvc_kill(self_pid + 99999, 9));
  ASSERT_EQ(0, auto_win_msvc_kill(self_pid, SIGUSR1));

  /* sig == 9 on self_pid: OpenProcess NULL */
  reset_cfg_mocks();
  g_openproc_ok = 0;
  ASSERT_EQ(0, auto_win_msvc_kill(self_pid, 9));

  /* TerminateProcess fails */
  reset_cfg_mocks();
  g_openproc_ok = 1;
  g_termproc_ok = 0;
  ASSERT_EQ(-1, auto_win_msvc_kill(self_pid, 9));

  /* CloseHandle fails */
  reset_cfg_mocks();
  g_openproc_ok = 1;
  g_termproc_ok = 1;
  g_closehandle_ok = 0;
  ASSERT_EQ(-1, auto_win_msvc_kill(self_pid, 9));

  /* Full TerminateProcess + CloseHandle mocked success */
  reset_cfg_mocks();
  g_openproc_ok = 1;
  g_termproc_ok = 1;
  g_closehandle_ok = 1;
  ASSERT_EQ(0, auto_win_msvc_kill(self_pid, 9));

  /* auto_win_msvc_waitpid: vfork reap > 0 path */
  if (awm_vfork_push(NULL) == 0) {
    pid_t cpid;
    int vrc = setjmp(*(jmp_buf *)awm_vfork_jb());
    if (vrc == 0) {
      awm_vfork_exit(7);
    }
    cpid = (pid_t)awm_vfork_parent_pid();
    status = 0;
    ASSERT_EQ(cpid, auto_win_msvc_waitpid(cpid, &status, 0));
    ASSERT_EQ((7 << 8), status);
  }

  /* pid <= 0 when nothing in vfork reap table */
  ASSERT_EQ((pid_t)-1, auto_win_msvc_waitpid(0, &status, 0));
  ASSERT_EQ(ECHILD, errno);

  /* OpenProcess NULL */
  reset_cfg_mocks();
  g_openproc_ok = 0;
  ASSERT_EQ((pid_t)-1, auto_win_msvc_waitpid(12345, &status, 0));
  ASSERT_EQ(ECHILD, errno);

  /* WaitForSingleObject == WAIT_FAILED */
  reset_cfg_mocks();
  g_openproc_ok = 1;
  g_wait_result = WAIT_FAILED;
  ASSERT_EQ((pid_t)-1, auto_win_msvc_waitpid(12345, &status, 0));

  /* GetExitCodeProcess fails */
  reset_cfg_mocks();
  g_openproc_ok = 1;
  g_wait_result = 0;
  g_getexitcode_ok = 0;
  ASSERT_EQ((pid_t)-1, auto_win_msvc_waitpid(12345, &status, 0));

  /* status == NULL + CloseHandle fails */
  reset_cfg_mocks();
  g_openproc_ok = 1;
  g_wait_result = 0;
  g_closehandle_ok = 0;
  ASSERT_EQ((pid_t)-1, auto_win_msvc_waitpid(12345, NULL, 0));

  /* status == NULL + CloseHandle succeeds */
  reset_cfg_mocks();
  g_openproc_ok = 1;
  g_wait_result = 0;
  g_closehandle_ok = 1;
  ASSERT_EQ((pid_t)12345, auto_win_msvc_waitpid(12345, NULL, 0));

  /* status != NULL + GetExitCodeProcess succeeds + CloseHandle succeeds */
  reset_cfg_mocks();
  g_openproc_ok = 1;
  g_wait_result = 0;
  g_getexitcode_ok = 1;
  g_exitcode_val = 42;
  g_closehandle_ok = 1;
  status = 0;
  ASSERT_EQ((pid_t)12345, auto_win_msvc_waitpid(12345, &status, 0));
  ASSERT_EQ((42 << 8), status);
  PASS();
}

TEST test_cfg_getsubopt_and_getline(void) {
  static char tok_ro[] = "ro";
  static char tok_rw[] = "rw";
  static char tok_rsize[] = "rsize";
  char *tokens[4];
  char buf[64];
  char *optp;
  char *valp = NULL;
  FILE *fp;
  char *line = NULL;
  size_t cap = 0;

  tokens[0] = tok_ro;
  tokens[1] = tok_rw;
  tokens[2] = tok_rsize;
  tokens[3] = NULL;

  /* NULL optionp and *optionp */
  ASSERT_EQ(-1, getsubopt(NULL, tokens, &valp));
  optp = NULL;
  ASSERT_EQ(-1, getsubopt(&optp, tokens, &valp));

  /* Comma, key=val,comma, key=val end, unknown bare, unknown=val */
  memcpy(buf, "ro,rsize=4096,unknown,bad=99,rw=1", 34);
  optp = buf;
  ASSERT_EQ(0, getsubopt(&optp, tokens, &valp));
  ASSERT(valp == NULL);

  ASSERT_EQ(2, getsubopt(&optp, tokens, &valp));
  ASSERT_STR_EQ("4096", valp);

  ASSERT_EQ(-1, getsubopt(&optp, tokens, &valp));
  ASSERT_STR_EQ("unknown", valp);

  ASSERT_EQ(-1, getsubopt(&optp, tokens, &valp));
  ASSERT_STR_EQ("99", valp);

  ASSERT_EQ(1, getsubopt(&optp, tokens, &valp));
  ASSERT_STR_EQ("1", valp);

  /* Single bare token ending at NUL */
  memcpy(buf, "ro", 3);
  optp = buf;
  ASSERT_EQ(0, getsubopt(&optp, tokens, &valp));

  /* getline invalid arguments */
#if defined(_MSC_VER) && _MSC_VER >= 1400
  fp = NULL;
  ASSERT_EQ(0, tmpfile_s(&fp));
#else
  fp = tmpfile();
#endif
  ASSERT(fp != NULL);
  ASSERT_EQ(-1, (int)getline(NULL, &cap, fp));
  ASSERT_EQ(-1, (int)getline(&line, NULL, fp));
  ASSERT_EQ(-1, (int)getline(&line, &cap, NULL));

  /* Empty stream EOF */
  reset_cfg_mocks();
  ASSERT_EQ(-1, (int)getline(&line, &cap, fp));
  free(line);
  line = NULL;
  cap = 0;

  /* Initial malloc OOM */
  g_fail_malloc = 1;
  ASSERT_EQ(-1, (int)getline(&line, &cap, fp));
  ASSERT_EQ(ENOMEM, errno);
  reset_cfg_mocks();

  /* Non-NULL line with *n == 0 triggers initial 128-byte allocation */
  {
    char short_line[7];
    char dummy;
    char *p = &dummy;
    size_t zero_cap = 0;
    memcpy(short_line, "short", 5);
    short_line[5] = (char)10;
    short_line[6] = '\0';
    fputs(short_line, fp);
    rewind(fp);
    ASSERT_EQ(6, (int)getline(&p, &zero_cap, fp));
    ASSERT_STR_EQ(short_line, p);
    free(p);
  }

  /* Small buffer forcing realloc success AND EOF without newline */
  fclose(fp);
#if defined(_MSC_VER) && _MSC_VER >= 1400
  fp = NULL;
  ASSERT_EQ(0, tmpfile_s(&fp));
#else
  fp = tmpfile();
#endif
  ASSERT(fp != NULL);
  fputs("0123456789abcdef", fp);
  rewind(fp);
  cap = 4;
  line = (char *)malloc(cap);
  ASSERT(line != NULL);
  ASSERT_EQ(16, (int)getline(&line, &cap, fp));
  ASSERT_STR_EQ("0123456789abcdef", line);
  ASSERT(cap > 16);

  /* Small buffer forcing realloc OOM */
  rewind(fp);
  free(line);
  cap = 4;
  line = (char *)malloc(cap);
  ASSERT(line != NULL);
  g_fail_realloc = 1;
  ASSERT_EQ(-1, (int)getline(&line, &cap, fp));
  ASSERT_EQ(ENOMEM, errno);
  reset_cfg_mocks();
  free(line);
  fclose(fp);
  PASS();
}
#endif /* _MSC_VER || _WIN32 */

TEST test_cfg_strrchr_slash(void) {
  char p1[6];
  char p2[6];
  char p3[4];
  char p4[4];
  const char *p5 = "abc";
  char exp_bs[3];

  /* Build paths using (char)92 for backslash */
  p1[0] = 'a';
  p1[1] = '/';
  p1[2] = 'b';
  p1[3] = (char)92;
  p1[4] = 'c';
  p1[5] = '\0';

  p2[0] = 'a';
  p2[1] = (char)92;
  p2[2] = 'b';
  p2[3] = '/';
  p2[4] = 'c';
  p2[5] = '\0';

  p3[0] = 'a';
  p3[1] = '/';
  p3[2] = 'b';
  p3[3] = '\0';

  p4[0] = 'a';
  p4[1] = (char)92;
  p4[2] = 'b';
  p4[3] = '\0';

  exp_bs[0] = (char)92;
  exp_bs[1] = 'c';
  exp_bs[2] = '\0';
  ASSERT_STR_EQ(exp_bs, auto_win_msvc_strrchr_slash(p1));
  ASSERT_STR_EQ("/c", auto_win_msvc_strrchr_slash(p2));
  ASSERT_STR_EQ("/b", auto_win_msvc_strrchr_slash(p3));
  exp_bs[1] = 'b';
  ASSERT_STR_EQ(exp_bs, auto_win_msvc_strrchr_slash(p4));
  ASSERT(auto_win_msvc_strrchr_slash(p5) == NULL);
  PASS();
}

SUITE(suite_auto_win_msvc_configure) {
#if defined(_MSC_VER) || defined(_WIN32)
  RUN_TEST(test_cfg_wsastartup_and_pipe);
  RUN_TEST(test_cfg_pthread_once_and_dup2);
  RUN_TEST(test_cfg_kill_and_waitpid);
  RUN_TEST(test_cfg_getsubopt_and_getline);
#endif
  RUN_TEST(test_cfg_strrchr_slash);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_auto_win_msvc_configure);
  GREATEST_MAIN_END();
}
