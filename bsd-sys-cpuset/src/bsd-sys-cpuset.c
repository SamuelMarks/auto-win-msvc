/* bsd-sys-cpuset implementation */
/* clang-format off */
#include "bsd-sys-cpuset.h"
#include <errno.h>
#if defined(_MSC_VER) || defined(__MINGW32__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
/* clang-format on */

#endif

#if defined(_MSC_VER) || defined(__MINGW32__)

/**
 * @brief Helper to convert a Windows affinity mask to a cpuset_t
 * @param mask The Windows affinity mask
 * @param set Pointer to the cpuset_t to populate
 * @return 0 on success
 */
static int convert_mask_to_cpuset(DWORD_PTR mask, cpuset_t *set) {
  unsigned int idx;
  CPU_ZERO(set);
  for (idx = 0; idx < sizeof(DWORD_PTR) * 8 && idx < CPU_SETSIZE; idx++) {
    if (mask & ((DWORD_PTR)1 << idx)) {
      CPU_SET(idx, set);
    }
  }
  return 0;
}

/**
 * @brief Helper to convert a cpuset_t to a Windows affinity mask
 * @param set Pointer to the cpuset_t
 * @return The constructed Windows affinity mask
 */
static DWORD_PTR convert_cpuset_to_mask(const cpuset_t *set) {
  DWORD_PTR newAffinity = 0;
  unsigned int idx;
  for (idx = 0; idx < sizeof(DWORD_PTR) * 8 && idx < CPU_SETSIZE; idx++) {
    if (CPU_ISSET(idx, set)) {
      newAffinity |= ((DWORD_PTR)1 << idx);
    }
  }
  return newAffinity;
}

/** \brief cpuset_getaffinity function. */
int cpuset_getaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                       size_t setsize, cpuset_t *mask) {
  HANDLE hProcess = NULL;
  HANDLE hThread = NULL;
  DWORD_PTR processAffinityMask = 0;
  DWORD_PTR systemAffinityMask = 0;

  if (!mask || setsize < sizeof(cpuset_t)) {
    errno = EINVAL;
    return -1;
  }

  /* CPU_LEVEL_ROOT ignores 'which' and 'id' in practice, returning system mask
   */
  if (level == CPU_LEVEL_ROOT) {
    if (GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask,
                               &systemAffinityMask)) {
      convert_mask_to_cpuset(systemAffinityMask, mask);
      return 0;
    }
    errno = EINVAL;
    return -1;
  }

  /* CPU_LEVEL_CPUSET represents the mask available to the process/jail.
     We map this to the process affinity mask. */
  if (level == CPU_LEVEL_CPUSET || level == CPU_LEVEL_WHICH) {
    if (which == CPU_WHICH_PID || which == CPU_WHICH_CPUSET ||
        which == CPU_WHICH_JAIL || which == CPU_WHICH_DOMAIN) {
      if (id == -1 || id == 0) {
        hProcess = GetCurrentProcess();
      } else {
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)id);
        if (!hProcess) {
          errno = ESRCH;
          return -1;
        }
      }

      if (GetProcessAffinityMask(hProcess, &processAffinityMask,
                                 &systemAffinityMask)) {
        convert_mask_to_cpuset(processAffinityMask, mask);
        if (hProcess != GetCurrentProcess()) {
          CloseHandle(hProcess);
        }
        return 0;
      } else {
        if (hProcess != GetCurrentProcess()) {
          CloseHandle(hProcess);
        }
        errno = EINVAL;
        return -1;
      }
    } else if (which == CPU_WHICH_TID || which == CPU_WHICH_ITHREAD ||
               which == CPU_WHICH_IRQ || which == CPU_WHICH_INTRHANDLER) {
      if (id == -1 || id == 0) {
        hThread = GetCurrentThread();
      } else {
        hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SET_INFORMATION,
                             FALSE, (DWORD)id);
        if (!hThread) {
          errno = ESRCH;
          return -1;
        }
      }

      /* Windows has no GetThreadAffinityMask until Win11, use Set to query */
      processAffinityMask = SetThreadAffinityMask(hThread, 1);
      if (processAffinityMask == 0) {
        if (hThread != GetCurrentThread()) {
          CloseHandle(hThread);
        }
        errno = EINVAL;
        return -1;
      }
      SetThreadAffinityMask(hThread, processAffinityMask); /* Restore */

      convert_mask_to_cpuset(processAffinityMask, mask);

      if (hThread != GetCurrentThread()) {
        CloseHandle(hThread);
      }
      return 0;
    }
  }

  errno = ENOSYS;
  return -1;
}

/** \brief cpuset_setaffinity function. */
int cpuset_setaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                       size_t setsize, const cpuset_t *mask) {
  HANDLE hProcess = NULL;
  HANDLE hThread = NULL;
  DWORD_PTR newAffinity = 0;

  if (!mask || setsize < sizeof(cpuset_t)) {
    errno = EINVAL;
    return -1;
  }

  newAffinity = convert_cpuset_to_mask(mask);
  if (newAffinity == 0) {
    errno = EINVAL;
    return -1;
  }

  if (level == CPU_LEVEL_ROOT) {
    /* Cannot modify the root system affinity mask in user-mode */
    errno = EPERM;
    return -1;
  }

  if (level == CPU_LEVEL_CPUSET || level == CPU_LEVEL_WHICH) {
    if (which == CPU_WHICH_PID || which == CPU_WHICH_CPUSET ||
        which == CPU_WHICH_JAIL || which == CPU_WHICH_DOMAIN) {
      if (id == -1 || id == 0) {
        hProcess = GetCurrentProcess();
      } else {
        hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, (DWORD)id);
        if (!hProcess) {
          errno = ESRCH;
          return -1;
        }
      }

      if (SetProcessAffinityMask(hProcess, newAffinity)) {
        if (hProcess != GetCurrentProcess()) {
          CloseHandle(hProcess);
        }
        return 0;
      } else {
        if (hProcess != GetCurrentProcess()) {
          CloseHandle(hProcess);
        }
        errno = EINVAL;
        return -1;
      }
    } else if (which == CPU_WHICH_TID || which == CPU_WHICH_ITHREAD ||
               which == CPU_WHICH_IRQ || which == CPU_WHICH_INTRHANDLER) {
      if (id == -1 || id == 0) {
        hThread = GetCurrentThread();
      } else {
        hThread = OpenThread(THREAD_SET_INFORMATION | THREAD_QUERY_INFORMATION,
                             FALSE, (DWORD)id);
        if (!hThread) {
          errno = ESRCH;
          return -1;
        }
      }

      if (SetThreadAffinityMask(hThread, newAffinity) != 0) {
        if (hThread != GetCurrentThread()) {
          CloseHandle(hThread);
        }
        return 0;
      } else {
        if (hThread != GetCurrentThread()) {
          CloseHandle(hThread);
        }
        errno = EINVAL;
        return -1;
      }
    }
  }

  errno = ENOSYS;
  return -1;
}

#endif

typedef int make_iso_compilers_happy_tu_bsd_sys_cpuset;
