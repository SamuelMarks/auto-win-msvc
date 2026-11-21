#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file bsd-sys-cpuset.c
 * @brief Implementation of BSD <sys/cpuset.h> affinity polyfills.
 */

/* clang-format off */
#include "bsd-sys-cpuset.h"
#include <errno.h>
#include <stddef.h>
#if defined(_MSC_VER) || defined(__MINGW32__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
/* clang-format on */

#if defined(_MSC_VER) || defined(__MINGW32__)

#ifndef BSD_SYS_CPUSET_MOCK_GetCurrentProcess
#define BSD_SYS_CPUSET_MOCK_GetCurrentProcess GetCurrentProcess
#endif
#ifndef BSD_SYS_CPUSET_MOCK_GetCurrentThread
#define BSD_SYS_CPUSET_MOCK_GetCurrentThread GetCurrentThread
#endif
#ifndef BSD_SYS_CPUSET_MOCK_GetProcessAffinityMask
#define BSD_SYS_CPUSET_MOCK_GetProcessAffinityMask GetProcessAffinityMask
#endif
#ifndef BSD_SYS_CPUSET_MOCK_SetProcessAffinityMask
#define BSD_SYS_CPUSET_MOCK_SetProcessAffinityMask SetProcessAffinityMask
#endif
#ifndef BSD_SYS_CPUSET_MOCK_SetThreadAffinityMask
#define BSD_SYS_CPUSET_MOCK_SetThreadAffinityMask SetThreadAffinityMask
#endif
#ifndef BSD_SYS_CPUSET_MOCK_OpenProcess
#define BSD_SYS_CPUSET_MOCK_OpenProcess OpenProcess
#endif
#ifndef BSD_SYS_CPUSET_MOCK_OpenThread
#define BSD_SYS_CPUSET_MOCK_OpenThread OpenThread
#endif
#ifndef BSD_SYS_CPUSET_MOCK_CloseHandle
#define BSD_SYS_CPUSET_MOCK_CloseHandle CloseHandle
#endif

/**
 * @brief Helper to convert a Windows affinity mask to a cpuset_t.
 * @param mask The Windows affinity mask.
 * @param[out] set Pointer to the cpuset_t to populate.
 */
static void convert_mask_to_cpuset(DWORD_PTR mask, cpuset_t *set) {
  CPU_ZERO(set);
  set->__bits[0] = (unsigned long)mask;
}

/**
 * @brief Helper to convert a cpuset_t to a Windows affinity mask.
 * @param set Pointer to the cpuset_t.
 * @return The constructed Windows affinity mask.
 */
static DWORD_PTR convert_cpuset_to_mask(const cpuset_t *set) {
  return (DWORD_PTR)set->__bits[0];
}

/**
 * @brief Get CPU affinity mask on Windows.
 * @return ERR_NONE on success, or -1 on failure with errno set.
 */
error_type_t cpuset_getaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                                size_t setsize, cpuset_t *mask) {
  HANDLE hProcess = NULL;
  HANDLE hThread = NULL;
  DWORD_PTR processAffinityMask = 0;
  DWORD_PTR systemAffinityMask = 0;

  if (!mask || setsize < sizeof(cpuset_t)) {
    errno = EINVAL;
    return -1;
  }

  if (level == CPU_LEVEL_ROOT) {
    if (BSD_SYS_CPUSET_MOCK_GetProcessAffinityMask(
            BSD_SYS_CPUSET_MOCK_GetCurrentProcess(), &processAffinityMask,
            &systemAffinityMask)) {
      convert_mask_to_cpuset(systemAffinityMask, mask);
      return ERR_NONE;
    }
    errno = EINVAL;
    return -1;
  }

  if (level == CPU_LEVEL_CPUSET || level == CPU_LEVEL_WHICH) {
    if (which == CPU_WHICH_PID || which == CPU_WHICH_CPUSET ||
        which == CPU_WHICH_JAIL || which == CPU_WHICH_DOMAIN) {
      if (id == -1 || id == 0) {
        hProcess = BSD_SYS_CPUSET_MOCK_GetCurrentProcess();
      } else {
        hProcess = BSD_SYS_CPUSET_MOCK_OpenProcess(PROCESS_QUERY_INFORMATION,
                                                   FALSE, (DWORD)id);
        if (!hProcess) {
          errno = ESRCH;
          return -1;
        }
      }

      if (BSD_SYS_CPUSET_MOCK_GetProcessAffinityMask(
              hProcess, &processAffinityMask, &systemAffinityMask)) {
        convert_mask_to_cpuset(processAffinityMask, mask);
        if (hProcess != BSD_SYS_CPUSET_MOCK_GetCurrentProcess()) {
          if (!BSD_SYS_CPUSET_MOCK_CloseHandle(hProcess)) {
            errno = EINVAL;
            return -1;
          }
        }
        return ERR_NONE;
      } else {
        if (hProcess != BSD_SYS_CPUSET_MOCK_GetCurrentProcess()) {
          (void)BSD_SYS_CPUSET_MOCK_CloseHandle(hProcess);
        }
        errno = EINVAL;
        return -1;
      }
    } else if (which == CPU_WHICH_TID || which == CPU_WHICH_ITHREAD ||
               which == CPU_WHICH_IRQ || which == CPU_WHICH_INTRHANDLER) {
      if (id == -1 || id == 0) {
        hThread = BSD_SYS_CPUSET_MOCK_GetCurrentThread();
      } else {
        hThread = BSD_SYS_CPUSET_MOCK_OpenThread(THREAD_QUERY_INFORMATION |
                                                     THREAD_SET_INFORMATION,
                                                 FALSE, (DWORD)id);
        if (!hThread) {
          errno = ESRCH;
          return -1;
        }
      }

      processAffinityMask =
          BSD_SYS_CPUSET_MOCK_SetThreadAffinityMask(hThread, 1);
      if (processAffinityMask == 0) {
        if (hThread != BSD_SYS_CPUSET_MOCK_GetCurrentThread()) {
          (void)BSD_SYS_CPUSET_MOCK_CloseHandle(hThread);
        }
        errno = EINVAL;
        return -1;
      }
      (void)BSD_SYS_CPUSET_MOCK_SetThreadAffinityMask(hThread,
                                                      processAffinityMask);

      convert_mask_to_cpuset(processAffinityMask, mask);

      if (hThread != BSD_SYS_CPUSET_MOCK_GetCurrentThread()) {
        if (!BSD_SYS_CPUSET_MOCK_CloseHandle(hThread)) {
          errno = EINVAL;
          return -1;
        }
      }
      return ERR_NONE;
    }
  }

  errno = ENOSYS;
  return -1;
}

/**
 * @brief Set CPU affinity mask on Windows.
 * @return ERR_NONE on success, or -1 on failure with errno set.
 */
error_type_t cpuset_setaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
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
    errno = EPERM;
    return -1;
  }

  if (level == CPU_LEVEL_CPUSET || level == CPU_LEVEL_WHICH) {
    if (which == CPU_WHICH_PID || which == CPU_WHICH_CPUSET ||
        which == CPU_WHICH_JAIL || which == CPU_WHICH_DOMAIN) {
      if (id == -1 || id == 0) {
        hProcess = BSD_SYS_CPUSET_MOCK_GetCurrentProcess();
      } else {
        hProcess = BSD_SYS_CPUSET_MOCK_OpenProcess(PROCESS_SET_INFORMATION,
                                                   FALSE, (DWORD)id);
        if (!hProcess) {
          errno = ESRCH;
          return -1;
        }
      }

      if (BSD_SYS_CPUSET_MOCK_SetProcessAffinityMask(hProcess, newAffinity)) {
        if (hProcess != BSD_SYS_CPUSET_MOCK_GetCurrentProcess()) {
          if (!BSD_SYS_CPUSET_MOCK_CloseHandle(hProcess)) {
            errno = EINVAL;
            return -1;
          }
        }
        return ERR_NONE;
      } else {
        if (hProcess != BSD_SYS_CPUSET_MOCK_GetCurrentProcess()) {
          (void)BSD_SYS_CPUSET_MOCK_CloseHandle(hProcess);
        }
        errno = EINVAL;
        return -1;
      }
    } else if (which == CPU_WHICH_TID || which == CPU_WHICH_ITHREAD ||
               which == CPU_WHICH_IRQ || which == CPU_WHICH_INTRHANDLER) {
      if (id == -1 || id == 0) {
        hThread = BSD_SYS_CPUSET_MOCK_GetCurrentThread();
      } else {
        hThread = BSD_SYS_CPUSET_MOCK_OpenThread(THREAD_SET_INFORMATION |
                                                     THREAD_QUERY_INFORMATION,
                                                 FALSE, (DWORD)id);
        if (!hThread) {
          errno = ESRCH;
          return -1;
        }
      }

      if (BSD_SYS_CPUSET_MOCK_SetThreadAffinityMask(hThread, newAffinity) !=
          0) {
        if (hThread != BSD_SYS_CPUSET_MOCK_GetCurrentThread()) {
          if (!BSD_SYS_CPUSET_MOCK_CloseHandle(hThread)) {
            errno = EINVAL;
            return -1;
          }
        }
        return ERR_NONE;
      } else {
        if (hThread != BSD_SYS_CPUSET_MOCK_GetCurrentThread()) {
          (void)BSD_SYS_CPUSET_MOCK_CloseHandle(hThread);
        }
        errno = EINVAL;
        return -1;
      }
    }
  }

  errno = ENOSYS;
  return -1;
}

#else

/**
 * @brief Helper to convert a cpuset_t to a mask.
 * @param set Pointer to cpuset_t.
 * @return The affinity mask.
 */
static unsigned long convert_cpuset_to_mask(const cpuset_t *set) {
  return set->__bits[0];
}

/**
 * @brief Fallback cpuset_getaffinity for non-Windows platforms.
 * @return ERR_NONE on success, or -1 on failure with errno set.
 */
error_type_t cpuset_getaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                                size_t setsize, cpuset_t *mask) {
  if (which < 0 || id == (id_t)-2) {
    errno = EINVAL;
    return -1;
  }
  if (!mask || setsize < sizeof(cpuset_t)) {
    errno = EINVAL;
    return -1;
  }
  if (level == CPU_LEVEL_ROOT || level == CPU_LEVEL_CPUSET ||
      level == CPU_LEVEL_WHICH) {
    CPU_ZERO(mask);
    mask->__bits[0] |= 1UL;
    return ERR_NONE;
  }
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Fallback cpuset_setaffinity for non-Windows platforms.
 * @return ERR_NONE on success, or -1 on failure with errno set.
 */
error_type_t cpuset_setaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                                size_t setsize, const cpuset_t *mask) {
  if (which < 0 || id == (id_t)-2) {
    errno = EINVAL;
    return -1;
  }
  if (!mask || setsize < sizeof(cpuset_t)) {
    errno = EINVAL;
    return -1;
  }
  if (convert_cpuset_to_mask(mask) == 0) {
    errno = EINVAL;
    return -1;
  }
  if (level == CPU_LEVEL_ROOT) {
    errno = EPERM;
    return -1;
  }
  if (level == CPU_LEVEL_CPUSET || level == CPU_LEVEL_WHICH) {
    return ERR_NONE;
  }
  errno = ENOSYS;
  return -1;
}

#endif

/**
 * @brief Initializes and validates the bsd-sys-cpuset module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t bsd_sys_cpuset_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef error_type_t make_iso_compilers_happy_tu_bsd_sys_cpuset;

#endif
typedef int make_iso_compilers_happy_mega_batch2_5;
