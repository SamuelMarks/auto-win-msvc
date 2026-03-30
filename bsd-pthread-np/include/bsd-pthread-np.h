#ifndef BSD_PTHREAD_NP_H
#define BSD_PTHREAD_NP_H
#if defined(_MSC_VER) && !defined(__clang__)
#include <pthread.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief pthread_setaffinity_np function. */
int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                           const void *cpuset);
#endif

#ifdef __cplusplus
}
#endif

#endif
