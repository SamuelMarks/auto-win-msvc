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
#include <stddef.h>
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
__extension__ typedef unsigned long long mask_bit_type;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 mask_bit_type;
#else
typedef unsigned long long mask_bit_type;
#endif

#if !defined(_BITS_CPU_SET_H) && !defined(_CPU_SET_T_DEFINED)
#define _CPU_SET_T_DEFINED
/**
 * @brief Structure for representing a CPU affinity mask.
 */
typedef struct cpu_set {
#if defined(__GNUC__)
  __extension__ unsigned long long bits;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
  unsigned __int64 bits;
#else
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

#else

/**
 * @brief Clears set, so that it contains no CPUs.
 */
#undef CPU_ZERO
#define CPU_ZERO(set)                                                          \
  do {                                                                         \
    (set)->bits = 0;                                                           \
  } while ((void)0, 0)

/**
 * @brief Add CPU cpu to set.
 */
#undef CPU_SET
#define CPU_SET(cpu, set)                                                      \
  do {                                                                         \
    if ((size_t)(cpu) < 64)                                                    \
      (set)->bits |= (((mask_bit_type)1) << ((cpu) < 64 ? (cpu) : 0));         \
  } while ((void)0, 0)

/**
 * @brief Remove CPU cpu from set.
 */
#undef CPU_CLR
#define CPU_CLR(cpu, set)                                                      \
  do {                                                                         \
    if ((size_t)(cpu) < 64)                                                    \
      (set)->bits &= ~(((mask_bit_type)1) << ((cpu) < 64 ? (cpu) : 0));        \
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
 * @brief Error codes returned by posix-sched functions.
 */
enum posix_sched_error_code {
  /** @brief Operation completed successfully. */
  POSIX_SCHED_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_SCHED_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-sched availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_SCHED_SUCCESS on success, or POSIX_SCHED_ERROR_NULL_POINTER on
 * NULL pointer.
 */
enum posix_sched_error_code posix_sched_get_info(int *out_available);

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
#define sched_yield posix_sched_yield
#endif
#ifndef sched_setaffinity
#define sched_setaffinity posix_sched_setaffinity
#endif
#ifndef sched_getaffinity
#define sched_getaffinity posix_sched_getaffinity
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SCHED_H */
