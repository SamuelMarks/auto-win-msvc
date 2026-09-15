#ifndef COREFOUNDATION_COREFOUNDATION_H
#define COREFOUNDATION_COREFOUNDATION_H

/**
 * @file CoreFoundation/CoreFoundation.h
 * @brief Polyfill for macOS <CoreFoundation/CoreFoundation.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
/** @brief Generic error type code. */
typedef int error_type_t;
/** @brief Success indicator for error_type_t. */
#define ERR_NONE 0
#endif

/**
 * @brief Error codes returned by macos-corefoundation functions.
 */
enum macos_corefoundation_error_code {
  /** @brief Successful operation. */
  MACOS_COREFOUNDATION_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  MACOS_COREFOUNDATION_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed to function. */
  MACOS_COREFOUNDATION_ERROR_INVALID_ARGUMENT = 2,
  /** @brief Out of memory error. */
  MACOS_COREFOUNDATION_ERROR_OUT_OF_MEMORY = 3
};

/**
 * @brief Initializes and validates the macos-corefoundation module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return MACOS_COREFOUNDATION_SUCCESS on success, or an error code on failure.
 */
enum macos_corefoundation_error_code macos_corefoundation_init(int *out_status);

typedef void *CFRunLoopRef;
typedef void *CFSocketRef;
typedef void *CFDataRef;
typedef void *CFRunLoopSourceRef;
typedef void *CFAllocatorRef;
typedef int CFSocketNativeHandle;
typedef unsigned long CFOptionFlags;
typedef void (*CFSocketCallBack)(CFSocketRef s, int type, CFDataRef address,
                                 const void *data, void *info);

/**
 * @brief Context structure for a CFSocket.
 */
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

/**
 * @brief Runs the current run loop.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFRunLoopRun(void);

/**
 * @brief Creates a CFSocket object from a native socket handle.
 * @param allocator Memory allocator.
 * @param sock Native socket descriptor.
 * @param callBackTypes Callback event flags.
 * @param callout Function pointer called on socket events.
 * @param context Socket context structure.
 * @param[out] out_ref Pointer receiving created CFSocket.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t
CFSocketCreateWithNative(CFAllocatorRef allocator, CFSocketNativeHandle sock,
                         CFOptionFlags callBackTypes, CFSocketCallBack callout,
                         const CFSocketContext *context, CFSocketRef *out_ref);

/**
 * @brief Adds a run loop source to a run loop.
 * @param rl Run loop reference.
 * @param source Source reference.
 * @param mode Mode name (unused).
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source,
                                void *mode);

/**
 * @brief Retrieves the run loop for the current thread.
 * @param[out] out_ref Pointer receiving current run loop.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFRunLoopGetCurrent(CFRunLoopRef *out_ref);

/**
 * @brief Stops a running run loop.
 * @param rl Run loop reference.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFRunLoopStop(CFRunLoopRef rl);

/**
 * @brief Creates a run loop source for a CFSocket.
 * @param allocator Allocator reference.
 * @param s CFSocket reference.
 * @param order Order index.
 * @param[out] out_ref Pointer receiving created source.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFSocketCreateRunLoopSource(CFAllocatorRef allocator,
                                         CFSocketRef s, long order,
                                         CFRunLoopSourceRef *out_ref);

/**
 * @brief Enables callback events on a socket.
 * @param s Socket reference.
 * @param callBackTypes Event types to enable.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFSocketEnableCallBacks(CFSocketRef s,
                                     CFOptionFlags callBackTypes);

/**
 * @brief Disables callback events on a socket.
 * @param s Socket reference.
 * @param callBackTypes Event types to disable.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFSocketDisableCallBacks(CFSocketRef s,
                                      CFOptionFlags callBackTypes);

/**
 * @brief Invalidates a socket object.
 * @param s Socket reference.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFSocketInvalidate(CFSocketRef s);

/**
 * @brief Releases a CoreFoundation object reference.
 * @param cf Object pointer.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t CFRelease(void *cf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COREFOUNDATION_COREFOUNDATION_H */
