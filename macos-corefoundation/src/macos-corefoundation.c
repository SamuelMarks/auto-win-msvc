/* macos-corefoundation.c - Strict C89 Implementation */

/* clang-format off */
#include "CoreFoundation/CoreFoundation.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER
#undef FD_ZERO
static void posix_fd_zero(fd_set *set) { set->fd_count = 0; }
#define FD_ZERO(set) posix_fd_zero((fd_set*)set)
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

#include <winsock2.h>
/* clang-format on */
#endif

#if defined(_WIN32)

#if defined(_MSC_VER)
#define CF_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define CF_THREAD_LOCAL __thread
#else
#define CF_THREAD_LOCAL
#endif

typedef struct __CFSocket {
  CFSocketNativeHandle sock;
  CFOptionFlags callBackTypes;
  CFSocketCallBack callout;
  CFSocketContext context;
  int valid;
} __CFSocket;

typedef struct __CFRunLoopSource {
  __CFSocket *socket;
  long order;
} __CFRunLoopSource;

typedef struct __CFRunLoop {
  int stopped;
  __CFRunLoopSource **sources;
  int num_sources;
  int capacity;
} __CFRunLoop;

static CF_THREAD_LOCAL __CFRunLoop *current_run_loop = NULL;

/** \brief Polyfill for
 * \return 0 or appropriate default value
 */
void *(*retain)(void *info) = NULL;

/** \brief Polyfill for void
 * \return 0 or appropriate default
 * value
 */
void (*release)(void *info) = NULL;

/** \brief Polyfill for
 * \return 0 or appropriate default
 * value
 */
const char *(*copyDescription)(void *info) = NULL;

/** \brief Polyfill for CFRunLoopGetCurrent
 * \return 0 or appropriate
 * default value
 */
error_type_t CFRunLoopGetCurrent(CFRunLoopRef *out_ref) {
  if (!current_run_loop) {
    current_run_loop = (__CFRunLoop *)calloc(1, sizeof(__CFRunLoop));
    if (!current_run_loop) {
      if (out_ref) {
        *out_ref = NULL;
      }
      return ENOMEM;
    }
  }
  if (out_ref) {
    *out_ref = (CFRunLoopRef)current_run_loop;
  }
  return ERR_NONE;
}

/** \brief Polyfill for CFRunLoopRun
 * \return 0 or appropriate default
 * value
 */
error_type_t CFRunLoopRun(void) {
  __CFRunLoop *loop;
  CFRunLoopRef current_ref;
  error_type_t err = CFRunLoopGetCurrent(&current_ref);
  if (err != ERR_NONE) {
    return err;
  }

  loop = (__CFRunLoop *)current_ref;
  if (!loop)
    return EINVAL;
  loop->stopped = 0;

  while (!loop->stopped) {
    fd_set readfds, writefds, exceptfds;
    int maxfd = -1;
    int i;
    struct timeval tv;
    int active_sources = 0;

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

    for (i = 0; i < loop->num_sources; i++) {
      __CFRunLoopSource *src = loop->sources[i];
      if (src && src->socket && src->socket->valid) {
        if (src->socket->callBackTypes & kCFSocketReadCallBack ||
            src->socket->callBackTypes & kCFSocketAcceptCallBack ||
            src->socket->callBackTypes & kCFSocketDataCallBack) {
          FD_SET((SOCKET)src->socket->sock, &readfds);
        }
        if (src->socket->callBackTypes & kCFSocketWriteCallBack ||
            src->socket->callBackTypes & kCFSocketConnectCallBack) {
          FD_SET((SOCKET)src->socket->sock, &writefds);
        }
        /* exceptfds for out-of-band data or connect errors */
        if (src->socket->callBackTypes & kCFSocketConnectCallBack) {
          FD_SET((SOCKET)src->socket->sock, &exceptfds);
        }

        if ((int)src->socket->sock > maxfd) {
          maxfd = (int)src->socket->sock;
        }
        active_sources++;
      }
    }

    if (active_sources == 0) {
      /* No sources, sleep briefly to prevent spinning, then check stopped */
      Sleep(10);
      continue;
    }

    tv.tv_sec = 0;
    tv.tv_usec = 10000; /* 10ms */

#undef select
    if (select(maxfd + 1, &readfds, &writefds, &exceptfds, &tv) > 0) {
      for (i = 0; i < loop->num_sources; i++) {
        __CFRunLoopSource *src = loop->sources[i];
        if (src && src->socket && src->socket->valid) {
          __CFSocket *sock = src->socket;
          int triggered = 0;
          int type = 0;

          if (FD_ISSET((SOCKET)sock->sock, &readfds)) {
            if (sock->callBackTypes & kCFSocketReadCallBack) {
              type = kCFSocketReadCallBack;
              triggered = 1;
            } else if (sock->callBackTypes & kCFSocketAcceptCallBack) {
              type = kCFSocketAcceptCallBack;
              triggered = 1;
            } else if (sock->callBackTypes & kCFSocketDataCallBack) {
              type = kCFSocketDataCallBack;
              triggered = 1;
            }
          }
          if (!triggered && FD_ISSET((SOCKET)sock->sock, &writefds)) {
            if (sock->callBackTypes & kCFSocketWriteCallBack) {
              type = kCFSocketWriteCallBack;
              triggered = 1;
            } else if (sock->callBackTypes & kCFSocketConnectCallBack) {
              type = kCFSocketConnectCallBack;
              triggered = 1;
            }
          }
          if (!triggered && FD_ISSET((SOCKET)sock->sock, &exceptfds)) {
            if (sock->callBackTypes & kCFSocketConnectCallBack) {
              type = kCFSocketConnectCallBack;
              triggered = 1;
            }
          }

          if (triggered && type && sock->callout) {
            sock->callout((CFSocketRef)sock, type, NULL, NULL,
                          sock->context.info);
          }
        }
      }
    }
  }
  return ERR_NONE;
}

/** \brief Polyfill for CFSocketCreateWithNative
 * \return 0 or
 * appropriate default value
 */
error_type_t
CFSocketCreateWithNative(CFAllocatorRef allocator, CFSocketNativeHandle sock,
                         CFOptionFlags callBackTypes, CFSocketCallBack callout,
                         const CFSocketContext *context, CFSocketRef *out_ref) {
  __CFSocket *s;
  if (allocator) { /* unused */
  }

  if (sock < 0 || !callout) {
    return EINVAL;
  }

  s = (__CFSocket *)malloc(sizeof(__CFSocket));
  if (!s) {
    return ENOMEM;
  }

  s->sock = sock;
  s->callBackTypes = callBackTypes;
  s->callout = callout;
  s->valid = 1;
  if (context) {
    memcpy(&s->context, context, sizeof(CFSocketContext));
    if (s->context.retain && s->context.info) {
      s->context.info = s->context.retain(s->context.info);
    }
  } else {
    memset(&s->context, 0, sizeof(CFSocketContext));
  }

  if (out_ref) {
    *out_ref = (CFSocketRef)s;
  }
  return ERR_NONE;
}

/** \brief Polyfill for CFRunLoopAddSource
 * \return 0 or appropriate
 * default value
 */
error_type_t CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source,
                                void *mode) {
  __CFRunLoop *loop = (__CFRunLoop *)rl;
  __CFRunLoopSource *src = (__CFRunLoopSource *)source;
  if (mode) { /* unused */
  }

  if (!loop || !src)
    return EINVAL;

  if (loop->num_sources >= loop->capacity) {
    int new_capacity = loop->capacity == 0 ? 4 : loop->capacity * 2;
    __CFRunLoopSource **new_sources = (__CFRunLoopSource **)realloc(
        loop->sources, new_capacity * sizeof(__CFRunLoopSource *));
    if (!new_sources)
      return ENOMEM;
    loop->sources = new_sources;
    loop->capacity = new_capacity;
  }

  loop->sources[loop->num_sources++] = src;
  return ERR_NONE;
}

/** \brief Polyfill for CFRunLoopStop
 * \return 0 or appropriate
 * default value
 */
error_type_t CFRunLoopStop(CFRunLoopRef rl) {
  __CFRunLoop *loop = (__CFRunLoop *)rl;
  if (loop) {
    loop->stopped = 1;
  }
  return ERR_NONE;
}

/** \brief Polyfill for CFSocketCreateRunLoopSource
 * \return 0 or
 * appropriate default value
 */
error_type_t CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                         CFSocketRef s, long order,
                                         CFRunLoopSourceRef *out_ref) {
  __CFRunLoopSource *src;
  __CFSocket *sock = (__CFSocket *)s;
  if (allocator) { /* unused */
  }

  if (!sock) {
    return EINVAL;
  }

  src = (__CFRunLoopSource *)malloc(sizeof(__CFRunLoopSource));
  if (!src) {
    return ENOMEM;
  }

  src->socket = sock;
  src->order = order;
  if (out_ref) {
    *out_ref = (CFRunLoopSourceRef)src;
  }
  return ERR_NONE;
}

/** \brief Polyfill for CFSocketEnableCallBacks
 * \return 0 or
 * appropriate default value
 */
error_type_t CFSocketEnableCallBacks(CFSocketRef s,
                                     CFOptionFlags callBackTypes) {
  __CFSocket *sock = (__CFSocket *)s;
  if (sock) {
    sock->callBackTypes |= callBackTypes;
  }
  return ERR_NONE;
}

/** \brief Polyfill for CFSocketDisableCallBacks
 * \return 0 or
 * appropriate default value
 */
error_type_t CFSocketDisableCallBacks(CFSocketRef s,
                                      CFOptionFlags callBackTypes) {
  __CFSocket *sock = (__CFSocket *)s;
  if (sock) {
    sock->callBackTypes &= ~callBackTypes;
  }
  return ERR_NONE;
}

/** \brief Polyfill for CFSocketInvalidate
 * \return 0 or appropriate
 * default value
 */
error_type_t CFSocketInvalidate(CFSocketRef s) {
  __CFSocket *sock = (__CFSocket *)s;
  if (sock && sock->valid) {
    sock->valid = 0;
    if (sock->context.release && sock->context.info) {
      sock->context.release(sock->context.info);
      sock->context.info = NULL;
    }
  }
  return ERR_NONE;
}

/** \brief Polyfill for CFRelease
 * \return 0 or appropriate default
 * value
 */
error_type_t CFRelease(void *cf) {
  /* Extremely naive CFRelease that just frees the memory.
     In a real CF framework, this would decrement a refcount and free if 0.
     For this polyfill, we assume it's freeing the CFSocketRef or
     CFRunLoopSourceRef. */
  if (cf) {
    free(cf);
  }
  return ERR_NONE;
}

#else

void *(*retain)(void *info) = NULL;
void (*release)(void *info) = NULL;
const char *(*copyDescription)(void *info) = NULL;

error_type_t CFRunLoopRun(void) { return ERR_NONE; }

error_type_t
CFSocketCreateWithNative(CFAllocatorRef allocator, CFSocketNativeHandle sock,
                         CFOptionFlags callBackTypes, CFSocketCallBack callout,
                         const CFSocketContext *context, CFSocketRef *out_ref) {
  if (allocator) { /* unused */
  }
  if (sock) { /* unused */
  }
  if (callBackTypes) { /* unused */
  }
  if (callout) { /* unused */
  }
  if (context) { /* unused */
  }
  if (out_ref) {
    *out_ref = NULL;
  }
  return ENOSYS;
}

error_type_t CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source,
                                void *mode) {
  if (rl) { /* unused */
  }
  if (source) { /* unused */
  }
  if (mode) { /* unused */
  }
  return ERR_NONE;
}

error_type_t CFRunLoopGetCurrent(CFRunLoopRef *out_ref) {
  if (out_ref) {
    *out_ref = NULL;
  }
  return ERR_NONE;
}

error_type_t CFRunLoopStop(CFRunLoopRef rl) {
  if (rl) { /* unused */
  }
  return ERR_NONE;
}

error_type_t CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                         CFSocketRef s, long order,
                                         CFRunLoopSourceRef *out_ref) {
  if (allocator) { /* unused */
  }
  if (s) { /* unused */
  }
  if (order) { /* unused */
  }
  if (out_ref) {
    *out_ref = NULL;
  }
  return ENOSYS;
}

error_type_t CFSocketEnableCallBacks(CFSocketRef s,
                                     CFOptionFlags callBackTypes) {
  if (s) { /* unused */
  }
  if (callBackTypes) { /* unused */
  }
  return ERR_NONE;
}

error_type_t CFSocketDisableCallBacks(CFSocketRef s,
                                      CFOptionFlags callBackTypes) {
  if (s) { /* unused */
  }
  if (callBackTypes) { /* unused */
  }
  return ERR_NONE;
}

error_type_t CFSocketInvalidate(CFSocketRef s) {
  if (s) { /* unused */
  }
  return ERR_NONE;
}

error_type_t CFRelease(void *cf) {
  if (cf) { /* unused */
  }
  return ERR_NONE;
}

#endif
