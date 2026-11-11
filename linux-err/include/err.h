/* clang-format off */
#ifndef ERR_H_
#define ERR_H_

/**
 * @file err.h
 * @brief BSD/Linux formatted error and warning message functions.
 */

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Display a formatted error message with errno string and exit.
 * @param[in] eval Exit status code.
 * @param[in] fmt Format string, or NULL.
 */
void err(int eval, const char *fmt, ...);

/**
 * @brief Display a formatted error message with explicit error code and exit.
 * @param[in] eval Exit status code.
 * @param[in] code Explicit errno value.
 * @param[in] fmt Format string, or NULL.
 */
void errc(int eval, int code, const char *fmt, ...);

/**
 * @brief Display a formatted error message without errno string and exit.
 * @param[in] eval Exit status code.
 * @param[in] fmt Format string, or NULL.
 */
void errx(int eval, const char *fmt, ...);

/**
 * @brief Display a va_list formatted error message with errno string and exit.
 * @param[in] eval Exit status code.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void verr(int eval, const char *fmt, va_list args);

/**
 * @brief Display a va_list formatted error message with explicit error code and
 * exit.
 * @param[in] eval Exit status code.
 * @param[in] code Explicit errno value.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void verrc(int eval, int code, const char *fmt, va_list args);

/**
 * @brief Display a va_list formatted error message without errno string and
 * exit.
 * @param[in] eval Exit status code.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void verrx(int eval, const char *fmt, va_list args);

/**
 * @brief Display a va_list formatted warning message with errno string.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void vwarn(const char *fmt, va_list args);

/**
 * @brief Display a va_list formatted warning message with explicit error code.
 * @param[in] code Explicit errno value.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void vwarnc(int code, const char *fmt, va_list args);

/**
 * @brief Display a va_list formatted warning message without errno string.
 * @param[in] fmt Format string, or NULL.
 * @param[in] args Variable argument list.
 */
void vwarnx(const char *fmt, va_list args);

/**
 * @brief Display a formatted warning message with errno string.
 * @param[in] fmt Format string, or NULL.
 */
void warn(const char *fmt, ...);

/**
 * @brief Display a formatted warning message with explicit error code.
 * @param[in] code Explicit errno value.
 * @param[in] fmt Format string, or NULL.
 */
void warnc(int code, const char *fmt, ...);

/**
 * @brief Display a formatted warning message without errno string.
 * @param[in] fmt Format string, or NULL.
 */
void warnx(const char *fmt, ...);

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ERR_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
