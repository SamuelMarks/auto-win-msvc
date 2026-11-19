/* clang-format off */
#include "greatest.h"
#include "posix-sys-ioctl.h"
#include "sys/ioctl.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_MSC_VER) || defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER)
#include <share.h>
#endif
#else
#include <unistd.h>
#endif
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
extern void posix_sys_ioctl_set_hooks(
    int (*csbi_fn)(void *, void *), int (*peek_fn)(void *, unsigned long *),
    int (*fsize_fn)(void *, void *), int (*fseek_fn)(void *, void *),
    int (*pipe_state_fn)(void *, unsigned long *));
extern void posix_sys_ioctl_reset_hooks(void);

static int g_csbi_ret = 1;
static int g_peek_ret = 0;
static int g_fsize_ret = 1;
static int g_fseek_ret = 0;
static int g_pstate_ret = 0;

static int mock_csbi(void *h, void *info) {
  CONSOLE_SCREEN_BUFFER_INFO *csbi = (CONSOLE_SCREEN_BUFFER_INFO *)info;
  (void)h;
  memset(csbi, 0, sizeof(*csbi));
  csbi->srWindow.Top = 0;
  csbi->srWindow.Bottom = 24;
  csbi->srWindow.Left = 0;
  csbi->srWindow.Right = 79;
  return g_csbi_ret;
}

static int mock_peek(void *h, unsigned long *avail) {
  (void)h;
  *avail = 0;
  return g_peek_ret;
}

static int mock_fsize(void *h, void *sz) {
  LARGE_INTEGER *li = (LARGE_INTEGER *)sz;
  (void)h;
  li->QuadPart = 10;
  return g_fsize_ret;
}

static int mock_fseek(void *h, void *pos) {
  (void)h;
  (void)pos;
  return g_fseek_ret;
}

static int mock_pipe_state(void *h, unsigned long *mode) {
  (void)h;
  (void)mode;
  return g_pstate_ret;
}
#endif

TEST test_posix_sys_ioctl_get_info(void) {
  int available = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            posix_sys_ioctl_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_sys_ioctl_get_info(&available));
  ASSERT_EQ(1, available);
  PASS();
}

TEST test_posix_ioctl_operations(void) {
  int ret;
  struct winsize ws;
  unsigned long val = 0;

  /* Negative fd -> EBADF */
  errno = 0;
  ret = ioctl(-1, TIOCGWINSZ, &ws);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EBADF, errno);

  /* Null argp tests on valid fd 0 */
  errno = 0;
  ASSERT_EQ(-1, ioctl(0, TIOCGWINSZ, NULL));
  ASSERT_EQ(-1, ioctl(0, TIOCSWINSZ, NULL));
  ASSERT_EQ(-1, ioctl(0, FIONREAD, NULL));
  ASSERT_EQ(-1, ioctl(0, FIONBIO, NULL));
  (void)ioctl(9999, 0UL, NULL);

  memset(&ws, 0, sizeof(ws));
  (void)ioctl(0, TIOCGWINSZ, &ws);
  (void)ioctl(0, FIONREAD, &val);

#if defined(_MSC_VER) || defined(_WIN32)
  {
    WSADATA wsaData;
    SOCKET s;
    int pfd[2];
    int dfd;
    int nul_fd = -1;
    const char *tmp_file = "test_ioctl_disk_tmp.dat";

    /* Uninitialized Winsock path (WSANOTINITIALISED) before WSAStartup */
    (void)ioctl(3000, FIONREAD, &val);

    ASSERT_EQ(0, WSAStartup(MAKEWORD(2, 2), &wsaData));

    /* Socket paths: success, WSAEFAULT (NULL argp), WSAEINVAL (bad cmd) */
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ASSERT(s != INVALID_SOCKET);
    val = 1;
    ASSERT_EQ(0, ioctl((intptr_t)s, FIONBIO, &val));
    ASSERT_EQ(-1, ioctl((intptr_t)s, FIONBIO, NULL));
    ASSERT_EQ(EFAULT, errno);
    ASSERT_EQ(-1, ioctl((intptr_t)s, 0x12345678UL, &val));
    ASSERT_EQ(EINVAL, errno);
    closesocket(s);

    /* Out-of-range non-socket fd >= 2048 with SIOCATMARK -> WSAENOTSOCK ->
     * ENOTTY */
    ASSERT_EQ(-1, ioctl(3000, (unsigned long)SIOCATMARK, &val));
    ASSERT_EQ(ENOTTY, errno);

    /* Uninitialized Winsock + SIOCATMARK after temporary WSACleanup ->
     * WSANOTINITIALISED -> ENOTTY */
    WSACleanup();
    ASSERT_EQ(-1, ioctl(3000, (unsigned long)SIOCATMARK, &val));
    ASSERT_EQ(ENOTTY, errno);
    ASSERT_EQ(0, WSAStartup(MAKEWORD(2, 2), &wsaData));

    /* Real anonymous pipe tests: FIONREAD, FIONBIO (nowait + wait), TIOCSWINSZ
     */
    ASSERT_EQ(0, _pipe(pfd, 256, _O_BINARY));
    val = 99;
    ASSERT_EQ(0, ioctl(pfd[0], FIONREAD, &val));
    ASSERT_EQ(0UL, val);
    ASSERT_EQ(4, _write(pfd[1], "abcd", 4));
    ASSERT_EQ(0, ioctl(pfd[0], FIONREAD, &val));
    ASSERT_EQ(4UL, val);

    val = 1;
    ASSERT_EQ(0, ioctl(pfd[0], FIONBIO, &val));
    val = 0;
    ASSERT_EQ(0, ioctl(pfd[0], FIONBIO, &val));

    ws.ws_col = 80;
    ws.ws_row = 25;
    ASSERT_EQ(0, ioctl(pfd[0], TIOCSWINSZ, &ws));

    /* Real disk file tests: FIONREAD > 0, FIONREAD == 0 (at EOF), FIONBIO ->
     * ENOTTY */
#if defined(_MSC_VER)
    _sopen_s(&dfd, tmp_file, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY,
             _SH_DENYNO, _S_IREAD | _S_IWRITE);
#else
    dfd = _open(tmp_file, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY,
                _S_IREAD | _S_IWRITE);
#endif
    ASSERT(dfd >= 0);
    ASSERT_EQ(8, _write(dfd, "12345678", 8));
    _lseek(dfd, 0, SEEK_SET);
    val = 0;
    ASSERT_EQ(0, ioctl(dfd, FIONREAD, &val));
    ASSERT_EQ(8UL, val);

    _lseek(dfd, 0, SEEK_END);
    val = 99;
    ASSERT_EQ(0, ioctl(dfd, FIONREAD, &val));
    ASSERT_EQ(0UL, val);

    ASSERT_EQ(-1, ioctl(dfd, FIONBIO, &val));
    ASSERT_EQ(ENOTTY, errno);

    /* Character device ("NUL") for FIONREAD fallthrough (neither pipe nor disk)
     */
#if defined(_MSC_VER)
    _sopen_s(&nul_fd, "NUL", _O_RDWR, _SH_DENYNO, 0);
#else
    nul_fd = _open("NUL", _O_RDWR);
#endif
    if (nul_fd >= 0) {
      ASSERT_EQ(-1, ioctl(nul_fd, FIONREAD, &val));
      _close(nul_fd);
    }

    /* Hooked Win32 edge cases: TIOCGWINSZ success, PeekNamedPipe fail,
     * GetFileSizeEx fail, SetFilePointerEx fail, SetNamedPipeHandleState fail
     */
    posix_sys_ioctl_set_hooks(mock_csbi, mock_peek, mock_fsize, mock_fseek,
                              mock_pipe_state);

    g_csbi_ret = 1;
    memset(&ws, 0, sizeof(ws));
    ASSERT_EQ(0, ioctl(pfd[0], TIOCGWINSZ, &ws));
    ASSERT_EQ(25, ws.ws_row);
    ASSERT_EQ(80, ws.ws_col);

    g_peek_ret = 0;
    ASSERT_EQ(-1, ioctl(pfd[0], FIONREAD, &val));

    g_pstate_ret = 0;
    ASSERT_EQ(-1, ioctl(pfd[0], FIONBIO, &val));

    g_fsize_ret = 1;
    g_fseek_ret = 0;
    ASSERT_EQ(-1, ioctl(dfd, FIONREAD, &val));

    g_fsize_ret = 0;
    ASSERT_EQ(-1, ioctl(dfd, FIONREAD, &val));

    posix_sys_ioctl_reset_hooks();

    _close(dfd);
    _unlink(tmp_file);
    _close(pfd[0]);
    _close(pfd[1]);

    /* Also test WSANOTINITIALISED explicitly after WSACleanup */
    WSACleanup();
    (void)ioctl(-1, TIOCGWINSZ, &ws);
  }
#endif

  PASS();
}

TEST test_posix_ioctl_constants(void) {
  unsigned long v;

  v = (unsigned long)TIOCGWINSZ;
  ASSERT(v != 0);
  v = (unsigned long)TIOCSWINSZ;
  ASSERT(v != 0);
  v = (unsigned long)FIONREAD;
  ASSERT(v != 0);
  v = (unsigned long)FIONBIO;
  ASSERT(v != 0);

  PASS();
}

SUITE(suite_posix_sys_ioctl_core) {
  RUN_TEST(test_posix_sys_ioctl_get_info);
  RUN_TEST(test_posix_ioctl_operations);
  RUN_TEST(test_posix_ioctl_constants);
}
