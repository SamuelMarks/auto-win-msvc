#ifndef AUTO_WIN_MSVC_ERROR_H
#define AUTO_WIN_MSVC_ERROR_H

/**
 * @file auto-win-msvc-error.h
 * @brief Central error enumeration for auto-win-msvc.
 */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus) && __cplusplus >= 201703L
#define NO_DISCARD [[nodiscard]]
#elif defined(__GNUC__) || defined(__clang__)
#define NO_DISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && _MSC_VER >= 1700
#include <sal.h>
#define NO_DISCARD _Check_return_
#else
#define NO_DISCARD
#endif

/**
 * @brief Central error codes returned by auto-win-msvc operations.
 */
typedef enum NO_DISCARD auto_win_msvc_error {
  AUTO_WIN_MSVC_SUCCESS = 0,
  AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT = 1,
  AUTO_WIN_MSVC_ERROR_OUT_OF_MEMORY = 2,
  AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED = 3,
  AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED = 4,
  AUTO_WIN_MSVC_ERROR_IO = 5,
  AUTO_WIN_MSVC_ERROR_PERMISSION_DENIED = 6,
  AUTO_WIN_MSVC_ERROR_NOT_FOUND = 7,
  AUTO_WIN_MSVC_ERROR_ALREADY_EXISTS = 8,
  AUTO_WIN_MSVC_ERROR_TIMEOUT = 9,
  AUTO_WIN_MSVC_ERROR_WOULD_BLOCK = 10,
  AUTO_WIN_MSVC_ERROR_UNKNOWN = 11
} auto_win_msvc_error_t;

/**
 * @brief Returns a string representation of the error code.
 * @param err The error code.
 * @return String representation of the error.
 */
static __inline const char *
auto_win_msvc_error_string(auto_win_msvc_error_t err) {
  switch (err) {
  case AUTO_WIN_MSVC_SUCCESS:
    return "Success";
  case AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT:
    return "Invalid argument";
  case AUTO_WIN_MSVC_ERROR_OUT_OF_MEMORY:
    return "Out of memory";
  case AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED:
    return "Not implemented";
  case AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED:
    return "OS call failed";
  case AUTO_WIN_MSVC_ERROR_IO:
    return "I/O error";
  case AUTO_WIN_MSVC_ERROR_PERMISSION_DENIED:
    return "Permission denied";
  case AUTO_WIN_MSVC_ERROR_NOT_FOUND:
    return "Not found";
  case AUTO_WIN_MSVC_ERROR_ALREADY_EXISTS:
    return "Already exists";
  case AUTO_WIN_MSVC_ERROR_TIMEOUT:
    return "Timeout";
  case AUTO_WIN_MSVC_ERROR_WOULD_BLOCK:
    return "Would block";
  case AUTO_WIN_MSVC_ERROR_UNKNOWN:
  default:
    return "Unknown error";
  }
}

#include <errno.h>

/**
 * @brief Translates an auto_win_msvc_error_t to a standard errno value.
 * @param err The error code.
 * @return The standard errno value.
 */
static __inline int auto_win_msvc_error_to_errno(auto_win_msvc_error_t err) {
  switch (err) {
  case AUTO_WIN_MSVC_SUCCESS:
    return 0;
  case AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT:
    return EINVAL;
  case AUTO_WIN_MSVC_ERROR_OUT_OF_MEMORY:
    return ENOMEM;
  case AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED:
    return ENOSYS;
  case AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED:
  case AUTO_WIN_MSVC_ERROR_UNKNOWN:
    return EFAULT;
  case AUTO_WIN_MSVC_ERROR_IO:
    return EIO;
  case AUTO_WIN_MSVC_ERROR_PERMISSION_DENIED:
    return EACCES;
  case AUTO_WIN_MSVC_ERROR_NOT_FOUND:
    return ENOENT;
  case AUTO_WIN_MSVC_ERROR_ALREADY_EXISTS:
    return EEXIST;
  case AUTO_WIN_MSVC_ERROR_TIMEOUT:
    return ETIMEDOUT;
  case AUTO_WIN_MSVC_ERROR_WOULD_BLOCK:
    return EWOULDBLOCK;
  default:
    return EINVAL;
  }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUTO_WIN_MSVC_ERROR_H */
