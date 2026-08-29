/* posix-sockets.c - Strict C89 Implementation */
/* clang-format off */
#if defined(_MSC_VER)
#endif
#include "posix-sockets.h"



#include <errno.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle((int)(fd)))
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif

#define GET_SOCKET(fd) ((safe_get_osfhandle(fd) == -1) ? (SOCKET)(fd) : (SOCKET)safe_get_osfhandle(fd))

#ifdef _WIN32


static int _wsaErrorToErrno(int err) {
    switch (err) {
    case WSAEWOULDBLOCK: return EWOULDBLOCK;
    case WSAEINPROGRESS: return EINPROGRESS;
    case WSAEALREADY: return EALREADY;
    case WSAENOTSOCK: return ENOTSOCK;
    case WSAEDESTADDRREQ: return EDESTADDRREQ;
    case WSAEMSGSIZE: return EMSGSIZE;
    case WSAEPROTOTYPE: return EPROTOTYPE;
    case WSAENOPROTOOPT: return ENOPROTOOPT;
    case WSAEPROTONOSUPPORT: return EPROTONOSUPPORT;
    case WSAEOPNOTSUPP: return EOPNOTSUPP;
    case WSAEAFNOSUPPORT: return EAFNOSUPPORT;
    case WSAEADDRINUSE: return EADDRINUSE;
    case WSAEADDRNOTAVAIL: return EADDRNOTAVAIL;
    case WSAENETDOWN: return ENETDOWN;
    case WSAENETUNREACH: return ENETUNREACH;
    case WSAENETRESET: return ENETRESET;
    case WSAECONNABORTED: return ECONNABORTED;
    case WSAECONNRESET: return ECONNRESET;
    case WSAENOBUFS: return ENOBUFS;
    case WSAEISCONN: return EISCONN;
    case WSAENOTCONN: return ENOTCONN;
    case WSAETIMEDOUT: return ETIMEDOUT;
    case WSAECONNREFUSED: return ECONNREFUSED;
    case WSAELOOP: return ELOOP;
    case WSAENAMETOOLONG: return ENAMETOOLONG;
    case WSAEHOSTUNREACH: return EHOSTUNREACH;
    case WSAENOTEMPTY: return ENOTEMPTY;
    default: return EINVAL;
    }
}

#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
static void __cdecl null_invalid_parameter_handler_auto(const wchar_t* a, const wchar_t* b, const wchar_t* c, unsigned int d, uintptr_t e) {
  (void)a;
  (void)b;
  (void)c;
  (void)d;
  (void)e;
}

static void __cdecl __init_winsock_auto(void) {
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  _set_invalid_parameter_handler((_invalid_parameter_handler)null_invalid_parameter_handler_auto);
  #if defined(_MSC_VER)
  _CrtSetReportMode(_CRT_ASSERT, 0);
#endif

}

#if defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
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
void clear_nonblock(SOCKET s) { set_nonblock(s, 0); }
void copy_nonblock(SOCKET src, SOCKET dst) {
  if (get_nonblock(src))
    set_nonblock(dst, 1);
}

static unsigned char g_is_socket[8192] = {0};
void mark_as_socket(intptr_t fd) {
  if (fd >= 0 && fd < 8192)
    g_is_socket[fd] = 1;
}
void clear_as_socket(intptr_t fd) {
  if (fd >= 0 && fd < 8192)
    g_is_socket[fd] = 0;
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
void posix_freeaddrinfo(struct addrinfo *ai) {
  (void)ai;
#ifdef _WIN32
#undef freeaddrinfo
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
  sprintf(buf, "Unknown error %d", (int)ecode);
#endif
  return buf;
#endif
}

int posix_getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints, struct addrinfo **res) {
#ifdef _WIN32
#undef getaddrinfo
  int ret = getaddrinfo(nodename, servname, hints, res);
  if (ret != 0) {
    errno = _wsaErrorToErrno(WSAGetLastError());
  }
  return ret;
#else
  (void)nodename;
  (void)servname;
  (void)hints;
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
  (void)sa;
  (void)salen;
  (void)node;
  (void)nodelen;
  (void)service;
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
  (void)name;
  (void)proto;
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
}

struct servent *posix_getservbyport(int port, const char *proto) {
  (void)port;
  (void)proto;
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return NULL;
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
  (void)nfds;
  (void)readfds;
  (void)writefds;
  (void)errorfds;
  (void)timeout;
  (void)sigmask;
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
#endif

extern int is_socket(intptr_t fd);

int posix_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                 struct timeval *timeout) {
#if defined(_WIN32)
  fd_set ws_readfds = {0}, ws_writefds = {0}, ws_errorfds = {0};
  fd_set *p_ws_readfds = NULL, *p_ws_writefds = NULL, *p_ws_errorfds = NULL;
  unsigned int i;
  int ret;
  (void)nfds;

  if (readfds) {
#if defined(_MSC_VER)
#endif
    FD_ZERO(&ws_readfds);
#if defined(_MSC_VER)
#endif
    for (i = 0; i < readfds->fd_count; i++) {
      intptr_t fd = (intptr_t)readfds->fd_array[i];
      if (is_socket(fd)) {
        intptr_t handle = safe_get_osfhandle(fd);
        SOCKET s = (handle == -1) ? (SOCKET)fd : (SOCKET)handle;
        if (s != INVALID_SOCKET) {
#if defined(_MSC_VER)
#endif
          FD_SET(s, &ws_readfds);
#if defined(_MSC_VER)
#endif
        }
      } else {
#if defined(_MSC_VER)
#endif
        FD_SET((SOCKET)fd, &ws_readfds); /* Pass through */
#if defined(_MSC_VER)
#endif
      }
    }
    p_ws_readfds = &ws_readfds;
  }

  if (writefds) {
#if defined(_MSC_VER)
#endif
    FD_ZERO(&ws_writefds);
#if defined(_MSC_VER)
#endif
    for (i = 0; i < writefds->fd_count; i++) {
      intptr_t fd = (intptr_t)writefds->fd_array[i];
      if (is_socket(fd)) {
        intptr_t handle = safe_get_osfhandle(fd);
        SOCKET s = (handle == -1) ? (SOCKET)fd : (SOCKET)handle;
        if (s != INVALID_SOCKET) {
#if defined(_MSC_VER)
#endif
          FD_SET(s, &ws_writefds);
#if defined(_MSC_VER)
#endif
        }
      } else {
#if defined(_MSC_VER)
#endif
        FD_SET((SOCKET)fd, &ws_writefds);
#if defined(_MSC_VER)
#endif
      }
    }
    p_ws_writefds = &ws_writefds;
  }

  if (errorfds) {
#if defined(_MSC_VER)
#endif
    FD_ZERO(&ws_errorfds);
#if defined(_MSC_VER)
#endif
    for (i = 0; i < errorfds->fd_count; i++) {
      intptr_t fd = (intptr_t)errorfds->fd_array[i];
      if (is_socket(fd)) {
        intptr_t handle = safe_get_osfhandle(fd);
        SOCKET s = (handle == -1) ? (SOCKET)fd : (SOCKET)handle;
        if (s != INVALID_SOCKET) {
#if defined(_MSC_VER)
#endif
          FD_SET(s, &ws_errorfds);
#if defined(_MSC_VER)
#endif
        }
      } else {
#if defined(_MSC_VER)
#endif
        FD_SET((SOCKET)fd, &ws_errorfds);
#if defined(_MSC_VER)
#endif
      }
    }
    p_ws_errorfds = &ws_errorfds;
  }

#undef select
  ret = select(0, p_ws_readfds, p_ws_writefds, p_ws_errorfds, timeout);
#define select posix_select
  if (ret == SOCKET_ERROR) {
    errno = EBADF; /* Simplify */
    return -1;
  }

  if (readfds) {
    fd_set orig = *readfds;
    FD_ZERO(readfds);
    for (i = 0; i < orig.fd_count; i++) {
      intptr_t fd = (intptr_t)orig.fd_array[i];
      intptr_t handle = safe_get_osfhandle(fd);
      SOCKET s = (handle == -1) ? (SOCKET)fd : (SOCKET)handle;
      if (FD_ISSET(s, &ws_readfds)) {
        FD_SET((SOCKET)fd, readfds);
      }
    }
  }

  if (writefds) {
    fd_set orig = *writefds;
    FD_ZERO(writefds);
    for (i = 0; i < orig.fd_count; i++) {
      intptr_t fd = (intptr_t)orig.fd_array[i];
      intptr_t handle = safe_get_osfhandle(fd);
      SOCKET s = (handle == -1) ? (SOCKET)fd : (SOCKET)handle;
      if (FD_ISSET(s, &ws_writefds)) {
        FD_SET((SOCKET)fd, writefds);
      }
    }
  }

  if (errorfds) {
    fd_set orig = *errorfds;
    FD_ZERO(errorfds);
    for (i = 0; i < orig.fd_count; i++) {
      intptr_t fd = (intptr_t)orig.fd_array[i];
      intptr_t handle = safe_get_osfhandle(fd);
      SOCKET s = (handle == -1) ? (SOCKET)fd : (SOCKET)handle;
      if (FD_ISSET(s, &ws_errorfds)) {
        FD_SET((SOCKET)fd, errorfds);
      }
    }
  }

  return ret;
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

  {
    intptr_t fd = _open_osfhandle((intptr_t)ret, 0);
    if (fd == -1) {
      closesocket(ret);
      return -1;
    }
    mark_as_socket(fd);
    return (int)fd;
  }
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
  if (address->sa_family == AF_UNIX) {
    struct sockaddr_in fake_addr;

    memset(&fake_addr, 0, sizeof(fake_addr));
    fake_addr.sin_family = AF_INET;
    fake_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    fake_addr.sin_port = 0;
    ret = bind(GET_SOCKET(socket),
               (const struct sockaddr *)&fake_addr, sizeof(fake_addr));
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
  int ret = connect(GET_SOCKET(socket), address, address_len);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
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

int posix_getsockopt(intptr_t socket, int level, int option_name, void *option_value,
                     posix_socklen_t *option_len) {
#ifdef _WIN32
#undef getsockopt
  int ret;
  if (level == SOL_SOCKET &&
      (option_name == SO_SNDTIMEO || option_name == SO_RCVTIMEO)) {
    if (option_len && *option_len == sizeof(struct timeval)) {
      DWORD ms = 0;
      int ms_len = sizeof(DWORD);
      ret = getsockopt(GET_SOCKET(socket), level, option_name,
                       (char *)&ms, &ms_len);
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

posix_ssize_t posix_recv(intptr_t socket, void *buffer, size_t length, int flags) {
#ifdef _WIN32
#undef recv
  int ret =
      recv(GET_SOCKET(socket), (char *)buffer, (int)length, flags);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
    return -1;
  }
  return ret;
#else
  (void)socket;
  (void)buffer;
  (void)length;
  (void)flags;
  errno = EINVAL;
  return -1;
#endif
}

/** \brief posix_recvfrom function. */
posix_ssize_t posix_recvfrom(intptr_t socket, void *buffer, size_t length, int flags,
                             struct sockaddr *address,
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
posix_ssize_t posix_recvmsg(intptr_t socket, struct msghdr *message, int flags) {
  (void)socket;
  (void)message;
  (void)flags;
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return -1;
}

posix_ssize_t posix_send(intptr_t socket, const void *message, size_t length,
                         int flags) {
#ifdef _WIN32
#undef send
  int ret = send(GET_SOCKET(socket), (const char *)message,
                 (int)length, flags);
  if (ret == SOCKET_ERROR) {
    errno = _wsaErrorToErrno(WSAGetLastError());
    return -1;
  }
  return ret;
#else
  (void)socket;
  (void)message;
  (void)length;
  (void)flags;
  errno = EINVAL;
  return -1;
#endif
}

/** \brief posix_sendmsg function. */
posix_ssize_t posix_sendmsg(intptr_t socket, const struct msghdr *message,
                            int flags) {
  (void)socket;
  (void)message;
  (void)flags;
#ifdef _WIN32
  /** \brief posix_accept function. */
#endif
  errno = EINVAL;
  return -1;
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
  s = WSASocketW(domain, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
  if (s == INVALID_SOCKET) {
    errno = _wsaErrorToErrno(WSAGetLastError());
    return -1;
  }

  {
    intptr_t fd = _open_osfhandle((intptr_t)s, 0);
    if (fd == -1) {
      closesocket(s);
      return -1;
    }
    mark_as_socket(fd);
    return (int)fd;
  }
#else
  (void)domain;
  (void)type;
  (void)protocol;
  errno = EINVAL;
  return -1;
#endif
}

/** \brief posix_socketpair function. */

#if defined(_WIN32)
#include <fcntl.h>
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <winsock2.h>
#endif
#include <ws2tcpip.h>

#ifdef _MSC_VER
#undef FD_ZERO
#define FD_ZERO(set) (((fd_set *)(set))->fd_count = 0)
#undef FD_SET
#define FD_SET(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set *)(set))->fd_count) { \
        if (((fd_set *)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set *)(set))->fd_array[__i] = (fd); \
            ((fd_set *)(set))->fd_count++; \
        } \
    } \
} while((void)0, 0)
#undef FD_CLR
#define FD_CLR(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == (fd)) { \
            while (__i < ((fd_set *)(set))->fd_count - 1) { \
                ((fd_set *)(set))->fd_array[__i] = \
                    ((fd_set *)(set))->fd_array[__i + 1]; \
                __i++; \
            } \
            ((fd_set *)(set))->fd_count--; \
            break; \
        } \
    } \
} while((void)0, 0)
#endif

/* clang-format on */

#endif

int posix_socketpair(int domain, int type, int protocol,
                     intptr_t socket_vector[2]) {
#if defined(_WIN32)
  SOCKET listener, client, server;
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  if (domain != AF_INET && domain != AF_UNSPEC)
    return -1;

  listener = WSASocketW(AF_INET, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
  if (listener == INVALID_SOCKET) {
    if (WSAGetLastError() == WSANOTINITIALISED) {
      WSADATA wsaData;
      WSAStartup(MAKEWORD(2, 2), &wsaData);
      listener =
          WSASocketW(AF_INET, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
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

  client = WSASocketW(AF_INET, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
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
    intptr_t fd_server = _open_osfhandle((intptr_t)server, 0);
    intptr_t fd_client = _open_osfhandle((intptr_t)client, 0);
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
    mark_as_socket(fd_server);
    mark_as_socket(fd_client);
    return 0;
  }

err:
  closesocket(listener);
  return -1;
#else
  (void)domain;
  (void)type;
  (void)protocol;
  (void)socket_vector;
  errno = EINVAL;
  return -1;
#endif
}

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_sockets(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sockets;
