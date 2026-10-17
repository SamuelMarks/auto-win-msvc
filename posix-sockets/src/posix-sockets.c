/* posix-sockets.c - Strict C89 Implementation */
#define POSIX_SOCKETS_EXPORTS
#if defined(_MSC_VER)
#endif
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
#include <stdlib.h>
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

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1400
static void sock_null_invalid_param_handler(const wchar_t *expression,
                                            const wchar_t *function,
                                            const wchar_t *file,
                                            unsigned int line,
                                            uintptr_t pReserved) {
  if (expression || function || file || line || pReserved) {
  }
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
    h = (ptrdiff_t)_get_osfhandle((int)fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle((int)fd);
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

#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
static void __cdecl null_invalid_parameter_handler_auto(const wchar_t *a,
                                                        const wchar_t *b,
                                                        const wchar_t *c,
                                                        unsigned int d,
                                                        uintptr_t e) {
  if (a || b || c || d || e) {
    /* parameter error handled */
  }
}

static void __cdecl __init_winsock_auto(void) {
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  _set_invalid_parameter_handler(
      (_invalid_parameter_handler)null_invalid_parameter_handler_auto);
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
#endif

#ifndef EINVAL
#define EINVAL 38
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#else
#endif

#ifdef _WIN32
/* Helper removed */

#ifndef O_NONBLOCK
#define O_NONBLOCK 0x4000
#endif

static SOCKET nb_sockets[8192] = {0};
void set_nonblock(SOCKET s, int nb) {
  int i;
  for (i = 0; i < 8192; i++) {
    if (nb_sockets[i] == s) {
      if (!nb)
        nb_sockets[i] = 0;
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
int get_nonblock(SOCKET s) {
  int i;
  for (i = 0; i < 8192; i++)
    if (nb_sockets[i] == s)
      return O_NONBLOCK;
  return 0;
}
error_type_t clear_nonblock(SOCKET s) {
  set_nonblock(s, 0);
  return ERR_NONE;
}
void copy_nonblock(SOCKET src, SOCKET dst) {
  if (get_nonblock(src))
    set_nonblock(dst, 1);
}

static unsigned char g_is_socket[8192] = {0};
static intptr_t g_dup_sockets[64] = {0};
static intptr_t g_crt_sockets[2048] = {0};
intptr_t posix_get_dup_socket(intptr_t fd) {
  if (fd >= 0 && fd < 64) {
    return g_dup_sockets[fd];
  }
  return 0;
}
void posix_set_dup_socket(intptr_t fd, intptr_t s) {
  if (fd >= 0 && fd < 64) {
    g_dup_sockets[fd] = s;
  }
}
intptr_t posix_lookup_crt_socket(intptr_t fd) {
  if (fd >= 0 && fd < 2048) {
    return g_crt_sockets[fd];
  }
  return 0;
}
void posix_register_crt_socket(intptr_t fd, intptr_t s) {
  if (fd >= 0 && fd < 2048) {
    g_crt_sockets[fd] = s;
  }
}
error_type_t mark_as_socket(intptr_t fd) {
  if (fd >= 0 && fd < 8192)
    g_is_socket[fd] = 1;
  return ERR_NONE;
}
error_type_t clear_as_socket(intptr_t fd) {
  if (fd >= 0 && fd < 8192)
    g_is_socket[fd] = 0;
  return ERR_NONE;
}
int is_socket(intptr_t fd) {
  if (fd >= 0 && fd < 8192)
    return g_is_socket[fd];
  return 1; /* Assuming large FDs are sockets */
}

#endif

/** \brief posix_endhostent function. */
void posix_endhostent(void) {
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  return;
}

void posix_endnetent(void) {
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  return;
}

void posix_endprotoent(void) {
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  return;
}

void posix_endservent(void) {
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  return;
}
#define POSIX_AI_CUSTOM_MAGIC 0x41574D53

void posix_freeaddrinfo(struct addrinfo *ai) {
  (void)ai;
#ifdef _WIN32
#undef freeaddrinfo
  if (ai == NULL) {
    return;
  }
  if (ai->ai_flags == POSIX_AI_CUSTOM_MAGIC) {
    free(ai);
    return;
  }
  freeaddrinfo(ai);
#endif
}
const char *posix_gai_strerror(int ecode) {
#ifdef _WIN32
#undef gai_strerror
  return gai_strerrorA(ecode);
#else
  static char buf[64];
#if defined(_MSC_VER)
  sprintf_s(buf, sizeof(buf), "Unknown error %d", (int)ecode);
#else
#if defined(_MSC_VER)
  sprintf_s(buf, sizeof(buf), "Unknown error %d", (int)ecode);
#else
  sprintf(buf, "Unknown error %d", (int)ecode);
#endif
#endif
  return buf;
#endif
}

int posix_getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints, struct addrinfo **res) {
#ifdef _WIN32
#undef getaddrinfo
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
      struct addrinfo *ai = (struct addrinfo *)calloc(
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
        free(ai);
      }
    }
  }
  ret = getaddrinfo(nodename, servname, hints, res);
  if (ret != 0) {
    if (ret == WSANOTINITIALISED || WSAGetLastError() == WSANOTINITIALISED) {
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
        ret = getaddrinfo(nodename, servname, hints, res);
      }
    }
#ifdef AI_ADDRCONFIG
    if (ret != 0 && hints != NULL && (hints->ai_flags & AI_ADDRCONFIG)) {
      struct addrinfo fallback_hints = *hints;
      fallback_hints.ai_flags &= ~AI_ADDRCONFIG;
      ret = getaddrinfo(nodename, servname, &fallback_hints, res);
    }
#endif
    if (ret != 0) {
      errno = _wsaErrorToErrno(WSAGetLastError());
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

struct hostent *posix_gethostbyaddr(const void *addr, posix_socklen_t len,
                                    int type) {
  (void)addr;
  (void)len;
  (void)type;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct hostent *posix_gethostbyname(const char *name) {
  (void)name;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct hostent *posix_gethostent(void) {
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

/** \brief posix_getnameinfo function. */
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

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return -1;
}

struct netent *posix_getnetbyaddr(uint32_t net, int type) {
  (void)net;
  (void)type;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct netent *posix_getnetbyname(const char *name) {
  (void)name;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct netent *posix_getnetent(void) {
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct protoent *posix_getprotobyname(const char *name) {
  (void)name;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct protoent *posix_getprotobynumber(int proto) {
  (void)proto;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct protoent *posix_getprotoent(void) {
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct servent *posix_getservbyname(const char *name, const char *proto) {
#ifdef _WIN32
  static struct servent s_se;
  static char *s_aliases[1] = {NULL};
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
      s_se.s_name = (char *)name;
      s_se.s_aliases = s_aliases;
      s_se.s_port = htons(p);
      s_se.s_proto = (char *)(proto != NULL ? proto : "tcp");
      return &s_se;
    }
  }
#undef getservbyname
  {
    struct servent *se = getservbyname(name, proto);
    if (se == NULL && WSAGetLastError() == WSANOTINITIALISED) {
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
        se = getservbyname(name, proto);
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

struct servent *posix_getservbyport(int port, const char *proto) {
#ifdef _WIN32
#undef getservbyport
  struct servent *se = getservbyport(port, proto);
  if (se == NULL && WSAGetLastError() == WSANOTINITIALISED) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
      se = getservbyport(port, proto);
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

struct servent *posix_getservent(void) {
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

/** \brief posix_sethostent function. */
void posix_sethostent(int stayopen) {
  (void)stayopen;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  return;
}

void posix_setnetent(int stayopen) {
  (void)stayopen;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  return;
}

void posix_setprotoent(int stayopen) {
  (void)stayopen;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  return;
}

void posix_setservent(int stayopen) {
  (void)stayopen;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  return;
}

/** \brief posix_pselect function. */
int posix_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                  const struct timespec *timeout, const void *sigmask) {
  if (!readfds || !writefds || !errorfds || !timeout || !sigmask) {
    errno = EINVAL;
    return -1;
  }
  (void)nfds;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return -1;
}

/** \brief posix_select function. */
#include <errno.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <winsock2.h>
#if defined(_MSC_VER)

#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif
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
#undef FD_CLR
static void posix_fd_clr(SOCKET fd, fd_set *set) {
  u_int __i;
  for (__i = 0; __i < set->fd_count; __i++) {
    if (set->fd_array[__i] == fd) {
      while (__i < set->fd_count - 1) {
        set->fd_array[__i] = set->fd_array[__i + 1];
        __i++;
      }
      set->fd_count--;
      break;
    }
  }
}
#define FD_CLR(fd, set) posix_fd_clr((SOCKET)(fd), (fd_set *)(set))

#endif

#endif

extern int is_socket(intptr_t fd);
#if defined(_WIN32)
__declspec(dllimport) unsigned long __stdcall GetFileType(void *hFile);
__declspec(dllimport) int __stdcall PeekNamedPipe(
    void *hNamedPipe, void *lpBuffer, unsigned long nBufferSize,
    unsigned long *lpBytesRead, unsigned long *lpTotalBytesAvail,
    unsigned long *lpBytesLeftThisMessage);
#endif

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
          getsockopt(s, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen) == 0) {
        FD_SET(s, &ws_readfds);
        num_sockets++;
      } else {
        ptrdiff_t h = (fd >= 0 && fd < 64) ? safe_get_osfhandle(fd) : -1;
        if (h != -1) {
          unsigned long ftype = GetFileType((void *)h);
          if (ftype == 3 /* FILE_TYPE_PIPE */) {
            unsigned long avail = 0;
            if (!PeekNamedPipe((void *)h, NULL, 0, NULL, &avail, NULL) ||
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
          getsockopt(s, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen) == 0) {
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
          getsockopt(s, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen) == 0) {
        FD_SET(s, &ws_errorfds);
        num_sockets++;
      }
    }
    if (ws_errorfds.fd_count > 0) {
      p_ws_errorfds = &ws_errorfds;
    }
  }

  if (num_sockets > 0) {
#undef select
    ret = select(0, p_ws_readfds, p_ws_writefds, p_ws_errorfds,
                 num_non_sock_ready > 0 ? &zero_tv : timeout);
#define select posix_select
    if (ret == SOCKET_ERROR) {
      if (num_non_sock_ready > 0) {
        ret = 0;
      } else {
        errno = EBADF;
        return -1;
      }
    }
  } else if (num_non_sock_ready == 0 && timeout != NULL) {
    Sleep((DWORD)(timeout->tv_sec * 1000 + timeout->tv_usec / 1000));
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

int posix_accept(intptr_t socket, struct sockaddr *address,
                 posix_socklen_t *address_len) {
#ifdef _WIN32
#undef accept
  SOCKET ret = accept(GET_SOCKET(socket), address, address_len);
  if (ret == INVALID_SOCKET) {
    int err = WSAGetLastError();
    if (err == WSAEWOULDBLOCK)
      errno = EWOULDBLOCK;
    else
      errno = _wsaErrorToErrno(err);
    return -1;
  }

  copy_nonblock(GET_SOCKET(socket), ret);
  posix_register_crt_socket((intptr_t)ret, (intptr_t)ret);
  if (mark_as_socket((intptr_t)ret) != ERR_NONE) { /* Ignore */
  }
  return (int)ret;
#else
  (void)socket;
  (void)address;
  (void)address_len;
  errno = EINVAL;
  return -1;
#endif
}

int posix_bind(intptr_t socket, const struct sockaddr *address,
               posix_socklen_t address_len) {
#ifdef _WIN32
#undef bind
  int ret;
  if (address != NULL && address->sa_family == AF_UNIX) {
    struct sockaddr_in fake_addr;

    memset(&fake_addr, 0, sizeof(fake_addr));
    fake_addr.sin_family = AF_INET;
    fake_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    fake_addr.sin_port = 0;
    ret = bind(GET_SOCKET(socket), (const struct sockaddr *)&fake_addr,
               sizeof(fake_addr));
    if (ret == SOCKET_ERROR) {
      errno = _wsaErrorToErrno(WSAGetLastError());
      return -1;
    }
    return 0;
  }
  ret = bind(GET_SOCKET(socket), address, address_len);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
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

int posix_connect(intptr_t socket, const struct sockaddr *address,
                  posix_socklen_t address_len) {
#ifdef _WIN32
#undef connect
  SOCKET s;
  int ret;
  if (!address) {
    errno = EFAULT;
    return -1;
  }
  s = GET_SOCKET(socket);
  ret = connect(s, address, address_len);
  if (ret == SOCKET_ERROR && WSAGetLastError() == WSAECONNREFUSED &&
      address->sa_family == AF_INET && !get_nonblock(s)) {
    const struct sockaddr_in *sin = (const struct sockaddr_in *)address;
    if (sin->sin_addr.s_addr == htonl(0x7f000001UL) && sin->sin_port != 0) {
      int attempt;
      for (attempt = 0; attempt < 60; attempt++) {
        SOCKET new_s;
        Sleep(100);
        new_s = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
        if (new_s == INVALID_SOCKET) {
          break;
        }
        closesocket(s);
        s = new_s;
        posix_register_crt_socket(socket, (intptr_t)s);
        ret = connect(s, address, address_len);
        if (ret == 0 || WSAGetLastError() != WSAECONNREFUSED) {
          break;
        }
      }
    }
  }
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSAEISCONN) {
      return 0;
    }
    if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS || err == WSAEALREADY)
      errno = EINPROGRESS;
    else
      errno = _wsaErrorToErrno(err);
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

int posix_getpeername(intptr_t socket, struct sockaddr *address,
                      posix_socklen_t *address_len) {
#ifdef _WIN32
#undef getpeername
  int ret = getpeername(GET_SOCKET(socket), address, address_len);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
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

int posix_getsockname(intptr_t socket, struct sockaddr *address,
                      posix_socklen_t *address_len) {
#ifdef _WIN32
#undef getsockname
  int ret = getsockname(GET_SOCKET(socket), address, address_len);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
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

int posix_getsockopt(intptr_t socket, int level, int option_name,
                     void *option_value, posix_socklen_t *option_len) {
#ifdef _WIN32
#undef getsockopt
  int ret;
  if (level == SOL_SOCKET &&
      (option_name == SO_SNDTIMEO || option_name == SO_RCVTIMEO)) {
    if (option_len && *option_len == sizeof(struct timeval)) {
      DWORD ms = 0;
      int ms_len = sizeof(DWORD);
      ret = getsockopt(GET_SOCKET(socket), level, option_name, (char *)&ms,
                       &ms_len);
      if (ret == 0) {
        struct timeval *tv = (struct timeval *)((size_t)option_value);
        tv->tv_sec = ms / 1000;
        tv->tv_usec = (ms % 1000) * 1000;
      }
    } else {
      ret = getsockopt(GET_SOCKET(socket), level, option_name,
                       (char *)option_value, option_len);
    }
  } else {
    ret = getsockopt(GET_SOCKET(socket), level, option_name,
                     (char *)option_value, option_len);
    if (ret == 0 && level == SOL_SOCKET && option_name == SO_ERROR) {
      if (*((int *)option_value) != 0) {
        *((int *)option_value) = _wsaErrorToErrno(*((int *)option_value));
      }
    }
  }
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
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

int posix_listen(intptr_t socket, int backlog) {
#ifdef _WIN32
#undef listen
  int ret = listen(GET_SOCKET(socket), backlog);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
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

posix_ssize_t posix_recv(intptr_t socket, void *buffer, size_t length,
                         int flags) {
#ifdef _WIN32
#undef recv
  int ret = recv(GET_SOCKET(socket), (char *)buffer, (int)length, flags);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
    return -1;
  }
  return ret;
#else
  return recv((int)socket, buffer, length, flags);
#endif
}

/** \brief posix_recvfrom function. */
posix_ssize_t posix_recvfrom(intptr_t socket, void *buffer, size_t length,
                             int flags, struct sockaddr *address,
                             posix_socklen_t *address_len) {
  (void)socket;
  (void)buffer;
  (void)length;
  (void)flags;
  (void)address;
  (void)address_len;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return -1;
}

/** \brief posix_recvmsg function. */
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
    bufs = (WSABUF *)malloc(sizeof(WSABUF) * (size_t)message->msg_iovlen);
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
  if (getsockopt(s, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen) ==
          SOCKET_ERROR &&
      WSAGetLastError() == WSAENOTSOCK) {
    posix_ssize_t total = 0;
    for (i = 0; i < message->msg_iovlen; i++) {
      int r = _read((int)socket, message->msg_iov[i].iov_base,
                    (unsigned int)message->msg_iov[i].iov_len);
      if (r < 0) {
        if (bufs != stack_bufs) {
          free(bufs);
        }
        return total > 0 ? total : -1;
      }
      total += r;
      if ((size_t)r < message->msg_iov[i].iov_len || r == 0) {
        break;
      }
    }
    if (bufs != stack_bufs) {
      free(bufs);
    }
    message->msg_flags = 0;
    return total;
  }

  bytes_recvd = 0;
  dwFlags = (DWORD)flags;
  if (message->msg_name != NULL && message->msg_namelen > 0) {
    INT fromlen = (INT)message->msg_namelen;
    rc =
        WSARecvFrom(s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd, &dwFlags,
                    (struct sockaddr *)message->msg_name, &fromlen, NULL, NULL);
    if (rc == 0 || WSAGetLastError() != WSAEMSGSIZE) {
      message->msg_namelen = (posix_socklen_t)fromlen;
    }
  } else {
    rc = WSARecv(s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd, &dwFlags,
                 NULL, NULL);
  }

  if (bufs != stack_bufs) {
    free(bufs);
  }

  if (rc == SOCKET_ERROR) {
    int err = WSAGetLastError();
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

posix_ssize_t posix_send(intptr_t socket, const void *message, size_t length,
                         int flags) {
#ifdef _WIN32
#undef send
  int ret = send(GET_SOCKET(socket), (const char *)message, (int)length, flags);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
    return -1;
  }
  return ret;
#else
  return send((int)socket, message, length, flags);
#endif
}

/** \brief posix_sendmsg function. */
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
    bufs = (WSABUF *)malloc(sizeof(WSABUF) * (size_t)message->msg_iovlen);
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
  if (getsockopt(s, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen) ==
          SOCKET_ERROR &&
      WSAGetLastError() == WSAENOTSOCK) {
    posix_ssize_t total = 0;
    for (i = 0; i < message->msg_iovlen; i++) {
      int w = _write((int)socket, message->msg_iov[i].iov_base,
                     (unsigned int)message->msg_iov[i].iov_len);
      if (w < 0) {
        if (bufs != stack_bufs) {
          free(bufs);
        }
        return total > 0 ? total : -1;
      }
      total += w;
      if ((size_t)w < message->msg_iov[i].iov_len) {
        break;
      }
    }
    if (bufs != stack_bufs) {
      free(bufs);
    }
    return total;
  }

  bytes_sent = 0;
  if (message->msg_name != NULL && message->msg_namelen > 0) {
    rc = WSASendTo(s, bufs, (DWORD)message->msg_iovlen, &bytes_sent,
                   (DWORD)flags, (const struct sockaddr *)message->msg_name,
                   (int)message->msg_namelen, NULL, NULL);
  } else {
    rc = WSASend(s, bufs, (DWORD)message->msg_iovlen, &bytes_sent, (DWORD)flags,
                 NULL, NULL);
  }

  if (bufs != stack_bufs) {
    free(bufs);
  }

  if (rc == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
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

/** \brief Native Winsock sendmsg helper operating directly on SOCKET handles.
 */
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
  if (socket < 0) {
    errno = EBADF;
    return -1;
  }

  if (message->msg_iovlen == 0) {
    return 0;
  }

  bufs = stack_bufs;
  if (message->msg_iovlen > 16) {
    bufs = (WSABUF *)malloc(sizeof(WSABUF) * (size_t)message->msg_iovlen);
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
    rc = WSASendTo((SOCKET)s, bufs, (DWORD)message->msg_iovlen, &bytes_sent,
                   (DWORD)flags, (const struct sockaddr *)message->msg_name,
                   (int)message->msg_namelen, NULL, NULL);
  } else {
    rc = WSASend((SOCKET)s, bufs, (DWORD)message->msg_iovlen, &bytes_sent,
                 (DWORD)flags, NULL, NULL);
  }

  if (rc == SOCKET_ERROR && WSAGetLastError() == WSAENOTSOCK) {
    SOCKET real_s = (SOCKET)safe_get_osfhandle((intptr_t)s);
    if (real_s != (SOCKET)-1 && real_s != 0) {
      if (message->msg_name != NULL && message->msg_namelen > 0) {
        rc = WSASendTo(real_s, bufs, (DWORD)message->msg_iovlen, &bytes_sent,
                       (DWORD)flags, (const struct sockaddr *)message->msg_name,
                       (int)message->msg_namelen, NULL, NULL);
      } else {
        rc = WSASend(real_s, bufs, (DWORD)message->msg_iovlen, &bytes_sent,
                     (DWORD)flags, NULL, NULL);
      }
    }
  }

  if (bufs != stack_bufs) {
    free(bufs);
  }

  if (rc == SOCKET_ERROR) {
    int err = WSAGetLastError();
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

/** \brief Native Winsock recvmsg helper operating directly on SOCKET handles.
 */
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
  if (socket < 0) {
    errno = EBADF;
    return -1;
  }

  if (message->msg_iovlen == 0) {
    return 0;
  }

  bufs = stack_bufs;
  if (message->msg_iovlen > 16) {
    bufs = (WSABUF *)malloc(sizeof(WSABUF) * (size_t)message->msg_iovlen);
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
    rc = WSARecvFrom((SOCKET)s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd,
                     &dwFlags, (struct sockaddr *)message->msg_name, &fromlen,
                     NULL, NULL);
    if (rc == 0 || WSAGetLastError() != WSAEMSGSIZE) {
      message->msg_namelen = (posix_socklen_t)fromlen;
    }
  } else {
    rc = WSARecv((SOCKET)s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd,
                 &dwFlags, NULL, NULL);
  }

  if (rc == SOCKET_ERROR && WSAGetLastError() == WSAENOTSOCK) {
    SOCKET real_s = (SOCKET)safe_get_osfhandle((intptr_t)s);
    if (real_s != (SOCKET)-1 && real_s != 0) {
      if (message->msg_name != NULL && message->msg_namelen > 0) {
        int fromlen = (int)message->msg_namelen;
        rc = WSARecvFrom(real_s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd,
                         &dwFlags, (struct sockaddr *)message->msg_name,
                         &fromlen, NULL, NULL);
        if (rc == 0 || WSAGetLastError() != WSAEMSGSIZE) {
          message->msg_namelen = (posix_socklen_t)fromlen;
        }
      } else {
        rc = WSARecv(real_s, bufs, (DWORD)message->msg_iovlen, &bytes_recvd,
                     &dwFlags, NULL, NULL);
      }
    }
  }

  if (bufs != stack_bufs) {
    free(bufs);
  }

  if (rc == SOCKET_ERROR) {
    int err = WSAGetLastError();
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

/** \brief Connection retry helper for asynchronous socket servers. */
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

/** \brief posix_sendto function. */
posix_ssize_t posix_sendto(intptr_t socket, const void *message, size_t length,
                           int flags, const struct sockaddr *dest_addr,
                           posix_socklen_t dest_len) {
  (void)socket;
  (void)message;
  (void)length;
  (void)flags;
  (void)dest_addr;
  (void)dest_len;

#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return -1;
}

int posix_setsockopt(intptr_t socket, int level, int option_name,
                     const void *option_value, posix_socklen_t option_len) {
#ifdef _WIN32
#undef setsockopt
  int ret;
  if (level == SOL_SOCKET && option_name == SO_REUSEADDR) {
    return 0;
  }
  if (level == SOL_SOCKET &&
      (option_name == SO_SNDTIMEO || option_name == SO_RCVTIMEO)) {
    if (option_len == sizeof(struct timeval)) {
      struct timeval *tv = (struct timeval *)((size_t)option_value);
      DWORD ms = (DWORD)(tv->tv_sec * 1000 + tv->tv_usec / 1000);
      ret = setsockopt(GET_SOCKET(socket), level, option_name,
                       (const char *)&ms, sizeof(DWORD));
    } else {
      ret = setsockopt(GET_SOCKET(socket), level, option_name,
                       (const char *)option_value, option_len);
    }
  } else {
    ret = setsockopt(GET_SOCKET(socket), level, option_name,
                     (const char *)option_value, option_len);
  }
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
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

int posix_shutdown(intptr_t socket, int how) {
#ifdef _WIN32
#undef shutdown
  int ret = shutdown(GET_SOCKET(socket), how);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
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

int posix_socket(int domain, int type, int protocol) {
#ifdef _WIN32
#undef socket
  SOCKET s;
  if (domain == AF_UNIX)
    domain = AF_INET;
  s = WSASocketW(domain, type, protocol, NULL, 0, 0);
  if (s == INVALID_SOCKET) {
    if (WSAGetLastError() == WSANOTINITIALISED) {
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
        s = WSASocketW(domain, type, protocol, NULL, 0, 0);
      }
    }
    if (s == INVALID_SOCKET) {
      int werr = WSAGetLastError();
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
  posix_register_crt_socket((intptr_t)s, (intptr_t)s);
  if (mark_as_socket((intptr_t)s) != ERR_NONE) { /* Ignore */
  }
  return (int)s;
#else
  (void)domain;
  (void)type;
  (void)protocol;

  errno = EINVAL;
  return -1;
#endif
}

/** \brief posix_socketpair function. */

int posix_socketpair(int domain, int type, int protocol,
                     intptr_t socket_vector[2]) {
#if defined(_WIN32)
  SOCKET listener, client, server;
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  if (domain != AF_INET && domain != AF_UNSPEC)
    return -1;

  listener = WSASocketW(AF_INET, type, protocol, NULL, 0, 0);
  if (listener == INVALID_SOCKET) {
    if (WSAGetLastError() == WSANOTINITIALISED) {
      WSADATA wsaData;
      WSAStartup(MAKEWORD(2, 2), &wsaData);
      listener = WSASocketW(AF_INET, type, protocol, NULL, 0, 0);
    }
    if (listener == INVALID_SOCKET)
      return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;

  if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    goto err;
  if (getsockname(listener, (struct sockaddr *)&addr, &addrlen) == SOCKET_ERROR)
    goto err;
  if (listen(listener, 1) == SOCKET_ERROR)
    goto err;

  client = WSASocketW(AF_INET, type, protocol, NULL, 0, 0);
  if (client == INVALID_SOCKET)
    goto err;

  if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
    closesocket(client);
    goto err;
  }

  server = accept(listener, NULL, NULL);
  if (server == INVALID_SOCKET) {
    closesocket(client);
    goto err;
  }

  closesocket(listener);

  {
    intptr_t fd_server = _open_osfhandle((intptr_t)server, _O_RDWR | _O_BINARY);
    intptr_t fd_client = _open_osfhandle((intptr_t)client, _O_RDWR | _O_BINARY);
    if (fd_server == -1 || fd_client == -1) {
      if (fd_server != -1)
        _close((int)fd_server);
      else
        closesocket(server);
      if (fd_client != -1)
        _close((int)fd_client);
      else
        closesocket(client);
      return -1;
    }
    socket_vector[0] = (int)fd_server;
    socket_vector[1] = (int)fd_client;
    posix_register_crt_socket(fd_server, (intptr_t)server);
    posix_register_crt_socket(fd_client, (intptr_t)client);
    if (mark_as_socket(fd_server) != ERR_NONE) { /* Ignore */
    }
    if (mark_as_socket(fd_client) != ERR_NONE) { /* Ignore */
    }
    return 0;
  }

err:
  closesocket(listener);
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

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
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

#include "ifaddrs.h"

int posix_getifaddrs(struct ifaddrs **ifap) {
  if (ifap == NULL) {
    errno = EINVAL;
    return -1;
  }
  *ifap = NULL;
  errno = ENOSYS;
  return -1;
}

void posix_freeifaddrs(struct ifaddrs *ifa) {
  if (ifa != NULL) {
    /* no-op for now as getifaddrs returns ENOSYS */
  }
}

int dummy_posix_sockets(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sockets;
