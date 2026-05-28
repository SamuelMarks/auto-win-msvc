/* clang-format off */
#include "bsd-pthread-np.h"
#include <errno.h>

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
/* clang-format on */

/** \brief pthread_setaffinity_np function. */
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
#endif

/* Dummy function to prevent empty translation unit */
int dummy_bsd_pthread_np(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_pthread_np;
