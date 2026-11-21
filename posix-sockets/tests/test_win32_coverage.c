/* test_win32_coverage.c - Comprehensive Win32 & OOM branch coverage for
 * posix-sockets.c */
#define AUTO_WIN_MSVC_NO_FUNCTION_MACROS 1
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <fcntl.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#endif
/* clang-format on */

#if defined(_WIN32)

extern void set_nonblock(SOCKET s, int nb);
extern int get_nonblock(SOCKET s);
extern int clear_nonblock(SOCKET s);
extern void copy_nonblock(SOCKET src, SOCKET dst);
extern intptr_t posix_get_dup_socket(intptr_t fd);
extern void posix_set_dup_socket(intptr_t fd, intptr_t s);
extern intptr_t posix_lookup_crt_socket(intptr_t fd);
extern void posix_register_crt_socket(intptr_t fd, intptr_t s);
extern int mark_as_socket(intptr_t fd);
extern int clear_as_socket(intptr_t fd);
extern int is_socket(intptr_t fd);

static struct posix_sockets_win32_hooks g_def_hooks;

/* Mock state variables */
static int g_oom_countdown = 0;
static intptr_t g_osfhandle_ret = -1;
static int g_open_osfhandle_mode = 0;
static int g_open_osfhandle_calls = 0;
static int g_read_mode = 0;
static int g_read_calls = 0;
static int g_write_mode = 0;
static int g_write_calls = 0;
static int g_wsa_startup_ret = 0;
static int g_wsa_err = WSAEINVAL;
static int g_socket_mode = 0;
static int g_socket_calls = 0;
static int g_bind_ret = 0;
static int g_getsockname_ret = 0;
static int g_getpeername_ret = 0;
static int g_listen_ret = 0;
static SOCKET g_accept_ret = (SOCKET)44;
static int g_connect_mode = 0;
static int g_connect_calls = 0;
static int g_getsockopt_mode = 0;
static int g_getsockopt_so_err = 0;
static DWORD g_getsockopt_ms = 2500;
static int g_setsockopt_ret = 0;
static int g_shutdown_ret = 0;
static int g_recv_ret = 3;
static int g_send_ret = 3;
static int g_select_mode = 0;
static int g_wsa_io_mode = 0;
static int g_wsa_io_calls = 0;
static int g_gai_mode = 0;
static int g_gai_calls = 0;
static int g_serv_mode = 0;
static int g_serv_calls = 0;
static unsigned long g_ftype_ret = 1;
static int g_peek_ok = 1;
static unsigned long g_peek_avail = 0;
static int g_sleep_calls = 0;
static struct servent g_fake_servent;
static char *g_fake_aliases[1] = {NULL};
static char g_fake_sname[8] = "mock";
static char g_fake_sproto[8] = "tcp";

static void reset_win32_mocks(void) {
  g_oom_countdown = 0;
  g_osfhandle_ret = -1;
  g_open_osfhandle_mode = 0;
  g_open_osfhandle_calls = 0;
  g_read_mode = 0;
  g_read_calls = 0;
  g_write_mode = 0;
  g_write_calls = 0;
  g_wsa_startup_ret = 0;
  g_wsa_err = WSAEINVAL;
  g_socket_mode = 0;
  g_socket_calls = 0;
  g_bind_ret = 0;
  g_getsockname_ret = 0;
  g_getpeername_ret = 0;
  g_listen_ret = 0;
  g_accept_ret = (SOCKET)44;
  g_connect_mode = 0;
  g_connect_calls = 0;
  g_getsockopt_mode = 0;
  g_getsockopt_so_err = 0;
  g_getsockopt_ms = 2500;
  g_setsockopt_ret = 0;
  g_shutdown_ret = 0;
  g_recv_ret = 3;
  g_send_ret = 3;
  g_select_mode = 0;
  g_wsa_io_mode = 0;
  g_wsa_io_calls = 0;
  g_gai_mode = 0;
  g_gai_calls = 0;
  g_serv_mode = 0;
  g_serv_calls = 0;
  g_ftype_ret = 1;
  g_peek_ok = 1;
  g_peek_avail = 0;
  g_sleep_calls = 0;
}

static void *mock_sock_malloc(size_t size) {
  if (g_oom_countdown > 0) {
    g_oom_countdown--;
    if (g_oom_countdown == 0) {
      return NULL;
    }
  }
  return malloc(size);
}

static void *mock_sock_calloc(size_t count, size_t size) {
  if (g_oom_countdown > 0) {
    g_oom_countdown--;
    if (g_oom_countdown == 0) {
      return NULL;
    }
  }
  return calloc(count, size);
}

static void mock_sock_free(void *ptr) { free(ptr); }

static intptr_t mock_sock_get_osfhandle(int fd) {
  if (fd == 11) {
    return -1;
  }
  if (fd == 12) {
    return 0x1012;
  }
  if (fd == 13) {
    return 0x1013;
  }
  if (fd == 14) {
    return 0x1014;
  }
  return g_osfhandle_ret;
}

static int mock_sock_open_osfhandle(intptr_t osfhandle, int flags) {
  (void)osfhandle;
  (void)flags;
  g_open_osfhandle_calls++;
  if (g_open_osfhandle_mode == 1) {
    return -1;
  }
  if (g_open_osfhandle_mode == 2) {
    return (g_open_osfhandle_calls == 1) ? 31 : -1;
  }
  if (g_open_osfhandle_mode == 3) {
    return (g_open_osfhandle_calls == 1) ? -1 : 32;
  }
  return 30 + g_open_osfhandle_calls;
}

static int mock_sock_close(int fd) {
  (void)fd;
  return 0;
}

static int mock_sock_read(int fd, void *buf, unsigned int count) {
  (void)fd;
  g_read_calls++;
  if (g_read_mode == 1) {
    errno = EIO;
    return -1;
  }
  if (g_read_mode == 2) {
    if (g_read_calls == 1) {
      if (buf && count > 0) {
        memset(buf, 'A', count);
      }
      return (int)count;
    }
    errno = EIO;
    return -1;
  }
  if (g_read_mode == 3) {
    if (buf && count > 0) {
      ((char *)buf)[0] = 'P';
    }
    return 1;
  }
  if (g_read_mode == 4) {
    return 0;
  }
  if (buf && count > 0) {
    memset(buf, 'R', count);
  }
  return (int)count;
}

static int mock_sock_write(int fd, const void *buf, unsigned int count) {
  (void)fd;
  (void)buf;
  g_write_calls++;
  if (g_write_mode == 1) {
    errno = EIO;
    return -1;
  }
  if (g_write_mode == 2) {
    if (g_write_calls == 1) {
      return (int)count;
    }
    errno = EIO;
    return -1;
  }
  if (g_write_mode == 3) {
    return 1;
  }
  return (int)count;
}

static int mock_wsa_startup(WORD ver, LPWSADATA data) {
  (void)ver;
  if (data != NULL) {
    memset(data, 0, sizeof(*data));
  }
  return g_wsa_startup_ret;
}

static int mock_wsa_get_last_error(void) { return g_wsa_err; }

static SOCKET mock_wsa_socket_w(int af, int type, int protocol,
                                LPWSAPROTOCOL_INFOW info, GROUP g,
                                DWORD flags) {
  (void)af;
  (void)type;
  (void)protocol;
  (void)info;
  (void)g;
  (void)flags;
  g_socket_calls++;
  if (g_socket_mode == 1) {
    return INVALID_SOCKET;
  }
  if (g_socket_mode == 2) {
    if (g_socket_calls == 1) {
      g_wsa_err = WSANOTINITIALISED;
      return INVALID_SOCKET;
    }
    return (SOCKET)41;
  }
  if (g_socket_mode == 3) {
    g_wsa_err = WSANOTINITIALISED;
    return INVALID_SOCKET;
  }
  if (g_socket_mode == 4) {
    if (g_socket_calls == 2) {
      return INVALID_SOCKET;
    }
    return (SOCKET)(40 + g_socket_calls);
  }
  return (SOCKET)(40 + g_socket_calls);
}

static int mock_closesocket(SOCKET s) {
  (void)s;
  return 0;
}

static int mock_ioctlsocket(SOCKET s, long cmd, u_long *argp) {
  (void)s;
  (void)cmd;
  (void)argp;
  return 0;
}

static int mock_bind(SOCKET s, const struct sockaddr *name, int namelen) {
  (void)s;
  (void)name;
  (void)namelen;
  return g_bind_ret;
}

static int mock_getsockname(SOCKET s, struct sockaddr *name, int *namelen) {
  (void)s;
  if (g_getsockname_ret == 0 && name != NULL && namelen != NULL &&
      *namelen >= (int)sizeof(struct sockaddr_in)) {
    struct sockaddr_in *sin_p = (struct sockaddr_in *)name;
    sin_p->sin_port = htons(12345);
  }
  return g_getsockname_ret;
}

static int mock_getpeername(SOCKET s, struct sockaddr *name, int *namelen) {
  (void)s;
  (void)name;
  (void)namelen;
  return g_getpeername_ret;
}

static int mock_listen(SOCKET s, int backlog) {
  (void)s;
  (void)backlog;
  return g_listen_ret;
}

static SOCKET mock_accept(SOCKET s, struct sockaddr *addr, int *addrlen) {
  (void)s;
  (void)addr;
  (void)addrlen;
  return g_accept_ret;
}

static int mock_connect(SOCKET s, const struct sockaddr *name, int namelen) {
  (void)s;
  (void)name;
  (void)namelen;
  g_connect_calls++;
  if (g_connect_mode == 0) {
    return 0;
  }
  if (g_connect_mode == 1) {
    return SOCKET_ERROR;
  }
  if (g_connect_mode == 2) {
    if (g_connect_calls == 1) {
      g_wsa_err = WSAECONNREFUSED;
      return SOCKET_ERROR;
    }
    return 0;
  }
  if (g_connect_mode == 3) {
    if (g_connect_calls == 1) {
      g_wsa_err = WSAECONNREFUSED;
    } else {
      g_wsa_err = WSAENETUNREACH;
    }
    return SOCKET_ERROR;
  }
  return SOCKET_ERROR;
}

static int mock_getsockopt(SOCKET s, int level, int optname, char *optval,
                           int *optlen) {
  (void)level;
  if (g_getsockopt_mode == 1) {
    return SOCKET_ERROR;
  }
  /* For posix_select: sockets >= 100 and sockets 21, 22 are real sockets;
   * others are non-sockets */
  if (g_getsockopt_mode == 2) {
    if (s == (SOCKET)21 || s == (SOCKET)22 || s >= (SOCKET)100) {
      if (optval && optlen && *optlen >= (int)sizeof(int)) {
        *((int *)optval) = SOCK_STREAM;
      }
      return 0;
    }
    g_wsa_err = WSAENOTSOCK;
    return SOCKET_ERROR;
  }
  if (optname == SO_SNDTIMEO || optname == SO_RCVTIMEO) {
    if (optval && optlen && *optlen >= (int)sizeof(DWORD)) {
      *((DWORD *)optval) = g_getsockopt_ms;
    }
    return 0;
  }
  if (optname == SO_ERROR && optval && optlen && *optlen >= (int)sizeof(int)) {
    *((int *)optval) = g_getsockopt_so_err;
  }
  return 0;
}

static int mock_setsockopt(SOCKET s, int level, int optname, const char *optval,
                           int optlen) {
  (void)s;
  (void)level;
  (void)optname;
  (void)optval;
  (void)optlen;
  return g_setsockopt_ret;
}

static int mock_shutdown(SOCKET s, int how) {
  (void)s;
  (void)how;
  return g_shutdown_ret;
}

static int mock_recv(SOCKET s, char *buf, int len, int flags) {
  (void)s;
  (void)flags;
  if (g_recv_ret >= 0 && buf && len > 0) {
    buf[0] = 'x';
  }
  return g_recv_ret;
}

static int mock_send(SOCKET s, const char *buf, int len, int flags) {
  (void)s;
  (void)buf;
  (void)len;
  (void)flags;
  return g_send_ret;
}

static int mock_select(int nfds, fd_set *readfds, fd_set *writefds,
                       fd_set *exceptfds, const struct timeval *timeout) {
  (void)nfds;
  (void)timeout;
  if (g_select_mode == 1) {
    return SOCKET_ERROR;
  }
  if (g_select_mode == 2) {
    /* Only keep socket 21 ready; drop socket 22 */
    if (readfds) {
      readfds->fd_count = 1;
      readfds->fd_array[0] = (SOCKET)21;
    }
    if (writefds) {
      writefds->fd_count = 1;
      writefds->fd_array[0] = (SOCKET)21;
    }
    if (exceptfds) {
      exceptfds->fd_count = 1;
      exceptfds->fd_array[0] = (SOCKET)21;
    }
    return 1;
  }
  return 0;
}

static int mock_wsa_recv(SOCKET s, LPWSABUF buffers, DWORD buffer_count,
                         LPDWORD bytes_recvd, LPDWORD flags,
                         LPWSAOVERLAPPED overlapped,
                         LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  (void)s;
  (void)buffers;
  (void)buffer_count;
  (void)overlapped;
  (void)comp;
  g_wsa_io_calls++;
  if (g_wsa_io_mode == 0) {
    *bytes_recvd = 5;
    *flags = 0;
    return 0;
  }
  if (g_wsa_io_mode == 1) {
    *bytes_recvd = 2;
    *flags = 0;
    g_wsa_err = WSAEMSGSIZE;
    return SOCKET_ERROR;
  }
  if (g_wsa_io_mode == 3) {
    if (g_wsa_io_calls == 1) {
      g_wsa_err = WSAENOTSOCK;
      return SOCKET_ERROR;
    }
    *bytes_recvd = 6;
    *flags = 0;
    return 0;
  }
  return SOCKET_ERROR;
}

static int mock_wsa_recv_from(SOCKET s, LPWSABUF buffers, DWORD buffer_count,
                              LPDWORD bytes_recvd, LPDWORD flags,
                              struct sockaddr *from, LPINT fromlen,
                              LPWSAOVERLAPPED overlapped,
                              LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  (void)s;
  (void)buffers;
  (void)buffer_count;
  (void)from;
  (void)overlapped;
  (void)comp;
  g_wsa_io_calls++;
  if (g_wsa_io_mode == 0) {
    *bytes_recvd = 7;
    *flags = 0;
    if (fromlen) {
      *fromlen = (INT)sizeof(struct sockaddr_in);
    }
    return 0;
  }
  if (g_wsa_io_mode == 1) {
    *bytes_recvd = 3;
    *flags = 0;
    g_wsa_err = WSAEMSGSIZE;
    return SOCKET_ERROR;
  }
  if (g_wsa_io_mode == 3) {
    if (g_wsa_io_calls == 1) {
      g_wsa_err = WSAENOTSOCK;
      return SOCKET_ERROR;
    }
    *bytes_recvd = 8;
    *flags = 0;
    return 0;
  }
  if (g_wsa_io_mode == 4) {
    if (g_wsa_io_calls == 1) {
      g_wsa_err = WSAENOTSOCK;
      return SOCKET_ERROR;
    }
    *bytes_recvd = 4;
    *flags = 0;
    g_wsa_err = WSAEMSGSIZE;
    return SOCKET_ERROR;
  }
  if (g_wsa_io_mode == 5) {
    if (g_wsa_io_calls == 1) {
      g_wsa_err = WSAENOTSOCK;
      return SOCKET_ERROR;
    }
    g_wsa_err = WSAEINVAL;
    return SOCKET_ERROR;
  }
  return SOCKET_ERROR;
}

static int mock_wsa_send(SOCKET s, LPWSABUF buffers, DWORD buffer_count,
                         LPDWORD bytes_sent, DWORD flags,
                         LPWSAOVERLAPPED overlapped,
                         LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  (void)s;
  (void)buffers;
  (void)buffer_count;
  (void)flags;
  (void)overlapped;
  (void)comp;
  g_wsa_io_calls++;
  if (g_wsa_io_mode == 0) {
    *bytes_sent = 5;
    return 0;
  }
  if (g_wsa_io_mode == 3) {
    if (g_wsa_io_calls == 1) {
      g_wsa_err = WSAENOTSOCK;
      return SOCKET_ERROR;
    }
    *bytes_sent = 6;
    return 0;
  }
  return SOCKET_ERROR;
}

static int mock_wsa_send_to(SOCKET s, LPWSABUF buffers, DWORD buffer_count,
                            LPDWORD bytes_sent, DWORD flags,
                            const struct sockaddr *to, int tolen,
                            LPWSAOVERLAPPED overlapped,
                            LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  (void)s;
  (void)buffers;
  (void)buffer_count;
  (void)flags;
  (void)to;
  (void)tolen;
  (void)overlapped;
  (void)comp;
  g_wsa_io_calls++;
  if (g_wsa_io_mode == 0) {
    *bytes_sent = 7;
    return 0;
  }
  if (g_wsa_io_mode == 3) {
    if (g_wsa_io_calls == 1) {
      g_wsa_err = WSAENOTSOCK;
      return SOCKET_ERROR;
    }
    *bytes_sent = 8;
    return 0;
  }
  return SOCKET_ERROR;
}

static int mock_getaddrinfo(PCSTR nodename, PCSTR servname,
                            const ADDRINFOA *hints, PADDRINFOA *res) {
  (void)nodename;
  (void)servname;
  g_gai_calls++;
  if (g_gai_mode == 0) {
    if (res) {
      *res = NULL;
    }
    return 0;
  }
  if (g_gai_mode == 1) {
    return WSANOTINITIALISED;
  }
  if (g_gai_mode == 2) {
    if (g_gai_calls == 1) {
      return WSANOTINITIALISED;
    }
    return 0;
  }
  if (g_gai_mode == 3) {
    if (g_gai_calls == 1) {
      g_wsa_err = WSANOTINITIALISED;
      return EAI_FAIL;
    }
    return 0;
  }
  if (g_gai_mode == 4) {
    if (hints != NULL && (hints->ai_flags & AI_ADDRCONFIG)) {
      return EAI_FAIL;
    }
    return 0;
  }
  return EAI_FAIL;
}

static void mock_freeaddrinfo(PADDRINFOA ai) { free(ai); }

static struct servent *mock_getservbyname(const char *name, const char *proto) {
  (void)name;
  (void)proto;
  g_serv_calls++;
  g_fake_servent.s_name = g_fake_sname;
  g_fake_servent.s_aliases = g_fake_aliases;
  g_fake_servent.s_port = (short)htons(8080);
  g_fake_servent.s_proto = g_fake_sproto;
  if (g_serv_mode == 0) {
    return &g_fake_servent;
  }
  if (g_serv_mode == 1) {
    if (g_serv_calls == 1) {
      g_wsa_err = WSANOTINITIALISED;
      return NULL;
    }
    return &g_fake_servent;
  }
  return NULL;
}

static struct servent *mock_getservbyport(int port, const char *proto) {
  (void)port;
  (void)proto;
  g_serv_calls++;
  g_fake_servent.s_name = g_fake_sname;
  g_fake_servent.s_aliases = g_fake_aliases;
  g_fake_servent.s_port = (short)htons(8080);
  g_fake_servent.s_proto = g_fake_sproto;
  if (g_serv_mode == 0) {
    return &g_fake_servent;
  }
  if (g_serv_mode == 1) {
    if (g_serv_calls == 1) {
      g_wsa_err = WSANOTINITIALISED;
      return NULL;
    }
    return &g_fake_servent;
  }
  return NULL;
}

static unsigned long mock_get_file_type(void *file_handle) {
  if (file_handle == (void *)0x1012) {
    return 3; /* FILE_TYPE_PIPE */
  }
  if (file_handle == (void *)0x1013) {
    return 1; /* FILE_TYPE_DISK */
  }
  if (file_handle == (void *)0x1014) {
    return 2; /* FILE_TYPE_CHAR */
  }
  return g_ftype_ret;
}

static int mock_peek_named_pipe(void *named_pipe, void *buffer,
                                unsigned long buffer_size,
                                unsigned long *bytes_read,
                                unsigned long *total_bytes_avail,
                                unsigned long *bytes_left_this_message) {
  (void)named_pipe;
  (void)buffer;
  (void)buffer_size;
  (void)bytes_read;
  (void)bytes_left_this_message;
  if (!g_peek_ok) {
    return 0;
  }
  if (total_bytes_avail) {
    *total_bytes_avail = g_peek_avail;
  }
  return 1;
}

static void mock_sleep(DWORD milliseconds) {
  (void)milliseconds;
  g_sleep_calls++;
}

static void install_mock_hooks(void) {
  struct posix_sockets_win32_hooks h;
  h.malloc_fn = mock_sock_malloc;
  h.calloc_fn = mock_sock_calloc;
  h.free_fn = mock_sock_free;
  h.get_osfhandle_fn = mock_sock_get_osfhandle;
  h.open_osfhandle_fn = mock_sock_open_osfhandle;
  h.close_fn = mock_sock_close;
  h.read_fn = mock_sock_read;
  h.write_fn = mock_sock_write;
  h.wsa_startup_fn = mock_wsa_startup;
  h.wsa_get_last_error_fn = mock_wsa_get_last_error;
  h.wsa_socket_w_fn = mock_wsa_socket_w;
  h.closesocket_fn = mock_closesocket;
  h.ioctlsocket_fn = mock_ioctlsocket;
  h.bind_fn = mock_bind;
  h.getsockname_fn = mock_getsockname;
  h.getpeername_fn = mock_getpeername;
  h.listen_fn = mock_listen;
  h.accept_fn = mock_accept;
  h.connect_fn = mock_connect;
  h.getsockopt_fn = mock_getsockopt;
  h.setsockopt_fn = mock_setsockopt;
  h.shutdown_fn = mock_shutdown;
  h.recv_fn = mock_recv;
  h.send_fn = mock_send;
  h.select_fn = mock_select;
  h.wsa_recv_fn = mock_wsa_recv;
  h.wsa_recv_from_fn = mock_wsa_recv_from;
  h.wsa_send_fn = mock_wsa_send;
  h.wsa_send_to_fn = mock_wsa_send_to;
  h.getaddrinfo_fn = mock_getaddrinfo;
  h.freeaddrinfo_fn = mock_freeaddrinfo;
  h.getservbyname_fn = mock_getservbyname;
  h.getservbyport_fn = mock_getservbyport;
  h.get_file_type_fn = mock_get_file_type;
  h.peek_named_pipe_fn = mock_peek_named_pipe;
  h.sleep_fn = mock_sleep;
  posix_sockets_set_win32_hooks(&h);
}

TEST test_win32_default_hooks_and_real_io(void) {
  WSADATA wsa_data;
  void *p1;
  void *p2;
  FILE *tmpf;
  int pipe_fds[2];
  SOCKET udp_a;
  SOCKET udp_b;
  struct sockaddr_in sin_a;
  struct sockaddr_in sin_b;
  int slen;
  u_long nb_mode = 0;
  int opt_val = 1;
  int opt_len = (int)sizeof(opt_val);
  fd_set rfds;
  struct timeval tv;
  WSABUF wbuf;
  char sdata[8];
  char rdata[8];
  DWORD xfer = 0;
  DWORD wflags = 0;
  INT fromlen;
  ADDRINFOA gai_hints;
  PADDRINFOA gai_res = NULL;
  int tcp_l;
  int tcp_c;
  int tcp_s;

  posix_sockets_set_win32_hooks(NULL);
  posix_sockets_get_default_win32_hooks(NULL);
  posix_sockets_get_default_win32_hooks(&g_def_hooks);

  /* Exercise allocator, startup, error, sleep, serv wrappers */
  p1 = g_def_hooks.malloc_fn(16);
  ASSERT(p1 != NULL);
  g_def_hooks.free_fn(p1);
  p2 = g_def_hooks.calloc_fn(2, 8);
  ASSERT(p2 != NULL);
  g_def_hooks.free_fn(p2);

  ASSERT_EQ(0, g_def_hooks.wsa_startup_fn(MAKEWORD(2, 2), &wsa_data));
  (void)g_def_hooks.wsa_get_last_error_fn();
  g_def_hooks.sleep_fn(0);
  (void)g_def_hooks.getservbyname_fn("nonexistent_svc_awm", "tcp");
  (void)g_def_hooks.getservbyport_fn((int)htons(80), "tcp");

  memset(&gai_hints, 0, sizeof(gai_hints));
  gai_hints.ai_family = AF_INET;
  gai_hints.ai_socktype = SOCK_STREAM;
  gai_hints.ai_flags = AI_PASSIVE;
  if (g_def_hooks.getaddrinfo_fn(NULL, "80", &gai_hints, &gai_res) == 0 &&
      gai_res != NULL) {
    g_def_hooks.freeaddrinfo_fn(gai_res);
  }

  /* Exercise CRT fd wrappers + pipe GetFileType / PeekNamedPipe wrappers */
#if defined(_MSC_VER) && _MSC_VER >= 1400
  tmpf = NULL;
  ASSERT_EQ(0, tmpfile_s(&tmpf));
#else
  tmpf = tmpfile();
#endif
  ASSERT(tmpf != NULL);
  {
    int base_fd = _fileno(tmpf);
    int dup_fd = _dup(base_fd);
    intptr_t osfh = g_def_hooks.get_osfhandle_fn(base_fd);
    ASSERT(osfh != -1);
    ASSERT(g_def_hooks.get_file_type_fn((void *)osfh) != 0UL);
    ASSERT_EQ(2, g_def_hooks.write_fn(base_fd, "ok", 2));
    rewind(tmpf);
    memset(rdata, 0, sizeof(rdata));
    ASSERT_EQ(2, g_def_hooks.read_fn(base_fd, rdata, 2));
    if (dup_fd >= 0) {
      ASSERT_EQ(0, g_def_hooks.close_fn(dup_fd));
    }
  }
  fclose(tmpf);

  /* Invalid CRT fd triggers sock_null_invalid_param_handler on MSVC */
  (void)posix_sendmsg_native((uintptr_t)999, NULL, 0);
  {
    struct iovec iov1;
    struct msghdr m1;
    memcpy(sdata, "z", 2);
    iov1.iov_base = sdata;
    iov1.iov_len = 1;
    memset(&m1, 0, sizeof(m1));
    m1.msg_iov = &iov1;
    m1.msg_iovlen = 1;
    (void)posix_sendmsg_native((uintptr_t)999, &m1, 0);
  }

  if (_pipe(pipe_fds, 256, _O_BINARY) == 0) {
    intptr_t r_h = g_def_hooks.get_osfhandle_fn(pipe_fds[0]);
    unsigned long avail = 0;
    (void)g_def_hooks.peek_named_pipe_fn((void *)r_h, NULL, 0, NULL, &avail,
                                         NULL);
    _close(pipe_fds[0]);
    _close(pipe_fds[1]);
  }

  /* Exercise TCP accept, getpeername, shutdown, select, ioctlsocket, sockopt */
  tcp_l = posix_socket(AF_INET, SOCK_STREAM, 0);
  ASSERT(tcp_l >= 0);
  memset(&sin_a, 0, sizeof(sin_a));
  sin_a.sin_family = AF_INET;
  sin_a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sin_a.sin_port = 0;
  ASSERT_EQ(0, posix_bind(tcp_l, (struct sockaddr *)&sin_a, sizeof(sin_a)));
  slen = (int)sizeof(sin_a);
  ASSERT_EQ(0, posix_getsockname(tcp_l, (struct sockaddr *)&sin_a, &slen));
  ASSERT_EQ(0, posix_listen(tcp_l, 1));

  tcp_c = posix_socket(AF_INET, SOCK_STREAM, 0);
  ASSERT(tcp_c >= 0);
  ASSERT_EQ(0, posix_connect(tcp_c, (struct sockaddr *)&sin_a, sizeof(sin_a)));
  tcp_s = posix_accept(tcp_l, NULL, NULL);
  ASSERT(tcp_s >= 0);

  ASSERT_EQ(0,
            g_def_hooks.ioctlsocket_fn((SOCKET)tcp_c, (long)FIONBIO, &nb_mode));
  ASSERT_EQ(0, g_def_hooks.setsockopt_fn((SOCKET)tcp_c, SOL_SOCKET,
                                         SO_KEEPALIVE, (const char *)&opt_val,
                                         (int)sizeof(opt_val)));
  opt_len = (int)sizeof(opt_val);
  ASSERT_EQ(0,
            g_def_hooks.getsockopt_fn((SOCKET)tcp_c, SOL_SOCKET, SO_KEEPALIVE,
                                      (char *)&opt_val, &opt_len));
  slen = (int)sizeof(sin_b);
  ASSERT_EQ(0, g_def_hooks.getpeername_fn((SOCKET)tcp_c,
                                          (struct sockaddr *)&sin_b, &slen));

  memcpy(sdata, "abc", 4);
  wbuf.buf = sdata;
  wbuf.len = 3;
  ASSERT_EQ(0, g_def_hooks.wsa_send_fn((SOCKET)tcp_c, &wbuf, 1, &xfer, 0, NULL,
                                       NULL));
  FD_ZERO(&rfds);
  FD_SET((SOCKET)tcp_s, &rfds);
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  ASSERT(g_def_hooks.select_fn(0, &rfds, NULL, NULL, &tv) >= 0);
  memset(rdata, 0, sizeof(rdata));
  wbuf.buf = rdata;
  wbuf.len = 3;
  wflags = 0;
  ASSERT_EQ(0, g_def_hooks.wsa_recv_fn((SOCKET)tcp_s, &wbuf, 1, &xfer, &wflags,
                                       NULL, NULL));
  ASSERT_EQ(0, posix_shutdown(tcp_c, SHUT_RDWR));

  clear_as_socket(tcp_c);
  clear_as_socket(tcp_s);
  clear_as_socket(tcp_l);
  posix_register_crt_socket(tcp_c, 0);
  posix_register_crt_socket(tcp_s, 0);
  posix_register_crt_socket(tcp_l, 0);
  g_def_hooks.closesocket_fn((SOCKET)tcp_c);
  g_def_hooks.closesocket_fn((SOCKET)tcp_s);
  g_def_hooks.closesocket_fn((SOCKET)tcp_l);

  /* Exercise WSASendTo and WSARecvFrom on real loopback UDP sockets */
  udp_a = g_def_hooks.wsa_socket_w_fn(AF_INET, SOCK_DGRAM, 0, NULL, 0, 0);
  udp_b = g_def_hooks.wsa_socket_w_fn(AF_INET, SOCK_DGRAM, 0, NULL, 0, 0);
  ASSERT(udp_a != INVALID_SOCKET);
  ASSERT(udp_b != INVALID_SOCKET);
  memset(&sin_b, 0, sizeof(sin_b));
  sin_b.sin_family = AF_INET;
  sin_b.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sin_b.sin_port = 0;
  ASSERT_EQ(0, g_def_hooks.bind_fn(udp_b, (struct sockaddr *)&sin_b,
                                   (int)sizeof(sin_b)));
  slen = (int)sizeof(sin_b);
  ASSERT_EQ(
      0, g_def_hooks.getsockname_fn(udp_b, (struct sockaddr *)&sin_b, &slen));
  wbuf.buf = sdata;
  wbuf.len = 3;
  ASSERT_EQ(0, g_def_hooks.wsa_send_to_fn(udp_a, &wbuf, 1, &xfer, 0,
                                          (const struct sockaddr *)&sin_b,
                                          (int)sizeof(sin_b), NULL, NULL));
  wbuf.buf = rdata;
  wbuf.len = 3;
  wflags = 0;
  fromlen = (INT)sizeof(sin_a);
  ASSERT_EQ(0, g_def_hooks.wsa_recv_from_fn(udp_b, &wbuf, 1, &xfer, &wflags,
                                            (struct sockaddr *)&sin_a, &fromlen,
                                            NULL, NULL));
  g_def_hooks.closesocket_fn(udp_a);
  g_def_hooks.closesocket_fn(udp_b);
  PASS();
}

TEST test_win32_tables_nonblock_and_wsa_errors(void) {
  static const int k_wsa_codes[] = {
      WSAEWOULDBLOCK,     WSAEINPROGRESS,  WSAEALREADY,     WSAENOTSOCK,
      WSAEDESTADDRREQ,    WSAEMSGSIZE,     WSAEPROTOTYPE,   WSAENOPROTOOPT,
      WSAEPROTONOSUPPORT, WSAEOPNOTSUPP,   WSAEAFNOSUPPORT, WSAEADDRINUSE,
      WSAEADDRNOTAVAIL,   WSAENETDOWN,     WSAENETUNREACH,  WSAENETRESET,
      WSAECONNABORTED,    WSAECONNRESET,   WSAENOBUFS,      WSAEISCONN,
      WSAENOTCONN,        WSAETIMEDOUT,    WSAECONNREFUSED, WSAELOOP,
      WSAENAMETOOLONG,    WSAEHOSTUNREACH, WSAENOTEMPTY,    999999};
  size_t idx;
  int k;

  /* Untracked socket with nb=0 */
  set_nonblock((SOCKET)70001, 0);
  ASSERT_EQ(0, get_nonblock((SOCKET)70001));

  /* Track two sockets, re-set nb=1 on already tracked, copy, clear */
  set_nonblock((SOCKET)70001, 1);
  set_nonblock((SOCKET)70002, 1);
  set_nonblock((SOCKET)70001, 1);
  ASSERT(get_nonblock((SOCKET)70001) != 0);
  copy_nonblock((SOCKET)70001, (SOCKET)70003);
  ASSERT(get_nonblock((SOCKET)70003) != 0);
  copy_nonblock((SOCKET)70099, (SOCKET)70004);
  ASSERT_EQ(0, get_nonblock((SOCKET)70004));
  ASSERT_EQ(0, clear_nonblock((SOCKET)70001));
  ASSERT_EQ(0, clear_nonblock((SOCKET)70002));
  ASSERT_EQ(0, clear_nonblock((SOCKET)70003));

  /* Fill all 8192 slots to cover second-loop exhaustion branch */
  for (k = 1; k <= 8192; k++) {
    set_nonblock((SOCKET)k, 1);
  }
  set_nonblock((SOCKET)99999, 1);
  ASSERT_EQ(0, get_nonblock((SOCKET)99999));
  for (k = 1; k <= 8192; k++) {
    set_nonblock((SOCKET)k, 0);
  }

  /* Descriptor mapping boundary checks */
  posix_set_dup_socket(-1, 55);
  posix_set_dup_socket(64, 55);
  ASSERT_EQ(0, (int)posix_get_dup_socket(-1));
  ASSERT_EQ(0, (int)posix_get_dup_socket(64));
  posix_set_dup_socket(5, 77);
  ASSERT_EQ(77, (int)posix_get_dup_socket(5));

  posix_register_crt_socket(-1, 88);
  posix_register_crt_socket(65536, 88);
  ASSERT_EQ(0, (int)posix_lookup_crt_socket(-1));
  ASSERT_EQ(0, (int)posix_lookup_crt_socket(65536));
  posix_register_crt_socket(6, 88);
  ASSERT_EQ(88, (int)posix_lookup_crt_socket(6));

  (void)mark_as_socket(-1);
  (void)mark_as_socket(65536);
  (void)clear_as_socket(-1);
  (void)clear_as_socket(65536);
  ASSERT_EQ(1, is_socket(-1));
  ASSERT_EQ(1, is_socket(65536));
  (void)mark_as_socket(6);
  ASSERT_EQ(1, is_socket(6));
  (void)clear_as_socket(6);
  ASSERT_EQ(0, is_socket(6));

  /* Exercise GET_SOCKET via dup_socket, crt_socket, and raw fallback */
  install_mock_hooks();
  reset_win32_mocks();
  ASSERT_EQ(0, posix_listen(5, 1));
  ASSERT_EQ(0, posix_listen(6, 1));
  ASSERT_EQ(0, posix_listen(7, 1));
  posix_set_dup_socket(5, 0);
  posix_register_crt_socket(6, 0);

  /* Exercise all 28 _wsaErrorToErrno switch arms */
  g_listen_ret = SOCKET_ERROR;
  for (idx = 0; idx < sizeof(k_wsa_codes) / sizeof(k_wsa_codes[0]); idx++) {
    g_wsa_err = k_wsa_codes[idx];
    errno = 0;
    ASSERT_EQ(-1, posix_listen(10, 1));
    ASSERT(errno != 0);
  }

  posix_sockets_set_win32_hooks(NULL);
  PASS();
}

TEST test_win32_addrinfo_and_serv(void) {
  static const char *const k_builtin_svcs[] = {
      "http", "https", "ftp", "telnet", "ssh", "domain", "tftp", "ntp"};
  struct addrinfo hints;
  struct addrinfo *res = NULL;
  struct addrinfo *fake_sys_ai;
  size_t i;

  install_mock_hooks();
  reset_win32_mocks();

  /* Invalid / non-IPv4 nodenames hitting every parser exit */
  g_gai_mode = 0;
  ASSERT_EQ(0, posix_getaddrinfo(NULL, "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo(".1.2.3", "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo("localhost", "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo("1-2", "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo("1a", "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo("256.0.0.1", "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo("1..2.3", "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo("1.2.3.4.5", "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo("1.2.3.", "80", NULL, &res));
  ASSERT_EQ(0, posix_getaddrinfo("1.2.3", "80", NULL, &res));

  /* Valid IPv4 literal: OOM, NULL res, servname & hints variations */
  g_oom_countdown = 1;
  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", "80", NULL, &res));

  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", "80", NULL, NULL));

  res = NULL;
  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", NULL, NULL, &res));
  posix_freeaddrinfo(res);

  res = NULL;
  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", "-1", NULL, &res));
  posix_freeaddrinfo(res);

  res = NULL;
  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", "http", NULL, &res));
  posix_freeaddrinfo(res);

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = 0;
  hints.ai_protocol = IPPROTO_TCP;
  res = NULL;
  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", "8080", &hints, &res));
  posix_freeaddrinfo(res);

  hints.ai_socktype = SOCK_DGRAM;
  res = NULL;
  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", "8080", &hints, &res));
  posix_freeaddrinfo(res);

  /* Non-custom addrinfo passed to posix_freeaddrinfo */
  fake_sys_ai = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
  ASSERT(fake_sys_ai != NULL);
  fake_sys_ai->ai_flags = 0;
  posix_freeaddrinfo(fake_sys_ai);
  posix_freeaddrinfo(NULL);

  /* WSANOTINITIALISED and AI_ADDRCONFIG branches */
  reset_win32_mocks();
  g_gai_mode = 1;
  g_wsa_startup_ret = -1;
  ASSERT(posix_getaddrinfo("host", "80", NULL, &res) != 0);

  reset_win32_mocks();
  g_gai_mode = 1;
  g_wsa_startup_ret = 0;
  ASSERT(posix_getaddrinfo("host", "80", NULL, &res) != 0);

  reset_win32_mocks();
  g_gai_mode = 2;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_ADDRCONFIG;
  ASSERT_EQ(0, posix_getaddrinfo("host", "80", &hints, &res));

  reset_win32_mocks();
  g_gai_mode = 3;
  ASSERT_EQ(0, posix_getaddrinfo("host", "80", NULL, &res));

  reset_win32_mocks();
  g_gai_mode = 5;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = 0;
  ASSERT(posix_getaddrinfo("host", "80", &hints, &res) != 0);
  g_gai_mode = 4;
  hints.ai_flags = AI_ADDRCONFIG;
  ASSERT_EQ(0, posix_getaddrinfo("host", "80", &hints, &res));

  reset_win32_mocks();
  g_gai_mode = 5;
  hints.ai_flags = AI_ADDRCONFIG;
  ASSERT(posix_getaddrinfo("host", "80", &hints, &res) != 0);

  /* posix_getservbyname & posix_getservbyport */
  for (i = 0; i < sizeof(k_builtin_svcs) / sizeof(k_builtin_svcs[0]); i++) {
    ASSERT(posix_getservbyname(k_builtin_svcs[i], NULL) != NULL);
    ASSERT(posix_getservbyname(k_builtin_svcs[i], "udp") != NULL);
  }

  reset_win32_mocks();
  g_serv_mode = 0;
  ASSERT(posix_getservbyname(NULL, "tcp") != NULL);
  ASSERT(posix_getservbyname("other_svc", "tcp") != NULL);
  ASSERT(posix_getservbyport(80, "tcp") != NULL);

  reset_win32_mocks();
  g_serv_mode = 2;
  g_wsa_err = WSAEINVAL;
  ASSERT(posix_getservbyname("other_svc", "tcp") == NULL);
  ASSERT(posix_getservbyport(80, "tcp") == NULL);

  reset_win32_mocks();
  g_serv_mode = 1;
  g_wsa_startup_ret = 0;
  ASSERT(posix_getservbyname("other_svc", "tcp") != NULL);
  g_serv_calls = 0;
  ASSERT(posix_getservbyport(80, "tcp") != NULL);

  reset_win32_mocks();
  g_serv_mode = 1;
  g_wsa_startup_ret = -1;
  ASSERT(posix_getservbyname("other_svc", "tcp") == NULL);
  g_serv_calls = 0;
  ASSERT(posix_getservbyport(80, "tcp") == NULL);

  posix_sockets_set_win32_hooks(NULL);
  PASS();
}

TEST test_win32_select_branches(void) {
  struct {
    u_int fd_count;
    SOCKET fd_array[FD_SETSIZE + 1];
  } big_rfds;
  fd_set rfds;
  fd_set wfds;
  fd_set efds;
  struct timeval tv;
  u_int idx;

  install_mock_hooks();
  reset_win32_mocks();
  g_getsockopt_mode = 2;

  /* 1. Duplicate fd and FD_SETSIZE overflow protection in posix_fd_set */
  big_rfds.fd_count = FD_SETSIZE + 1;
  for (idx = 0; idx < FD_SETSIZE + 1; idx++) {
    big_rfds.fd_array[idx] = (SOCKET)(100 + idx);
  }
  ASSERT_EQ(0, posix_select(0, (fd_set *)&big_rfds, NULL, NULL, NULL));

  rfds.fd_count = 3;
  rfds.fd_array[0] = (SOCKET)21;
  rfds.fd_array[1] = (SOCKET)22;
  rfds.fd_array[2] = (SOCKET)21;
  wfds.fd_count = 2;
  wfds.fd_array[0] = (SOCKET)21;
  wfds.fd_array[1] = (SOCKET)22;
  efds.fd_count = 2;
  efds.fd_array[0] = (SOCKET)21;
  efds.fd_array[1] = (SOCKET)22;
  g_select_mode = 2;
  tv.tv_sec = 0;
  tv.tv_usec = 1000;
  ASSERT_EQ(1, posix_select(0, &rfds, &wfds, &efds, &tv));
  ASSERT_EQ(1U, rfds.fd_count);
  ASSERT_EQ(1U, wfds.fd_count);
  ASSERT_EQ(1U, efds.fd_count);

  /* 2. Socket + non-socket mix, and select_fn == SOCKET_ERROR recovery */
  g_select_mode = 1;
  rfds.fd_count = 1;
  rfds.fd_array[0] = (SOCKET)21;
  ASSERT_EQ(-1, posix_select(0, &rfds, NULL, NULL, &tv));

  rfds.fd_count = 2;
  rfds.fd_array[0] = (SOCKET)21;
  rfds.fd_array[1] = (SOCKET)13; /* disk file -> ready_non_sock_r */
  ASSERT_EQ(1, posix_select(0, &rfds, NULL, NULL, &tv));

  /* 3. Non-socket variants: INVALID_SOCKET, 0, >=64, h==-1, h==-2,
   * pipe/disk/char */
  g_select_mode = 0;
  g_osfhandle_ret = -2; /* covers safe_get_osfhandle h == -2 -> -1 */
  g_peek_ok = 1;
  g_peek_avail = 0; /* empty pipe -> not ready */
  rfds.fd_count = 7;
  rfds.fd_array[0] = INVALID_SOCKET;
  rfds.fd_array[1] = (SOCKET)0;
  rfds.fd_array[2] = (SOCKET)70;
  rfds.fd_array[3] = (SOCKET)11; /* h == -1 */
  rfds.fd_array[4] = (SOCKET)15; /* h == -2 -> -1 */
  rfds.fd_array[5] = (SOCKET)12; /* pipe, avail == 0 */
  rfds.fd_array[6] = (SOCKET)14; /* char device */

  wfds.fd_count = 5;
  wfds.fd_array[0] = INVALID_SOCKET;
  wfds.fd_array[1] = (SOCKET)0;
  wfds.fd_array[2] = (SOCKET)70;
  wfds.fd_array[3] = (SOCKET)11;
  wfds.fd_array[4] = (SOCKET)13; /* disk -> ready */

  efds.fd_count = 3;
  efds.fd_array[0] = INVALID_SOCKET;
  efds.fd_array[1] = (SOCKET)0;
  efds.fd_array[2] = (SOCKET)70;

  ASSERT_EQ(1, posix_select(0, &rfds, &wfds, &efds, &tv));
  ASSERT_EQ(0U, rfds.fd_count);
  ASSERT_EQ(1U, wfds.fd_count);
  ASSERT_EQ(0U, efds.fd_count);

  /* Pipe with avail > 0 and pipe with PeekNamedPipe failure */
  g_peek_ok = 1;
  g_peek_avail = 5;
  rfds.fd_count = 1;
  rfds.fd_array[0] = (SOCKET)12;
  ASSERT_EQ(1, posix_select(0, &rfds, NULL, NULL, NULL));

  g_peek_ok = 0;
  rfds.fd_count = 1;
  rfds.fd_array[0] = (SOCKET)12;
  ASSERT_EQ(1, posix_select(0, &rfds, NULL, NULL, NULL));

  /* num_sockets == 0 && num_non_sock_ready == 0 with NULL and non-NULL timeout
   */
  rfds.fd_count = 0;
  wfds.fd_count = 0;
  efds.fd_count = 0;
  ASSERT_EQ(0, posix_select(0, &rfds, &wfds, &efds, NULL));
  tv.tv_sec = 0;
  tv.tv_usec = 2000;
  g_sleep_calls = 0;
  ASSERT_EQ(0, posix_select(0, &rfds, &wfds, &efds, &tv));
  ASSERT_EQ(1, g_sleep_calls);

  posix_sockets_set_win32_hooks(NULL);
  PASS();
}

TEST test_win32_conn_sockopt_socket_socketpair(void) {
  static const int k_raw_errs[] = {WSAEINVAL, WSAEPROTONOSUPPORT,
                                   WSAESOCKTNOSUPPORT, WSAEACCES};
  struct sockaddr sa_unix;
  struct sockaddr_in sin;
  struct sockaddr sa_v6;
  struct timeval tv;
  posix_socklen_t len;
  int opt_val;
  intptr_t sv[2];
  size_t i;

  install_mock_hooks();
  reset_win32_mocks();

  /* posix_accept */
  g_accept_ret = INVALID_SOCKET;
  g_wsa_err = WSAEWOULDBLOCK;
  ASSERT_EQ(-1, posix_accept(21, NULL, NULL));
  ASSERT_EQ(EWOULDBLOCK, errno);
  g_wsa_err = WSAEINVAL;
  ASSERT_EQ(-1, posix_accept(21, NULL, NULL));

  g_accept_ret = (SOCKET)45;
  set_nonblock((SOCKET)21, 1);
  ASSERT_EQ(45, posix_accept(21, NULL, NULL));
  ASSERT(get_nonblock((SOCKET)45) != 0);
  set_nonblock((SOCKET)21, 0);
  set_nonblock((SOCKET)45, 0);
  clear_as_socket(45);
  posix_register_crt_socket(45, 0);

  /* posix_bind: AF_UNIX and AF_INET */
  memset(&sa_unix, 0, sizeof(sa_unix));
  sa_unix.sa_family = AF_UNIX;
  g_bind_ret = 0;
  ASSERT_EQ(0, posix_bind(21, &sa_unix, (posix_socklen_t)sizeof(sa_unix)));
  g_bind_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_bind(21, &sa_unix, (posix_socklen_t)sizeof(sa_unix)));

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  g_bind_ret = 0;
  ASSERT_EQ(
      0, posix_bind(21, (struct sockaddr *)&sin, (posix_socklen_t)sizeof(sin)));
  g_bind_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_bind(21, (struct sockaddr *)&sin,
                           (posix_socklen_t)sizeof(sin)));
  ASSERT_EQ(-1, posix_bind(21, NULL, 0));

  /* posix_connect: NULL, ok, retry short-circuits & full loop, final errors */
  ASSERT_EQ(-1, posix_connect(21, NULL, 0));
  ASSERT_EQ(EFAULT, errno);

  g_connect_mode = 0;
  ASSERT_EQ(0, posix_connect(21, (struct sockaddr *)&sin,
                             (posix_socklen_t)sizeof(sin)));

  g_connect_mode = 1;
  g_wsa_err = WSAETIMEDOUT;
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));

  memset(&sa_v6, 0, sizeof(sa_v6));
  sa_v6.sa_family = AF_INET6;
  g_wsa_err = WSAECONNREFUSED;
  ASSERT_EQ(-1, posix_connect(21, &sa_v6, (posix_socklen_t)sizeof(sa_v6)));

  sin.sin_addr.s_addr = htonl(0x7f000001UL);
  sin.sin_port = htons(8080);
  set_nonblock((SOCKET)21, 1);
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));
  set_nonblock((SOCKET)21, 0);

  sin.sin_addr.s_addr = htonl(0x0a000001UL);
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));

  sin.sin_addr.s_addr = htonl(0x7f000001UL);
  sin.sin_port = 0;
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));

  sin.sin_port = htons(8080);
  g_socket_mode = 1; /* wsa_socket_w_fn fails inside retry */
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));
  g_socket_mode = 0;

  g_connect_calls = 0;
  g_connect_mode = 2; /* succeeds on retry 1 */
  ASSERT_EQ(0, posix_connect(21, (struct sockaddr *)&sin,
                             (posix_socklen_t)sizeof(sin)));
  posix_register_crt_socket(21, 0);

  g_connect_calls = 0;
  g_connect_mode = 3; /* changes error on retry 1 */
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));
  posix_register_crt_socket(21, 0);

  g_connect_calls = 0;
  g_connect_mode = 1;
  g_wsa_err = WSAECONNREFUSED; /* exhausts all 60 retries */
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));
  posix_register_crt_socket(21, 0);

  g_wsa_err = WSAEISCONN;
  ASSERT_EQ(0, posix_connect(21, (struct sockaddr *)&sin,
                             (posix_socklen_t)sizeof(sin)));
  g_wsa_err = WSAEWOULDBLOCK;
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));
  ASSERT_EQ(EINPROGRESS, errno);
  g_wsa_err = WSAEINPROGRESS;
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));
  g_wsa_err = WSAEALREADY;
  ASSERT_EQ(-1, posix_connect(21, (struct sockaddr *)&sin,
                              (posix_socklen_t)sizeof(sin)));

  /* getpeername, getsockname, shutdown, recv, send */
  len = (posix_socklen_t)sizeof(sin);
  g_getpeername_ret = 0;
  ASSERT_EQ(0, posix_getpeername(21, (struct sockaddr *)&sin, &len));
  g_getpeername_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_getpeername(21, (struct sockaddr *)&sin, &len));

  g_getsockname_ret = 0;
  ASSERT_EQ(0, posix_getsockname(21, (struct sockaddr *)&sin, &len));
  g_getsockname_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_getsockname(21, (struct sockaddr *)&sin, &len));

  g_shutdown_ret = 0;
  ASSERT_EQ(0, posix_shutdown(21, SHUT_RDWR));
  g_shutdown_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_shutdown(21, SHUT_RDWR));

  g_recv_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, (int)posix_recv(21, &opt_val, 1, 0));
  g_send_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, (int)posix_send(21, &opt_val, 1, 0));

  /* getsockopt & setsockopt */
  reset_win32_mocks();
  len = (posix_socklen_t)sizeof(tv);
  ASSERT_EQ(0, posix_getsockopt(21, SOL_SOCKET, SO_SNDTIMEO, &tv, &len));
  ASSERT_EQ(2, (int)tv.tv_sec);
  ASSERT_EQ(0, posix_getsockopt(21, SOL_SOCKET, SO_RCVTIMEO, &tv, &len));
  ASSERT_EQ(0, posix_getsockopt(21, SOL_SOCKET, SO_RCVTIMEO, &tv, NULL));
  len = 4;
  ASSERT_EQ(0, posix_getsockopt(21, SOL_SOCKET, SO_RCVTIMEO, &opt_val, &len));

  g_getsockopt_mode = 1;
  len = (posix_socklen_t)sizeof(tv);
  ASSERT_EQ(-1, posix_getsockopt(21, SOL_SOCKET, SO_SNDTIMEO, &tv, &len));
  ASSERT_EQ(-1, posix_getsockopt(21, SOL_SOCKET, SO_ERROR, &opt_val, &len));

  g_getsockopt_mode = 0;
  len = (posix_socklen_t)sizeof(opt_val);
  ASSERT_EQ(0, posix_getsockopt(21, IPPROTO_TCP, 1, &opt_val, &len));
  ASSERT_EQ(0, posix_getsockopt(21, SOL_SOCKET, SO_KEEPALIVE, &opt_val, &len));
  g_getsockopt_so_err = 0;
  ASSERT_EQ(0, posix_getsockopt(21, SOL_SOCKET, SO_ERROR, &opt_val, &len));
  ASSERT_EQ(0, opt_val);
  g_getsockopt_so_err = WSAECONNREFUSED;
  ASSERT_EQ(0, posix_getsockopt(21, SOL_SOCKET, SO_ERROR, &opt_val, &len));
  ASSERT_EQ(ECONNREFUSED, opt_val);

  opt_val = 1;
  ASSERT_EQ(0, posix_setsockopt(21, SOL_SOCKET, SO_REUSEADDR, &opt_val,
                                (posix_socklen_t)sizeof(opt_val)));
  tv.tv_sec = 1;
  tv.tv_usec = 500000;
  ASSERT_EQ(0, posix_setsockopt(21, SOL_SOCKET, SO_SNDTIMEO, &tv,
                                (posix_socklen_t)sizeof(tv)));
  ASSERT_EQ(0, posix_setsockopt(21, SOL_SOCKET, SO_RCVTIMEO, &tv,
                                (posix_socklen_t)sizeof(tv)));
  ASSERT_EQ(0, posix_setsockopt(21, SOL_SOCKET, SO_SNDTIMEO, &opt_val, 4));
  ASSERT_EQ(0, posix_setsockopt(21, IPPROTO_TCP, 1, &opt_val, 4));
  ASSERT_EQ(0, posix_setsockopt(21, SOL_SOCKET, SO_KEEPALIVE, &opt_val, 4));
  g_setsockopt_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_setsockopt(21, SOL_SOCKET, SO_KEEPALIVE, &opt_val, 4));

  /* posix_socket */
  reset_win32_mocks();
  {
    int s1 = posix_socket(AF_UNIX, SOCK_STREAM, 0);
    int s2 = posix_socket(AF_INET, SOCK_DGRAM, 1);
    int s3 = posix_socket(AF_INET, SOCK_DGRAM, 58);
    int s4 = posix_socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(s1 >= 0 && s2 >= 0 && s3 >= 0 && s4 >= 0);
    set_nonblock((SOCKET)s2, 0);
    set_nonblock((SOCKET)s3, 0);
    clear_as_socket(s1);
    clear_as_socket(s2);
    clear_as_socket(s3);
    clear_as_socket(s4);
    posix_register_crt_socket(s1, 0);
    posix_register_crt_socket(s2, 0);
    posix_register_crt_socket(s3, 0);
    posix_register_crt_socket(s4, 0);
  }

  reset_win32_mocks();
  g_socket_mode = 2;
  {
    int s5 = posix_socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_EQ(41, s5);
    clear_as_socket(s5);
    posix_register_crt_socket(s5, 0);
  }

  reset_win32_mocks();
  g_socket_mode = 3;
  g_wsa_startup_ret = 0;
  ASSERT_EQ(-1, posix_socket(AF_INET, SOCK_STREAM, 0));
  g_wsa_startup_ret = -1;
  ASSERT_EQ(-1, posix_socket(AF_INET, SOCK_STREAM, 0));

  reset_win32_mocks();
  g_socket_mode = 1;
  for (i = 0; i < sizeof(k_raw_errs) / sizeof(k_raw_errs[0]); i++) {
    g_wsa_err = k_raw_errs[i];
    ASSERT_EQ(-1, posix_socket(AF_INET, SOCK_RAW, 0));
    ASSERT_EQ(EACCES, errno);
  }
  g_wsa_err = WSAENOBUFS;
  ASSERT_EQ(-1, posix_socket(AF_INET, SOCK_RAW, 0));
  ASSERT_EQ(ENOBUFS, errno);

  /* posix_socketpair error & WSANOTINITIALISED paths */
  ASSERT_EQ(-1, posix_socketpair(AF_INET6, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_socket_mode = 1;
  g_wsa_err = WSAEINVAL;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_socket_mode = 3;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_socket_mode = 2;
  ASSERT_EQ(0, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));
  clear_as_socket(sv[0]);
  clear_as_socket(sv[1]);
  posix_register_crt_socket(sv[0], 0);
  posix_register_crt_socket(sv[1], 0);

  reset_win32_mocks();
  g_bind_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_getsockname_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_listen_ret = SOCKET_ERROR;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_socket_mode = 4;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_connect_mode = 1;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_accept_ret = INVALID_SOCKET;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_open_osfhandle_mode = 1;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_open_osfhandle_mode = 2;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  reset_win32_mocks();
  g_open_osfhandle_mode = 3;
  ASSERT_EQ(-1, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));

  posix_sockets_set_win32_hooks(NULL);
  PASS();
}

TEST test_win32_sendmsg_recvmsg_all_branches(void) {
  struct iovec iovs[18];
  char bufs[18][4];
  struct msghdr msg;
  struct sockaddr_in addr;
  int i;

  for (i = 0; i < 18; i++) {
    memset(bufs[i], 'x', sizeof(bufs[i]));
    iovs[i].iov_base = bufs[i];
    iovs[i].iov_len = 2;
  }

  install_mock_hooks();
  reset_win32_mocks();

  /* 1. Validation & msg_iovlen == 0 across all 4 functions */
  memset(&msg, 0, sizeof(msg));
  msg.msg_iovlen = -1;
  ASSERT_EQ(-1, (int)posix_recvmsg(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_sendmsg(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_sendmsg_native(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_recvmsg_native(21, &msg, 0));

  msg.msg_iovlen = 1;
  msg.msg_iov = NULL;
  ASSERT_EQ(-1, (int)posix_recvmsg(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_sendmsg(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_sendmsg_native(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_recvmsg_native(21, &msg, 0));

  msg.msg_iov = iovs;
  msg.msg_iovlen = 0;
  ASSERT_EQ(0, (int)posix_recvmsg(21, &msg, 0));
  ASSERT_EQ(0, (int)posix_sendmsg(21, &msg, 0));
  ASSERT_EQ(0, (int)posix_sendmsg_native(21, &msg, 0));
  ASSERT_EQ(0, (int)posix_recvmsg_native(21, &msg, 0));

  /* 2. Heap OOM (>16 iovecs) across all 4 functions */
  msg.msg_iovlen = 17;
  g_oom_countdown = 1;
  ASSERT_EQ(-1, (int)posix_recvmsg(21, &msg, 0));
  g_oom_countdown = 1;
  ASSERT_EQ(-1, (int)posix_sendmsg(21, &msg, 0));
  g_oom_countdown = 1;
  ASSERT_EQ(-1, (int)posix_sendmsg_native(21, &msg, 0));
  g_oom_countdown = 1;
  ASSERT_EQ(-1, (int)posix_recvmsg_native(21, &msg, 0));

  /* 3. CRT fd fallback (WSAENOTSOCK) for both stack (2) and heap (17) */
  g_getsockopt_mode = 1;
  g_wsa_err = WSAENOTSOCK;

  /* read/write fail on call 1 (total == 0 -> -1) */
  g_read_mode = 1;
  g_write_mode = 1;
  msg.msg_iovlen = 2;
  ASSERT_EQ(-1, (int)posix_recvmsg(10, &msg, 0));
  ASSERT_EQ(-1, (int)posix_sendmsg(10, &msg, 0));
  msg.msg_iovlen = 17;
  ASSERT_EQ(-1, (int)posix_recvmsg(10, &msg, 0));
  ASSERT_EQ(-1, (int)posix_sendmsg(10, &msg, 0));

  /* read/write succeed on call 1, fail on call 2 (total > 0 -> total) */
  g_read_mode = 2;
  g_write_mode = 2;
  msg.msg_iovlen = 2;
  g_read_calls = 0;
  g_write_calls = 0;
  ASSERT_EQ(2, (int)posix_recvmsg(10, &msg, 0));
  ASSERT_EQ(2, (int)posix_sendmsg(10, &msg, 0));

  /* partial read/write (1 < 2 -> break) */
  g_read_mode = 3;
  g_write_mode = 3;
  msg.msg_iovlen = 2;
  ASSERT_EQ(1, (int)posix_recvmsg(10, &msg, 0));
  ASSERT_EQ(1, (int)posix_sendmsg(10, &msg, 0));

  /* zero-length iov with read returning 0 (r == 0 -> break) */
  g_read_mode = 4;
  iovs[0].iov_len = 0;
  msg.msg_iovlen = 2;
  ASSERT_EQ(0, (int)posix_recvmsg(10, &msg, 0));
  iovs[0].iov_len = 2;

  /* full read/write of all iovecs (stack & heap) */
  g_read_mode = 0;
  g_write_mode = 0;
  msg.msg_iovlen = 2;
  ASSERT_EQ(4, (int)posix_recvmsg(10, &msg, 0));
  ASSERT_EQ(4, (int)posix_sendmsg(10, &msg, 0));
  msg.msg_iovlen = 17;
  ASSERT_EQ(34, (int)posix_recvmsg(10, &msg, 0));
  ASSERT_EQ(34, (int)posix_sendmsg(10, &msg, 0));

  /* getsockopt == SOCKET_ERROR with err != WSAENOTSOCK falls through to Winsock
   */
  g_getsockopt_mode = 1;
  g_wsa_err = WSAEINVAL;
  g_wsa_io_mode = 0;
  msg.msg_iovlen = 2;
  ASSERT_EQ(5, (int)posix_recvmsg(21, &msg, 0));
  ASSERT_EQ(5, (int)posix_sendmsg(21, &msg, 0));

  /* 4. Winsock sendmsg/recvmsg/native with msg_name variations & heap */
  reset_win32_mocks();
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;

  /* msg_name != NULL && msg_namelen == 0 */
  msg.msg_name = &addr;
  msg.msg_namelen = 0;
  msg.msg_iovlen = 17;
  ASSERT_EQ(5, (int)posix_recvmsg(21, &msg, 0));
  ASSERT_EQ(5, (int)posix_sendmsg(21, &msg, 0));
  ASSERT_EQ(5, (int)posix_sendmsg_native(21, &msg, 0));
  ASSERT_EQ(5, (int)posix_recvmsg_native(21, &msg, 0));

  /* msg_name != NULL && msg_namelen > 0: ok, WSAEMSGSIZE, and error */
  msg.msg_namelen = (posix_socklen_t)sizeof(addr);
  msg.msg_iovlen = 2;
  g_wsa_io_mode = 0;
  ASSERT_EQ(7, (int)posix_recvmsg(21, &msg, 0));
  ASSERT_EQ(7, (int)posix_sendmsg(21, &msg, 0));
  ASSERT_EQ(7, (int)posix_sendmsg_native(21, &msg, 0));
  ASSERT_EQ(7, (int)posix_recvmsg_native(21, &msg, 0));

  g_wsa_io_mode = 1; /* WSAEMSGSIZE */
  ASSERT_EQ(3, (int)posix_recvmsg(21, &msg, 0));
  ASSERT((msg.msg_flags & 0x20) != 0);
  ASSERT_EQ(3, (int)posix_recvmsg_native(21, &msg, 0));
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  ASSERT_EQ(2, (int)posix_recvmsg(21, &msg, 0));
  ASSERT_EQ(2, (int)posix_recvmsg_native(21, &msg, 0));

  msg.msg_name = &addr;
  msg.msg_namelen = (posix_socklen_t)sizeof(addr);
  g_wsa_io_mode = 2;
  g_wsa_err = WSAEINVAL;
  ASSERT_EQ(-1, (int)posix_recvmsg(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_sendmsg(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_sendmsg_native(21, &msg, 0));
  ASSERT_EQ(-1, (int)posix_recvmsg_native(21, &msg, 0));

  /* Specific WSAEWOULDBLOCK and WSAECONNRESET mappings in _native */
  g_wsa_err = WSAEWOULDBLOCK;
  ASSERT_EQ(-1, (int)posix_sendmsg_native(21, &msg, 0));
  ASSERT_EQ(140, errno);
  ASSERT_EQ(-1, (int)posix_recvmsg_native(21, &msg, 0));
  ASSERT_EQ(140, errno);

  g_wsa_err = WSAECONNRESET;
  ASSERT_EQ(-1, (int)posix_sendmsg_native(21, &msg, 0));
  ASSERT_EQ(104, errno);
  ASSERT_EQ(-1, (int)posix_recvmsg_native(21, &msg, 0));
  ASSERT_EQ(104, errno);

  /* 5. _native WSAENOTSOCK CRT handle fallback */
  /* fd < 0 (s == -2 != -1) and fd >= 2048 -> real_s == -1 */
  g_wsa_io_mode = 3;
  g_wsa_io_calls = 0;
  ASSERT_EQ(-1, (int)posix_sendmsg_native((uintptr_t)(intptr_t)-2, &msg, 0));
  g_wsa_io_calls = 0;
  ASSERT_EQ(-1, (int)posix_sendmsg_native(2048, &msg, 0));
  g_wsa_io_calls = 0;
  ASSERT_EQ(-1, (int)posix_recvmsg_native(2048, &msg, 0));

  /* get_osfhandle_fn == 0 -> real_s == 0 */
  g_osfhandle_ret = 0;
  g_wsa_io_calls = 0;
  ASSERT_EQ(-1, (int)posix_sendmsg_native(15, &msg, 0));
  g_wsa_io_calls = 0;
  ASSERT_EQ(-1, (int)posix_recvmsg_native(15, &msg, 0));

  /* Valid real_s: with msg_name (namelen > 0, namelen == 0) and without */
  g_osfhandle_ret = 55;
  msg.msg_name = &addr;
  msg.msg_namelen = 0;
  g_wsa_io_mode = 3;
  g_wsa_io_calls = 0;
  ASSERT_EQ(6, (int)posix_sendmsg_native(15, &msg, 0));
  g_wsa_io_calls = 0;
  ASSERT_EQ(6, (int)posix_recvmsg_native(15, &msg, 0));

  msg.msg_namelen = (posix_socklen_t)sizeof(addr);
  g_wsa_io_mode = 3;
  g_wsa_io_calls = 0;
  ASSERT_EQ(8, (int)posix_sendmsg_native(15, &msg, 0));
  g_wsa_io_calls = 0;
  ASSERT_EQ(8, (int)posix_recvmsg_native(15, &msg, 0));

  g_wsa_io_mode = 4; /* retry returns WSAEMSGSIZE */
  g_wsa_io_calls = 0;
  ASSERT_EQ(4, (int)posix_recvmsg_native(15, &msg, 0));

  g_wsa_io_mode = 5; /* retry returns WSAEINVAL */
  g_wsa_io_calls = 0;
  ASSERT_EQ(-1, (int)posix_recvmsg_native(15, &msg, 0));

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  g_wsa_io_mode = 3;
  g_wsa_io_calls = 0;
  ASSERT_EQ(6, (int)posix_sendmsg_native(15, &msg, 0));
  g_wsa_io_calls = 0;
  ASSERT_EQ(6, (int)posix_recvmsg_native(15, &msg, 0));

  posix_sockets_set_win32_hooks(NULL);
  PASS();
}

#else

TEST test_win32_noop_on_unix(void) { PASS(); }

#endif /* _WIN32 */

SUITE(suite_posix_sockets_win32_coverage) {
#if defined(_WIN32)
  RUN_TEST(test_win32_default_hooks_and_real_io);
  RUN_TEST(test_win32_tables_nonblock_and_wsa_errors);
  RUN_TEST(test_win32_addrinfo_and_serv);
  RUN_TEST(test_win32_select_branches);
  RUN_TEST(test_win32_conn_sockopt_socket_socketpair);
  RUN_TEST(test_win32_sendmsg_recvmsg_all_branches);
#else
  RUN_TEST(test_win32_noop_on_unix);
#endif
}
