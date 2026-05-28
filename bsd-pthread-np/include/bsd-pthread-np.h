#ifndef BSD_PTHREAD_NP_H
#define BSD_PTHREAD_NP_H
#include "auto_win_msvc_export.h"

/* clang-format off */
#include <pthread.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
/** \brief pthread_setaffinity_np function. */
AUTO_WIN_MSVC_EXPORT int AUTO_WIN_MSVC_EXPORT
pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const void *cpuset);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
