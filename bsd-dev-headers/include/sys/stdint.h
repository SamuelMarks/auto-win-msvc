#ifndef SYS_STDINT_H
#define SYS_STDINT_H

/**
 * @file stdint.h
 * @brief FreeBSD <sys/stdint.h> compatibility wrapper forwarding to <stdint.h>.
 */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_STDINT_H */
