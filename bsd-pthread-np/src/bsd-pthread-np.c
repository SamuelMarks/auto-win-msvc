#include "bsd-pthread-np.h"
#include <errno.h>
#if defined(_MSC_VER) && !defined(__clang__)
/** \brief pthread_setaffinity_np function. */
int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                           const void *cpuset) {
  (void)thread;
  (void)cpusetsize;
  (void)cpuset;
  errno = ENOSYS;
  return -1;
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_bsd_pthread_np(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_pthread_np;
