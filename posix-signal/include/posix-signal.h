/* posix-signal.h - Strict C89 Implementation */
#ifndef POSIX_SIGNAL_H
#define POSIX_SIGNAL_H

/**
 * @file posix-signal.h
 * @brief Strict C89 POSIX signal compatibility interface and types.
 */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/* clang-format off */
#include <signal.h>
#include <stddef.h>
#include "posix-signal-ext.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-signal functions.
 */
enum posix_signal_error_code {
  /** @brief Operation completed successfully. */
  POSIX_SIGNAL_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_SIGNAL_ERROR_NULL_POINTER = 1,
  /** @brief An invalid argument was passed. */
  POSIX_SIGNAL_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Retrieves information on posix-signal availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_SIGNAL_SUCCESS on success, or POSIX_SIGNAL_ERROR_NULL_POINTER
 * on NULL pointer.
 */
enum posix_signal_error_code posix_signal_get_info(int *out_available);

/**
 * @brief Initializes and validates the posix-signal module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_init(int *out_status);

#if defined(_MSC_VER) && !defined(__clang__)
#define POSIX_SIGNAL_MSVC 1
#endif

#if defined(POSIX_SIGNAL_MSVC) || defined(_WIN32) || defined(__MSDOS__) ||     \
    defined(__WATCOMC__)

#ifndef _SIGSET_T_DEFINED
#define _SIGSET_T_DEFINED
/** \brief Signal set bitmask type. */
typedef unsigned long sigset_t;
#endif

#ifndef SIGHUP
/** \brief Hangup detected. */
#define SIGHUP 1
#endif
#ifndef SIGINT
/** \brief Interactive attention signal. */
#define SIGINT 2
#endif
#ifndef SIGQUIT
/** \brief Interactive termination signal. */
#define SIGQUIT 3
#endif
#ifndef SIGKILL
/** \brief Kill process signal. */
#define SIGKILL 9
#endif
#ifndef SIGUSR1
/** \brief User defined signal 1. */
#define SIGUSR1 10
#endif
#ifndef SIGUSR2
/** \brief User defined signal 2. */
#define SIGUSR2 12
#endif
#ifndef SIGPIPE
/** \brief Broken pipe signal. */
#define SIGPIPE 13
#endif
#ifndef SIGALRM
/** \brief Alarm clock signal. */
#define SIGALRM 14
#endif

/** \brief Signal handler function pointer type. */
typedef void (*posix_sighandler_t)(int);

#ifndef _PID_T_DEFINED
#define _PID_T_DEFINED
/** \brief Process ID type. */
typedef int pid_t;
#endif

#ifndef _UID_T_DEFINED
#define _UID_T_DEFINED
/** \brief User ID type. */
typedef int uid_t;
/** \brief Group ID type. */
typedef int gid_t;
#endif

#ifndef _SIGINFO_T_DEFINED
#define _SIGINFO_T_DEFINED
/**
 * @brief Signal information structure.
 */
typedef struct {
  /** @brief Signal number. */
  int si_signo;
  /** @brief Signal code. */
  int si_code;
  /** @brief Errno value associated with this signal. */
  int si_errno;
  /** @brief Sending process ID. */
  pid_t si_pid;
  /** @brief Real user ID of sending process. */
  uid_t si_uid;
  /** @brief Address of faulting instruction. */
  void *si_addr;
  /** @brief Exit value or signal. */
  int si_status;
  /** @brief Band event for SIGPOLL. */
  long si_band;
} siginfo_t;
#endif

#ifndef SA_SIGINFO
/** \brief Invoke signal-catching function with three arguments instead of one.
 */
#define SA_SIGINFO 0x00000004
#endif

/**
 * @brief Structure specifying a signal-handling action.
 */
struct sigaction {
  /** @brief Signal handling function pointer or SIG_DFL / SIG_IGN. */
  void (*sa_handler)(int);
  /** @brief Pointer to a 3-argument signal-handling action. */
  void (*sa_sigaction)(int, siginfo_t *, void *);
  /** @brief Additional set of signals to be blocked during handler execution.
   */
  sigset_t sa_mask;
  /** @brief Special flags affecting the signal behavior. */
  int sa_flags;
  /** @brief Signal handler restorer function pointer. */
  void (*sa_restorer)(void);
};

/**
 * @brief Initialize an empty signal set.
 * @param[out] set Pointer to the signal set to clear.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigemptyset(sigset_t *set);

/**
 * @brief Initialize a full signal set containing all signals.
 * @param[out] set Pointer to the signal set to fill.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigfillset(sigset_t *set);

/**
 * @brief Add a signal to a signal set.
 * @param[in,out] set Pointer to the signal set.
 * @param[in] signum Signal number to add.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigaddset(sigset_t *set, int signum);

/**
 * @brief Examine and change a signal action.
 * @param[in] signum Signal number to modify.
 * @param[in] act New action to set, or NULL to query.
 * @param[out] oldact Pointer to receive previous action, or NULL.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigaction(int signum, const struct sigaction *act,
                           struct sigaction *oldact);

/**
 * @brief Delete a signal from a signal set.
 * @param[in,out] set Pointer to the signal set.
 * @param[in] signum Signal number to remove.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigdelset(sigset_t *set, int signum);

/**
 * @brief Test whether a signal is a member of a signal set.
 * @param[in] set Pointer to the signal set.
 * @param[in] signum Signal number to test.
 * @return 1 if member, 0 if not member, or -1 on error.
 */
int posix_signal_sigismember(const sigset_t *set, int signum);

/**
 * @brief Examine and change blocked signals.
 * @param[in] how Action to perform (SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK).
 * @param[in] set Signal set to apply, or NULL to query.
 * @param[out] oldset Pointer to receive previous mask, or NULL.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

/**
 * @brief Examine pending signals.
 * @param[out] set Pointer to receive the set of pending signals.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigpending(sigset_t *set);

/**
 * @brief Wait for a signal with a temporary mask.
 * @param[in] mask Temporary signal mask.
 * @return Always returns -1 with errno set to EINTR.
 */
int posix_signal_sigsuspend(const sigset_t *mask);

/**
 * @brief Send a signal to a process.
 * @param[in] pid Process ID of target process.
 * @param[in] sig Signal number to send.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_kill(pid_t pid, int sig);

/**
 * @brief Set a signal handling function.
 * @param[in] signum Signal number.
 * @param[in] handler Function pointer for signal handler.
 * @return Previous handler on success, or SIG_ERR on error.
 */
posix_sighandler_t posix_signal_signal(int signum, posix_sighandler_t handler);

#if defined(_MSC_VER) || defined(_WIN32)
/**
 * @brief Checks whether a signal is natively supported by the C runtime.
 * @param[in] signum Signal number to check.
 * @param[out] out_is_crt Pointer receiving 1 if native CRT signal, 0 otherwise.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_is_crt_signal(int signum,
                                                        int *out_is_crt);

/**
 * @brief Retrieves the current process ID.
 * @param[out] out_pid Pointer to pid_t receiving current PID.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_get_current_pid(pid_t *out_pid);

/**
 * @brief Simulates exception handling for posix-signal testing.
 * @param[in] code Exception code to simulate.
 * @param[out] out_result Pointer to receive the handler result.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_simulate_exception(unsigned long code,
                                                             long *out_result);

/**
 * @brief Simulates signal handling for posix-signal testing.
 * @param[in] signum Signal number to simulate.
 * @param[out] out_status Pointer to receive completion status.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_simulate_signal(int signum,
                                                          int *out_status);

/**
 * @brief Resets all internal signal handler state.
 * @param[out] out_status Pointer to integer receiving status (1).
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_reset(int *out_status);
#endif

#ifndef SIG_BLOCK
/** \brief Block signals. */
#define SIG_BLOCK 0
/** \brief Unblock signals. */
#define SIG_UNBLOCK 1
/** \brief Set mask of blocked signals. */
#define SIG_SETMASK 2
#endif

#ifndef sigemptyset
#define sigemptyset posix_signal_sigemptyset
#endif
#ifndef sigfillset
#define sigfillset posix_signal_sigfillset
#endif
#ifndef sigaddset
#define sigaddset posix_signal_sigaddset
#endif
#ifndef sigdelset
#define sigdelset posix_signal_sigdelset
#endif
#ifndef sigismember
#define sigismember posix_signal_sigismember
#endif
#ifndef sigprocmask
#define sigprocmask posix_signal_sigprocmask
#endif
#ifndef sigpending
#define sigpending posix_signal_sigpending
#endif
#ifndef sigsuspend
#define sigsuspend posix_signal_sigsuspend
#endif
#ifndef sigaction
#define sigaction(sig, act, oact) posix_signal_sigaction((sig), (act), (oact))
#endif
#ifndef signal
#define signal(sig, handler) posix_signal_signal((sig), (handler))
#endif
#ifndef kill
#define kill posix_signal_kill
#endif

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SIGNAL_H */
