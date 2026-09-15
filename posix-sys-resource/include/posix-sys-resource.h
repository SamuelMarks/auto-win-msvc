#ifndef POSIX_SYS_RESOURCE_H
#define POSIX_SYS_RESOURCE_H

/**
 * @file posix-sys-resource.h
 * @brief POSIX sys/resource.h implementation for MSVC
 *
 * This header maps getrusage, getrlimit, and setrlimit functions
 * using Windows process APIs.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef _TIMEVAL_DEFINED
#include <winsock2.h>
#endif
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#ifndef _TIMEVAL_DEFINED
/**
 * @brief Time value structure.
 */
struct timeval {
  /** @brief Seconds. */
  long tv_sec;
  /** @brief Microseconds. */
  long tv_usec;
};
#define _TIMEVAL_DEFINED
#endif
#else
#include <sys/resource.h>
#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(__MSDOS__) ||              \
    defined(__WATCOMC__)

/** @brief Return resource usage for the calling process. */
#define RUSAGE_SELF 0
/** @brief Return resource usage for terminated children. */
#define RUSAGE_CHILDREN -1

/**
 * @brief Structure containing resource usage metrics.
 */
struct rusage {
  /** @brief user CPU time used */
  struct timeval ru_utime;
  /** @brief system CPU time used */
  struct timeval ru_stime;
  /** @brief maximum resident set size */
  long ru_maxrss;
  /** @brief integral shared memory size */
  long ru_ixrss;
  /** @brief integral unshared data size */
  long ru_idrss;
  /** @brief integral unshared stack size */
  long ru_isrss;
  /** @brief page reclaims (soft page faults) */
  long ru_minflt;
  /** @brief page faults (hard page faults) */
  long ru_majflt;
  /** @brief swaps */
  long ru_nswap;
  /** @brief block input operations */
  long ru_inblock;
  /** @brief block output operations */
  long ru_oublock;
  /** @brief IPC messages sent */
  long ru_msgsnd;
  /** @brief IPC messages received */
  long ru_msgrcv;
  /** @brief signals received */
  long ru_nsignals;
  /** @brief voluntary context switches */
  long ru_nvcsw;
  /** @brief involuntary context switches */
  long ru_nivcsw;
};

/** @brief CPU time limit in seconds. */
#define RLIMIT_CPU 0
/** @brief Maximum file size in bytes. */
#define RLIMIT_FSIZE 1
/** @brief Maximum size of data segment. */
#define RLIMIT_DATA 2
/** @brief Maximum size of process stack. */
#define RLIMIT_STACK 3
/** @brief Maximum size of core file. */
#define RLIMIT_CORE 4
/** @brief Maximum resident set size. */
#define RLIMIT_RSS 5
/** @brief Maximum number of open files. */
#define RLIMIT_NOFILE 7
/** @brief Maximum address space (virtual memory) size. */
#define RLIMIT_AS 9

/** @brief Resource limit scalar type. */
typedef unsigned long rlim_t;

/**
 * @brief Structure indicating soft and hard limits.
 */
struct rlimit {
  /** @brief Soft limit. */
  rlim_t rlim_cur;
  /** @brief Hard limit. */
  rlim_t rlim_max;
};

/** \brief RLIM_INFINITY macro. */
#define RLIM_INFINITY (~0UL)

#endif /* defined(_MSC_VER) || defined(_WIN32) */

/**
 * @brief Retrieves system resource usage measures for the calling process.
 *
 * @param who Specifies whose resources should be measured (RUSAGE_SELF).
 * @param usage A pointer to the rusage struct to populate.
 * @return 0 on success, -1 on error.
 */
int posix_getrusage(int who, struct rusage *usage);

/**
 * @brief Gets resource limits.
 *
 * @param resource The resource to check limits for.
 * @param rlp The rlimit struct to populate.
 * @return 0 on success, -1 on error.
 */
int posix_getrlimit(int resource, struct rlimit *rlp);

/**
 * @brief Sets resource limits.
 *
 * @param resource The resource to set limits for.
 * @param rlp The new limits to apply.
 * @return 0 on success, -1 on error.
 */
int posix_setrlimit(int resource, const struct rlimit *rlp);

#ifndef getrusage
#define getrusage posix_getrusage
#endif
#ifndef getrlimit
#define getrlimit posix_getrlimit
#endif
#ifndef setrlimit
#define setrlimit posix_setrlimit
#endif

/**
 * @brief Error codes returned by posix-sys-resource functions.
 */
enum posix_sys_resource_error_code {
  /** @brief Operation completed successfully. */
  POSIX_SYS_RESOURCE_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_SYS_RESOURCE_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-sys-resource availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_SYS_RESOURCE_SUCCESS on success, or
 * POSIX_SYS_RESOURCE_ERROR_NULL_POINTER on NULL pointer.
 */
enum posix_sys_resource_error_code
posix_sys_resource_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_RESOURCE_H */
