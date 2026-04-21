#include <errno.h>
#ifndef ENOSYS
#define ENOSYS 38
#endif
/* posix-signal.c - Strict C89 Implementation */
#ifdef _MSC_VER
#pragma warning(disable : 4306)
#endif
/* clang-format off */
#include "posix-signal.h"
/* clang-format on */

#if defined(POSIX_SIGNAL_MSVC) || defined(_WIN32)

/* Minimal WINAPI declarations without windows.h */
__declspec(dllimport) void *__stdcall OpenProcess(unsigned long dwDesiredAccess,
                                                  int bInheritHandle,
                                                  unsigned long dwProcessId);
__declspec(dllimport) int __stdcall TerminateProcess(void *hProcess,
                                                     unsigned int uExitCode);
__declspec(dllimport) int __stdcall CloseHandle(void *hObject);

#define PROCESS_TERMINATE 0x0001
#define PROCESS_QUERY_INFORMATION 0x0400

/** \brief posix_signal_sigemptyset function. */
int posix_signal_sigemptyset(sigset_t *set) {
  if (!set)
    return -1;
  *set = 0;
  return 0;
}

/** \brief posix_signal_sigfillset function. */
int posix_signal_sigfillset(sigset_t *set) {
  if (!set)
    return -1;
  *set = ~((sigset_t)0);
  return 0;
}

/** \brief posix_signal_sigaddset function. */
int posix_signal_sigaddset(sigset_t *set, int signum) {
  if (!set)
    return -1;
  if (signum < 1 || signum > 31)
    return -1;
  *set |= (1UL << signum);
  return 0;
}

/** \brief posix_signal_sigdelset function. */
int posix_signal_sigdelset(sigset_t *set, int signum) {
  if (!set || signum < 1 || signum > 31) {
    errno = EINVAL;
    return -1;
  }
  *set &= ~(1UL << signum);
  return 0;
}

/** \brief posix_signal_sigismember function. */
int posix_signal_sigismember(const sigset_t *set, int signum) {
  if (!set || signum < 1 || signum > 31) {
    errno = EINVAL;
    return -1;
  }
  return (*set & (1UL << signum)) ? 1 : 0;
}

static void (*g_signal_handlers[32])(int) = {0};
static void (*g_sigaction_handlers[32])(int, siginfo_t *, void *) = {0};
static int g_signal_flags[32] = {0};

static volatile unsigned long g_blocked_signals = 0;
static volatile unsigned long g_pending_signals = 0;

static void *g_veh_handle = NULL;

__declspec(dllimport) void *__stdcall AddVectoredExceptionHandler(
    unsigned long First, void *Handler);
__declspec(dllimport) unsigned long __stdcall RemoveVectoredExceptionHandler(
    void *Handle);

struct _EXCEPTION_RECORD;
struct _CONTEXT;

typedef struct _EXCEPTION_RECORD {
  unsigned long ExceptionCode;
  unsigned long ExceptionFlags;
  struct _EXCEPTION_RECORD *ExceptionRecord;
  void *ExceptionAddress;
  unsigned long NumberParameters;
  size_t ExceptionInformation[15];
} EXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
  EXCEPTION_RECORD *ExceptionRecord;
  struct _CONTEXT *ContextRecord;
} EXCEPTION_POINTERS;

#define EXCEPTION_ACCESS_VIOLATION 0xC0000005
#define EXCEPTION_ILLEGAL_INSTRUCTION 0xC000001D
#define EXCEPTION_INT_DIVIDE_BY_ZERO 0xC0000094
#define EXCEPTION_FLT_DIVIDE_BY_ZERO 0xC000008E
#define EXCEPTION_CONTINUE_SEARCH 0
#define EXCEPTION_EXECUTE_HANDLER 1

static long __stdcall veh_handler(EXCEPTION_POINTERS *ExceptionInfo) {
  int signum = 0;
  siginfo_t si;
  unsigned long code = ExceptionInfo->ExceptionRecord->ExceptionCode;

  if (code == EXCEPTION_ACCESS_VIOLATION) {
    signum = SIGSEGV;
  } else if (code == EXCEPTION_ILLEGAL_INSTRUCTION) {
    signum = SIGILL;
  } else if (code == EXCEPTION_INT_DIVIDE_BY_ZERO ||
             code == EXCEPTION_FLT_DIVIDE_BY_ZERO) {
    signum = SIGFPE;
  }

  if (signum > 0) {
    if (g_blocked_signals & (1UL << signum)) {
      g_pending_signals |= (1UL << signum);
      return EXCEPTION_CONTINUE_SEARCH;
    }

    if (g_signal_flags[signum] & SA_SIGINFO) {
      if (g_sigaction_handlers[signum]) {
        si.si_signo = signum;
        si.si_code = 0;
        si.si_errno = 0;
        si.si_pid = 0;
        si.si_uid = 0;
        si.si_addr = (void *)ExceptionInfo->ExceptionRecord
                         ->ExceptionInformation[1]; /* Fault address */
        si.si_status = 0;
        si.si_band = 0;

        g_sigaction_handlers[signum](signum, &si, ExceptionInfo->ContextRecord);
        /* Continue execution, assuming the handler resolved the fault */
        return EXCEPTION_CONTINUE_SEARCH;
      }
    } else if (g_signal_handlers[signum] &&
               g_signal_handlers[signum] != SIG_DFL &&
               g_signal_handlers[signum] != SIG_IGN) {
      g_signal_handlers[signum](signum);
      return EXCEPTION_CONTINUE_SEARCH;
    }
  }
  return EXCEPTION_CONTINUE_SEARCH;
}

static void internal_signal_handler(int signum) {
  if (signum < 1 || signum > 31)
    return;

  if (g_blocked_signals & (1UL << signum)) {
    g_pending_signals |= (1UL << signum);
  } else {
    if (g_signal_flags[signum] & SA_SIGINFO) {
      if (g_sigaction_handlers[signum]) {
        siginfo_t si;
        si.si_signo = signum;
        si.si_code = 0;
        si.si_errno = 0;
        si.si_pid = 0;
        si.si_uid = 0;
        si.si_addr = NULL;
        si.si_status = 0;
        si.si_band = 0;
        g_sigaction_handlers[signum](signum, &si, NULL);
      }
    } else {
      void (*handler)(int) = g_signal_handlers[signum];
      if (handler && handler != SIG_DFL && handler != SIG_IGN) {
        handler(signum);
      }
    }
  }

  signal(signum, internal_signal_handler);
}

/** \brief posix_signal_sigprocmask function. */
int posix_signal_sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
  unsigned long old_blocked = g_blocked_signals;
  unsigned long new_blocked = old_blocked;
  int i;
  int unblocked = 0;

  if (oldset) {
    *oldset = old_blocked;
  }

  if (!set) {
    return 0;
  }

  switch (how) {
  case SIG_BLOCK:
    new_blocked |= *set;
    break;
  case SIG_UNBLOCK:
    new_blocked &= ~(*set);
    break;
  case SIG_SETMASK:
    new_blocked = *set;
    break;
  default:
    errno = EINVAL;
    return -1;
  }

  g_blocked_signals = new_blocked;

  /* Check if any pending signals were just unblocked */
  unblocked = (int)(old_blocked & ~new_blocked & g_pending_signals);
  if (unblocked) {
    for (i = 1; i <= 31; i++) {
      if (unblocked & (1UL << i)) {
        g_pending_signals &= ~(1UL << i);
        if (g_signal_handlers[i] && g_signal_handlers[i] != SIG_DFL &&
            g_signal_handlers[i] != SIG_IGN) {
          g_signal_handlers[i](i);
        }
      }
    }
  }

  return 0;
}

/** \brief posix_signal_sigpending function. */
int posix_signal_sigpending(sigset_t *set) {
  if (!set) {
    errno = EINVAL;
    return -1;
  }
  *set = g_pending_signals;
  return 0;
}

__declspec(dllimport) void __stdcall SleepEx(unsigned long dwMilliseconds,
                                             int bAlertable);

/** \brief posix_signal_sigsuspend function. */
int posix_signal_sigsuspend(const sigset_t *mask) {
  sigset_t old_mask;
  if (!mask) {
    errno = EINVAL;
    return -1;
  }

  posix_signal_sigprocmask(SIG_SETMASK, mask, &old_mask);

  /* Wait for an event, typically APC or message */
  SleepEx(0xFFFFFFFF, 1 /* TRUE */);

  /* Restore old mask after waking up */
  posix_signal_sigprocmask(SIG_SETMASK, &old_mask, NULL);

  /* POSIX says sigsuspend always returns -1 with EINTR */
  errno = EINTR;
  return -1;
}

/** \brief posix_signal_sigaction function. */
int posix_signal_sigaction(int signum, const struct sigaction *act,
                           struct sigaction *oldact) {
  void (*prev_handler)(int);

  if (signum < 1 || signum > 31) {
    errno = EINVAL;
    return -1;
  }

  if (act == NULL && oldact == NULL) {
    errno = EINVAL;
    return -1;
  }

#if defined(_MSC_VER) || defined(__MINGW32__)
  if (signum != SIGINT && signum != SIGILL && signum != SIGFPE &&
      signum != SIGSEGV && signum != SIGTERM && signum != SIGBREAK &&
      signum != SIGABRT) {
    errno = EINVAL;
    return -1;
  }
#endif

  if (act) {
    if (!g_veh_handle) {
      g_veh_handle =
          AddVectoredExceptionHandler(1, (void *)(intptr_t)veh_handler);
    }
    g_signal_flags[signum] = act->sa_flags;
    if (act->sa_flags & SA_SIGINFO) {
      g_sigaction_handlers[signum] = act->sa_sigaction;
      g_signal_handlers[signum] = NULL;
      prev_handler = signal(signum, internal_signal_handler);
    } else {
      g_sigaction_handlers[signum] = NULL;
      if (act->sa_handler != SIG_DFL && act->sa_handler != SIG_IGN) {
        g_signal_handlers[signum] = act->sa_handler;
        prev_handler = signal(signum, internal_signal_handler);
      } else {
        g_signal_handlers[signum] = act->sa_handler;
        prev_handler = signal(signum, act->sa_handler);
      }
    }
    if (prev_handler == SIG_ERR) {
      return -1;
    }
    if (oldact) {
      if (prev_handler == internal_signal_handler) {
        oldact->sa_handler = g_signal_handlers[signum];
        oldact->sa_sigaction = g_sigaction_handlers[signum];
      } else {
        oldact->sa_handler = prev_handler;
        oldact->sa_sigaction = NULL;
      }
      posix_signal_sigemptyset(&oldact->sa_mask);
      oldact->sa_flags = g_signal_flags[signum];
    }
  } else if (oldact) {
    prev_handler = signal(signum, SIG_IGN);
    if (prev_handler == SIG_ERR) {
      return -1;
    }
    signal(signum, prev_handler);
    if (prev_handler == internal_signal_handler) {
      oldact->sa_handler = g_signal_handlers[signum];
      oldact->sa_sigaction = g_sigaction_handlers[signum];
    } else {
      oldact->sa_handler = prev_handler;
      oldact->sa_sigaction = NULL;
    }
    posix_signal_sigemptyset(&oldact->sa_mask);
    oldact->sa_flags = g_signal_flags[signum];
  }

  return 0;
}
/** \brief posix_signal_kill function. */
int posix_signal_kill(pid_t pid, int sig) {
  void *hProcess;
  if (pid <= 0)
    return -1;
  if (sig == 0) {
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (unsigned long)pid);
    if (hProcess) {
      CloseHandle(hProcess);
      return 0;
    }
    return -1;
  }

  hProcess = OpenProcess(PROCESS_TERMINATE, 0, (unsigned long)pid);
  if (!hProcess)
    return -1;
  TerminateProcess(hProcess, (unsigned int)sig);
  CloseHandle(hProcess);
  return 0;
}

#elif defined(__MSDOS__) || defined(__WATCOMC__)

int posix_signal_sigemptyset(sigset_t *set) {
  if (!set)
    return -1;
  *set = 0;
  return 0;
}

int posix_signal_sigfillset(sigset_t *set) {
  if (!set)
    return -1;
  *set = ~((sigset_t)0);
  return 0;
}

int posix_signal_sigaddset(sigset_t *set, int signum) {
  if (!set || signum < 1 || signum > 31)
    return -1;
  *set |= (1UL << signum);
  return 0;
}

int posix_signal_sigdelset(sigset_t *set, int signum) {
  if (!set || signum < 1 || signum > 31)
    return -1;
  *set &= ~(1UL << signum);
  return 0;
}

int posix_signal_sigismember(const sigset_t *set, int signum) {
  if (!set || signum < 1 || signum > 31)
    return -1;
  return (*set & (1UL << signum)) ? 1 : 0;
}

int posix_signal_sigprocmask(int how, const sigset_t *set, sigset_t *oset) {
  (void)how;
  (void)set;
  (void)oset;
  errno = ENOSYS;
  return -1;
}

int posix_signal_sigpending(sigset_t *set) {
  (void)set;
  errno = ENOSYS;
  return -1;
}

int posix_signal_sigsuspend(const sigset_t *mask) {
  (void)mask;
  errno = ENOSYS;
  return -1;
}

int posix_signal_sigaction(int sig, const struct sigaction *act,
                           struct sigaction *oact) {
  (void)sig;
  (void)act;
  (void)oact;
  errno = ENOSYS;
  return -1;
}

int posix_signal_kill(pid_t pid, int sig) {
  (void)pid;
  (void)sig;
  errno = ENOSYS;
  return -1;
}

#else

typedef int make_iso_compilers_happy_tu_posix_signal;

#endif
