#ifndef SYS_PROCDESC_H
#define SYS_PROCDESC_H

/**
 * @file procdesc.h
 * @brief FreeBSD process descriptors (<sys/procdesc.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Allow daemon mode for process descriptor. */
#define PD_DAEMON 0x00000001
/** @brief Close process descriptor on exec. */
#define PD_CLOEXEC 0x00000002
/** @brief Allowed pdfork flags mask. */
#define PD_ALLOWED_AT_FORK (PD_DAEMON | PD_CLOEXEC)

struct rusage;

/**
 * @brief Fork a child process and return a process descriptor.
 * @param[out] fdp Pointer receiving process descriptor.
 * @param[in] flags PD_* flags.
 * @return Process ID, or -1 on error.
 */
pid_t pdfork(int *fdp, int flags);

/**
 * @brief Retrieve process ID associated with a process descriptor.
 * @param[in] fd Process descriptor.
 * @param[out] pidp Pointer receiving process ID.
 * @return 0 on success, -1 on error.
 */
int pdgetpid(int fd, pid_t *pidp);

/**
 * @brief Send a signal to a process represented by a process descriptor.
 * @param[in] fd Process descriptor.
 * @param[in] signum Signal number.
 * @return 0 on success, -1 on error.
 */
int pdkill(int fd, int signum);

/**
 * @brief Wait for process represented by a process descriptor.
 * @param[in] fd Process descriptor.
 * @param[out] status Exit status pointer.
 * @param[in] options Wait options.
 * @param[out] rusage Resource usage pointer.
 * @return Process ID, or -1 on error.
 */
int pdwait4(int fd, int *status, int options, struct rusage *rusage);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_PROCDESC_H */
