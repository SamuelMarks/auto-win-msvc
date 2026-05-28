/* macos-mach.c - Strict C89 Implementation */

/* clang-format off */
#include <errno.h>

#include "mach/mach.h"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
/* clang-format on */
#endif

/** \brief Polyfill for mach_absolute_time
 * \return Absolute time in nanoseconds, or 0 on error.
 */
uint64_t mach_absolute_time(void) {
#if defined(_WIN32)
  LARGE_INTEGER count;
  LARGE_INTEGER freq;
  if (QueryPerformanceFrequency(&freq) && QueryPerformanceCounter(&count)) {
    uint64_t q = (uint64_t)(count.QuadPart / freq.QuadPart);
    uint64_t r = (uint64_t)(count.QuadPart % freq.QuadPart);
    return (q * (uint64_t)1000000000) +
           ((r * (uint64_t)1000000000) / (uint64_t)freq.QuadPart);
  }
  return 0;
#else
  return 0;
#endif
}

/** \brief Polyfill for task_info
 * \param target_task The task to get info for.
 * \param flavor The type of info requested.
 * \param task_info_out Pointer to output struct.
 * \param task_info_outCnt Pointer to size of output struct.
 * \return KERN_SUCCESS on success, or -1 on error.
 */
kern_return_t task_info(task_t target_task, task_flavor_t flavor,
                        task_info_t task_info_out,
                        mach_msg_type_number_t *task_info_outCnt) {
#if defined(_WIN32)
  if (flavor == TASK_BASIC_INFO && task_info_out && task_info_outCnt &&
      *task_info_outCnt >= sizeof(struct task_basic_info) / sizeof(int)) {
    struct task_basic_info *info =
        (struct task_basic_info *)(void *)task_info_out;
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    FILETIME creation_time, exit_time, kernel_time, user_time;

    info->virtual_size = 0;
    info->resident_size = 0;
    info->user_time = 0;
    info->system_time = 0;
    info->policy = 0;
    info->suspend_count = 0;

    if (target_task == (task_t)-1 || target_task == 0) {
      hProcess = GetCurrentProcess();
    } else {
      hProcess = (HANDLE)(size_t)target_task;
    }

    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
      info->resident_size = (int)pmc.WorkingSetSize;
      info->virtual_size = (int)pmc.PagefileUsage;
    }

    if (GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time,
                        &user_time)) {
      ULARGE_INTEGER ku, uu;
      ku.LowPart = kernel_time.dwLowDateTime;
      ku.HighPart = kernel_time.dwHighDateTime;
      uu.LowPart = user_time.dwLowDateTime;
      uu.HighPart = user_time.dwHighDateTime;
      info->system_time = (int)(ku.QuadPart / (uint64_t)10000000);
      info->user_time = (int)(uu.QuadPart / (uint64_t)10000000);
    }
    return KERN_SUCCESS;
  }
#else
  (void)target_task;
  (void)flavor;
  (void)task_info_out;
  (void)task_info_outCnt;
#endif
  errno = ENOSYS;
  return -1;
}

/** \brief Polyfill for mach_task_self
 * \return The task port for the current process.
 */
task_t mach_task_self(void) {
#if defined(_WIN32)
  return (task_t)-1;
#else
  return 0;
#endif
}

/** \brief Polyfill for task_for_pid
 * \param target_tport The target task port (usually mach_task_self()).
 * \param pid The process ID to look up.
 * \param t Pointer to output task port.
 * \return KERN_SUCCESS on success, or -1 on error.
 */
kern_return_t task_for_pid(mach_port_t target_tport, int pid, mach_port_t *t) {
#if defined(_WIN32)
  HANDLE hProcess;
  (void)target_tport;
  if (!t) {
    errno = EINVAL;
    return -1;
  }
  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
                         (DWORD)pid);
  if (hProcess) {
    *t = (mach_port_t)(size_t)hProcess;
    return KERN_SUCCESS;
  }
#else
  (void)target_tport;
  (void)pid;
  (void)t;
#endif
  errno = ENOSYS;
  return -1;
}
