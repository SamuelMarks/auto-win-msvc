#ifndef SYS_QMATH_H
#define SYS_QMATH_H

/**
 * @file qmath.h
 * @brief FreeBSD fixed-point math (<sys/qmath.h>) compatibility header.
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

typedef int32_t s32q_t;
typedef uint32_t u32q_t;
typedef int64_t s64q_t;
typedef uint64_t u64q_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_QMATH_H */
