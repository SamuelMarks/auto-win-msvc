/* clang-format off */
#ifndef SYS_CDEFS_H_
#define SYS_CDEFS_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#if (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#if defined(__has_include_next)
#if __has_include_next(<sys/cdefs.h>)
#pragma GCC system_header
#include_next <sys/cdefs.h>
#endif
#else
#pragma GCC system_header
#include_next <sys/cdefs.h>
#endif
#elif !defined(_WIN32)
#include <sys/cdefs.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_CDEFS_H_ */
/* clang-format on */

/* API Contract symbols: __include_next__ */
