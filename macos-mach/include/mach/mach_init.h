/* mach/mach_init.h - Strict C89 Header */
#ifndef MACH_MACH_INIT_H
#define MACH_MACH_INIT_H
#include "auto_win_msvc_export.h"

/* clang-format off */
#include <posix-stdint.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

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

AUTO_WIN_MSVC_EXPORT uint64_t mach_absolute_time(void);
AUTO_WIN_MSVC_EXPORT kern_return_t AUTO_WIN_MSVC_EXPORT
task_info(task_t target_task, task_flavor_t flavor, task_info_t task_info_out,
          mach_msg_type_number_t *task_info_outCnt);
AUTO_WIN_MSVC_EXPORT task_t mach_task_self(void);
AUTO_WIN_MSVC_EXPORT kern_return_t task_for_pid(mach_port_t target_tport,
                                                int pid, mach_port_t *t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACH_MACH_INIT_H */
