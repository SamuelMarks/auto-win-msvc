/* clang-format off */
#include "greatest.h"
#include "poll.h"
#include "posix-poll.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#if defined(_WIN32) || defined(_MSC_VER)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#endif
/* clang-format on */

#if defined(_WIN32)
#ifdef POSIX_POLL_STANDALONE_TEST
intptr_t posix_get_dup_socket(intptr_t fd);
intptr_t posix_get_dup_socket(intptr_t fd) {
  (void)fd;
  return 0;
}
intptr_t posix_lookup_crt_socket(intptr_t fd);
intptr_t posix_lookup_crt_socket(intptr_t fd) {
  (void)fd;
  return 0;
}
#endif

extern void posix_poll_set_hooks(
    intptr_t (*raw_osf_fn)(int), intptr_t (*dup_sock_fn)(intptr_t),
    intptr_t (*crt_sock_fn)(intptr_t),
    int (*getsockopt_fn)(uintptr_t, char *, int *),
    unsigned long (*ftype_fn)(void *), int (*peek_fn)(void *, unsigned long *),
    int (*select_fn)(int, void *, void *, void *, void *));
extern void posix_poll_reset_hooks(void);

static intptr_t g_mock_osf = -1;
static intptr_t g_mock_dup = 0;
static intptr_t g_mock_crt = 0;
static int g_mock_is_sock = 0;
static unsigned long g_mock_ftype = 3UL;
static int g_mock_peek_ok = 1;
static unsigned long g_mock_peek_avail = 0UL;
static int g_mock_select_ret = 0;
static int g_mock_select_mode = 0;

static intptr_t mock_raw_osf(int fd) {
  if (fd == 63) {
    return (intptr_t)-1;
  }
  return g_mock_osf;
}

static intptr_t mock_dup_sock(intptr_t fd) {
  if (fd == 900) {
    return (intptr_t)-1; /* s == (SOCKET)-1 */
  }
  return g_mock_dup;
}

static intptr_t mock_crt_sock(intptr_t fd) {
  (void)fd;
  return g_mock_crt;
}

static int mock_getsockopt(uintptr_t s, char *optval, int *optlen) {
  (void)optval;
  (void)optlen;
  if (g_mock_is_sock == 2) {
    return (s >= 100U && s < 300U) ? 0 : -1;
  }
  return g_mock_is_sock ? 0 : -1;
}

static unsigned long mock_ftype(void *h) {
  (void)h;
  return g_mock_ftype;
}

static int mock_peek(void *h, unsigned long *avail) {
  (void)h;
  *avail = g_mock_peek_avail;
  return g_mock_peek_ok;
}

static int mock_select(int nfds, void *rfds_p, void *wfds_p, void *efds_p,
                       void *tv_p) {
  fd_set *rfds = (fd_set *)rfds_p;
  fd_set *wfds = (fd_set *)wfds_p;
  fd_set *efds = (fd_set *)efds_p;
  (void)nfds;
  (void)tv_p;
  if (g_mock_select_mode == 1) {
    /* Clear all sets so FD_ISSET false branches execute */
    rfds->fd_count = 0;
    wfds->fd_count = 0;
    efds->fd_count = 0;
  }
  return g_mock_select_ret;
}
#endif

TEST test_posix_poll_get_info(void) {
  int available = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_poll_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_poll_get_info(&available));
  ASSERT_EQ(1, available);
  PASS();
}

TEST test_posix_poll_args_and_real_io(void) {
  struct pollfd pfd[2];

  ASSERT_EQ(-1, poll(NULL, 1, 0));
  ASSERT_EQ(0, poll(NULL, 0, 0));
#if !defined(__EMSCRIPTEN__)
  ASSERT_EQ(0, poll(NULL, 0, 1));
#endif

  pfd[0].fd = -1;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
#if defined(__EMSCRIPTEN__)
  {
    int prc = poll(pfd, 1, 0);
    ASSERT(prc == 0 || prc == 1);
  }
#else
  ASSERT_EQ(0, poll(pfd, 1, 0));
  ASSERT_EQ(0, pfd[0].revents);
#endif

#if defined(_WIN32)
  {
    WSADATA wsaData;
    SOCKET s;
    int pipe_fds[2];

    ASSERT_EQ(0, WSAStartup(MAKEWORD(2, 2), &wsaData));

    /* Real pipe test so default_poll_raw_osfhandle, default_poll_file_type,
     * and default_poll_peek_pipe execute */
    ASSERT_EQ(0, _pipe(pipe_fds, 256, _O_BINARY));
    pfd[0].fd = pipe_fds[0];
    pfd[0].events = POLLIN;
    pfd[0].revents = 0;
    ASSERT_EQ(0, poll(pfd, 1, 1));
    _close(pipe_fds[0]);
    _close(pipe_fds[1]);

    /* Real UDP socket test so default_poll_getsockopt and default_poll_select
     * execute */
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ASSERT(s != INVALID_SOCKET);
    pfd[0].fd = (int)s;
    pfd[0].events = POLLOUT | POLLIN;
    pfd[0].revents = 0;
    ASSERT(poll(pfd, 1, 10) >= 0);
    closesocket(s);

    WSACleanup();
  }
#endif

  PASS();
}

#if defined(_WIN32)
TEST test_posix_poll_win32_branches(void) {
  struct pollfd fds[70];
  unsigned long i;

  posix_poll_set_hooks(mock_raw_osf, mock_dup_sock, mock_crt_sock,
                       mock_getsockopt, mock_ftype, mock_peek, mock_select);

  /* 1. safe_get_osfhandle: fd < 0 (via direct call? fd < 0 skipped before
   * safe_get_osfhandle unless we pass negative? Wait: in posix_poll, fd < 0
   * continues at line 209, so safe_get_osfhandle is only called with fd >= 0!
   * Let's check: fd >= 2048 hits fd >= 2048, and h == -2 hits h == -2 -> -1!)
   */
  g_mock_osf = -2;
  g_mock_dup = 0;
  g_mock_crt = 0;
  g_mock_is_sock = 0;
  g_mock_ftype = 2UL; /* FILE_TYPE_CHAR: neither pipe nor disk */
  fds[0].fd = 5;
  fds[0].events = POLLIN;
  ASSERT_EQ(0, poll(fds, 1, 0));

  /* 2. dup_sock != 0, crt_sock != 0, and s == (SOCKET)-1 (fd == 900) */
  g_mock_dup = 101;
  g_mock_is_sock = 1;
  g_mock_select_mode = 0;
  g_mock_select_ret = 1;
  fds[0].fd = 5;
  fds[0].events = POLLIN | POLLOUT;
  fds[1].fd = 5; /* duplicate socket in FD_SET */
  fds[1].events = 0;
  ASSERT_EQ(1, poll(fds, 2, 0));
  ASSERT_NEQ(0, fds[0].revents & POLLERR);

  /* select > 0 with FD_ISSET false + fd < 0 + s == (SOCKET)-1 in result loop */
  g_mock_dup = 0;
  g_mock_crt = 105;
  g_mock_select_mode = 1;
  g_mock_select_ret = 1;
  fds[0].fd = -1;
  fds[0].events = POLLIN;
  fds[1].fd = 900; /* dup_sock returns -1 -> s == (SOCKET)-1 */
  fds[1].events = POLLIN;
  fds[2].fd = 6;
  fds[2].events = POLLIN | POLLOUT;
  ASSERT_EQ(1, poll(fds, 3, -1)); /* timeout < 0 -> tvp = NULL */

  /* 3. Overflow FD_SETSIZE (64 unique sockets + 1 extra) and descending socket
   * values ((int)s > max_fd both true and false) */
  g_mock_crt = 0;
  g_mock_osf = -1;
  g_mock_is_sock = 1;
  g_mock_select_ret = 0;
  for (i = 0; i < 66; ++i) {
    fds[i].fd = (int)(200 - i);
    fds[i].events = POLLIN;
  }
  ASSERT_EQ(0, poll(fds, 66, 0));

  /* 4. Non-socket branches: fd >= 2048, POLLOUT, pipe EOF, pipe avail > 0,
   * pipe avail == 0, disk file */
  g_mock_is_sock = 0;
  g_mock_osf = 0x50;

  /* Pipe broken/EOF (PeekNamedPipe == 0) + POLLOUT */
  g_mock_ftype = 3UL;
  g_mock_peek_ok = 0;
  fds[0].fd = 10;
  fds[0].events = POLLIN | POLLOUT;
  ASSERT_EQ(1, poll(fds, 1, 0));
  ASSERT_EQ(POLLIN | POLLOUT, fds[0].revents);

  /* Pipe avail > 0 on fd >= 2048 */
  g_mock_peek_ok = 1;
  g_mock_peek_avail = 12UL;
  fds[0].fd = 2500;
  fds[0].events = POLLIN;
  ASSERT_EQ(1, poll(fds, 1, 0));
  ASSERT_EQ(POLLIN, fds[0].revents);

  /* Pipe avail == 0 -> not ready */
  g_mock_peek_avail = 0UL;
  fds[0].fd = 10;
  fds[0].events = POLLIN;
  ASSERT_EQ(0, poll(fds, 1, 0));

  /* Disk file -> POLLIN ready, plus mixed with socket:
   * select < 0 with num_other_ready > 0 returns num_other_ready;
   * select < 0 with num_other_ready == 0 returns -1 */
  g_mock_is_sock = 2; /* 100..299 are sockets, others are files */
  g_mock_ftype = 1UL; /* FILE_TYPE_DISK */
  g_mock_select_ret = -1;
  fds[0].fd = 10; /* disk file -> ready */
  fds[0].events = POLLIN;
  fds[1].fd = 150; /* socket */
  fds[1].events = POLLIN;
  ASSERT_EQ(1, poll(fds, 2, 0));

  /* Mixed with events == 0 on file so num_other_ready == 0 and select < 0 -> -1
   */
  fds[0].fd = 10;
  fds[0].events = 0;
  fds[1].fd = 150;
  fds[1].events = POLLIN;
  ASSERT_EQ(-1, poll(fds, 2, 0));

  posix_poll_reset_hooks();
  PASS();
}
#endif

TEST test_posix_poll_constants(void) {
  int mask = POLLIN | POLLPRI | POLLOUT | POLLERR | POLLHUP | POLLNVAL |
             POLLRDNORM | POLLRDBAND | POLLWRNORM | POLLWRBAND;
  ASSERT(mask != 0);
  PASS();
}

TEST test_posix_ppoll(void) {
  struct pollfd pfd;
  struct timespec ts;
  sigset_t mask;

  pfd.fd = -1;
  pfd.events = POLLIN;
  pfd.revents = 0;

  ts.tv_sec = 0;
#if defined(__EMSCRIPTEN__)
  ts.tv_nsec = 0;
#else
  ts.tv_nsec = 1000000;
#endif
  memset(&mask, 0, sizeof(mask));

#if defined(__EMSCRIPTEN__)
  {
    int prc = ppoll(&pfd, 1, &ts, NULL);
    ASSERT(prc == 0 || prc == 1);
    prc = ppoll(&pfd, 1, &ts, &mask);
    ASSERT(prc == 0 || prc == 1);
  }
#else
  ASSERT_EQ(0, ppoll(&pfd, 1, &ts, NULL));
  ASSERT_EQ(0, ppoll(&pfd, 1, &ts, &mask));
#endif
  ASSERT_EQ(-1, ppoll(NULL, 1, NULL, NULL));

  PASS();
}

SUITE(suite_posix_poll_core) {
  RUN_TEST(test_posix_poll_get_info);
  RUN_TEST(test_posix_poll_args_and_real_io);
#if defined(_WIN32)
  RUN_TEST(test_posix_poll_win32_branches);
#endif
  RUN_TEST(test_posix_poll_constants);
  RUN_TEST(test_posix_ppoll);
}
