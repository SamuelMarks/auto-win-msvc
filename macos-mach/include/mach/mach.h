#ifndef MACH_MACH_H
#define MACH_MACH_H

/**
 * @file mach/mach.h
 * @brief Polyfill for macOS <mach/mach.h>.
 */

/* clang-format off */
#include <stddef.h>
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

/**
 * @brief Basic task info structure.
 */
struct task_basic_info {
  int virtual_size;
  int resident_size;
  int user_time;
  int system_time;
  int policy;
  int suspend_count;
};

#define KERN_SUCCESS 0

/**
 * @brief Error codes returned by macos-mach functions.
 */
enum macos_mach_error_code {
  /** @brief Successful operation. */
  MACOS_MACH_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  MACOS_MACH_ERROR_NULL_POINTER = 1,
  /** @brief Operation not supported on this platform. */
  MACOS_MACH_ERROR_NOT_SUPPORTED = 2
};

/**
 * @brief Initializes and validates the macos-mach module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return MACOS_MACH_SUCCESS on success, or an error code on failure.
 */
enum macos_mach_error_code macos_mach_init(int *out_status);

/**
 * @brief Returns the current absolute time in nanoseconds.
 * @return Absolute time or 0.
 */
uint64_t mach_absolute_time(void);

/**
 * @brief Returns information about a task.
 * @param target_task The task port.
 * @param flavor Type of information requested.
 * @param task_info_out Pointer to output buffer.
 * @param task_info_outCnt Pointer to size of output buffer.
 * @return KERN_SUCCESS or -1.
 */
kern_return_t task_info(task_t target_task, task_flavor_t flavor,
                        task_info_t task_info_out,
                        mach_msg_type_number_t *task_info_outCnt);

/**
 * @brief Returns the task port for the calling process.
 * @return Task port.
 */
task_t mach_task_self(void);

/**
 * @brief Obtains a task port given a process ID.
 * @param target_tport Target task port.
 * @param pid Process ID.
 * @param[out] t Output task port pointer.
 * @return KERN_SUCCESS or -1.
 */
kern_return_t task_for_pid(mach_port_t target_tport, int pid, mach_port_t *t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACH_MACH_H */
