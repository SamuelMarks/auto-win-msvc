#ifndef POSIX_SCHED_H
#define POSIX_SCHED_H

/**
 * @file posix-sched.h
 * @brief POSIX sched.h implementation for MSVC
 *
 * This header maps thread scheduling and CPU affinity functions
 * using Windows process APIs.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
#include <string.h>
#if !defined(_MSC_VER) && !defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)
#include <sched.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CPU_SETSIZE
/** @brief Maximum number of CPUs in cpu_set_t. */
#define CPU_SETSIZE 64
#endif

#if defined(__GNUC__)
/** @brief Underlying 64-bit unsigned integer bitmask type for cpu_set_t. */
__extension__ typedef unsigned long long mask_bit_type;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
/** @brief Underlying 64-bit unsigned integer bitmask type for cpu_set_t. */
typedef unsigned __int64 mask_bit_type;
#else
/** @brief Underlying 64-bit unsigned integer bitmask type for cpu_set_t. */
typedef unsigned long long mask_bit_type;
#endif

#if !defined(_BITS_CPU_SET_H) && !defined(_SYS_CPUSET_H_) &&                   \
    !defined(_CPU_SET_T_DEFINED)
/** @brief Guard macro indicating cpu_set_t has been defined. */
#define _CPU_SET_T_DEFINED
/**
 * @brief Structure for representing a CPU affinity mask.
 */
typedef struct cpu_set {
#if defined(__GNUC__)
  /** @brief Bitmask of active CPUs. */
  __extension__ unsigned long long bits;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
  /** @brief Bitmask of active CPUs. */
  unsigned __int64 bits;
#else
  /** @brief Bitmask of active CPUs. */
  unsigned long long bits;
#endif
} cpu_set_t;
#endif

#if defined(_BITS_CPU_SET_H)

#ifndef CPU_ZERO
/**
 * @brief Clears set, so that it contains no CPUs.
 */
#define CPU_ZERO(set) __CPU_ZERO_S(sizeof(cpu_set_t), set)
#endif

#ifndef CPU_SET
/**
 * @brief Add CPU cpu to set.
 */
#define CPU_SET(cpu, set) __CPU_SET_S(cpu, sizeof(cpu_set_t), set)
#endif

#ifndef CPU_CLR
/**
 * @brief Remove CPU cpu from set.
 */
#define CPU_CLR(cpu, set) __CPU_CLR_S(cpu, sizeof(cpu_set_t), set)
#endif

#ifndef CPU_ISSET
/**
 * @brief Test to see if CPU cpu is a member of set.
 */
#define CPU_ISSET(cpu, set) __CPU_ISSET_S(cpu, sizeof(cpu_set_t), set)
#endif

#elif defined(_SYS_CPUSET_H_)

/**
 * @brief Clears set, so that it contains no CPUs.
 */
#undef CPU_ZERO
#define CPU_ZERO(set)                                                          \
  do {                                                                         \
    memset((set), 0, sizeof(*(set)));                                          \
  } while ((void)0, 0)

#undef CPU_SET
/**
 * @brief Add CPU cpu to set.
 */
#define CPU_SET(cpu, set)                                                      \
  do {                                                                         \
    size_t _awm_sched_cpu = (size_t)(cpu);                                     \
    if (_awm_sched_cpu < sizeof((set)->__bits) * 8U) {                         \
      (set)->__bits[_awm_sched_cpu / (sizeof((set)->__bits[0]) * 8U)] |=       \
          ((__cpu_mask)1 << (_awm_sched_cpu %                                  \
                             (sizeof((set)->__bits[0]) * 8U)));                \
    }                                                                          \
  } while ((void)0, 0)

#undef CPU_CLR
/**
 * @brief Remove CPU cpu from set.
 */
#define CPU_CLR(cpu, set)                                                      \
  do {                                                                         \
    size_t _awm_sched_cpu = (size_t)(cpu);                                     \
    if (_awm_sched_cpu < sizeof((set)->__bits) * 8U) {                         \
      (set)->__bits[_awm_sched_cpu / (sizeof((set)->__bits[0]) * 8U)] &=       \
          ~((__cpu_mask)1 << (_awm_sched_cpu %                                 \
                              (sizeof((set)->__bits[0]) * 8U)));               \
    }                                                                          \
  } while ((void)0, 0)

/**
 * @brief Test to see if CPU cpu is a member of set.
 */
#undef CPU_ISSET
#define CPU_ISSET(cpu, set)                                                    \
  (((size_t)(cpu) < sizeof((set)->__bits) * 8U)                                \
       ? (((set)->__bits[(size_t)(cpu) / (sizeof((set)->__bits[0]) * 8U)] &    \
           ((__cpu_mask)1 << (((size_t)(cpu) < sizeof((set)->__bits) * 8U      \
                                   ? (size_t)(cpu)                             \
                                   : 0U) %                                     \
                              (sizeof((set)->__bits[0]) * 8U)))) != 0)         \
       : 0)

#else

/**
 * @brief Clears set, so that it contains no CPUs.
 */
#undef CPU_ZERO
#define CPU_ZERO(set)                                                          \
  do {                                                                         \
    (set)->bits = 0;                                                           \
  } while ((void)0, 0)

#undef CPU_SET
/**
 * @brief Add CPU cpu to set.
 */
#define CPU_SET(cpu, set)                                                      \
  do {                                                                         \
    (set)->bits |= ((size_t)(cpu) < 64)                                        \
                       ? (((mask_bit_type)1) << ((size_t)(cpu) & 63U))         \
                       : (mask_bit_type)0;                                     \
  } while ((void)0, 0)

#undef CPU_CLR
/**
 * @brief Remove CPU cpu from set.
 */
#define CPU_CLR(cpu, set)                                                      \
  do {                                                                         \
    (set)->bits &=                                                             \
        ~(((size_t)(cpu) < 64) ? (((mask_bit_type)1) << ((size_t)(cpu) & 63U)) \
                               : (mask_bit_type)0);                            \
  } while ((void)0, 0)

/**
 * @brief Test to see if CPU cpu is a member of set.
 */
#undef CPU_ISSET
#define CPU_ISSET(cpu, set)                                                    \
  (((size_t)(cpu) < 64) ? (((set)->bits & (((mask_bit_type)1)                  \
                                           << ((cpu) < 64 ? (cpu) : 0))) != 0) \
                        : 0)

#endif /* cpu_set macros */

/**
 * @brief Retrieves information on posix-sched availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_sched_get_info(int *out_available);

/**
 * @brief Relinquish the CPU.
 *
 * Mapped to Windows Sleep(0) API or sched_yield().
 *
 * @return 0 on success, or -1 on error.
 */
int posix_sched_yield(void);

/**
 * @brief Set a process's CPU affinity mask.
 *
 * @param pid Process ID. (0 means current process).
 * @param cpusetsize Size of mask struct in bytes.
 * @param mask Pointer to the new cpu_set_t mask.
 * @return 0 on success, -1 on error.
 */
int posix_sched_setaffinity(int pid, size_t cpusetsize, const cpu_set_t *mask);

/**
 * @brief Get a process's CPU affinity mask.
 *
 * @param pid Process ID. (0 means current process).
 * @param cpusetsize Size of mask struct in bytes.
 * @param mask Pointer to the cpu_set_t mask to be populated.
 * @return 0 on success, -1 on error.
 */
int posix_sched_getaffinity(int pid, size_t cpusetsize, cpu_set_t *mask);

#ifndef sched_yield
/** @brief POSIX sched_yield alias. */
#define sched_yield posix_sched_yield
#endif
#ifndef sched_setaffinity
/** @brief Linux/GNU sched_setaffinity alias. */
#define sched_setaffinity posix_sched_setaffinity
#endif
#ifndef sched_getaffinity
/** @brief Linux/GNU sched_getaffinity alias. */
#define sched_getaffinity posix_sched_getaffinity
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SCHED_H */
