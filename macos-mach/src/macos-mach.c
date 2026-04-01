/* macos-mach.c - Strict C89 Implementation */

/* clang-format off */
#include "mach/mach.h"
/* clang-format on */

/** \brief Polyfill for mach_absolute_time
 * \return 0 or appropriate
 * default value
 */
uint64_t mach_absolute_time(void) { return 0; }

/** \brief Polyfill for task_info
 * \return 0 or appropriate default
 * value

 */
kern_return_t task_info(task_t target_task, task_flavor_t flavor,
                        task_info_t task_info_out,
                        mach_msg_type_number_t *task_info_outCnt) {
  target_task = target_task;
  flavor = flavor;
  task_info_out = task_info_out;
  task_info_outCnt = task_info_outCnt;
  errno = ENOSYS;
  return -1;
}

/** \brief Polyfill for mach_task_self
 * \return 0 or appropriate
 * default value
 */
task_t mach_task_self(void) { return 0; }

/** \brief Polyfill for task_for_pid
 * \return 0 or appropriate
 * default value
 */
kern_return_t task_for_pid(mach_port_t target_tport, int pid, mach_port_t *t) {
  target_tport = target_tport;
  pid = pid;
  t = t;
  errno = ENOSYS;
  return -1;
}
