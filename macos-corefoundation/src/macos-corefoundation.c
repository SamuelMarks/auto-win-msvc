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
#include <windows.h>
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
CFRunLoopRef CFRunLoopGetCurrent(void) {
  if (!current_run_loop) {
    current_run_loop = (__CFRunLoop *)calloc(1, sizeof(__CFRunLoop));
    if (!current_run_loop)
      return NULL;
  }
  return (CFRunLoopRef)current_run_loop;
}

/** \brief Polyfill for CFRunLoopRun
 * \return 0 or appropriate default
 * value
 */
void CFRunLoopRun(void) {
  __CFRunLoop *loop = (__CFRunLoop *)CFRunLoopGetCurrent();
  if (!loop)
    return;
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
}

/** \brief Polyfill for CFSocketCreateWithNative
 * \return 0 or
 * appropriate default value
 */
CFSocketRef CFSocketCreateWithNative(CFAllocatorRef allocator,
                                     CFSocketNativeHandle sock,
                                     CFOptionFlags callBackTypes,
                                     CFSocketCallBack callout,
                                     const CFSocketContext *context) {
  __CFSocket *s;
  (void)allocator;

  if (sock < 0 || !callout) {
    errno = EINVAL;
    return NULL;
  }

  s = (__CFSocket *)malloc(sizeof(__CFSocket));
  if (!s) {
    errno = ENOMEM;
    return NULL;
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

  return (CFSocketRef)s;
}

/** \brief Polyfill for CFRunLoopAddSource
 * \return 0 or appropriate
 * default value
 */
void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source,
                        void *mode) {
  __CFRunLoop *loop = (__CFRunLoop *)rl;
  __CFRunLoopSource *src = (__CFRunLoopSource *)source;
  (void)mode;

  if (!loop || !src)
    return;

  if (loop->num_sources >= loop->capacity) {
    int new_capacity = loop->capacity == 0 ? 4 : loop->capacity * 2;
    __CFRunLoopSource **new_sources = (__CFRunLoopSource **)realloc(
        loop->sources, new_capacity * sizeof(__CFRunLoopSource *));
    if (!new_sources)
      return;
    loop->sources = new_sources;
    loop->capacity = new_capacity;
  }

  loop->sources[loop->num_sources++] = src;
}

/** \brief Polyfill for CFRunLoopStop
 * \return 0 or appropriate
 * default value
 */
void CFRunLoopStop(CFRunLoopRef rl) {
  __CFRunLoop *loop = (__CFRunLoop *)rl;
  if (loop) {
    loop->stopped = 1;
  }
}

/** \brief Polyfill for CFSocketCreateRunLoopSource
 * \return 0 or
 * appropriate default value
 */
CFRunLoopSourceRef CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                               CFSocketRef s, long order) {
  __CFRunLoopSource *src;
  __CFSocket *sock = (__CFSocket *)s;
  (void)allocator;

  if (!sock)
    return NULL;

  src = (__CFRunLoopSource *)malloc(sizeof(__CFRunLoopSource));
  if (!src)
    return NULL;

  src->socket = sock;
  src->order = order;
  return (CFRunLoopSourceRef)src;
}

/** \brief Polyfill for CFSocketEnableCallBacks
 * \return 0 or
 * appropriate default value
 */
void CFSocketEnableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes) {
  __CFSocket *sock = (__CFSocket *)s;
  if (sock) {
    sock->callBackTypes |= callBackTypes;
  }
}

/** \brief Polyfill for CFSocketDisableCallBacks
 * \return 0 or
 * appropriate default value
 */
void CFSocketDisableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes) {
  __CFSocket *sock = (__CFSocket *)s;
  if (sock) {
    sock->callBackTypes &= ~callBackTypes;
  }
}

/** \brief Polyfill for CFSocketInvalidate
 * \return 0 or appropriate
 * default value
 */
void CFSocketInvalidate(CFSocketRef s) {
  __CFSocket *sock = (__CFSocket *)s;
  if (sock && sock->valid) {
    sock->valid = 0;
    if (sock->context.release && sock->context.info) {
      sock->context.release(sock->context.info);
      sock->context.info = NULL;
    }
  }
}

/** \brief Polyfill for CFRelease
 * \return 0 or appropriate default
 * value
 */
void CFRelease(void *cf) {
  /* Extremely naive CFRelease that just frees the memory.
     In a real CF framework, this would decrement a refcount and free if 0.
     For this polyfill, we assume it's freeing the CFSocketRef or
     CFRunLoopSourceRef. */
  if (cf) {
    free(cf);
  }
}

#else

void *(*retain)(void *info) = NULL;
void (*release)(void *info) = NULL;
const char *(*copyDescription)(void *info) = NULL;

void CFRunLoopRun(void) {}

CFSocketRef CFSocketCreateWithNative(CFAllocatorRef allocator,
                                     CFSocketNativeHandle sock,
                                     CFOptionFlags callBackTypes,
                                     CFSocketCallBack callout,
                                     const CFSocketContext *context) {
  allocator = allocator;
  sock = sock;
  callBackTypes = callBackTypes;
  callout = callout;
  context = context;
  errno = ENOSYS;
  return NULL;
}

void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source,
                        void *mode) {
  rl = rl;
  source = source;
  mode = mode;
}

CFRunLoopRef CFRunLoopGetCurrent(void) { return NULL; }

void CFRunLoopStop(CFRunLoopRef rl) { rl = rl; }

CFRunLoopSourceRef CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                               CFSocketRef s, long order) {
  allocator = allocator;
  s = s;
  order = order;
  errno = ENOSYS;
  return NULL;
}

void CFSocketEnableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes) {
  s = s;
  callBackTypes = callBackTypes;
}

void CFSocketDisableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes) {
  s = s;
  callBackTypes = callBackTypes;
}

void CFSocketInvalidate(CFSocketRef s) { s = s; }

void CFRelease(void *cf) { cf = cf; }

#endif
