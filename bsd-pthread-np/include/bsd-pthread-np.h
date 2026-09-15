#ifndef BSD_PTHREAD_NP_H
#define BSD_PTHREAD_NP_H

/**
 * @file bsd-pthread-np.h
 * @brief Polyfill for BSD non-portable pthread extensions.
 */

/* clang-format off */
#include <pthread.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by bsd-pthread-np functions.
 */
enum bsd_pthread_np_error_code {
  /** @brief Successful operation. */
  BSD_PTHREAD_NP_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  BSD_PTHREAD_NP_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed to function. */
  BSD_PTHREAD_NP_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Initializes and validates the bsd-pthread-np module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return BSD_PTHREAD_NP_SUCCESS on success, or an error code on failure.
 */
enum bsd_pthread_np_error_code bsd_pthread_np_init(int *out_status);

#if !defined(__linux__) && !defined(__FreeBSD__) && !defined(__NetBSD__) &&    \
    !defined(__OpenBSD__)
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
