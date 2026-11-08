#ifndef AUTO_WIN_MSVC_ERROR_H
#define AUTO_WIN_MSVC_ERROR_H

/**
 * @file auto-win-msvc-error.h
 * @brief Central error enumeration for auto-win-msvc.
 */

/* clang-format off */
#include <errno.h>
#if defined(_MSC_VER) && _MSC_VER >= 1700
#include <sal.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus) && __cplusplus >= 201703L
#define NO_DISCARD [[nodiscard]]
#elif defined(__clang__)
#define NO_DISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && _MSC_VER >= 1700
#define NO_DISCARD _Check_return_
#else
#define NO_DISCARD
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT 138
#endif
#ifndef EWOULDBLOCK
#ifdef EAGAIN
#define EWOULDBLOCK EAGAIN
#else
#define EWOULDBLOCK 140
#endif
#endif

/**
 * @brief Suppresses MSVC Debug CRT modal assertion/error/warning dialogs.
 *
 * Under static Debug CRT (/MTd), _CrtSetReportMode is statically linked and
 * always safe to invoke. Under dynamic Debug CRT (/MDd, _DLL), Wine's builtin
 * ucrtbased.dll leaves _CrtSetReportMode unimplemented and aborts if called,
 * whereas native Windows ucrtbased.dll requires it to prevent modal dialogs.
 */
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#if defined(_DLL)
#define AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT()                                    \
  do {                                                                         \
    static int awm_is_wine = -1;                                               \
    if (awm_is_wine < 0) {                                                     \
      char *awm_w = NULL;                                                      \
      size_t awm_wlen = 0;                                                     \
      awm_is_wine =                                                            \
          (_dupenv_s(&awm_w, &awm_wlen, "WINELOADER") == 0 && awm_w != NULL)   \
              ? 1                                                              \
              : 0;                                                             \
      if (awm_w != NULL) {                                                     \
        free(awm_w);                                                           \
      }                                                                        \
    }                                                                          \
    if (!awm_is_wine) {                                                        \
      _CrtSetReportMode(2, 0);                                                 \
      _CrtSetReportMode(1, 0);                                                 \
      _CrtSetReportMode(0, 0);                                                 \
    }                                                                          \
  } while ((void)0, 0)
#else
#define AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT()                                    \
  do {                                                                         \
    _CrtSetReportMode(2, 0);                                                   \
    _CrtSetReportMode(1, 0);                                                   \
    _CrtSetReportMode(0, 0);                                                   \
  } while ((void)0, 0)
#endif
#else
#define AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT()                                    \
  do {                                                                         \
  } while (0)
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
