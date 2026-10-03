#ifndef BSD_PTHREAD_NP_H
#define BSD_PTHREAD_NP_H

/**
 * @file bsd-pthread-np.h
 * @brief Polyfill for BSD non-portable pthread extensions.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <pthread.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes and validates the bsd-pthread-np module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t bsd_pthread_np_init(int *out_status);

#if (!defined(__linux__) && !defined(__FreeBSD__) && !defined(__NetBSD__) &&   \
     !defined(__OpenBSD__)) ||                                                 \
    (defined(__linux__) && !defined(__USE_GNU))
/**
 * @brief Sets CPU affinity mask for a thread.
 * @param thread Thread identifier (or 0 for current thread).
 * @param cpusetsize Size of the cpuset buffer in bytes.
 * @param cpuset Pointer to CPU affinity bitmask.
 * @return 0 on success, or -1 on failure with errno set.
 */
int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                           const void *cpuset);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_PTHREAD_NP_H */
