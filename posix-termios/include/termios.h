#if defined(__GNUC__)
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#pragma GCC system_header
#include_next <termios.h>
#else
#include <termios.h>
#endif
#else
#include "posix-termios.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* API Contract symbols: cfgetispeed, cfgetospeed, cfsetispeed, cfsetospeed,
 * tcdrain, tcflow, tcflush, tcgetattr, tcgetsid, tcsendbreak, tcsetattr,
 * __dependencies__, __include_next__ */
