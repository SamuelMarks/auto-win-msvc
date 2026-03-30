#include "posix-sys-resource.h"

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <errno.h>
#include <psapi.h>
#include <stdio.h>
#include <winsock2.h>

/** \brief posix_getrusage function. */
int posix_getrusage(int who, struct rusage *usage) {
  FILETIME creation_time, exit_time, kernel_time, user_time;
  PROCESS_MEMORY_COUNTERS pmc;

  if (!usage) {
    errno = EFAULT;
    return -1;
  }

  if (who != RUSAGE_SELF) {
    errno = EINVAL;
    return -1;
  }

  memset(usage, 0, sizeof(struct rusage));

  if (GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,
                      &kernel_time, &user_time)) {
    ULARGE_INTEGER kernel, user;
    kernel.LowPart = kernel_time.dwLowDateTime;
    kernel.HighPart = kernel_time.dwHighDateTime;
    user.LowPart = user_time.dwLowDateTime;
    user.HighPart = user_time.dwHighDateTime;

    usage->ru_utime.tv_sec = (long)(user.QuadPart / 10000000);
    usage->ru_utime.tv_usec = (long)((user.QuadPart % 10000000) / 10);

    usage->ru_stime.tv_sec = (long)(kernel.QuadPart / 10000000);
    usage->ru_stime.tv_usec = (long)((kernel.QuadPart % 10000000) / 10);
  }

  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
    usage->ru_maxrss = (long)(pmc.PeakWorkingSetSize / 1024);
    usage->ru_majflt = pmc.PageFaultCount;
  }

  return 0;
}

/** \brief posix_getrlimit function. */
int posix_getrlimit(int resource, struct rlimit *rlp) {
  if (!rlp) {
    errno = EFAULT;
    return -1;
  }

  if (resource == RLIMIT_NOFILE) {
    rlp->rlim_cur = _getmaxstdio();
    rlp->rlim_max = 8192;
    return 0;
  }

  rlp->rlim_cur = RLIM_INFINITY;
  rlp->rlim_max = RLIM_INFINITY;
  return 0;
}

/** \brief posix_setrlimit function. */
int posix_setrlimit(int resource, const struct rlimit *rlp) {
  if (!rlp) {
    errno = EFAULT;
    return -1;
  }

  if (resource == RLIMIT_NOFILE) {
    if (rlp->rlim_cur > 8192) {
      errno = EINVAL;
      return -1;
    }
    _setmaxstdio((int)rlp->rlim_cur);
    return 0;
  }

  return 0;
}

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_sys_resource(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sys_resource;
