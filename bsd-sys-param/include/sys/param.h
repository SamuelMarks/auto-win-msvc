/* clang-format off */
#ifndef SYS_PARAM_H_
#define SYS_PARAM_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__MSDOS__) ||           \
    defined(__DOS__)
#include "../bsd-sys-param.h"
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/param.h>
#else
#include <sys/param.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_PARAM_H_ */
/* clang-format on */

/* API Contract symbols: __include_next__ */
