/* clang-format off */
#ifndef ERR_H_
#define ERR_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The err() and warn() family of functions display a formatted error  message on the standard error output. In all cases, the last  component of the program name, a colon character, and a space are  output. If the fmt argument is not NULL, the printf(3)-like  formatted error message is output. The output is terminated by a  newline character.
 */
void err(int eval, const char *fmt, ...);

/**
 * @brief The err() and warn() family of functions display a formatted error  message on the standard error output. In all cases, the last  component of the program name, a colon character, and a space are  output. If the fmt argument is not NULL, the printf(3)-like  formatted error message is output. The output is terminated by a  newline character.
 */
void errx(int eval, const char *fmt, ...);

/**
 * @brief The err() and warn() family of functions display a formatted error  message on the standard error output. In all cases, the last  component of the program name, a colon character, and a space are  output. If the fmt argument is not NULL, the printf(3)-like  formatted error message is output. The output is terminated by a  newline character.
 */
void verr(int eval, const char *fmt, va_list args);

/**
 * @brief The err() and warn() family of functions display a formatted error  message on the standard error output. In all cases, the last  component of the program name, a colon character, and a space are  output. If the fmt argument is not NULL, the printf(3)-like  formatted error message is output. The output is terminated by a  newline character.
 */
void verrx(int eval, const char *fmt, va_list args);

/**
 * @brief The err() and warn() family of functions display a formatted error  message on the standard error output. In all cases, the last  component of the program name, a colon character, and a space are  output. If the fmt argument is not NULL, the printf(3)-like  formatted error message is output. The output is terminated by a  newline character.
 */
void vwarn(const char *fmt, va_list args);

/**
 * @brief The err() and warn() family of functions display a formatted error  message on the standard error output. In all cases, the last  component of the program name, a colon character, and a space are  output. If the fmt argument is not NULL, the printf(3)-like  formatted error message is output. The output is terminated by a  newline character.
 */
void vwarnx(const char *fmt, va_list args);

/**
 * @brief The err() and warn() family of functions display a formatted error  message on the standard error output. In all cases, the last  component of the program name, a colon character, and a space are  output. If the fmt argument is not NULL, the printf(3)-like  formatted error message is output. The output is terminated by a  newline character.
 */
void warn(const char *fmt, ...);

/**
 * @brief The err() and warn() family of functions display a formatted error  message on the standard error output. In all cases, the last  component of the program name, a colon character, and a space are  output. If the fmt argument is not NULL, the printf(3)-like  formatted error message is output. The output is terminated by a  newline character.
 */
void warnx(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* ERR_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
