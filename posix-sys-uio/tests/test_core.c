/* clang-format off */
#include "greatest.h"
#include "posix-sys-uio.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <share.h>
#include <winsock2.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

extern int dummy_posix_sys_uio(void);

#if defined(_MSC_VER) || defined(_WIN32)
typedef int(WSAAPI *test_uio_wsasend_fn)(SOCKET, LPWSABUF, DWORD, LPDWORD,
                                         DWORD, LPWSAOVERLAPPED,
                                         LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int(WSAAPI *test_uio_wsarecv_fn)(SOCKET, LPWSABUF, DWORD, LPDWORD,
                                         LPDWORD, LPWSAOVERLAPPED,
                                         LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int(WSAAPI *test_uio_wsagetlasterror_fn)(void);
typedef int (*test_uio_write_fn)(int, const void *, unsigned int);
typedef int (*test_uio_read_fn)(int, void *, unsigned int);
typedef __int64 (*test_uio_lseeki64_fn)(int, __int64, int);

extern void posix_sys_uio_set_alloc_hook(void *(*malloc_fn)(size_t));
extern void posix_sys_uio_set_win32_hooks(test_uio_wsasend_fn send_fn,
                                          test_uio_wsarecv_fn recv_fn,
                                          test_uio_wsagetlasterror_fn err_fn,
                                          test_uio_write_fn write_fn,
                                          test_uio_read_fn read_fn,
                                          test_uio_lseeki64_fn seek_fn);
extern void posix_sys_uio_reset_hooks(void);

static int g_uio_oom = 0;
static int g_mock_wsa_err = WSAENOTSOCK;
static int g_mock_io_mode = 0;
static int g_mock_io_calls = 0;
static __int64 g_mock_seek_ret = 0;

static void *mock_uio_malloc(size_t sz) {
  if (g_uio_oom) {
    return NULL;
  }
  return malloc(sz);
}

static int WSAAPI mock_wsasend_fail(SOCKET s, LPWSABUF bufs, DWORD cnt,
                                    LPDWORD sent, DWORD flags,
                                    LPWSAOVERLAPPED ovl,
                                    LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  (void)s;
  (void)bufs;
  (void)cnt;
  (void)sent;
  (void)flags;
  (void)ovl;
  (void)comp;
  return SOCKET_ERROR;
}

static int WSAAPI mock_wsarecv_fail(SOCKET s, LPWSABUF bufs, DWORD cnt,
                                    LPDWORD recvd, LPDWORD flags,
                                    LPWSAOVERLAPPED ovl,
                                    LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  (void)s;
  (void)bufs;
  (void)cnt;
  (void)recvd;
  (void)flags;
  (void)ovl;
  (void)comp;
  return SOCKET_ERROR;
}

static int WSAAPI mock_wsagetlasterror(void) { return g_mock_wsa_err; }

static int mock_uio_write(int fd, const void *buf, unsigned int cnt) {
  (void)fd;
  (void)buf;
  g_mock_io_calls++;
  if (g_mock_io_mode == 1) {
    errno = EIO;
    return -1;
  }
  if (g_mock_io_mode == 2) {
    if (g_mock_io_calls == 1) {
      return (int)cnt;
    }
    errno = EIO;
    return -1;
  }
  if (g_mock_io_mode == 3) {
    return 2; /* Partial write (< cnt) */
  }
  return (int)cnt;
}

static int mock_uio_read(int fd, void *buf, unsigned int cnt) {
  (void)fd;
  g_mock_io_calls++;
  if (g_mock_io_mode == 1) {
    errno = EIO;
    return -1;
  }
  if (g_mock_io_mode == 2) {
    if (g_mock_io_calls == 1) {
      memset(buf, 'A', cnt);
      return (int)cnt;
    }
    errno = EIO;
    return -1;
  }
  if (g_mock_io_mode == 3) {
    memset(buf, 'B', 2);
    return 2; /* Partial read (< cnt) */
  }
  memset(buf, 'C', cnt);
  return (int)cnt;
}

static __int64 mock_uio_lseeki64(int fd, __int64 off, int whence) {
  (void)fd;
  (void)off;
  (void)whence;
  if (g_mock_seek_ret < 0) {
    errno = ESPIPE;
  }
  return g_mock_seek_ret;
}
#endif

TEST test_posix_sys_uio_get_info(void) {
  auto_win_msvc_error_t rc;
  int info = 0;

  ASSERT_EQ(0, dummy_posix_sys_uio());
  rc = posix_sys_uio_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_sys_uio_get_info(&info);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_uio_readv_writev_file(void) {
  int fd;
  long res;
  struct iovec iov[2];
  char buf1[8];
  char buf2[8];
  char rbuf1[8];
  char rbuf2[8];
  const char *test_filename = "test_uio_tmp.bin";

#if defined(_MSC_VER)
  if (_sopen_s(&fd, test_filename, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY,
               _SH_DENYNO, _S_IREAD | _S_IWRITE) != 0) {
    fd = -1;
  }
#elif defined(_WIN32)
  fd = _open(test_filename, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, 0666);
#else
  fd = open(test_filename, O_CREAT | O_TRUNC | O_RDWR, 0666);
#endif
  ASSERT(fd >= 0);

  /* Invalid fd and iovcnt */
  ASSERT_EQ(-1, posix_writev(-1, iov, 1));
  ASSERT_EQ(-1, posix_readv(-1, iov, 1));
  ASSERT_EQ(-1, posix_writev(fd, iov, -1));
  ASSERT_EQ(-1, posix_writev(fd, iov, 1025));
  ASSERT_EQ(-1, posix_writev(fd, NULL, 1));
  ASSERT_EQ(-1, posix_readv(fd, iov, -1));
  ASSERT_EQ(-1, posix_readv(fd, iov, 1025));
  ASSERT_EQ(-1, posix_readv(fd, NULL, 1));
#if defined(_MSC_VER) || defined(_WIN32)
  ASSERT_EQ(-1, posix_writev(fd, iov, 0));
  ASSERT_EQ(-1, posix_readv(fd, iov, 0));
#endif

  /* Valid writev */
  memcpy(buf1, "HELLO_", 6);
  memcpy(buf2, "WORLD!", 6);
  iov[0].iov_base = buf1;
  iov[0].iov_len = 6;
  iov[1].iov_base = buf2;
  iov[1].iov_len = 6;

  res = posix_writev(fd, iov, 2);
  ASSERT_EQ(12, res);

#if defined(_MSC_VER) || defined(_WIN32)
  _lseek(fd, 0, 0);
#else
  lseek(fd, 0, SEEK_SET);
#endif

  /* Valid readv */
  memset(rbuf1, 0, sizeof(rbuf1));
  memset(rbuf2, 0, sizeof(rbuf2));
  iov[0].iov_base = rbuf1;
  iov[0].iov_len = 6;
  iov[1].iov_base = rbuf2;
  iov[1].iov_len = 6;

  res = posix_readv(fd, iov, 2);
  ASSERT_EQ(12, res);
  ASSERT_MEM_EQ("HELLO_", rbuf1, 6);
  ASSERT_MEM_EQ("WORLD!", rbuf2, 6);

  /* preadv and pwritev */
  memcpy(buf1, "1234", 4);
  iov[0].iov_base = buf1;
  iov[0].iov_len = 4;
  res = posix_pwritev(fd, iov, 1, 0);
  ASSERT_EQ(4, res);

  memset(rbuf1, 0, sizeof(rbuf1));
  iov[0].iov_base = rbuf1;
  iov[0].iov_len = 4;
  res = posix_preadv(fd, iov, 1, 0);
  ASSERT_EQ(4, res);
  ASSERT_MEM_EQ("1234", rbuf1, 4);

  /* Invalid fd and negative offset */
  ASSERT_EQ(-1, posix_preadv(-1, iov, 1, 0));
  ASSERT_EQ(-1, posix_pwritev(-1, iov, 1, 0));
  ASSERT_EQ(-1, posix_preadv(fd, iov, 1, -1));
  ASSERT_EQ(-1, posix_pwritev(fd, iov, 1, -1));

#if defined(_MSC_VER) || defined(_WIN32)
  _close(fd);
  _unlink(test_filename);
#else
  close(fd);
  unlink(test_filename);

#if !defined(__WATCOMC__) && !defined(__MSDOS__) && !defined(__DOS__)
  /* Non-seekable pipe fallback for posix_preadv and posix_pwritev */
  {
    int pfd[2];
    ASSERT_EQ(0, pipe(pfd));
    memcpy(buf1, "PIPE", 4);
    iov[0].iov_base = buf1;
    iov[0].iov_len = 4;
    ASSERT_EQ(4, posix_pwritev(pfd[1], iov, 1, 0));
    memset(rbuf1, 0, sizeof(rbuf1));
    iov[0].iov_base = rbuf1;
    iov[0].iov_len = 4;
    ASSERT_EQ(4, posix_preadv(pfd[0], iov, 1, 0));
    ASSERT_MEM_EQ("PIPE", rbuf1, 4);
    close(pfd[0]);
    close(pfd[1]);
  }
#endif
#endif

  PASS();
}

#if defined(_MSC_VER) || defined(_WIN32)
TEST test_posix_uio_win32_sockets_and_oom(void) {
  struct iovec iov[2];
  char b1[4] = {'A', 'B', 'C', 'D'};
  char b2[4] = {'E', 'F', 'G', 'H'};
  char r1[4];
  char r2[4];
  WSADATA wsa;
  SOCKET listener;
  SOCKET s_client;
  SOCKET s_server;
  struct sockaddr_in addr;
  int addrlen = (int)sizeof(addr);
  static const int wsa_errs[] = {
      WSAEWOULDBLOCK,  WSA_IO_PENDING,  WSAECONNRESET, WSAEINPROGRESS,
      WSAEALREADY,     WSAEMSGSIZE,     WSAEADDRINUSE, WSAEADDRNOTAVAIL,
      WSAECONNABORTED, WSAECONNREFUSED, WSAEINVAL};
  size_t k;

  iov[0].iov_base = b1;
  iov[0].iov_len = 4;
  iov[1].iov_base = b2;
  iov[1].iov_len = 4;

  /* OOM in posix_writev and posix_readv */
  posix_sys_uio_set_alloc_hook(mock_uio_malloc);
  g_uio_oom = 1;
  ASSERT_EQ(-1, posix_writev(1, iov, 2));
  ASSERT_EQ(ENOMEM, errno);
  ASSERT_EQ(-1, posix_readv(1, iov, 2));
  ASSERT_EQ(ENOMEM, errno);
  g_uio_oom = 0;
  posix_sys_uio_set_alloc_hook(NULL);
  posix_sys_uio_reset_hooks();

  /* Real loopback TCP socket writev / readv */
  ASSERT_EQ(0, WSAStartup(MAKEWORD(2, 2), &wsa));
  listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ASSERT(listener != INVALID_SOCKET);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  ASSERT_EQ(0, bind(listener, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_EQ(0, getsockname(listener, (struct sockaddr *)&addr, &addrlen));
  ASSERT_EQ(0, listen(listener, 1));
  s_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ASSERT(s_client != INVALID_SOCKET);
  ASSERT_EQ(0, connect(s_client, (struct sockaddr *)&addr, sizeof(addr)));
  s_server = accept(listener, NULL, NULL);
  ASSERT(s_server != INVALID_SOCKET);
  closesocket(listener);

  ASSERT_EQ(8, posix_writev((int)s_client, iov, 2));
  memset(r1, 0, sizeof(r1));
  memset(r2, 0, sizeof(r2));
  iov[0].iov_base = r1;
  iov[0].iov_len = 4;
  iov[1].iov_base = r2;
  iov[1].iov_len = 4;
  ASSERT_EQ(8, posix_readv((int)s_server, iov, 2));
  ASSERT_MEM_EQ("ABCD", r1, 4);
  ASSERT_MEM_EQ("EFGH", r2, 4);

  closesocket(s_client);
  closesocket(s_server);

  /* Test hook default NULL fallbacks */
  posix_sys_uio_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL);

  /* Test fd >= 2048 hitting safe_get_osfhandle boundary + fallback I/O modes */
  posix_sys_uio_set_win32_hooks(mock_wsasend_fail, mock_wsarecv_fail,
                                mock_wsagetlasterror, mock_uio_write,
                                mock_uio_read, mock_uio_lseeki64);

  /* Mode 1: _write / _read fail on 1st chunk (total == 0 -> -1) with fd=2048 */
  g_mock_wsa_err = WSAENOTSOCK;
  g_mock_io_mode = 1;
  g_mock_io_calls = 0;
  ASSERT_EQ(-1, posix_writev(2048, iov, 2));
  g_mock_io_calls = 0;
  ASSERT_EQ(-1, posix_readv(2048, iov, 2));

  /* Mode 2: _write / _read succeed on 1st chunk, fail on 2nd (returns total) */
  g_mock_wsa_err = WSANOTINITIALISED;
  g_mock_io_mode = 2;
  g_mock_io_calls = 0;
  ASSERT_EQ(4, posix_writev(1, iov, 2));
  g_mock_io_calls = 0;
  ASSERT_EQ(4, posix_readv(1, iov, 2));

  /* Mode 3: partial _write / _read (< iov_len -> breaks loop) */
  g_mock_io_mode = 3;
  g_mock_io_calls = 0;
  ASSERT_EQ(2, posix_writev(1, iov, 2));
  g_mock_io_calls = 0;
  ASSERT_EQ(2, posix_readv(1, iov, 2));

  /* Non-seekable _lseeki64 == -1 fallback in posix_preadv / posix_pwritev */
  g_mock_io_mode = 0;
  g_mock_seek_ret = -1;
  ASSERT_EQ(8, posix_pwritev(1, iov, 2, 0));
  ASSERT_EQ(8, posix_preadv(1, iov, 2, 0));

  /* All Winsock error code mappings */
  for (k = 0; k < sizeof(wsa_errs) / sizeof(wsa_errs[0]); ++k) {
    g_mock_wsa_err = wsa_errs[k];
    ASSERT_EQ(-1, posix_writev(1, iov, 1));
    ASSERT_EQ(-1, posix_readv(1, iov, 1));
  }

  posix_sys_uio_reset_hooks();
  WSACleanup();
  PASS();
}
#endif

SUITE(suite_posix_sys_uio_core) {
  RUN_TEST(test_posix_sys_uio_get_info);
  RUN_TEST(test_posix_uio_readv_writev_file);
#if defined(_MSC_VER) || defined(_WIN32)
  RUN_TEST(test_posix_uio_win32_sockets_and_oom);
#endif
}
