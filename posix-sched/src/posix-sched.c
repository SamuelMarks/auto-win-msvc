#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file posix-sched.c
 * @brief Implementation of posix-sched polyfills.
 */

/* clang-format off */
#include "posix-sched.h"
#include <errno.h>
#include <string.h>
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
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT if out_available is NULL.
 */
auto_win_msvc_error_t posix_sched_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef POSIX_SCHED_MOCK_Sleep
#define POSIX_SCHED_MOCK_Sleep Sleep
#endif
#ifndef POSIX_SCHED_MOCK_GetCurrentProcess
#define POSIX_SCHED_MOCK_GetCurrentProcess GetCurrentProcess
#endif
#ifndef POSIX_SCHED_MOCK_OpenProcess
#define POSIX_SCHED_MOCK_OpenProcess OpenProcess
#endif
#ifndef POSIX_SCHED_MOCK_SetProcessAffinityMask
#define POSIX_SCHED_MOCK_SetProcessAffinityMask SetProcessAffinityMask
#endif
#ifndef POSIX_SCHED_MOCK_GetProcessAffinityMask
#define POSIX_SCHED_MOCK_GetProcessAffinityMask GetProcessAffinityMask
#endif
#ifndef POSIX_SCHED_MOCK_CloseHandle
#define POSIX_SCHED_MOCK_CloseHandle CloseHandle
#endif

/**
 * @brief Yields processor execution on Windows.
 * @return Always 0.
 */
int posix_sched_yield(void) {
  POSIX_SCHED_MOCK_Sleep(0);
  return 0;
}

static int posix_sched_setaffinity_win_impl(int pid, size_t cpusetsize,
                                            const cpu_set_t *mask) {
  HANDLE hProcess;
  BOOL res;

  if (mask == NULL || cpusetsize < sizeof(cpu_set_t)) {
    errno = EINVAL;
    return -1;
  }

  if (pid == 0) {
    hProcess = POSIX_SCHED_MOCK_GetCurrentProcess();
  } else {
    hProcess = POSIX_SCHED_MOCK_OpenProcess(PROCESS_SET_INFORMATION, FALSE,
                                            (DWORD)pid);
    if (hProcess == NULL) {
      errno = ESRCH;
      return -1;
    }
  }

  res =
      POSIX_SCHED_MOCK_SetProcessAffinityMask(hProcess, (DWORD_PTR)mask->bits);
  if (pid != 0) {
    POSIX_SCHED_MOCK_CloseHandle(hProcess);
  }

  if (!res) {
    errno = EINVAL;
    return -1;
  }

  return 0;
}

static int posix_sched_getaffinity_win_impl(int pid, size_t cpusetsize,
                                            cpu_set_t *mask) {
  HANDLE hProcess;
  BOOL res;
  DWORD_PTR processMask = 0;
  DWORD_PTR systemMask = 0;

  if (mask == NULL || cpusetsize < sizeof(cpu_set_t)) {
    errno = EINVAL;
    return -1;
  }

  if (pid == 0) {
    hProcess = POSIX_SCHED_MOCK_GetCurrentProcess();
  } else {
#ifdef PROCESS_QUERY_LIMITED_INFORMATION
    hProcess = POSIX_SCHED_MOCK_OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
        (DWORD)pid);
#else
    hProcess = NULL;
#endif
    if (hProcess == NULL) {
      hProcess = POSIX_SCHED_MOCK_OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,
                                              (DWORD)pid);
    }
    if (hProcess == NULL) {
      errno = ESRCH;
      return -1;
    }
  }

  res = POSIX_SCHED_MOCK_GetProcessAffinityMask(hProcess, &processMask,
                                                &systemMask);
  if (pid != 0) {
    POSIX_SCHED_MOCK_CloseHandle(hProcess);
  }

  if (!res) {
    errno = EINVAL;
    return -1;
  }

  mask->bits = (mask_bit_type)processMask;
  return 0;
}

/**
 * @brief Sets CPU affinity mask for a process on Windows.
 * @return 0 on success, or -1 on error with errno set.
 */
int posix_sched_setaffinity(int pid, size_t cpusetsize, const cpu_set_t *mask) {
  return posix_sched_setaffinity_win_impl(pid, cpusetsize, mask);
}

/**
 * @brief Gets CPU affinity mask for a process on Windows.
 * @return 0 on success, or -1 on error with errno set.
 */
int posix_sched_getaffinity(int pid, size_t cpusetsize, cpu_set_t *mask) {
  return posix_sched_getaffinity_win_impl(pid, cpusetsize, mask);
}

#else

/**
 * @brief Yields processor execution on POSIX platforms.
 * @return 0 on success, or -1 on error.
 */
int posix_sched_yield(void) {
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) ||           \
    defined(__CYGWIN__)
  return sched_yield();
#else
  return 0;
#endif
}

/**
 * @brief Sets CPU affinity mask on non-Windows platforms.
 * @return 0 on success, or -1 on invalid arguments.
 */
int posix_sched_setaffinity(int pid, size_t cpusetsize, const cpu_set_t *mask) {
  if (pid < 0 || mask == NULL || cpusetsize < sizeof(cpu_set_t)) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

/**
 * @brief Gets CPU affinity mask on non-Windows platforms.
 * @return 0 on success, or -1 on invalid arguments.
 */
int posix_sched_getaffinity(int pid, size_t cpusetsize, cpu_set_t *mask) {
  if (pid < 0 || mask == NULL || cpusetsize < sizeof(cpu_set_t)) {
    errno = EINVAL;
    return -1;
  }
#if defined(_BITS_CPU_SET_H)
  memset(mask, 0, sizeof(*mask));
  mask->__bits[0] = 1UL;
#else
  mask->bits = 1U;
#endif
  return 0;
}

#endif

typedef int make_iso_compilers_happy_tu_posix_sched;

#endif
typedef int make_iso_compilers_happy_mega_batch3_13;
