#ifndef POSIX_WAIT_H
#define POSIX_WAIT_H

/**
 * @file posix-wait.h
 * @brief POSIX sys/wait.h implementation and compatibility layer.
 */

/* clang-format off */
#include <stddef.h>
#if !defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-wait functions.
 */
enum posix_wait_error_code {
  /** @brief Operation completed successfully. */
  POSIX_WAIT_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_WAIT_ERROR_NULL_POINTER = 1,
  /** @brief No child processes available or wait condition failed. */
  POSIX_WAIT_ERROR_CHILD = 2
};

/**
 * @brief Retrieves information on posix-wait polyfill availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_WAIT_SUCCESS on success, or POSIX_WAIT_ERROR_NULL_POINTER on
 * NULL pointer.
 */
enum posix_wait_error_code posix_wait_get_info(int *out_available);

#if defined(_WIN32) || defined(__MSDOS__) || defined(__WATCOMC__)

/**
 * @brief PID type definition for Windows.
 */
#ifndef _PID_T_DEFINED
#define _PID_T_DEFINED
typedef int pid_t;
#endif

/**
 * @brief ID type definition for Windows.
 */
#ifndef _ID_T_DEFINED
#define _ID_T_DEFINED
typedef int id_t;
#endif

/**
 * @brief ID type enumeration.
 */
typedef enum { P_ALL, P_PID, P_PGID } idtype_t;

/**
 * @brief Signal info structure for waitid.
 */
#ifndef _SIGINFO_T_DEFINED
#define _SIGINFO_T_DEFINED
typedef struct {
  int si_signo;  /**< Signal number */
  int si_code;   /**< Signal code */
  int si_pid;    /**< Sending process ID */
  int si_uid;    /**< Real user ID of sending process */
  int si_status; /**< Exit value or signal */
} siginfo_t;
#endif

/* Macros for waitpid */
#define WNOHANG 1
#define WUNTRACED 2
#define WCONTINUED 8

/* Macros for waitid */
#define WEXITED 4
#define WSTOPPED 2
#define WNOWAIT 0x01000000

/* POSIX wait status macros */
#define WIFEXITED(status) (((status) & 0x7F) == 0)
#define WEXITSTATUS(status) (((status) & 0xFF00) >> 8)
#define WIFSIGNALED(status)                                                    \
  (((status) & 0x7F) != 0 && ((status) & 0x7F) != 0x7F)
#define WTERMSIG(status) ((status) & 0x7F)
#define WIFSTOPPED(status) (((status) & 0xFF) == 0x7F)
#define WSTOPSIG(status) (((status) & 0xFF00) >> 8)

/**
 * @brief Waits for a child process to terminate.
 *
 * @param stat_loc Pointer to an integer where status information is stored.
 * @return The process ID of the terminated child, or -1 on error.
 */
pid_t wait(int *stat_loc);

/**
 * @brief Waits for a specific process or process group to terminate.
 *
 * @param pid The process ID or process group ID to wait for.
 * @param stat_loc Pointer to an integer where status information is stored.
 * @param options Options modifying wait behavior.
 * @return The process ID of the terminated child, 0 if WNOHANG and child
 * running, or -1 on error.
 */
pid_t waitpid(pid_t pid, int *stat_loc, int options);

/**
 * @brief Waits for a child process to change state.
 *
 * @param idtype The type of ID (P_ALL, P_PID, P_PGID).
 * @param id The ID to wait for.
 * @param infop Pointer to a siginfo_t structure where status information is
 * stored.
 * @param options Options modifying wait behavior.
 * @return 0 on success, or -1 on error.
 */
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

/**
 * @brief Sends a signal to a process using OpenProcess and TerminateProcess.
 *
 * @param pid Process ID.
 * @param sig Signal number to send.
 * @return 0 on success, or -1 on failure with errno set.
 */
int posix_wait_kill(pid_t pid, int sig);

#ifndef _KILL_DECLARED
#define _KILL_DECLARED
#ifndef kill
#define kill posix_wait_kill
#endif
#endif

#else /* !defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__) */

/**
 * @brief Sends a signal to a process using native kill.
 * @param pid Process ID.
 * @param sig Signal number.
 * @return 0 on success, or -1 on failure.
 */
int posix_wait_kill(pid_t pid, int sig);

#endif /* defined(_WIN32) || defined(__MSDOS__) || defined(__WATCOMC__) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_WAIT_H */
