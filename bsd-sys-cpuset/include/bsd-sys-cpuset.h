#ifndef BSD_SYS_CPUSET_H
#define BSD_SYS_CPUSET_H

/**
 * @file bsd-sys-cpuset.h
 * @brief Polyfill for BSD <sys/cpuset.h>.
 */

/* clang-format off */
#include <stddef.h>
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
    defined(__NetBSD__) || defined(__linux__) || defined(__CYGWIN__)
#include <sys/types.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
/** @brief Generic error type code. */
typedef int error_type_t;
/** @brief Success indicator for error_type_t. */
#define ERR_NONE 0
#endif

/**
 * @brief Error codes returned by bsd-sys-cpuset functions.
 */
enum bsd_sys_cpuset_error_code {
  /** @brief Successful operation. */
  BSD_SYS_CPUSET_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  BSD_SYS_CPUSET_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed to function. */
  BSD_SYS_CPUSET_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Initializes and validates the bsd-sys-cpuset module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return BSD_SYS_CPUSET_SUCCESS on success, or an error code on failure.
 */
enum bsd_sys_cpuset_error_code bsd_sys_cpuset_init(int *out_status);

#ifndef CPU_SETSIZE
/** @brief Maximum number of CPUs in a cpuset. */
#define CPU_SETSIZE 64
#endif

/** @brief Number of bits in an unsigned long. */
#define _CPU_SET_ULONG_BITS (sizeof(unsigned long) * 8)

/**
 * @brief CPU set structure
 */
typedef struct {
  /** @brief Bit array representing CPUs. */
  unsigned long __bits[CPU_SETSIZE / 32];
} cpuset_t;

/**
 * @brief Initialize CPU set to zero
 * @param set Pointer to cpuset_t
 */
#define CPU_ZERO(set)                                                          \
  do {                                                                         \
    size_t _i;                                                                 \
    for (_i = 0; _i < sizeof((set)->__bits) / sizeof(unsigned long); _i++)     \
      (set)->__bits[_i] = 0;                                                   \
  } while ((void)0, 0)

/**
 * @brief Set a CPU in the set
 * @param cpu CPU index
 * @param set Pointer to cpuset_t
 */
#define CPU_SET(cpu, set)                                                      \
  do {                                                                         \
    if ((cpu) < CPU_SETSIZE)                                                   \
      (set)->__bits[(cpu) / _CPU_SET_ULONG_BITS] |=                            \
          (1UL << ((cpu) % _CPU_SET_ULONG_BITS));                              \
  } while ((void)0, 0)

/**
 * @brief Clear a CPU from the set
 * @param cpu CPU index
 * @param set Pointer to cpuset_t
 */
#define CPU_CLR(cpu, set)                                                      \
  do {                                                                         \
    if ((cpu) < CPU_SETSIZE)                                                   \
      (set)->__bits[(cpu) / _CPU_SET_ULONG_BITS] &=                            \
          ~(1UL << ((cpu) % _CPU_SET_ULONG_BITS));                             \
  } while ((void)0, 0)

/**
 * @brief Check if a CPU is in the set
 * @param cpu CPU index
 * @param set Pointer to cpuset_t
 * @return 1 if set, 0 otherwise
 */
#define CPU_ISSET(cpu, set)                                                    \
  (((cpu) < CPU_SETSIZE) ? (((set)->__bits[(cpu) / _CPU_SET_ULONG_BITS] &      \
                             (1UL << ((cpu) % _CPU_SET_ULONG_BITS))) != 0)     \
                         : 0)

/** @brief CPU level type. */
typedef int cpulevel_t;
/** @brief CPU target which type. */
typedef int cpuwhich_t;

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__) && !defined(__CYGWIN__)
#if !defined(__id_t_defined) && !defined(_ID_T)
#define _ID_T
#define __id_t_defined
/** @brief Identifier type. */
typedef int id_t;
#endif
#endif

/** @brief Root affinity level. */
#define CPU_LEVEL_ROOT 1
/** @brief CPU set affinity level. */
#define CPU_LEVEL_CPUSET 2
/** @brief Which affinity level. */
#define CPU_LEVEL_WHICH 3

/** @brief Target is a thread ID. */
#define CPU_WHICH_TID 1
/** @brief Target is a process ID. */
#define CPU_WHICH_PID 2
/** @brief Target is a cpuset ID. */
#define CPU_WHICH_CPUSET 3
/** @brief Target is an IRQ. */
#define CPU_WHICH_IRQ 4
/** @brief Target is a jail ID. */
#define CPU_WHICH_JAIL 5
/** @brief Target is a NUMA domain. */
#define CPU_WHICH_DOMAIN 6
/** @brief Target is an interrupt handler. */
#define CPU_WHICH_INTRHANDLER 7
/** @brief Target is an interrupt thread. */
#define CPU_WHICH_ITHREAD 8

/**
 * @brief Get CPU affinity mask
 * @param level The cpulevel_t
 * @param which The cpuwhich_t
 * @param id The id_t
 * @param setsize The size of the mask
 * @param mask Pointer to cpuset_t
 * @return 0 on success, -1 on failure
 */
error_type_t cpuset_getaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                                size_t setsize, cpuset_t *mask);

/**
 * @brief Set CPU affinity mask
 * @param level The cpulevel_t
 * @param which The cpuwhich_t
 * @param id The id_t
 * @param setsize The size of the mask
 * @param mask Pointer to cpuset_t
 * @return 0 on success, -1 on failure
 */
error_type_t cpuset_setaffinity(cpulevel_t level, cpuwhich_t which, id_t id,
                                size_t setsize, const cpuset_t *mask);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_CPUSET_H */
