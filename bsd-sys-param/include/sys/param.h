/* clang-format off */
#ifndef SYS_PARAM_H_
#define SYS_PARAM_H_

/**
 * @file param.h
 * @brief Polyfill wrapper for BSD <sys/param.h>.
 */

#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) && \
    !defined(__DOS__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/param.h>
#else
#include <sys/param.h>
#endif
#endif
#include "../bsd-sys-param.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_PARAM_H_ */
/* clang-format on */

/* API Contract symbols: __include_next__ */
