/* posix-signal.h - Strict C89 Implementation */
#ifndef POSIX_SIGNAL_H
#define POSIX_SIGNAL_H

#include <signal.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define POSIX_SIGNAL_MSVC 1
#endif

#if defined(POSIX_SIGNAL_MSVC) || defined(_WIN32)

typedef unsigned long sigset_t;

#ifndef _PID_T_DEFINED
#define _PID_T_DEFINED
typedef int pid_t;
#endif

#ifndef _UID_T_DEFINED
#define _UID_T_DEFINED
typedef int uid_t;
typedef int gid_t;
#endif

typedef struct {
    int si_signo;
    int si_code;
    int si_errno;
    pid_t si_pid;
    uid_t si_uid;
    void *si_addr;
    int si_status;
    long si_band;
} siginfo_t;

struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};

/** \brief posix_signal_sigemptyset function. */
int posix_signal_sigemptyset(sigset_t *set);
/** \brief posix_signal_sigfillset function. */
int posix_signal_sigfillset(sigset_t *set);
/** \brief posix_signal_sigaddset function. */
int posix_signal_sigaddset(sigset_t *set, int signum);
/** \brief posix_signal_sigaction function. */
int posix_signal_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
/** \brief posix_signal_kill function. */
int posix_signal_kill(pid_t pid, int sig);

#ifndef sigemptyset
#define sigemptyset posix_signal_sigemptyset
#endif
#ifndef sigfillset
#define sigfillset posix_signal_sigfillset
#endif
#ifndef sigaddset
#define sigaddset posix_signal_sigaddset
#endif
#ifndef sigaction
#define sigaction(sig, act, oact) posix_signal_sigaction((sig), (act), (oact))
#endif
#ifndef kill
#define kill posix_signal_kill
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* POSIX_SIGNAL_H */
