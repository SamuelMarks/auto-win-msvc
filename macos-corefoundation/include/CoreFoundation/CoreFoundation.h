/* CoreFoundation/CoreFoundation.h - Strict C89 Header */
#ifndef COREFOUNDATION_COREFOUNDATION_H
#define COREFOUNDATION_COREFOUNDATION_H

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
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

error_type_t CFRunLoopRun(void);
error_type_t
CFSocketCreateWithNative(CFAllocatorRef allocator, CFSocketNativeHandle sock,
                         CFOptionFlags callBackTypes, CFSocketCallBack callout,
                         const CFSocketContext *context, CFSocketRef *out_ref);
error_type_t CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source,
                                void *mode);
error_type_t CFRunLoopGetCurrent(CFRunLoopRef *out_ref);
error_type_t CFRunLoopStop(CFRunLoopRef rl);
error_type_t CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                         CFSocketRef s, long order,
                                         CFRunLoopSourceRef *out_ref);
error_type_t CFSocketEnableCallBacks(CFSocketRef s,
                                     CFOptionFlags callBackTypes);
error_type_t CFSocketDisableCallBacks(CFSocketRef s,
                                      CFOptionFlags callBackTypes);
error_type_t CFSocketInvalidate(CFSocketRef s);
error_type_t CFRelease(void *cf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COREFOUNDATION_COREFOUNDATION_H */
