#ifndef POSIX_STDCKDINT_H
#define POSIX_STDCKDINT_H

/**
 * @file stdckdint.h
 * @brief C23 checked integer arithmetic (<stdckdint.h>) polyfill for C89, MSVC,
 * and OpenWatcom.
 */

/* clang-format off */
#include "posix-stdint.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
#if defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow)
#define ckd_add(res, a, b) __builtin_add_overflow((a), (b), (res))
#define ckd_sub(res, a, b) __builtin_sub_overflow((a), (b), (res))
#define ckd_mul(res, a, b) __builtin_mul_overflow((a), (b), (res))
#endif
#endif
#endif

#ifndef ckd_add
/** @brief Checked addition storing (a + b) into *res and returning 1 on
 * overflow. */
#define ckd_add(res, a, b)                                                     \
  ((*(res) = (a) + (b)), (((b) > 0 && (a) > 0 && *(res) < (a)) ||              \
                          ((b) < 0 && (a) < 0 && *(res) > (a)))                \
                             ? 1                                               \
                             : 0)
#endif

#ifndef ckd_sub
/** @brief Checked subtraction storing (a - b) into *res and returning 1 on
 * overflow. */
#define ckd_sub(res, a, b)                                                     \
  ((*(res) = (a) - (b)), (((b) < 0 && (a) > 0 && *(res) < (a)) ||              \
                          ((b) > 0 && (a) < 0 && *(res) > (a)))                \
                             ? 1                                               \
                             : 0)
#endif

#ifndef ckd_mul
/** @brief Checked multiplication storing (a * b) into *res and returning 1 on
 * overflow. */
#define ckd_mul(res, a, b)                                                     \
  (((a) == 0 || (b) == 0) ? ((*(res) = 0), 0)                                  \
                          : ((*(res) = (a) * (b)), ((*(res) / (b)) != (a))))
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STDCKDINT_H */
