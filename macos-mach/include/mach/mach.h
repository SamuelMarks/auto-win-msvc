#ifndef MACH_MACH_H
#define MACH_MACH_H

/**
 * @file mach/mach.h
 * @brief Polyfill for macOS <mach/mach.h> (forwards to system header on Apple).
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#if defined(__APPLE__) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <mach/mach.h>
#include <mach/mach_time.h>
#else
#include <stddef.h>
#include <posix-stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes macos-mach availability check.
 * @param[out] out_status Pointer receiving initialization status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code on NULL pointer.
 */
auto_win_msvc_error_t macos_mach_init(int *out_status);

#if !(defined(__APPLE__) && (defined(__GNUC__) || defined(__clang__)))

typedef unsigned int mach_port_t;
typedef mach_port_t task_t;
typedef int kern_return_t;
typedef int task_flavor_t;
typedef int *task_info_t;
typedef unsigned int mach_msg_type_number_t;

#define TASK_BASIC_INFO 20

struct task_basic_info {
  int virtual_size;
  int resident_size;
  int user_time;
  int system_time;
  int policy;
  int suspend_count;
};

#define KERN_SUCCESS 0

/** @brief Returns current monotonic Mach absolute time ticks. */
uint64_t mach_absolute_time(void);
/** @brief Retrieves task information for target_task. */
kern_return_t task_info(task_t target_task, task_flavor_t flavor,
                        task_info_t task_info_out,
                        mach_msg_type_number_t *task_info_outCnt);
/** @brief Returns the Mach port for the current task. */
task_t mach_task_self(void);
/** @brief Obtains the Mach task port for a given process ID. */
kern_return_t task_for_pid(mach_port_t target_tport, int pid, mach_port_t *t);

#endif /* !__APPLE__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACH_MACH_H */
