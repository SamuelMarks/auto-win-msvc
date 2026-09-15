/**
 * @file posix-sys-resource.c
 * @brief Implementation of posix-sys-resource polyfills.
 */

/* clang-format off */
#include "posix-sys-resource.h"
#include <errno.h>
#include <string.h>

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <psapi.h>
#include <stdio.h>
#include <winsock2.h>
#else
#include <sys/resource.h>
#endif
/* clang-format on */

#undef getrusage
#undef getrlimit
#undef setrlimit

/**
 * @brief Retrieves information on posix-sys-resource availability.
 */
enum posix_sys_resource_error_code
posix_sys_resource_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_SYS_RESOURCE_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_SYS_RESOURCE_SUCCESS;
}

#if defined(_MSC_VER) || defined(_WIN32)

int posix_getrusage(int who, struct rusage *usage) {
  FILETIME creation_time;
  FILETIME exit_time;
  FILETIME kernel_time;
  FILETIME user_time;
  PROCESS_MEMORY_COUNTERS pmc;

  if (usage == NULL) {
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
    ULARGE_INTEGER kernel;
    ULARGE_INTEGER user;
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
    usage->ru_majflt = (long)pmc.PageFaultCount;
  }

  return 0;
}

int posix_getrlimit(int resource, struct rlimit *rlp) {
  if (rlp == NULL) {
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

int posix_setrlimit(int resource, const struct rlimit *rlp) {
  if (rlp == NULL) {
    errno = EFAULT;
    return -1;
  }

  if (resource == RLIMIT_NOFILE) {
    if (rlp->rlim_cur > 8192) {
      _setmaxstdio(8192);
      return 0;
    }
    _setmaxstdio((int)rlp->rlim_cur);
    return 0;
  }

  return 0;
}

#else

int posix_getrusage(int who, struct rusage *usage) {
  if (usage == NULL) {
    errno = EFAULT;
    return -1;
  }
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) ||           \
    defined(__CYGWIN__)
  return getrusage(who, (struct rusage *)usage);
#else
  if (who != RUSAGE_SELF) {
    errno = EINVAL;
    return -1;
  }
  memset(usage, 0, sizeof(*usage));
  return 0;
#endif
}

int posix_getrlimit(int resource, struct rlimit *rlp) {
  if (rlp == NULL) {
    errno = EFAULT;
    return -1;
  }
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) ||           \
    defined(__CYGWIN__)
  return getrlimit(resource, (struct rlimit *)rlp);
#else
  (void)resource;
  rlp->rlim_cur = RLIM_INFINITY;
  rlp->rlim_max = RLIM_INFINITY;
  return 0;
#endif
}

int posix_setrlimit(int resource, const struct rlimit *rlp) {
  if (rlp == NULL) {
    errno = EFAULT;
    return -1;
  }
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) ||           \
    defined(__CYGWIN__)
  return setrlimit(resource, (const struct rlimit *)rlp);
#else
  (void)resource;
  return 0;
#endif
}

#endif

typedef int make_iso_compilers_happy_tu_posix_sys_resource;
