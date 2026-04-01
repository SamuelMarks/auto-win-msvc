/* CoreFoundation/CoreFoundation.h - Strict C89 Header */
#ifndef COREFOUNDATION_COREFOUNDATION_H
#define COREFOUNDATION_COREFOUNDATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */
#include <stddef.h>
/* clang-format on */

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

void CFRunLoopRun(void);
CFSocketRef CFSocketCreateWithNative(CFAllocatorRef allocator,
                                     CFSocketNativeHandle sock,
                                     CFOptionFlags callBackTypes,
                                     CFSocketCallBack callout,
                                     const CFSocketContext *context);
void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source, void *mode);
CFRunLoopRef CFRunLoopGetCurrent(void);
void CFRunLoopStop(CFRunLoopRef rl);
CFRunLoopSourceRef CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                               CFSocketRef s, long order);
void CFSocketEnableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes);
void CFSocketDisableCallBacks(CFSocketRef s, CFOptionFlags callBackTypes);
void CFSocketInvalidate(CFSocketRef s);
void CFRelease(void *cf);

#ifdef __cplusplus
}
#endif

#endif /* COREFOUNDATION_COREFOUNDATION_H */
