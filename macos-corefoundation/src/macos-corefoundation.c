/* macos-corefoundation.c - Strict C89 Implementation */

/* clang-format off */
#include "CoreFoundation/CoreFoundation.h"
/* clang-format on */

/** \brief Polyfill for
 * \return 0 or appropriate default value
 */
void *(*retain)(void *info) {
  (void)info;
  return 0;
}

/** \brief Polyfill for void
 * \return 0 or appropriate default
 * value

 */
void (*release)(void *info) {
  (void)info;
  return 0;
}

/** \brief Polyfill for
 * \return 0 or appropriate default
 * value

 */
const char *(*copyDescription)(void *info) {
  (void)info;
  return 0;
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
  (void)allocator;
  (void)sock;
  (void)callBackTypes;
  (void)callout;
  (void)context;
  return 0;
}

/** \brief Polyfill for CFRunLoopAddSource
 * \return 0 or appropriate
 * default value
 */
void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source,
                        void *mode) {
  (void)rl;
  (void)source;
  (void)mode;
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
void CFRunLoopStop(CFRunLoopRef rl) { (void)rl; }

/** \brief Polyfill for CFSocketCreateRunLoopSource
 * \return 0 or
 * appropriate default value
 */
CFRunLoopSourceRef CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                               CFSocketRef s, long order) {
  (void)allocator;
  (void)s;
  (void)order;
  return 0;
}

/** \brief Polyfill for CFSocketEnableCallBacks
 * \return 0 or
 * appropriate default value
 */
void CFSocketEnableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes) {
  (void)s;
  (void)callBackTypes;
}

/** \brief Polyfill for CFSocketDisableCallBacks
 * \return 0 or
 * appropriate default value
 */
void CFSocketDisableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes) {
  (void)s;
  (void)callBackTypes;
}

/** \brief Polyfill for CFSocketInvalidate
 * \return 0 or appropriate
 * default value
 */
void CFSocketInvalidate(CFSocketRef s) { (void)s; }

/** \brief Polyfill for CFRelease
 * \return 0 or appropriate default
 * value

 */
void CFRelease(void *cf) { (void)cf; }
