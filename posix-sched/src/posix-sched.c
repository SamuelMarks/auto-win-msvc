/**
 * @file posix-sched.c
 * @brief Implementation of posix-sched polyfills.
 */

/* clang-format off */
#include "posix-sched.h"
#include <errno.h>
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(__CYGWIN__)
#include <sched.h>
#endif
/* clang-format on */

#undef sched_yield
#undef sched_setaffinity
#undef sched_getaffinity

/**
 * @brief Retrieves information on posix-sched availability.
 */
enum posix_sched_error_code posix_sched_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_SCHED_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_SCHED_SUCCESS;
}

#if defined(_MSC_VER) || defined(_WIN32)

int posix_sched_yield(void) {
  Sleep(0);
  return 0;
}

int posix_sched_setaffinity(int pid, size_t cpusetsize, const cpu_set_t *mask) {
  HANDLE hProcess;
  BOOL res;

  if (mask == NULL || cpusetsize < sizeof(cpu_set_t)) {
    errno = EINVAL;
    return -1;
  }

  if (pid == 0) {
    hProcess = GetCurrentProcess();
  } else {
    hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, (DWORD)pid);
    if (hProcess == NULL) {
      errno = ESRCH;
      return -1;
    }
  }

  res = SetProcessAffinityMask(hProcess, (DWORD_PTR)mask->bits);
  if (pid != 0) {
    CloseHandle(hProcess);
  }

  if (!res) {
    errno = EINVAL;
    return -1;
  }

  return 0;
}

int posix_sched_getaffinity(int pid, size_t cpusetsize, cpu_set_t *mask) {
  HANDLE hProcess;
  BOOL res;
  DWORD_PTR processMask = 0;
  DWORD_PTR systemMask = 0;

  if (mask == NULL || cpusetsize < sizeof(cpu_set_t)) {
    errno = EINVAL;
    return -1;
  }

  if (pid == 0) {
    hProcess = GetCurrentProcess();
  } else {
#ifdef PROCESS_QUERY_LIMITED_INFORMATION
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                               PROCESS_QUERY_LIMITED_INFORMATION,
                           FALSE, (DWORD)pid);
#else
    hProcess = NULL;
#endif
    if (hProcess == NULL) {
      hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
    }
    if (hProcess == NULL) {
      errno = ESRCH;
      return -1;
    }
  }

  res = GetProcessAffinityMask(hProcess, &processMask, &systemMask);
  if (pid != 0) {
    CloseHandle(hProcess);
  }

  if (!res) {
    errno = EINVAL;
    return -1;
  }

  mask->bits = (mask_bit_type)processMask;
  return 0;
}

#else

int posix_sched_yield(void) {
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) ||           \
    defined(__CYGWIN__)
  return sched_yield();
#else
  return 0;
#endif
}

int posix_sched_setaffinity(int pid, size_t cpusetsize, const cpu_set_t *mask) {
  if (mask == NULL || cpusetsize < sizeof(cpu_set_t)) {
    errno = EINVAL;
    return -1;
  }
  (void)pid;
  return 0;
}

int posix_sched_getaffinity(int pid, size_t cpusetsize, cpu_set_t *mask) {
  if (mask == NULL || cpusetsize < sizeof(cpu_set_t)) {
    errno = EINVAL;
    return -1;
  }
  (void)pid;
  mask->bits = 1;
  return 0;
}

#endif

typedef int make_iso_compilers_happy_tu_posix_sched;
