/* CoreFoundation/CoreFoundation.h - Strict C89 Header */
#ifndef COREFOUNDATION_COREFOUNDATION_H
#define COREFOUNDATION_COREFOUNDATION_H
#include "auto_win_msvc_export.h"

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef void *CFRunLoopRef;
typedef void *CFSocketRef;
typedef void *CFDataRef;
typedef void *CFRunLoopSourceRef;
typedef void *CFAllocatorRef;
typedef int CFSocketNativeHandle;
typedef unsigned long CFOptionFlags;
typedef void (*CFSocketCallBack)(CFSocketRef s, int type, CFDataRef address,
                                 const void *data, void *info);
typedef struct {
  long version;
  void *info;
  void *(*retain)(void *info);
  void (*release)(void *info);
  const char *(*copyDescription)(void *info);
} CFSocketContext;

enum {
  kCFSocketReadCallBack = 1,
  kCFSocketAcceptCallBack = 2,
  kCFSocketDataCallBack = 3,
  kCFSocketConnectCallBack = 4,
  kCFSocketWriteCallBack = 8
};
typedef int CFSocketCallBackType;

AUTO_WIN_MSVC_EXPORT void CFRunLoopRun(void);
AUTO_WIN_MSVC_EXPORT CFSocketRef AUTO_WIN_MSVC_EXPORT
CFSocketCreateWithNative(CFAllocatorRef allocator, CFSocketNativeHandle sock,
                         CFOptionFlags callBackTypes, CFSocketCallBack callout,
                         const CFSocketContext *context);
AUTO_WIN_MSVC_EXPORT void AUTO_WIN_MSVC_EXPORT
CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source, void *mode);
AUTO_WIN_MSVC_EXPORT CFRunLoopRef CFRunLoopGetCurrent(void);
AUTO_WIN_MSVC_EXPORT void CFRunLoopStop(CFRunLoopRef rl);
AUTO_WIN_MSVC_EXPORT CFRunLoopSourceRef CFSocketCreateRunLoopSource(
    CFAllocatorRef allocator, CFSocketRef s, long order);
AUTO_WIN_MSVC_EXPORT void CFSocketEnableCallBacks(CFSocketRef s,
                                                  CFOptionFlags callBackTypes);
AUTO_WIN_MSVC_EXPORT void CFSocketDisableCallBacks(CFSocketRef s,
                                                   CFOptionFlags callBackTypes);
AUTO_WIN_MSVC_EXPORT void CFSocketInvalidate(CFSocketRef s);
AUTO_WIN_MSVC_EXPORT void CFRelease(void *cf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COREFOUNDATION_COREFOUNDATION_H */
