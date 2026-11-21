#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/* posix-sockets.c - Strict C89 Implementation */
#define POSIX_SOCKETS_EXPORTS
/* clang-format off */
#include "posix-sockets.h"

#ifdef closesocket
#undef closesocket
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
#endif

#include <errno.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ifaddrs.h"
#if defined(_WIN32)
#include <fcntl.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
/* clang-format on */

#ifndef _O_RDWR
#define _O_RDWR 2
#endif
#ifndef _O_BINARY
#define _O_BINARY 0x8000
#endif
#ifndef EINVAL
#define EINVAL 38
#endif

#if defined(_WIN32)
#undef freeaddrinfo
#undef gai_strerror
#undef getaddrinfo
#undef getnameinfo
#undef getservbyname
#undef getservbyport
#undef select
#undef accept
#undef bind
#undef connect
#undef getpeername
#undef getsockname
#undef getsockopt
#undef listen
#undef recv
#undef send
#undef setsockopt
#undef shutdown
#undef socket

__declspec(dllimport) unsigned long __stdcall GetFileType(void *hFile);
__declspec(dllimport) int __stdcall
PeekNamedPipe(void *hNamedPipe, void *lpBuffer, unsigned long nBufferSize,
              unsigned long *lpBytesRead, unsigned long *lpTotalBytesAvail,
              unsigned long *lpBytesLeftThisMessage);

static void *default_sock_malloc(size_t size) { return malloc(size); }
static void *default_sock_calloc(size_t count, size_t size) {
  return calloc(count, size);
}
static void default_sock_free(void *ptr) { free(ptr); }
static intptr_t default_sock_get_osfhandle(int fd) {
  return _get_osfhandle(fd);
}
static int default_sock_open_osfhandle(intptr_t osfhandle, int flags) {
  return _open_osfhandle(osfhandle, flags);
}
static int default_sock_close(int fd) { return _close(fd); }
static int default_sock_read(int fd, void *buf, unsigned int count) {
  return _read(fd, buf, count);
}
static int default_sock_write(int fd, const void *buf, unsigned int count) {
  return _write(fd, buf, count);
}
static int default_wsa_startup(WORD ver, LPWSADATA data) {
  return WSAStartup(ver, data);
}
static int default_wsa_get_last_error(void) { return WSAGetLastError(); }
static SOCKET default_wsa_socket_w(int af, int type, int protocol,
                                   LPWSAPROTOCOL_INFOW info, GROUP g,
                                   DWORD flags) {
  return WSASocketW(af, type, protocol, info, g, flags);
}
static int default_closesocket(SOCKET s) { return closesocket(s); }
static int default_ioctlsocket(SOCKET s, long cmd, u_long *argp) {
  return ioctlsocket(s, cmd, argp);
}
static int default_bind(SOCKET s, const struct sockaddr *name, int namelen) {
  return bind(s, name, namelen);
}
static int default_getsockname(SOCKET s, struct sockaddr *name, int *namelen) {
  return getsockname(s, name, namelen);
}
static int default_getpeername(SOCKET s, struct sockaddr *name, int *namelen) {
  return getpeername(s, name, namelen);
}
static int default_listen(SOCKET s, int backlog) { return listen(s, backlog); }
static SOCKET default_accept(SOCKET s, struct sockaddr *addr, int *addrlen) {
  return accept(s, addr, addrlen);
}
static int default_connect(SOCKET s, const struct sockaddr *name, int namelen) {
  return connect(s, name, namelen);
}
static int default_getsockopt(SOCKET s, int level, int optname, char *optval,
                              int *optlen) {
  return getsockopt(s, level, optname, optval, optlen);
}
static int default_setsockopt(SOCKET s, int level, int optname,
                              const char *optval, int optlen) {
  return setsockopt(s, level, optname, optval, optlen);
}
static int default_shutdown(SOCKET s, int how) { return shutdown(s, how); }
static int default_recv(SOCKET s, char *buf, int len, int flags) {
  return recv(s, buf, len, flags);
}
static int default_send(SOCKET s, const char *buf, int len, int flags) {
  return send(s, buf, len, flags);
}
static int default_select(int nfds, fd_set *readfds, fd_set *writefds,
                          fd_set *exceptfds, const struct timeval *timeout) {
  return select(nfds, readfds, writefds, exceptfds, timeout);
}
static int default_wsa_recv(SOCKET s, LPWSABUF buffers, DWORD buffer_count,
                            LPDWORD bytes_recvd, LPDWORD flags,
                            LPWSAOVERLAPPED overlapped,
                            LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  return WSARecv(s, buffers, buffer_count, bytes_recvd, flags, overlapped,
                 comp);
}
static int default_wsa_recv_from(SOCKET s, LPWSABUF buffers, DWORD buffer_count,
                                 LPDWORD bytes_recvd, LPDWORD flags,
                                 struct sockaddr *from, LPINT fromlen,
                                 LPWSAOVERLAPPED overlapped,
                                 LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  return WSARecvFrom(s, buffers, buffer_count, bytes_recvd, flags, from,
                     fromlen, overlapped, comp);
}
static int default_wsa_send(SOCKET s, LPWSABUF buffers, DWORD buffer_count,
                            LPDWORD bytes_sent, DWORD flags,
                            LPWSAOVERLAPPED overlapped,
                            LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  return WSASend(s, buffers, buffer_count, bytes_sent, flags, overlapped, comp);
}
static int default_wsa_send_to(SOCKET s, LPWSABUF buffers, DWORD buffer_count,
                               LPDWORD bytes_sent, DWORD flags,
                               const struct sockaddr *to, int tolen,
                               LPWSAOVERLAPPED overlapped,
                               LPWSAOVERLAPPED_COMPLETION_ROUTINE comp) {
  return WSASendTo(s, buffers, buffer_count, bytes_sent, flags, to, tolen,
                   overlapped, comp);
}
static int default_getaddrinfo(PCSTR nodename, PCSTR servname,
                               const ADDRINFOA *hints, PADDRINFOA *res) {
  return getaddrinfo(nodename, servname, hints, res);
}
static void default_freeaddrinfo(PADDRINFOA ai) { freeaddrinfo(ai); }
static struct servent *default_getservbyname(const char *name,
                                             const char *proto) {
  return getservbyname(name, proto);
}
static struct servent *default_getservbyport(int port, const char *proto) {
  return getservbyport(port, proto);
}
static unsigned long default_get_file_type(void *file_handle) {
  return GetFileType(file_handle);
}
static int default_peek_named_pipe(void *named_pipe, void *buffer,
                                   unsigned long buffer_size,
                                   unsigned long *bytes_read,
                                   unsigned long *total_bytes_avail,
                                   unsigned long *bytes_left_this_message) {
  return PeekNamedPipe(named_pipe, buffer, buffer_size, bytes_read,
                       total_bytes_avail, bytes_left_this_message);
}
static void default_sleep(DWORD milliseconds) { Sleep(milliseconds); }

static const struct posix_sockets_win32_hooks k_default_sock_hooks = {
    default_sock_malloc,
    default_sock_calloc,
    default_sock_free,
    default_sock_get_osfhandle,
    default_sock_open_osfhandle,
    default_sock_close,
    default_sock_read,
    default_sock_write,
    default_wsa_startup,
    default_wsa_get_last_error,
    default_wsa_socket_w,
    default_closesocket,
    default_ioctlsocket,
    default_bind,
    default_getsockname,
    default_getpeername,
    default_listen,
    default_accept,
    default_connect,
    default_getsockopt,
    default_setsockopt,
    default_shutdown,
    default_recv,
    default_send,
    default_select,
    default_wsa_recv,
    default_wsa_recv_from,
    default_wsa_send,
    default_wsa_send_to,
    default_getaddrinfo,
    default_freeaddrinfo,
    default_getservbyname,
    default_getservbyport,
    default_get_file_type,
    default_peek_named_pipe,
    default_sleep};

static struct posix_sockets_win32_hooks g_sock_hooks = {
    default_sock_malloc,
    default_sock_calloc,
    default_sock_free,
    default_sock_get_osfhandle,
    default_sock_open_osfhandle,
    default_sock_close,
    default_sock_read,
    default_sock_write,
    default_wsa_startup,
    default_wsa_get_last_error,
    default_wsa_socket_w,
    default_closesocket,
    default_ioctlsocket,
    default_bind,
    default_getsockname,
    default_getpeername,
    default_listen,
    default_accept,
    default_connect,
    default_getsockopt,
    default_setsockopt,
    default_shutdown,
    default_recv,
    default_send,
    default_select,
    default_wsa_recv,
    default_wsa_recv_from,
    default_wsa_send,
    default_wsa_send_to,
    default_getaddrinfo,
    default_freeaddrinfo,
    default_getservbyname,
    default_getservbyport,
    default_get_file_type,
    default_peek_named_pipe,
    default_sleep};

void posix_sockets_get_default_win32_hooks(
    struct posix_sockets_win32_hooks *out_hooks) {
  if (out_hooks != NULL) {
    *out_hooks = k_default_sock_hooks;
  }
}

void posix_sockets_set_win32_hooks(
    const struct posix_sockets_win32_hooks *hooks) {
  if (hooks != NULL) {
    g_sock_hooks = *hooks;
  } else {
    g_sock_hooks = k_default_sock_hooks;
  }
}
#endif /* _WIN32 */

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1400
static void sock_null_invalid_param_handler(const wchar_t *expression,
                                            const wchar_t *function,
                                            const wchar_t *file,
                                            unsigned int line,
                                            uintptr_t pReserved) {
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
}
#endif
static ptrdiff_t safe_get_osfhandle(intptr_t fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
  if (fd < 0 || fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(sock_null_invalid_param_handler);
    AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
    h = (ptrdiff_t)g_sock_hooks.get_osfhandle_fn((int)fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)g_sock_hooks.get_osfhandle_fn((int)fd);
#endif
  errno = saved_err;
  if (h == -2) {
    return -1;
  }
  return h;
}
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif

#if defined(_WIN32)
extern int is_socket(intptr_t fd);
intptr_t posix_get_dup_socket(intptr_t fd);
void posix_set_dup_socket(intptr_t fd, intptr_t s);
intptr_t posix_lookup_crt_socket(intptr_t fd);
void posix_register_crt_socket(intptr_t fd, intptr_t s);

static SOCKET resolve_socket_or_fd_sock(intptr_t fd) {
  intptr_t s = posix_get_dup_socket(fd);
  if (s != 0) {
    return (SOCKET)s;
  }
  s = posix_lookup_crt_socket(fd);
  if (s != 0) {
    return (SOCKET)s;
  }
  return (SOCKET)fd;
}
#define GET_SOCKET(fd) resolve_socket_or_fd_sock(fd)
#else
#define GET_SOCKET(fd) (fd)
#endif

#ifdef _WIN32
static int _wsaErrorToErrno(int err) {
  switch (err) {
  case WSAEWOULDBLOCK:
    return EWOULDBLOCK;
  case WSAEINPROGRESS:
    return EINPROGRESS;
  case WSAEALREADY:
    return EALREADY;
  case WSAENOTSOCK:
    return ENOTSOCK;
  case WSAEDESTADDRREQ:
    return EDESTADDRREQ;
  case WSAEMSGSIZE:
    return EMSGSIZE;
  case WSAEPROTOTYPE:
    return EPROTOTYPE;
  case WSAENOPROTOOPT:
    return ENOPROTOOPT;
  case WSAEPROTONOSUPPORT:
    return EPROTONOSUPPORT;
  case WSAEOPNOTSUPP:
    return EOPNOTSUPP;
  case WSAEAFNOSUPPORT:
    return EAFNOSUPPORT;
  case WSAEADDRINUSE:
    return EADDRINUSE;
  case WSAEADDRNOTAVAIL:
    return EADDRNOTAVAIL;
  case WSAENETDOWN:
    return ENETDOWN;
  case WSAENETUNREACH:
    return ENETUNREACH;
  case WSAENETRESET:
    return ENETRESET;
  case WSAECONNABORTED:
    return ECONNABORTED;
  case WSAECONNRESET:
    return ECONNRESET;
  case WSAENOBUFS:
    return ENOBUFS;
  case WSAEISCONN:
    return EISCONN;
  case WSAENOTCONN:
    return ENOTCONN;
  case WSAETIMEDOUT:
    return ETIMEDOUT;
  case WSAECONNREFUSED:
    return ECONNREFUSED;
  case WSAELOOP:
    return ELOOP;
  case WSAENAMETOOLONG:
    return ENAMETOOLONG;
  case WSAEHOSTUNREACH:
    return EHOSTUNREACH;
  case WSAENOTEMPTY:
    return ENOTEMPTY;
  default:
    return EINVAL;
  }
}

static void __cdecl null_invalid_parameter_handler_auto(const wchar_t *a,
                                                        const wchar_t *b,
                                                        const wchar_t *c,
                                                        unsigned int d,
                                                        uintptr_t e) {
  (void)a;
  (void)b;
  (void)c;
  (void)d;
  (void)e;
}

static void __cdecl __init_winsock_auto(void) {
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  _set_invalid_parameter_handler(
      (_invalid_parameter_handler)null_invalid_parameter_handler_auto);
  null_invalid_parameter_handler_auto(NULL, NULL, NULL, 0, 0);
}

#if defined(_MSC_VER)
#pragma push_macro("read")
#undef read
#pragma section(".CRT$XCU", read)
#pragma pop_macro("read")
__declspec(allocate(".CRT$XCU")) void(__cdecl *__init_winsock_ptr)(void) =
    __init_winsock_auto;

#if defined(_WIN64)
#pragma comment(linker, "/include:__init_winsock_ptr")
#else
#pragma comment(linker, "/include:___init_winsock_ptr")
#endif
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((constructor)) static void __init_winsock_auto_gcc(void) {
  __init_winsock_auto();
}
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK 0x4000
#endif

static SOCKET nb_sockets[8192] = {0};

/**
 * @brief Sets or clears tracked non-blocking state for a Winsock SOCKET.
 * @param[in] s Winsock SOCKET handle.
 * @param[in] nb Non-zero to mark non-blocking, 0 to clear.
 */
void set_nonblock(SOCKET s, int nb);
void set_nonblock(SOCKET s, int nb) {
  int i;
  for (i = 0; i < 8192; i++) {
    if (nb_sockets[i] == s) {
      if (!nb) {
        nb_sockets[i] = 0;
      }
      return;
    }
  }
  if (nb) {
    for (i = 0; i < 8192; i++) {
      if (nb_sockets[i] == 0) {
        nb_sockets[i] = s;
        return;
      }
    }
  }
}

/**
 * @brief Queries tracked non-blocking flag for a Winsock SOCKET.
 * @param[in] s Winsock SOCKET handle.
 * @return O_NONBLOCK when tracked as non-blocking, or 0 otherwise.
 */
int get_nonblock(SOCKET s);
int get_nonblock(SOCKET s) {
  int i;
  for (i = 0; i < 8192; i++) {
    if (nb_sockets[i] == s) {
      return O_NONBLOCK;
    }
  }
  return 0;
}

/**
 * @brief Clears tracked non-blocking state for a Winsock SOCKET.
 * @param[in] s Winsock SOCKET handle.
 * @return ERR_NONE.
 */
error_type_t clear_nonblock(SOCKET s);
error_type_t clear_nonblock(SOCKET s) {
  set_nonblock(s, 0);
  return ERR_NONE;
}

/**
 * @brief Copies tracked non-blocking state from src to dst SOCKET.
 * @param[in] src Source SOCKET handle.
 * @param[in] dst Destination SOCKET handle.
 */
void copy_nonblock(SOCKET src, SOCKET dst);
void copy_nonblock(SOCKET src, SOCKET dst) {
  if (get_nonblock(src)) {
    set_nonblock(dst, 1);
  }
}

static unsigned char g_is_socket[65536] = {0};
static intptr_t g_dup_sockets[64] = {0};
static intptr_t g_crt_sockets[65536] = {0};

/**
 * @brief Retrieves duplicated SOCKET handle mapped to a low descriptor.
 * @param[in] fd Descriptor index.
 * @return Mapped SOCKET handle, or 0 if none.
 */
intptr_t posix_get_dup_socket(intptr_t fd) {
  if (fd >= 0 && fd < 64) {
    return g_dup_sockets[fd];
  }
  return 0;
}

/**
 * @brief Associates a duplicated SOCKET handle with a low descriptor.
 * @param[in] fd Descriptor index.
 * @param[in] s SOCKET handle (or 0 to clear).
 */
void posix_set_dup_socket(intptr_t fd, intptr_t s) {
  if (fd >= 0 && fd < 64) {
    g_dup_sockets[fd] = s;
  }
}

/**
 * @brief Looks up a CRT descriptor's registered SOCKET handle.
 * @param[in] fd Descriptor index.
 * @return Registered SOCKET handle, or 0 if none.
 */
intptr_t posix_lookup_crt_socket(intptr_t fd) {
  if (fd >= 0 && fd < 65536) {
    return g_crt_sockets[fd];
  }
  return 0;
}

/**
 * @brief Registers a SOCKET handle for a CRT descriptor.
 * @param[in] fd Descriptor index.
 * @param[in] s SOCKET handle (or 0 to clear).
 */
void posix_register_crt_socket(intptr_t fd, intptr_t s) {
  if (fd >= 0 && fd < 65536) {
    g_crt_sockets[fd] = s;
  }
}

/**
 * @brief Marks a descriptor as a socket in the descriptor table.
 * @param[in] fd Descriptor index.
 * @return ERR_NONE.
 */
error_type_t mark_as_socket(intptr_t fd);
error_type_t mark_as_socket(intptr_t fd) {
  if (fd >= 0 && fd < 65536) {
    g_is_socket[fd] = 1;
  }
  return ERR_NONE;
}

/**
 * @brief Clears socket marker for a descriptor in the descriptor table.
 * @param[in] fd Descriptor index.
 * @return ERR_NONE.
 */
error_type_t clear_as_socket(intptr_t fd);
error_type_t clear_as_socket(intptr_t fd) {
  if (fd >= 0 && fd < 65536) {
    g_is_socket[fd] = 0;
  }
  return ERR_NONE;
}

/**
 * @brief Checks whether a descriptor is tracked as a socket.
 * @param[in] fd Descriptor index.
 * @return Non-zero if socket, 0 otherwise.
 */
int is_socket(intptr_t fd) {
  if (fd >= 0 && fd < 65536) {
    return g_is_socket[fd];
  }
  return 1;
}
#endif /* _WIN32 */

/** @brief Closes the host database connection (no-op stub). */
void posix_endhostent(void) { return; }

/** @brief Closes the network database connection (no-op stub). */
void posix_endnetent(void) { return; }

/** @brief Closes the protocol database connection (no-op stub). */
void posix_endprotoent(void) { return; }

/** @brief Closes the services database connection (no-op stub). */
void posix_endservent(void) { return; }

#define POSIX_AI_CUSTOM_MAGIC 0x41574D53

/**
 * @brief Frees address information allocated by posix_getaddrinfo.
 * @param[in] ai Address info list to free.
 */
void posix_freeaddrinfo(struct addrinfo *ai) {
  (void)ai;
#ifdef _WIN32
  if (ai == NULL) {
    return;
  }
  if (ai->ai_flags == POSIX_AI_CUSTOM_MAGIC) {
    g_sock_hooks.free_fn(ai);
    return;
  }
  g_sock_hooks.freeaddrinfo_fn(ai);
#endif
}

/**
 * @brief Translates a getaddrinfo error code into a human-readable string.
 * @return Static descriptive string.
 */
const char *posix_gai_strerror(int ecode) {
#ifdef _WIN32
  return gai_strerrorA(ecode);
#else
  static char buf[64];
#if defined(_MSC_VER)
  sprintf_s(buf, sizeof(buf), "Unknown error %d", (int)ecode);
#else
  sprintf(buf, "Unknown error %d", (int)ecode);
#endif
  return buf;
#endif
}

/**
 * @brief Resolves host and service names to socket addresses.
 * @return 0 on success, or non-zero error code.
 */
int posix_getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints, struct addrinfo **res) {
#ifdef _WIN32
  int ret;
  if (nodename != NULL && nodename[0] >= '0' && nodename[0] <= '9') {
    const char *p = nodename;
    unsigned int parts[4] = {0, 0, 0, 0};
    int part_idx = 0;
    int has_digit = 0;
    int valid_ipv4 = 1;
    while (*p) {
      if (*p >= '0' && *p <= '9') {
        parts[part_idx] = parts[part_idx] * 10U + (unsigned int)(*p - '0');
        if (parts[part_idx] > 255U) {
          valid_ipv4 = 0;
          break;
        }
        has_digit = 1;
      } else if (*p == '.') {
        if (!has_digit || part_idx >= 3) {
          valid_ipv4 = 0;
          break;
        }
        part_idx++;
        has_digit = 0;
      } else {
        valid_ipv4 = 0;
        break;
      }
      p++;
    }
    if (valid_ipv4 && has_digit && part_idx == 3) {
      unsigned long ip_host =
          (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
      unsigned long ip_be = htonl(ip_host);
      struct addrinfo *ai = (struct addrinfo *)g_sock_hooks.calloc_fn(
          1, sizeof(struct addrinfo) + sizeof(struct sockaddr_in));
      if (ai != NULL && res != NULL) {
        struct sockaddr_in *sin = (struct sockaddr_in *)(ai + 1);
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = ip_be;
        if (servname != NULL && servname[0] >= '0' && servname[0] <= '9') {
          sin->sin_port = htons((unsigned short)atoi(servname));
        }
        ai->ai_flags = POSIX_AI_CUSTOM_MAGIC;
        ai->ai_family = AF_INET;
        ai->ai_socktype = (hints != NULL && hints->ai_socktype != 0)
                              ? hints->ai_socktype
                              : SOCK_STREAM;
        ai->ai_protocol = (hints != NULL) ? hints->ai_protocol : 0;
        ai->ai_addrlen = sizeof(struct sockaddr_in);
        ai->ai_addr = (struct sockaddr *)sin;
        ai->ai_canonname = NULL;
        ai->ai_next = NULL;
        *res = ai;
        return 0;
      }
      if (ai != NULL) {
        g_sock_hooks.free_fn(ai);
      }
    }
  }
  ret = g_sock_hooks.getaddrinfo_fn(nodename, servname, hints, res);
  if (ret != 0) {
    if (ret == WSANOTINITIALISED ||
        g_sock_hooks.wsa_get_last_error_fn() == WSANOTINITIALISED) {
      WSADATA wsaData;
      if (g_sock_hooks.wsa_startup_fn(MAKEWORD(2, 2), &wsaData) == 0) {
        ret = g_sock_hooks.getaddrinfo_fn(nodename, servname, hints, res);
      }
    }
#ifdef AI_ADDRCONFIG
    if (ret != 0 && hints != NULL && (hints->ai_flags & AI_ADDRCONFIG)) {
      struct addrinfo fallback_hints = *hints;
      fallback_hints.ai_flags &= ~AI_ADDRCONFIG;
      ret =
          g_sock_hooks.getaddrinfo_fn(nodename, servname, &fallback_hints, res);
    }
#endif
    if (ret != 0) {
      errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    }
  }
  return ret;
#else
  (void)hints;
  (void)nodename;
  (void)servname;
  (void)res;
  errno = EINVAL;
  return -1;
#endif
}

/** @brief Looks up host entry by address (stub returning NULL with EINVAL). */
struct hostent *posix_gethostbyaddr(const void *addr, posix_socklen_t len,
                                    int type) {
  (void)addr;
  (void)len;
  (void)type;
  errno = EINVAL;
  return NULL;
}

/** @brief Looks up host entry by name (stub returning NULL with EINVAL). */
struct hostent *posix_gethostbyname(const char *name) {
  (void)name;
  errno = EINVAL;
  return NULL;
}

/** @brief Reads next host database entry (stub returning NULL with EINVAL). */
struct hostent *posix_gethostent(void) {
  errno = EINVAL;
  return NULL;
}

/** @brief Reverse-resolves socket address to host and service names (stub). */
int posix_getnameinfo(const struct sockaddr *sa, posix_socklen_t salen,
                      char *node, posix_socklen_t nodelen, char *service,
                      posix_socklen_t servicelen, int flags) {
  if (!sa || !node || !service) {
    errno = EINVAL;
    return -1;
  }
  (void)salen;
  (void)nodelen;
  (void)servicelen;
  (void)flags;
  errno = EINVAL;
  return -1;
}

/** @brief Looks up network entry by number (stub returning NULL with EINVAL).
 */
struct netent *posix_getnetbyaddr(uint32_t net, int type) {
  (void)net;
  (void)type;
  errno = EINVAL;
  return NULL;
}

/** @brief Looks up network entry by name (stub returning NULL with EINVAL). */
struct netent *posix_getnetbyname(const char *name) {
  (void)name;
  errno = EINVAL;
  return NULL;
}

/** @brief Reads next network database entry (stub returning NULL with EINVAL).
 */
struct netent *posix_getnetent(void) {
  errno = EINVAL;
  return NULL;
}

/** @brief Looks up protocol entry by name (stub returning NULL with EINVAL). */
struct protoent *posix_getprotobyname(const char *name) {
  (void)name;
  errno = EINVAL;
  return NULL;
}

/** @brief Looks up protocol entry by number (stub returning NULL with EINVAL).
 */
struct protoent *posix_getprotobynumber(int proto) {
  (void)proto;
  errno = EINVAL;
  return NULL;
}

/** @brief Reads next protocol database entry (stub returning NULL with EINVAL).
 */
struct protoent *posix_getprotoent(void) {
  errno = EINVAL;
  return NULL;
}

/**
 * @brief Looks up service entry by name and protocol.
 * @param[in] name Service name.
 * @param[in] proto Protocol name (or NULL).
 * @return Pointer to servent structure, or NULL on failure.
 */
struct servent *posix_getservbyname(const char *name, const char *proto) {
#ifdef _WIN32
  static struct servent s_se;
  static char *s_aliases[1] = {NULL};
  static char s_name_buf[64];
  static char s_proto_buf[16];
  if (name != NULL) {
    unsigned short p = 0;
    if (strcmp(name, "http") == 0) {
      p = 80;
    } else if (strcmp(name, "https") == 0) {
      p = 443;
    } else if (strcmp(name, "ftp") == 0) {
      p = 21;
    } else if (strcmp(name, "telnet") == 0) {
      p = 23;
    } else if (strcmp(name, "ssh") == 0) {
      p = 22;
    } else if (strcmp(name, "domain") == 0) {
      p = 53;
    } else if (strcmp(name, "tftp") == 0) {
      p = 69;
    } else if (strcmp(name, "ntp") == 0) {
      p = 123;
    }
    if (p != 0) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
      strncpy_s(s_name_buf, sizeof(s_name_buf), name, _TRUNCATE);
      strncpy_s(s_proto_buf, sizeof(s_proto_buf), proto != NULL ? proto : "tcp",
                _TRUNCATE);
#else
      strncpy(s_name_buf, name, sizeof(s_name_buf) - 1);
      s_name_buf[sizeof(s_name_buf) - 1] = '\0';
      strncpy(s_proto_buf, proto != NULL ? proto : "tcp",
              sizeof(s_proto_buf) - 1);
      s_proto_buf[sizeof(s_proto_buf) - 1] = '\0';
#endif
      s_se.s_name = s_name_buf;
      s_se.s_aliases = s_aliases;
      s_se.s_port = (short)htons(p);
      s_se.s_proto = s_proto_buf;
      return &s_se;
    }
  }
  {
    struct servent *se = g_sock_hooks.getservbyname_fn(name, proto);
    if (se == NULL &&
        g_sock_hooks.wsa_get_last_error_fn() == WSANOTINITIALISED) {
      WSADATA wsaData;
      if (g_sock_hooks.wsa_startup_fn(MAKEWORD(2, 2), &wsaData) == 0) {
        se = g_sock_hooks.getservbyname_fn(name, proto);
      }
    }
    return se;
  }
#else
  (void)name;
  (void)proto;
  errno = EINVAL;
  return NULL;
#endif
}

/**
 * @brief Looks up service entry by port and protocol.
 * @param[in] port Port number in network byte order.
 * @param[in] proto Protocol name (or NULL).
 * @return Pointer to servent structure, or NULL on failure.
 */
struct servent *posix_getservbyport(int port, const char *proto) {
#ifdef _WIN32
  struct servent *se = g_sock_hooks.getservbyport_fn(port, proto);
  if (se == NULL && g_sock_hooks.wsa_get_last_error_fn() == WSANOTINITIALISED) {
    WSADATA wsaData;
    if (g_sock_hooks.wsa_startup_fn(MAKEWORD(2, 2), &wsaData) == 0) {
      se = g_sock_hooks.getservbyport_fn(port, proto);
    }
  }
  return se;
#else
  (void)port;
  (void)proto;
  errno = EINVAL;
  return NULL;
#endif
}

/** @brief Reads next service database entry (stub returning NULL with EINVAL).
 */
struct servent *posix_getservent(void) {
  errno = EINVAL;
  return NULL;
}

/** @brief Opens/rewinds host database (no-op stub). */
void posix_sethostent(int stayopen) {
  (void)stayopen;
  return;
}

/** @brief Opens/rewinds network database (no-op stub). */
void posix_setnetent(int stayopen) {
  (void)stayopen;
  return;
}

/** @brief Opens/rewinds protocol database (no-op stub). */
void posix_setprotoent(int stayopen) {
  (void)stayopen;
  return;
}

/** @brief Opens/rewinds services database (no-op stub). */
void posix_setservent(int stayopen) {
  (void)stayopen;
  return;
}

/** @brief Monitors file descriptor sets with timespec and signal mask (stub).
 */
int posix_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                  const struct timespec *timeout, const void *sigmask) {
  if (!readfds || !writefds || !errorfds || !timeout || !sigmask) {
    errno = EINVAL;
    return -1;
  }
  (void)nfds;
  errno = EINVAL;
  return -1;
}

#if defined(_WIN32)
#undef FD_ZERO
static void posix_fd_zero(fd_set *set) { set->fd_count = 0; }
#define FD_ZERO(set) posix_fd_zero((fd_set *)set)
#undef FD_SET
static void posix_fd_set(SOCKET fd, fd_set *set) {
  u_int __i;
  for (__i = 0; __i < set->fd_count; __i++) {
    if (set->fd_array[__i] == fd) {
      break;
    }
  }
  if (__i == set->fd_count) {
    if (set->fd_count < FD_SETSIZE) {
      set->fd_array[__i] = fd;
      set->fd_count++;
    }
  }
}
#define FD_SET(fd, set) posix_fd_set((SOCKET)(fd), (fd_set *)(set))
#endif

/**
 * @brief Multiplexes readiness across Winsock sockets, pipes, and disk files.
 * @param[in] nfds Highest file descriptor plus one.
 * @param[in,out] readfds Optional read descriptor set.
 * @param[in,out] writefds Optional write descriptor set.
 * @param[in,out] errorfds Optional exception descriptor set.
 * @param[in,out] timeout Optional timeout interval.
 * @return Number of ready descriptors, or -1 on error.
 */
int posix_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                 struct timeval *timeout) {
#if defined(_WIN32)
  fd_set ws_readfds = {0}, ws_writefds = {0}, ws_errorfds = {0};
  fd_set ready_non_sock_r = {0}, ready_non_sock_w = {0};
  fd_set *p_ws_readfds = NULL, *p_ws_writefds = NULL, *p_ws_errorfds = NULL;
  struct timeval zero_tv = {0, 0};
  unsigned int i;
  int num_sockets = 0;
  int num_non_sock_ready = 0;
  int ret = 0;
  (void)nfds;

  FD_ZERO(&ready_non_sock_r);
  FD_ZERO(&ready_non_sock_w);

  if (readfds) {
    FD_ZERO(&ws_readfds);
    for (i = 0; i < readfds->fd_count; i++) {
      intptr_t fd = (intptr_t)readfds->fd_array[i];
      SOCKET s = GET_SOCKET(fd);
      int optval = 0;
      int optlen = sizeof(optval);
      if (s != INVALID_SOCKET && s != 0 &&
          g_sock_hooks.getsockopt_fn(s, SOL_SOCKET, SO_TYPE, (char *)&optval,
                                     &optlen) == 0) {
        FD_SET(s, &ws_readfds);
        num_sockets++;
      } else {
        ptrdiff_t h = (fd >= 0 && fd < 64) ? safe_get_osfhandle(fd) : -1;
        if (h != -1) {
          unsigned long ftype = g_sock_hooks.get_file_type_fn((void *)h);
          if (ftype == 3 /* FILE_TYPE_PIPE */) {
            unsigned long avail = 0;
            if (!g_sock_hooks.peek_named_pipe_fn((void *)h, NULL, 0, NULL,
                                                 &avail, NULL) ||
                avail > 0) {
              FD_SET((SOCKET)fd, &ready_non_sock_r);
              num_non_sock_ready++;
            }
          } else if (ftype == 1 /* FILE_TYPE_DISK */) {
            FD_SET((SOCKET)fd, &ready_non_sock_r);
            num_non_sock_ready++;
          }
        }
      }
    }
    if (ws_readfds.fd_count > 0) {
      p_ws_readfds = &ws_readfds;
    }
  }

  if (writefds) {
    FD_ZERO(&ws_writefds);
    for (i = 0; i < writefds->fd_count; i++) {
      intptr_t fd = (intptr_t)writefds->fd_array[i];
      SOCKET s = GET_SOCKET(fd);
      int optval = 0;
      int optlen = sizeof(optval);
      if (s != INVALID_SOCKET && s != 0 &&
          g_sock_hooks.getsockopt_fn(s, SOL_SOCKET, SO_TYPE, (char *)&optval,
                                     &optlen) == 0) {
        FD_SET(s, &ws_writefds);
        num_sockets++;
      } else {
        ptrdiff_t h = (fd >= 0 && fd < 64) ? safe_get_osfhandle(fd) : -1;
        if (h != -1) {
          FD_SET((SOCKET)fd, &ready_non_sock_w);
          num_non_sock_ready++;
        }
      }
    }
    if (ws_writefds.fd_count > 0) {
      p_ws_writefds = &ws_writefds;
    }
  }

  if (errorfds) {
    FD_ZERO(&ws_errorfds);
    for (i = 0; i < errorfds->fd_count; i++) {
      intptr_t fd = (intptr_t)errorfds->fd_array[i];
      SOCKET s = GET_SOCKET(fd);
      int optval = 0;
      int optlen = sizeof(optval);
      if (s != INVALID_SOCKET && s != 0 &&
          g_sock_hooks.getsockopt_fn(s, SOL_SOCKET, SO_TYPE, (char *)&optval,
                                     &optlen) == 0) {
        FD_SET(s, &ws_errorfds);
        num_sockets++;
      }
    }
    if (ws_errorfds.fd_count > 0) {
      p_ws_errorfds = &ws_errorfds;
    }
  }

  if (num_sockets > 0) {
    ret = g_sock_hooks.select_fn(0, p_ws_readfds, p_ws_writefds, p_ws_errorfds,
                                 num_non_sock_ready > 0 ? &zero_tv : timeout);
    if (ret == SOCKET_ERROR) {
      if (num_non_sock_ready > 0) {
        ret = 0;
      } else {
        errno = EBADF;
        return -1;
      }
    }
  } else if (num_non_sock_ready == 0 && timeout != NULL) {
    g_sock_hooks.sleep_fn(
        (DWORD)(timeout->tv_sec * 1000 + timeout->tv_usec / 1000));
  }

  if (readfds) {
    fd_set orig = *readfds;
    FD_ZERO(readfds);
    for (i = 0; i < orig.fd_count; i++) {
      intptr_t fd = (intptr_t)orig.fd_array[i];
      SOCKET s = GET_SOCKET(fd);
      if ((p_ws_readfds && FD_ISSET(s, &ws_readfds)) ||
          FD_ISSET((SOCKET)fd, &ready_non_sock_r)) {
        FD_SET((SOCKET)fd, readfds);
      }
    }
  }

  if (writefds) {
    fd_set orig = *writefds;
    FD_ZERO(writefds);
    for (i = 0; i < orig.fd_count; i++) {
      intptr_t fd = (intptr_t)orig.fd_array[i];
      SOCKET s = GET_SOCKET(fd);
      if ((p_ws_writefds && FD_ISSET(s, &ws_writefds)) ||
          FD_ISSET((SOCKET)fd, &ready_non_sock_w)) {
        FD_SET((SOCKET)fd, writefds);
      }
    }
  }

  if (errorfds) {
    fd_set orig = *errorfds;
    FD_ZERO(errorfds);
    for (i = 0; i < orig.fd_count; i++) {
      intptr_t fd = (intptr_t)orig.fd_array[i];
      SOCKET s = GET_SOCKET(fd);
      if (p_ws_errorfds && FD_ISSET(s, &ws_errorfds)) {
        FD_SET((SOCKET)fd, errorfds);
      }
    }
  }

  return ret + num_non_sock_ready;
#else
  (void)nfds;
  (void)readfds;
  (void)writefds;
  (void)errorfds;
  (void)timeout;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Accepts an incoming connection on a listening socket.
 * @param[in] socket Listening socket descriptor.
 * @param[out] address Optional peer address buffer.
 * @param[in,out] address_len Optional peer address buffer length.
 * @return Accepted socket descriptor, or -1 on error.
 */
int posix_accept(intptr_t socket, struct sockaddr *address,
                 posix_socklen_t *address_len) {
#ifdef _WIN32
  SOCKET ret = g_sock_hooks.accept_fn(GET_SOCKET(socket), address, address_len);
  if (ret == INVALID_SOCKET) {
    int err = g_sock_hooks.wsa_get_last_error_fn();
    if (err == WSAEWOULDBLOCK) {
      errno = EWOULDBLOCK;
    } else {
      errno = _wsaErrorToErrno(err);
    }
    return -1;
  }

  copy_nonblock(GET_SOCKET(socket), ret);
  posix_register_crt_socket((intptr_t)ret, (intptr_t)ret);
  (void)mark_as_socket((intptr_t)ret);
  return (int)ret;
#else
  (void)socket;
  (void)address;
  (void)address_len;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Binds a socket to a local address (translating AF_UNIX to loopback).
 * @param[in] socket Socket descriptor.
 * @param[in] address Local address.
 * @param[in] address_len Local address length.
 * @return 0 on success, or -1 on error.
 */
int posix_bind(intptr_t socket, const struct sockaddr *address,
               posix_socklen_t address_len) {
#ifdef _WIN32
  int ret;
  if (address != NULL && address->sa_family == AF_UNIX) {
    struct sockaddr_in fake_addr;

    memset(&fake_addr, 0, sizeof(fake_addr));
    fake_addr.sin_family = AF_INET;
    fake_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    fake_addr.sin_port = 0;
    ret = g_sock_hooks.bind_fn(GET_SOCKET(socket),
                               (const struct sockaddr *)&fake_addr,
                               sizeof(fake_addr));
    if (ret == SOCKET_ERROR) {
      errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
      return -1;
    }
    return 0;
  }
  ret = g_sock_hooks.bind_fn(GET_SOCKET(socket), address, address_len);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return 0;
#else
  (void)socket;
  (void)address;
  (void)address_len;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Connects a socket to a remote address with loopback startup retry.
 * @param[in] socket Socket descriptor.
 * @param[in] address Remote address.
 * @param[in] address_len Remote address length.
 * @return 0 on success, or -1 on error.
 */
int posix_connect(intptr_t socket, const struct sockaddr *address,
                  posix_socklen_t address_len) {
#ifdef _WIN32
  SOCKET s;
  int ret;
  if (!address) {
    errno = EFAULT;
    return -1;
  }
  s = GET_SOCKET(socket);
  ret = g_sock_hooks.connect_fn(s, address, address_len);
  if (ret == SOCKET_ERROR &&
      g_sock_hooks.wsa_get_last_error_fn() == WSAECONNREFUSED &&
      address->sa_family == AF_INET && !get_nonblock(s)) {
    const struct sockaddr_in *sin = (const struct sockaddr_in *)address;
    if (sin->sin_addr.s_addr == htonl(0x7f000001UL) && sin->sin_port != 0) {
      int attempt;
      for (attempt = 0; attempt < 60; attempt++) {
        SOCKET new_s;
        g_sock_hooks.sleep_fn(100);
        new_s =
            g_sock_hooks.wsa_socket_w_fn(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
        if (new_s == INVALID_SOCKET) {
          break;
        }
        g_sock_hooks.closesocket_fn(s);
        s = new_s;
        posix_register_crt_socket(socket, (intptr_t)s);
        ret = g_sock_hooks.connect_fn(s, address, address_len);
        if (ret == 0 ||
            g_sock_hooks.wsa_get_last_error_fn() != WSAECONNREFUSED) {
          break;
        }
      }
    }
  }
  if (ret == SOCKET_ERROR) {
    int err = g_sock_hooks.wsa_get_last_error_fn();
    if (err == WSAEISCONN) {
      return 0;
    }
    if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS || err == WSAEALREADY) {
      errno = EINPROGRESS;
    } else {
      errno = _wsaErrorToErrno(err);
    }
    return -1;
  }
  return 0;
#else
  (void)socket;
  (void)address;
  (void)address_len;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Queries connected peer address of a socket.
 * @param[in] socket Socket descriptor.
 * @param[out] address Peer address buffer.
 * @param[in,out] address_len Peer address buffer length.
 * @return 0 on success, or -1 on error.
 */
int posix_getpeername(intptr_t socket, struct sockaddr *address,
                      posix_socklen_t *address_len) {
#ifdef _WIN32
  int ret =
      g_sock_hooks.getpeername_fn(GET_SOCKET(socket), address, address_len);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return 0;
#else
  (void)socket;
  (void)address;
  (void)address_len;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Queries bound local address of a socket.
 * @param[in] socket Socket descriptor.
 * @param[out] address Local address buffer.
 * @param[in,out] address_len Local address buffer length.
 * @return 0 on success, or -1 on error.
 */
int posix_getsockname(intptr_t socket, struct sockaddr *address,
                      posix_socklen_t *address_len) {
#ifdef _WIN32
  int ret =
      g_sock_hooks.getsockname_fn(GET_SOCKET(socket), address, address_len);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return 0;
#else
  (void)socket;
  (void)address;
  (void)address_len;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Retrieves a socket option, translating timeval and SO_ERROR values.
 * @param[in] socket Socket descriptor.
 * @param[in] level Protocol level.
 * @param[in] option_name Option identifier.
 * @param[out] option_value Option output buffer.
 * @param[in,out] option_len Option buffer length.
 * @return 0 on success, or -1 on error.
 */
int posix_getsockopt(intptr_t socket, int level, int option_name,
                     void *option_value, posix_socklen_t *option_len) {
#ifdef _WIN32
  int ret;
  if (level == SOL_SOCKET &&
      (option_name == SO_SNDTIMEO || option_name == SO_RCVTIMEO)) {
    if (option_len && *option_len == sizeof(struct timeval)) {
      DWORD ms = 0;
      int ms_len = sizeof(DWORD);
      ret = g_sock_hooks.getsockopt_fn(GET_SOCKET(socket), level, option_name,
                                       (char *)&ms, &ms_len);
      if (ret == 0) {
        struct timeval *tv = (struct timeval *)option_value;
        tv->tv_sec = (long)(ms / 1000U);
        tv->tv_usec = (long)((ms % 1000U) * 1000U);
      }
    } else {
      ret = g_sock_hooks.getsockopt_fn(GET_SOCKET(socket), level, option_name,
                                       (char *)option_value, option_len);
    }
  } else {
    ret = g_sock_hooks.getsockopt_fn(GET_SOCKET(socket), level, option_name,
                                     (char *)option_value, option_len);
    if (ret == 0 && level == SOL_SOCKET && option_name == SO_ERROR) {
      if (*((int *)option_value) != 0) {
        *((int *)option_value) = _wsaErrorToErrno(*((int *)option_value));
      }
    }
  }
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return 0;
#else
  (void)socket;
  (void)level;
  (void)option_name;
  (void)option_value;
  (void)option_len;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Marks a socket as passive to accept incoming connections.
 * @param[in] socket Socket descriptor.
 * @param[in] backlog Maximum pending connection queue length.
 * @return 0 on success, or -1 on error.
 */
int posix_listen(intptr_t socket, int backlog) {
#ifdef _WIN32
  int ret = g_sock_hooks.listen_fn(GET_SOCKET(socket), backlog);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return 0;
#else
  (void)socket;
  (void)backlog;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Receives data from a connected socket.
 * @param[in] socket Socket descriptor.
 * @param[out] buffer Destination buffer.
 * @param[in] length Maximum bytes to receive.
 * @param[in] flags Receive flags.
 * @return Bytes received, or -1 on error.
 */
posix_ssize_t posix_recv(intptr_t socket, void *buffer, size_t length,
                         int flags) {
#ifdef _WIN32
  int ret = g_sock_hooks.recv_fn(GET_SOCKET(socket), (char *)buffer,
                                 (int)length, flags);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return ret;
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
  (void)socket;
  (void)buffer;
  (void)length;
  (void)flags;
  errno = ENOSYS;
  return -1;
#else
  return (posix_ssize_t)recv((int)socket, buffer, length, flags);
#endif
}

/** @brief Receives datagram and source address (stub returning -1 with EINVAL).
 */
posix_ssize_t posix_recvfrom(intptr_t socket, void *buffer, size_t length,
                             int flags, struct sockaddr *address,
                             posix_socklen_t *address_len) {
  (void)socket;
  (void)buffer;
  (void)length;
  (void)flags;
  (void)address;
  (void)address_len;
  errno = EINVAL;
  return -1;
}

/**
 * @brief Receives scatter/gather messages from a socket or CRT file descriptor.
 * @param[in] socket Socket or CRT file descriptor.
 * @param[in,out] message Message header with iovec array.
 * @param[in] flags Receive flags.
 * @return Total bytes received, or -1 on error.
 */
posix_ssize_t posix_recvmsg(intptr_t socket, struct msghdr *message,
                            int flags) {
#ifdef _WIN32
  WSABUF stack_bufs[16];
  WSABUF *bufs;
  SOCKET s;
  int optval;
  int optlen;
  int i;
  DWORD bytes_recvd;
  DWORD dwFlags;
  int rc;

  if (!message || message->msg_iovlen < 0 ||
      (message->msg_iovlen > 0 && !message->msg_iov)) {
    errno = EINVAL;
    return -1;
  }
  if (socket < 0) {
    errno = EBADF;
    return -1;
  }

  if (message->msg_iovlen == 0) {
    message->msg_flags = 0;
    return 0;
  }

  bufs = stack_bufs;
  if (message->msg_iovlen > 16) {
    bufs = (WSABUF *)g_sock_hooks.malloc_fn(sizeof(WSABUF) *
                                            (size_t)message->msg_iovlen);
    if (!bufs) {
      errno = ENOMEM;
      return -1;
    }
  }

  for (i = 0; i < message->msg_iovlen; i++) {
    bufs[i].buf = (char *)message->msg_iov[i].iov_base;
    bufs[i].len = (ULONG)message->msg_iov[i].iov_len;
  }

  s = GET_SOCKET(socket);
  optlen = sizeof(optval);
  if (g_sock_hooks.getsockopt_fn(s, SOL_SOCKET, SO_TYPE, (char *)&optval,
                                 &optlen) == SOCKET_ERROR &&
      g_sock_hooks.wsa_get_last_error_fn() == WSAENOTSOCK) {
    posix_ssize_t total = 0;
    for (i = 0; i < message->msg_iovlen; i++) {
      int r = g_sock_hooks.read_fn((int)socket, message->msg_iov[i].iov_base,
                                   (unsigned int)message->msg_iov[i].iov_len);
      if (r < 0) {
        if (bufs != stack_bufs) {
          g_sock_hooks.free_fn(bufs);
        }
        return total > 0 ? total : -1;
      }
      total += r;
      if ((size_t)r < message->msg_iov[i].iov_len || r == 0) {
        break;
      }
    }
    if (bufs != stack_bufs) {
      g_sock_hooks.free_fn(bufs);
    }
    message->msg_flags = 0;
    return total;
  }

  bytes_recvd = 0;
  dwFlags = (DWORD)flags;
  if (message->msg_name != NULL && message->msg_namelen > 0) {
    INT fromlen = (INT)message->msg_namelen;
    rc = g_sock_hooks.wsa_recv_from_fn(
        s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd, &dwFlags,
        (struct sockaddr *)message->msg_name, &fromlen, NULL, NULL);
    if (rc == 0 || g_sock_hooks.wsa_get_last_error_fn() != WSAEMSGSIZE) {
      message->msg_namelen = (posix_socklen_t)fromlen;
    }
  } else {
    rc = g_sock_hooks.wsa_recv_fn(s, bufs, (DWORD)message->msg_iovlen,
                                  &bytes_recvd, &dwFlags, NULL, NULL);
  }

  if (bufs != stack_bufs) {
    g_sock_hooks.free_fn(bufs);
  }

  if (rc == SOCKET_ERROR) {
    int err = g_sock_hooks.wsa_get_last_error_fn();
    if (err == WSAEMSGSIZE) {
      message->msg_flags = (int)dwFlags | 0x20;
      return (posix_ssize_t)bytes_recvd;
    }
    errno = _wsaErrorToErrno(err);
    return -1;
  }
  message->msg_flags = (int)dwFlags;
  return (posix_ssize_t)bytes_recvd;
#else
  (void)socket;
  (void)message;
  (void)flags;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Sends data over a connected socket.
 * @param[in] socket Socket descriptor.
 * @param[in] message Source buffer.
 * @param[in] length Number of bytes to send.
 * @param[in] flags Send flags.
 * @return Bytes sent, or -1 on error.
 */
posix_ssize_t posix_send(intptr_t socket, const void *message, size_t length,
                         int flags) {
#ifdef _WIN32
  int ret = g_sock_hooks.send_fn(GET_SOCKET(socket), (const char *)message,
                                 (int)length, flags);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return ret;
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
  (void)socket;
  (void)message;
  (void)length;
  (void)flags;
  errno = ENOSYS;
  return -1;
#else
  return (posix_ssize_t)send((int)socket, message, length, flags);
#endif
}

/**
 * @brief Sends scatter/gather messages over a socket or CRT file descriptor.
 * @param[in] socket Socket or CRT file descriptor.
 * @param[in] message Message header with iovec array.
 * @param[in] flags Send flags.
 * @return Total bytes sent, or -1 on error.
 */
posix_ssize_t posix_sendmsg(intptr_t socket, const struct msghdr *message,
                            int flags) {
#ifdef _WIN32
  WSABUF stack_bufs[16];
  WSABUF *bufs;
  SOCKET s;
  int optval;
  int optlen;
  int i;
  DWORD bytes_sent;
  int rc;

  if (!message || message->msg_iovlen < 0 ||
      (message->msg_iovlen > 0 && !message->msg_iov)) {
    errno = EINVAL;
    return -1;
  }
  if (socket < 0) {
    errno = EBADF;
    return -1;
  }

  if (message->msg_iovlen == 0) {
    return 0;
  }

  bufs = stack_bufs;
  if (message->msg_iovlen > 16) {
    bufs = (WSABUF *)g_sock_hooks.malloc_fn(sizeof(WSABUF) *
                                            (size_t)message->msg_iovlen);
    if (!bufs) {
      errno = ENOMEM;
      return -1;
    }
  }

  for (i = 0; i < message->msg_iovlen; i++) {
    bufs[i].buf = (char *)message->msg_iov[i].iov_base;
    bufs[i].len = (ULONG)message->msg_iov[i].iov_len;
  }

  s = GET_SOCKET(socket);
  optlen = sizeof(optval);
  if (g_sock_hooks.getsockopt_fn(s, SOL_SOCKET, SO_TYPE, (char *)&optval,
                                 &optlen) == SOCKET_ERROR &&
      g_sock_hooks.wsa_get_last_error_fn() == WSAENOTSOCK) {
    posix_ssize_t total = 0;
    for (i = 0; i < message->msg_iovlen; i++) {
      int w = g_sock_hooks.write_fn((int)socket, message->msg_iov[i].iov_base,
                                    (unsigned int)message->msg_iov[i].iov_len);
      if (w < 0) {
        if (bufs != stack_bufs) {
          g_sock_hooks.free_fn(bufs);
        }
        return total > 0 ? total : -1;
      }
      total += w;
      if ((size_t)w < message->msg_iov[i].iov_len) {
        break;
      }
    }
    if (bufs != stack_bufs) {
      g_sock_hooks.free_fn(bufs);
    }
    return total;
  }

  bytes_sent = 0;
  if (message->msg_name != NULL && message->msg_namelen > 0) {
    rc = g_sock_hooks.wsa_send_to_fn(s, bufs, (DWORD)message->msg_iovlen,
                                     &bytes_sent, (DWORD)flags,
                                     (const struct sockaddr *)message->msg_name,
                                     (int)message->msg_namelen, NULL, NULL);
  } else {
    rc = g_sock_hooks.wsa_send_fn(s, bufs, (DWORD)message->msg_iovlen,
                                  &bytes_sent, (DWORD)flags, NULL, NULL);
  }

  if (bufs != stack_bufs) {
    g_sock_hooks.free_fn(bufs);
  }

  if (rc == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return (posix_ssize_t)bytes_sent;
#else
  (void)socket;
  (void)message;
  (void)flags;
  errno = EINVAL;
  return -1;
#endif
}

/* Native Winsock sendmsg helper operating directly on SOCKET handles. */
posix_ssize_t posix_sendmsg_native(uintptr_t s, const struct msghdr *message,
                                   int flags) {
#ifdef _WIN32
  WSABUF stack_bufs[16];
  WSABUF *bufs;
  DWORD bytes_sent;
  int rc;
  int i;

  if (!message || message->msg_iovlen < 0 ||
      (message->msg_iovlen > 0 && !message->msg_iov)) {
    errno = EINVAL;
    return -1;
  }
  if (s == (uintptr_t)-1) {
    errno = EBADF;
    return -1;
  }

  if (message->msg_iovlen == 0) {
    return 0;
  }

  bufs = stack_bufs;
  if (message->msg_iovlen > 16) {
    bufs = (WSABUF *)g_sock_hooks.malloc_fn(sizeof(WSABUF) *
                                            (size_t)message->msg_iovlen);
    if (!bufs) {
      errno = ENOMEM;
      return -1;
    }
  }

  for (i = 0; i < message->msg_iovlen; i++) {
    bufs[i].buf = (char *)message->msg_iov[i].iov_base;
    bufs[i].len = (ULONG)message->msg_iov[i].iov_len;
  }

  bytes_sent = 0;
  if (message->msg_name != NULL && message->msg_namelen > 0) {
    rc = g_sock_hooks.wsa_send_to_fn(
        (SOCKET)s, bufs, (DWORD)message->msg_iovlen, &bytes_sent, (DWORD)flags,
        (const struct sockaddr *)message->msg_name, (int)message->msg_namelen,
        NULL, NULL);
  } else {
    rc = g_sock_hooks.wsa_send_fn((SOCKET)s, bufs, (DWORD)message->msg_iovlen,
                                  &bytes_sent, (DWORD)flags, NULL, NULL);
  }

  if (rc == SOCKET_ERROR &&
      g_sock_hooks.wsa_get_last_error_fn() == WSAENOTSOCK) {
    SOCKET real_s = (SOCKET)safe_get_osfhandle((intptr_t)s);
    if (real_s != (SOCKET)-1 && real_s != 0) {
      if (message->msg_name != NULL && message->msg_namelen > 0) {
        rc = g_sock_hooks.wsa_send_to_fn(
            real_s, bufs, (DWORD)message->msg_iovlen, &bytes_sent, (DWORD)flags,
            (const struct sockaddr *)message->msg_name,
            (int)message->msg_namelen, NULL, NULL);
      } else {
        rc = g_sock_hooks.wsa_send_fn(real_s, bufs, (DWORD)message->msg_iovlen,
                                      &bytes_sent, (DWORD)flags, NULL, NULL);
      }
    }
  }

  if (bufs != stack_bufs) {
    g_sock_hooks.free_fn(bufs);
  }

  if (rc == SOCKET_ERROR) {
    int err = g_sock_hooks.wsa_get_last_error_fn();
    if (err == WSAEWOULDBLOCK) {
      errno = 140;
    } else if (err == WSAECONNRESET) {
      errno = 104;
    } else {
      errno = _wsaErrorToErrno(err);
    }
    return -1;
  }
  return (posix_ssize_t)bytes_sent;
#else
  (void)s;
  (void)message;
  (void)flags;
  errno = EINVAL;
  return -1;
#endif
}

/* Native Winsock recvmsg helper operating directly on SOCKET handles. */
posix_ssize_t posix_recvmsg_native(uintptr_t s, struct msghdr *message,
                                   int flags) {
#ifdef _WIN32
  WSABUF stack_bufs[16];
  WSABUF *bufs;
  DWORD bytes_recvd;
  DWORD dwFlags;
  int rc;
  int i;

  if (!message || message->msg_iovlen < 0 ||
      (message->msg_iovlen > 0 && !message->msg_iov)) {
    errno = EINVAL;
    return -1;
  }
  if (s == (uintptr_t)-1) {
    errno = EBADF;
    return -1;
  }

  if (message->msg_iovlen == 0) {
    return 0;
  }

  bufs = stack_bufs;
  if (message->msg_iovlen > 16) {
    bufs = (WSABUF *)g_sock_hooks.malloc_fn(sizeof(WSABUF) *
                                            (size_t)message->msg_iovlen);
    if (!bufs) {
      errno = ENOMEM;
      return -1;
    }
  }

  for (i = 0; i < message->msg_iovlen; i++) {
    bufs[i].buf = (char *)message->msg_iov[i].iov_base;
    bufs[i].len = (ULONG)message->msg_iov[i].iov_len;
  }

  bytes_recvd = 0;
  dwFlags = (DWORD)flags;
  if (message->msg_name != NULL && message->msg_namelen > 0) {
    int fromlen = (int)message->msg_namelen;
    rc = g_sock_hooks.wsa_recv_from_fn(
        (SOCKET)s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd, &dwFlags,
        (struct sockaddr *)message->msg_name, &fromlen, NULL, NULL);
    if (rc == 0 || g_sock_hooks.wsa_get_last_error_fn() != WSAEMSGSIZE) {
      message->msg_namelen = (posix_socklen_t)fromlen;
    }
  } else {
    rc = g_sock_hooks.wsa_recv_fn((SOCKET)s, bufs, (DWORD)message->msg_iovlen,
                                  &bytes_recvd, &dwFlags, NULL, NULL);
  }

  if (rc == SOCKET_ERROR &&
      g_sock_hooks.wsa_get_last_error_fn() == WSAENOTSOCK) {
    SOCKET real_s = (SOCKET)safe_get_osfhandle((intptr_t)s);
    if (real_s != (SOCKET)-1 && real_s != 0) {
      if (message->msg_name != NULL && message->msg_namelen > 0) {
        int fromlen = (int)message->msg_namelen;
        rc = g_sock_hooks.wsa_recv_from_fn(
            real_s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd, &dwFlags,
            (struct sockaddr *)message->msg_name, &fromlen, NULL, NULL);
        if (rc == 0 || g_sock_hooks.wsa_get_last_error_fn() != WSAEMSGSIZE) {
          message->msg_namelen = (posix_socklen_t)fromlen;
        }
      } else {
        rc = g_sock_hooks.wsa_recv_fn(real_s, bufs, (DWORD)message->msg_iovlen,
                                      &bytes_recvd, &dwFlags, NULL, NULL);
      }
    }
  }

  if (bufs != stack_bufs) {
    g_sock_hooks.free_fn(bufs);
  }

  if (rc == SOCKET_ERROR) {
    int err = g_sock_hooks.wsa_get_last_error_fn();
    if (err == WSAEMSGSIZE) {
      message->msg_flags = (int)dwFlags | 0x20;
      return (posix_ssize_t)bytes_recvd;
    }
    if (err == WSAEWOULDBLOCK) {
      errno = 140;
    } else if (err == WSAECONNRESET) {
      errno = 104;
    } else {
      errno = _wsaErrorToErrno(err);
    }
    return -1;
  }
  message->msg_flags = (int)dwFlags;
  return (posix_ssize_t)bytes_recvd;
#else
  (void)s;
  (void)message;
  (void)flags;
  errno = EINVAL;
  return -1;
#endif
}

/* Connection retry helper for asynchronous socket servers. */
int posix_connect_retry(intptr_t socket, const struct sockaddr *address,
                        posix_socklen_t address_len, int max_retries,
                        int delay_ms) {
  if (max_retries <= 0) {
    max_retries = 10;
  }
  if (delay_ms <= 0) {
    delay_ms = 100;
  }
  (void)max_retries;
  (void)delay_ms;
  return posix_connect(socket, address, address_len);
}

/** @brief Sends datagram to destination address (stub returning -1 with
 * EINVAL). */
posix_ssize_t posix_sendto(intptr_t socket, const void *message, size_t length,
                           int flags, const struct sockaddr *dest_addr,
                           posix_socklen_t dest_len) {
  (void)socket;
  (void)message;
  (void)length;
  (void)flags;
  (void)dest_addr;
  (void)dest_len;
  errno = EINVAL;
  return -1;
}

/**
 * @brief Configures a socket option, translating timeval timeouts to DWORD ms.
 * @param[in] socket Socket descriptor.
 * @param[in] level Protocol level.
 * @param[in] option_name Option identifier.
 * @param[in] option_value Option input buffer.
 * @param[in] option_len Option input buffer length.
 * @return 0 on success, or -1 on error.
 */
int posix_setsockopt(intptr_t socket, int level, int option_name,
                     const void *option_value, posix_socklen_t option_len) {
#ifdef _WIN32
  int ret;
  if (level == SOL_SOCKET && option_name == SO_REUSEADDR) {
    return 0;
  }
  if (level == SOL_SOCKET &&
      (option_name == SO_SNDTIMEO || option_name == SO_RCVTIMEO)) {
    if (option_len == sizeof(struct timeval)) {
      const struct timeval *tv = (const struct timeval *)option_value;
      DWORD ms = (DWORD)(tv->tv_sec * 1000 + tv->tv_usec / 1000);
      ret = g_sock_hooks.setsockopt_fn(GET_SOCKET(socket), level, option_name,
                                       (const char *)&ms, sizeof(DWORD));
    } else {
      ret = g_sock_hooks.setsockopt_fn(GET_SOCKET(socket), level, option_name,
                                       (const char *)option_value, option_len);
    }
  } else {
    ret = g_sock_hooks.setsockopt_fn(GET_SOCKET(socket), level, option_name,
                                     (const char *)option_value, option_len);
  }
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return 0;
#else
  (void)socket;
  (void)level;
  (void)option_name;
  (void)option_value;
  (void)option_len;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Shuts down part or all of a full-duplex socket connection.
 * @param[in] socket Socket descriptor.
 * @param[in] how Shutdown mode (SHUT_RD, SHUT_WR, SHUT_RDWR).
 * @return 0 on success, or -1 on error.
 */
int posix_shutdown(intptr_t socket, int how) {
#ifdef _WIN32
  int ret = g_sock_hooks.shutdown_fn(GET_SOCKET(socket), how);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(g_sock_hooks.wsa_get_last_error_fn());
    return -1;
  }
  return 0;
#else
  (void)socket;
  (void)how;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Creates a communication endpoint socket.
 * @param[in] domain Address family (AF_UNIX mapped to AF_INET on Windows).
 * @param[in] type Socket type.
 * @param[in] protocol Protocol number.
 * @return Socket descriptor, or -1 on error.
 */
int posix_socket(int domain, int type, int protocol) {
#ifdef _WIN32
  SOCKET s;
  int is_icmp_dgram = 0;
  if (domain == AF_UNIX) {
    domain = AF_INET;
  }
  if (type == SOCK_DGRAM && (protocol == 1 || protocol == 58)) {
    is_icmp_dgram = 1;
    protocol = 0;
  }
  s = g_sock_hooks.wsa_socket_w_fn(domain, type, protocol, NULL, 0, 0);
  if (s == INVALID_SOCKET) {
    if (g_sock_hooks.wsa_get_last_error_fn() == WSANOTINITIALISED) {
      WSADATA wsaData;
      if (g_sock_hooks.wsa_startup_fn(MAKEWORD(2, 2), &wsaData) == 0) {
        s = g_sock_hooks.wsa_socket_w_fn(domain, type, protocol, NULL, 0, 0);
      }
    }
    if (s == INVALID_SOCKET) {
      int werr = g_sock_hooks.wsa_get_last_error_fn();
      if (type == SOCK_RAW &&
          (werr == WSAEINVAL || werr == WSAEPROTONOSUPPORT ||
           werr == WSAESOCKTNOSUPPORT || werr == WSAEACCES)) {
        errno = EACCES;
      } else {
        errno = _wsaErrorToErrno(werr);
      }
      return -1;
    }
  }
  if (is_icmp_dgram) {
    unsigned long nb = 1;
    g_sock_hooks.ioctlsocket_fn(s, (long)FIONBIO, &nb);
    set_nonblock(s, 1);
  }
  posix_register_crt_socket((intptr_t)s, (intptr_t)s);
  (void)mark_as_socket((intptr_t)s);
  return (int)s;
#else
  (void)domain;
  (void)type;
  (void)protocol;
  errno = EINVAL;
  return -1;
#endif
}

/**
 * @brief Creates an unnamed pair of connected sockets.
 * @param[in] domain Address family (AF_UNIX, AF_INET, or AF_UNSPEC).
 * @param[in] type Socket type.
 * @param[in] protocol Protocol number.
 * @param[out] socket_vector Two-element array receiving connected descriptors.
 * @return 0 on success, or -1 on error.
 */
int posix_socketpair(int domain, int type, int protocol,
                     intptr_t socket_vector[2]) {
#if defined(_WIN32)
  SOCKET listener, client, server;
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  if (domain != AF_INET && domain != AF_UNSPEC && domain != AF_UNIX) {
    return -1;
  }

  listener = g_sock_hooks.wsa_socket_w_fn(AF_INET, type, protocol, NULL, 0, 0);
  if (listener == INVALID_SOCKET) {
    if (g_sock_hooks.wsa_get_last_error_fn() == WSANOTINITIALISED) {
      WSADATA wsaData;
      (void)g_sock_hooks.wsa_startup_fn(MAKEWORD(2, 2), &wsaData);
      listener =
          g_sock_hooks.wsa_socket_w_fn(AF_INET, type, protocol, NULL, 0, 0);
    }
    if (listener == INVALID_SOCKET) {
      return -1;
    }
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;

  if (g_sock_hooks.bind_fn(listener, (struct sockaddr *)&addr, sizeof(addr)) ==
      SOCKET_ERROR) {
    goto err;
  }
  if (g_sock_hooks.getsockname_fn(listener, (struct sockaddr *)&addr,
                                  &addrlen) == SOCKET_ERROR) {
    goto err;
  }
  if (g_sock_hooks.listen_fn(listener, 1) == SOCKET_ERROR) {
    goto err;
  }

  client = g_sock_hooks.wsa_socket_w_fn(AF_INET, type, protocol, NULL, 0, 0);
  if (client == INVALID_SOCKET) {
    goto err;
  }

  if (g_sock_hooks.connect_fn(client, (struct sockaddr *)&addr, sizeof(addr)) ==
      SOCKET_ERROR) {
    g_sock_hooks.closesocket_fn(client);
    goto err;
  }

  server = g_sock_hooks.accept_fn(listener, NULL, NULL);
  if (server == INVALID_SOCKET) {
    g_sock_hooks.closesocket_fn(client);
    goto err;
  }

  g_sock_hooks.closesocket_fn(listener);

  {
    intptr_t fd_server =
        g_sock_hooks.open_osfhandle_fn((intptr_t)server, _O_RDWR | _O_BINARY);
    intptr_t fd_client =
        g_sock_hooks.open_osfhandle_fn((intptr_t)client, _O_RDWR | _O_BINARY);
    if (fd_server == -1 || fd_client == -1) {
      if (fd_server != -1) {
        g_sock_hooks.close_fn((int)fd_server);
      } else {
        g_sock_hooks.closesocket_fn(server);
      }
      if (fd_client != -1) {
        g_sock_hooks.close_fn((int)fd_client);
      } else {
        g_sock_hooks.closesocket_fn(client);
      }
      return -1;
    }
    socket_vector[0] = (int)fd_server;
    socket_vector[1] = (int)fd_client;
    posix_register_crt_socket(fd_server, (intptr_t)server);
    posix_register_crt_socket(fd_client, (intptr_t)client);
    (void)mark_as_socket(fd_server);
    (void)mark_as_socket(fd_client);
    return 0;
  }

err:
  g_sock_hooks.closesocket_fn(listener);
  return -1;
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
  (void)domain;
  (void)type;
  (void)protocol;
  (void)socket_vector;
  errno = ENOSYS;
  return -1;
#else
  int fds[2];
  int rc;
  if (domain == AF_INET || domain == AF_UNSPEC) {
    domain = AF_UNIX;
  }
  rc = socketpair(domain, type, protocol, fds);
  if (rc == 0) {
    socket_vector[0] = (intptr_t)fds[0];
    socket_vector[1] = (intptr_t)fds[1];
    return 0;
  }
  return -1;
#endif
}

typedef int make_iso_compilers_happy_tu;

/**
 * @brief Retrieves information on posix-sockets availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_sockets_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

/**
 * @brief Enumerates local network interface addresses (stub returning ENOSYS).
 * @param[out] ifap Output pointer set to NULL.
 * @return Always -1 with errno set to EINVAL or ENOSYS.
 */
int posix_getifaddrs(struct ifaddrs **ifap);
int posix_getifaddrs(struct ifaddrs **ifap) {
  if (ifap == NULL) {
    errno = EINVAL;
    return -1;
  }
  *ifap = NULL;
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Frees interface address list returned by posix_getifaddrs.
 * @param[in] ifa Head of interface list (may be NULL).
 */
void posix_freeifaddrs(struct ifaddrs *ifa);
void posix_freeifaddrs(struct ifaddrs *ifa) { (void)ifa; }

/**
 * @brief No-op helper preventing empty translation unit warnings.
 * @return Always 0.
 */
int dummy_posix_sockets(void);
int dummy_posix_sockets(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sockets;

#endif
typedef int make_iso_compilers_happy_mega_batch3_15;
