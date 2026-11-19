#ifndef ERR_H_
#define ERR_H_

/**
 * @file err.h
 * @brief BSD/Linux formatted error and warning message functions.
 */

/* clang-format off */
#include <stdarg.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
/** @brief Compiler attribute for printf-style format checking. */
#define LINUX_ERR_PRINTF(fmt_idx, arg_idx)                                     \
  __attribute__((__format__(__printf__, fmt_idx, arg_idx)))
#else
/** @brief Compiler attribute for printf-style format checking. */
#define LINUX_ERR_PRINTF(fmt_idx, arg_idx)
#endif

/**
 * @brief Display a formatted error message with errno string and exit.
 * @param[in] eval Exit status code.
 * @param[in] fmt Format string, or NULL.
 */
void err(int eval, const char *fmt, ...) LINUX_ERR_PRINTF(2, 3);

/**
 * @brief Display a formatted error message with explicit error code and exit.
 * @param[in] eval Exit status code.
 * @param[in] code Explicit errno value.
 * @param[in] fmt Format string, or NULL.
 */
void errc(int eval, int code, const char *fmt, ...) LINUX_ERR_PRINTF(3, 4);

/**
 * @brief Display a formatted error message without errno string and exit.
 * @param[in] eval Exit status code.
 * @param[in] fmt Format string, or NULL.
 */
void errx(int eval, const char *fmt, ...) LINUX_ERR_PRINTF(2, 3);

/**
 * @brief Display a va_list formatted error message with errno string and exit.
 * @param[in] eval Exit status code.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void verr(int eval, const char *fmt, va_list args) LINUX_ERR_PRINTF(2, 0);

/**
 * @brief Display a va_list formatted error message with explicit error code and
 * exit.
 * @param[in] eval Exit status code.
 * @param[in] code Explicit errno value.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void verrc(int eval, int code, const char *fmt, va_list args)
    LINUX_ERR_PRINTF(3, 0);

/**
 * @brief Display a va_list formatted error message without errno string and
 * exit.
 * @param[in] eval Exit status code.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void verrx(int eval, const char *fmt, va_list args) LINUX_ERR_PRINTF(2, 0);

/**
 * @brief Display a va_list formatted warning message with errno string.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void vwarn(const char *fmt, va_list args) LINUX_ERR_PRINTF(1, 0);

/**
 * @brief Display a va_list formatted warning message with explicit error code.
 * @param[in] code Explicit errno value.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void vwarnc(int code, const char *fmt, va_list args) LINUX_ERR_PRINTF(2, 0);

/**
 * @brief Display a va_list formatted warning message without errno string.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void vwarnx(const char *fmt, va_list args) LINUX_ERR_PRINTF(1, 0);

/**
 * @brief Display a formatted warning message with errno string.
 * @param[in] fmt Format string, or NULL.
 */
void warn(const char *fmt, ...) LINUX_ERR_PRINTF(1, 2);

/**
 * @brief Display a formatted warning message with explicit error code.
 * @param[in] code Explicit errno value.
 * @param[in] fmt Format string, or NULL.
 */
void warnc(int code, const char *fmt, ...) LINUX_ERR_PRINTF(2, 3);

/**
 * @brief Display a formatted warning message without errno string.
 * @param[in] fmt Format string, or NULL.
 */
void warnx(const char *fmt, ...) LINUX_ERR_PRINTF(1, 2);

/**
 * @brief Redirect error/warning output stream (defaults to stderr).
 * @param[in] vfp FILE pointer for output.
 */
void err_set_file(void *vfp);

/**
 * @brief Set a callback function to invoke prior to exiting in err*().
 * @param[in] ef Callback function taking exit code.
 */
void err_set_exit(void (*ef)(int));

/**
 * @brief Compilation unit anchor symbol for linux-err coverage.
 * @return Always 0.
 */
int linux_err_dummy_for_coverage(void);

/**
 * @brief Overrides the low-level termination handler used by err*() functions.
 * @param[in] fn Replacement termination callback.
 */
void linux_err_set_terminate_for_test(void (*fn)(int));

/**
 * @brief Restores the default termination handler used by err*() functions.
 */
void linux_err_reset_terminate_for_test(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ERR_H_ */

/* API Contract symbols: __dependencies__ */
