/* macos-corefoundation.c - Strict C89 Implementation */

/* clang-format off */
#include "CoreFoundation/CoreFoundation.h"
/* clang-format on */

/** \brief Polyfill for
 * \return 0 or appropriate default value
 */
void *(*retain)(void *info) {
  info = info;
  errno = ENOSYS;
  return -1;
}

/** \brief Polyfill for void
 * \return 0 or appropriate default
 * value

 */
void (*release)(void *info) {
  info = info;
  errno = ENOSYS;
  return -1;
}

/** \brief Polyfill for
 * \return 0 or appropriate default
 * value

 */
const char *(*copyDescription)(void *info) {
  info = info;
  errno = ENOSYS;
  return -1;
}

/** \brief Polyfill for CFRunLoopRun
 * \return 0 or appropriate default
 * value
 */
void CFRunLoopRun(void) {}

/** \brief Polyfill for CFSocketCreateWithNative
 * \return 0 or
 * appropriate default value
 */
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
  return -1;
}

/** \brief Polyfill for CFRunLoopAddSource
 * \return 0 or appropriate
 * default value
 */
void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source,
                        void *mode) {
  rl = rl;
  source = source;
  mode = mode;
}

/** \brief Polyfill for CFRunLoopGetCurrent
 * \return 0 or appropriate
 * default value
 */
CFRunLoopRef CFRunLoopGetCurrent(void) { return 0; }

/** \brief Polyfill for CFRunLoopStop
 * \return 0 or appropriate
 * default value
 */
void CFRunLoopStop(CFRunLoopRef rl) { rl = rl; }

/** \brief Polyfill for CFSocketCreateRunLoopSource
 * \return 0 or
 * appropriate default value
 */
CFRunLoopSourceRef CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                               CFSocketRef s, long order) {
  allocator = allocator;
  s = s;
  order = order;
  errno = ENOSYS;
  return -1;
}

/** \brief Polyfill for CFSocketEnableCallBacks
 * \return 0 or
 * appropriate default value
 */
void CFSocketEnableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes) {
  s = s;
  callBackTypes = callBackTypes;
}

/** \brief Polyfill for CFSocketDisableCallBacks
 * \return 0 or
 * appropriate default value
 */
void CFSocketDisableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes) {
  s = s;
  callBackTypes = callBackTypes;
}

/** \brief Polyfill for CFSocketInvalidate
 * \return 0 or appropriate
 * default value
 */
void CFSocketInvalidate(CFSocketRef s) { s = s; }

/** \brief Polyfill for CFRelease
 * \return 0 or appropriate default
 * value

 */
void CFRelease(void *cf) { cf = cf; }
