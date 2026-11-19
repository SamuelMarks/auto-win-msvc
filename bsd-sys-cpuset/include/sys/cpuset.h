#ifndef SYS_CPUSET_H
#define SYS_CPUSET_H

/**
 * @file cpuset.h
 * @brief Polyfill forwarding header for BSD <sys/cpuset.h>.
 */

/* clang-format off */
#include "../bsd-sys-cpuset.h"
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && _MSC_VER < 1600
/** @brief 64-bit signed identifier type for cpuset_getid on legacy MSVC. */
typedef __int64 cpuset_id64_t;
#else
/** @brief 64-bit signed identifier type for cpuset_getid. */
typedef int64_t cpuset_id64_t;
#endif

/** @brief BSD CPU set identifier type. */
typedef int cpusetid_t;

#ifndef CPUSET_INVALID
/** @brief Invalid CPU set identifier sentinel. */
#define CPUSET_INVALID (-1)
#endif
#ifndef CPU_LEVEL_ROOT
/** @brief Root affinity level. */
#define CPU_LEVEL_ROOT 1
/** @brief CPU set affinity level. */
#define CPU_LEVEL_CPUSET 2
/** @brief Which affinity level. */
#define CPU_LEVEL_WHICH 3
#endif
#ifndef CPU_WHICH_TID
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
#endif

/**
 * @brief Queries the CPU set identifier associated with a target object.
 * @param level CPU level selector.
 * @param which Target selector kind.
 * @param id Target identifier.
 * @param[out] setid Optional pointer receiving CPUSET_INVALID on unsupported
 * hosts.
 * @return Always -1 on non-FreeBSD platforms.
 */
static __inline int cpuset_getid(int level, int which, cpuset_id64_t id,
                                 cpusetid_t *setid) {
  (void)level;
  (void)which;
  (void)id;
  if (setid != NULL) {
    *setid = CPUSET_INVALID;
  }
  return -1;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_CPUSET_H */
