/* clang-format off */
#include "bsd-pthread-np.h"
#include <errno.h>
#include <stddef.h>

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
/* clang-format on */

/** @brief Initializes and validates the bsd-pthread-np module. */
enum bsd_pthread_np_error_code bsd_pthread_np_init(int *out_status) {
  if (out_status == NULL) {
    return BSD_PTHREAD_NP_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return BSD_PTHREAD_NP_SUCCESS;
}

#if !defined(__linux__) && !defined(__FreeBSD__) && !defined(__NetBSD__) &&    \
    !defined(__OpenBSD__)
#if defined(_MSC_VER) || defined(_WIN32)
/** @brief Sets CPU affinity mask for a thread on Windows. */
int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                           const void *cpuset) {
  DWORD_PTR mask = 0;
  HANDLE hThread;
  size_t i;
  const unsigned char *p = (const unsigned char *)cpuset;

  if (!cpuset || cpusetsize == 0) {
    errno = EINVAL;
    return -1;
  }

  for (i = 0; i < cpusetsize && i < sizeof(DWORD_PTR); i++) {
    mask |= ((DWORD_PTR)p[i]) << (i * 8);
  }

  if (mask == 0) {
    errno = EINVAL;
    return -1;
  }

  if (!thread) {
    hThread = GetCurrentThread();
  } else {
    hThread = (HANDLE)thread;
  }

  if (SetThreadAffinityMask(hThread, mask) == 0) {
    errno = EINVAL;
    return -1;
  }

  return 0;
}
#else
/** @brief Sets CPU affinity mask fallback for non-Windows platforms. */
int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                           const void *cpuset) {
  size_t i;
  unsigned long mask = 0;
  const unsigned char *p = (const unsigned char *)cpuset;

  (void)thread;
  if (!cpuset || cpusetsize == 0) {
    errno = EINVAL;
    return -1;
  }

  for (i = 0; i < cpusetsize && i < sizeof(unsigned long); i++) {
    mask |= ((unsigned long)p[i]) << (i * 8);
  }

  if (mask == 0) {
    errno = EINVAL;
    return -1;
  }

  return 0;
}
#endif
#endif

typedef int make_iso_compilers_happy_tu_bsd_pthread_np;
