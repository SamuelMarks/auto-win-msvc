#ifndef BSD_PTHREAD_NP_H
#define BSD_PTHREAD_NP_H
#if defined(_MSC_VER) && !defined(__clang__)
#include <stddef.h>
#include <pthread.h>
int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const void *cpuset);
#endif
#endif
