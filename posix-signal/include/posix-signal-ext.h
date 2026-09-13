/* posix-signal-ext.h - Strict C89 Implementation */
#ifndef POSIX_SIGNAL_EXT_H
#define POSIX_SIGNAL_EXT_H

#if defined(__GNUC__)
#pragma GCC system_header
#endif

/* clang-format off */
#include <signal.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(POSIX_SIGNAL_MSVC) || defined(_WIN32) || defined(_MSC_VER)

/** \brief Hangup detected on controlling terminal or death of controlling
 * process. */
#ifndef SIGHUP
#define SIGHUP 1
#endif

/** \brief Interrupt from keyboard. */
#ifndef SIGINT
#define SIGINT 2
#endif

/** \brief Quit from keyboard. */
#ifndef SIGQUIT
#define SIGQUIT 3
#endif

/** \brief Trace/breakpoint trap. */
#ifndef SIGTRAP
#define SIGTRAP 5
#endif

/** \brief User-defined signal 1. */
#ifndef SIGUSR1
#define SIGUSR1 10
#endif

/** \brief User-defined signal 2. */
#ifndef SIGUSR2
#define SIGUSR2 12
#endif

/** \brief Broken pipe: write to pipe with no readers. */
#ifndef SIGPIPE
#define SIGPIPE 13
#endif

/** \brief Timer signal from alarm. */
#ifndef SIGALRM
#define SIGALRM 14
#endif

/** \brief Termination signal. */
#ifndef SIGTERM
#define SIGTERM 15
#endif

/** \brief Child stopped or terminated. */
#ifndef SIGCHLD
#define SIGCHLD 17
#endif

/** \brief Do not generate SIGCHLD when children stop. */
#ifndef SA_NOCLDSTOP
#define SA_NOCLDSTOP 1
#endif

/** \brief Do not create zombie processes when child terminates. */
#ifndef SA_NOCLDWAIT
#define SA_NOCLDWAIT 2
#endif

/** \brief Invoke signal-catching function with three arguments instead of one.
 */
#ifndef SA_SIGINFO
#define SA_SIGINFO 4
#endif

/** \brief Call the signal handler on an alternate signal stack. */
#ifndef SA_ONSTACK
#define SA_ONSTACK 0x08000000
#endif

/** \brief Provide restartable behavior across signals. */
#ifndef SA_RESTART
#define SA_RESTART 0x10000000
#endif

/** \brief Do not prevent the signal from being received from within its
 * handler. */
#ifndef SA_NODEFER
#define SA_NODEFER 0x40000000
#endif

/** \brief Restore the signal action to default upon entry to handler. */
#ifndef SA_RESETHAND
#define SA_RESETHAND 0x80000000
#endif

#endif /* defined(POSIX_SIGNAL_MSVC) || defined(_WIN32) || defined(_MSC_VER)   \
        */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SIGNAL_EXT_H */
