/* posix-signal.c - Strict C89 Implementation */

/* clang-format off */
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include "posix-signal.h"
#ifndef ENOSYS
#define ENOSYS 38
#endif
#ifndef ESRCH
#define ESRCH 3
#endif
/* clang-format on */

/**
 * @brief Retrieves information on posix-signal availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_SIGNAL_SUCCESS on success, or POSIX_SIGNAL_ERROR_NULL_POINTER
 * on NULL pointer.
 */
enum posix_signal_error_code posix_signal_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_SIGNAL_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_SIGNAL_SUCCESS;
}

/**
 * @brief Initializes and validates the posix-signal module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_init(int *out_status) {
  if (out_status == NULL) {
    return POSIX_SIGNAL_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return POSIX_SIGNAL_SUCCESS;
}

#if defined(POSIX_SIGNAL_MSVC) || defined(_WIN32)

/* Minimal WINAPI declarations without windows.h */
__declspec(dllimport) void *__stdcall OpenProcess(unsigned long dwDesiredAccess,
                                                  int bInheritHandle,
                                                  unsigned long dwProcessId);
__declspec(dllimport) int __stdcall TerminateProcess(void *hProcess,
                                                     unsigned int uExitCode);
__declspec(dllimport) int __stdcall CloseHandle(void *hObject);
__declspec(dllimport) unsigned long __stdcall GetCurrentProcessId(void);
__declspec(dllimport) void __stdcall SleepEx(unsigned long dwMilliseconds,
                                             int bAlertable);

#define PROCESS_TERMINATE 0x0001
#define PROCESS_QUERY_INFORMATION 0x0400

/**
 * @brief Initialize an empty signal set.
 * @param[out] set Pointer to the signal set to clear.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigemptyset(sigset_t *set) {
  if (!set) {
    errno = EINVAL;
    return -1;
  }
  *set = 0;
  return 0;
}

/**
 * @brief Initialize a full signal set containing all signals.
 * @param[out] set Pointer to the signal set to fill.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigfillset(sigset_t *set) {
  if (!set) {
    errno = EINVAL;
    return -1;
  }
  *set = ~((sigset_t)0);
  return 0;
}

/**
 * @brief Add a signal to a signal set.
 * @param[in,out] set Pointer to the signal set.
 * @param[in] signum Signal number to add.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigaddset(sigset_t *set, int signum) {
  if (!set || signum < 1 || signum > 31) {
    errno = EINVAL;
    return -1;
  }
  *set |= (1UL << signum);
  return 0;
}

/**
 * @brief Delete a signal from a signal set.
 * @param[in,out] set Pointer to the signal set.
 * @param[in] signum Signal number to remove.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigdelset(sigset_t *set, int signum) {
  if (!set || signum < 1 || signum > 31) {
    errno = EINVAL;
    return -1;
  }
  *set &= ~(1UL << signum);
  return 0;
}

/**
 * @brief Test whether a signal is a member of a signal set.
 * @param[in] set Pointer to the signal set.
 * @param[in] signum Signal number to test.
 * @return 1 if member, 0 if not member, or -1 on error.
 */
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

#if defined(_MSC_VER) || defined(__MINGW32__)
/**
 * @brief Checks whether a signal is natively supported by the C runtime.
 * @param[in] signum Signal number to check.
 * @param[out] out_is_crt Pointer receiving 1 if native CRT signal, 0 otherwise.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_is_crt_signal(int signum,
                                                        int *out_is_crt) {
  if (out_is_crt == NULL) {
    return POSIX_SIGNAL_ERROR_NULL_POINTER;
  }
  *out_is_crt = (signum == SIGINT || signum == SIGILL || signum == SIGFPE ||
                 signum == SIGSEGV || signum == SIGTERM || signum == SIGABRT
#ifdef SIGBREAK
                 || signum == SIGBREAK
#endif
                 )
                    ? 1
                    : 0;
  return POSIX_SIGNAL_SUCCESS;
}
#endif

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

/**
 * @brief Vectored exception handler for synchronous hardware exceptions.
 * @param[in] ExceptionInfo Exception information pointers.
 * @return EXCEPTION_CONTINUE_SEARCH or EXCEPTION_EXECUTE_HANDLER.
 */
static long __stdcall veh_handler(EXCEPTION_POINTERS *ExceptionInfo) {
  int signum = 0;
  siginfo_t si;
  unsigned long code;

  if (ExceptionInfo == NULL || ExceptionInfo->ExceptionRecord == NULL) {
    return EXCEPTION_CONTINUE_SEARCH;
  }
  code = ExceptionInfo->ExceptionRecord->ExceptionCode;

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

/**
 * @brief Internal signal dispatch function.
 * @param[in] signum Signal number to dispatch.
 */
static void internal_signal_handler(int signum) {
  if (signum < 1 || signum > 31) {
    return;
  }

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

#if defined(_MSC_VER) || defined(__MINGW32__)
  {
    int is_crt = 0;
    enum posix_signal_error_code sig_rc;
    sig_rc = posix_signal_is_crt_signal(signum, &is_crt);
    if (sig_rc == POSIX_SIGNAL_SUCCESS && is_crt) {
      (signal)(signum, internal_signal_handler);
    }
  }
#else
  (signal)(signum, internal_signal_handler);
#endif
}

/**
 * @brief Examine and change blocked signals.
 * @param[in] how Action to perform (SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK).
 * @param[in] set Signal set to apply, or NULL to query.
 * @param[out] oldset Pointer to receive previous mask, or NULL.
 * @return 0 on success, or -1 on error.
 */
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
        if (g_signal_flags[i] & SA_SIGINFO) {
          if (g_sigaction_handlers[i]) {
            siginfo_t si;
            si.si_signo = i;
            si.si_code = 0;
            si.si_errno = 0;
            si.si_pid = 0;
            si.si_uid = 0;
            si.si_addr = NULL;
            si.si_status = 0;
            si.si_band = 0;
            g_sigaction_handlers[i](i, &si, NULL);
          }
        } else if (g_signal_handlers[i] && g_signal_handlers[i] != SIG_DFL &&
                   g_signal_handlers[i] != SIG_IGN) {
          g_signal_handlers[i](i);
        }
      }
    }
  }

  return 0;
}

/**
 * @brief Examine pending signals.
 * @param[out] set Pointer to receive the set of pending signals.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_sigpending(sigset_t *set) {
  if (!set) {
    errno = EINVAL;
    return -1;
  }
  *set = g_pending_signals;
  return 0;
}

/**
 * @brief Wait for a signal with a temporary mask.
 * @param[in] mask Temporary signal mask.
 * @return Always returns -1 with errno set to EINTR.
 */
int posix_signal_sigsuspend(const sigset_t *mask) {
  sigset_t old_mask;
  if (!mask) {
    errno = EINVAL;
    return -1;
  }

  posix_signal_sigprocmask(SIG_SETMASK, mask, &old_mask);

  /* Alertable wait to check for queued APCs */
  SleepEx(0, 1 /* TRUE */);

  /* Restore old mask after waking up */
  posix_signal_sigprocmask(SIG_SETMASK, &old_mask, NULL);

  /* POSIX says sigsuspend always returns -1 with EINTR */
  errno = EINTR;
  return -1;
}

/**
 * @brief Examine and change a signal action.
 * @param[in] signum Signal number to modify.
 * @param[in] act New action to set, or NULL to query.
 * @param[out] oldact Pointer to receive previous action, or NULL.
 * @return 0 on success, or -1 on error.
 */
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
  {
    int is_crt = 0;
    enum posix_signal_error_code sig_rc;
    sig_rc = posix_signal_is_crt_signal(signum, &is_crt);
    if (sig_rc != POSIX_SIGNAL_SUCCESS) {
      errno = EINVAL;
      return -1;
    }
    if (!is_crt) {
      if (oldact) {
        oldact->sa_handler = g_signal_handlers[signum];
        oldact->sa_sigaction = g_sigaction_handlers[signum];
        posix_signal_sigemptyset(&oldact->sa_mask);
        oldact->sa_flags = g_signal_flags[signum];
        oldact->sa_restorer = NULL;
      }
      if (act) {
        g_signal_flags[signum] = act->sa_flags;
        if (act->sa_flags & SA_SIGINFO) {
          g_sigaction_handlers[signum] = act->sa_sigaction;
          g_signal_handlers[signum] = NULL;
        } else {
          g_sigaction_handlers[signum] = NULL;
          g_signal_handlers[signum] = act->sa_handler;
        }
      }
      return 0;
    }
  }
#endif

  if (act) {
    if (!g_veh_handle) {
      g_veh_handle =
          AddVectoredExceptionHandler(1, (void *)(size_t)veh_handler);
    }
    g_signal_flags[signum] = act->sa_flags;
    if (act->sa_flags & SA_SIGINFO) {
      g_sigaction_handlers[signum] = act->sa_sigaction;
      g_signal_handlers[signum] = NULL;
      prev_handler = (signal)(signum, internal_signal_handler);
    } else {
      g_sigaction_handlers[signum] = NULL;
      if (act->sa_handler != SIG_DFL && act->sa_handler != SIG_IGN) {
        g_signal_handlers[signum] = act->sa_handler;
        prev_handler = (signal)(signum, internal_signal_handler);
      } else {
        g_signal_handlers[signum] = act->sa_handler;
        prev_handler = (signal)(signum, act->sa_handler);
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
      oldact->sa_restorer = NULL;
    }
  } else if (oldact) {
    prev_handler = (signal)(signum, SIG_IGN);
    if (prev_handler == SIG_ERR) {
      return -1;
    }
    (signal)(signum, prev_handler);
    if (prev_handler == internal_signal_handler) {
      oldact->sa_handler = g_signal_handlers[signum];
      oldact->sa_sigaction = g_sigaction_handlers[signum];
    } else {
      oldact->sa_handler = prev_handler;
      oldact->sa_sigaction = NULL;
    }
    posix_signal_sigemptyset(&oldact->sa_mask);
    oldact->sa_flags = g_signal_flags[signum];
    oldact->sa_restorer = NULL;
  }

  return 0;
}

/**
 * @brief Set a signal handling function.
 * @param[in] signum Signal number.
 * @param[in] handler Function pointer for signal handler.
 * @return Previous handler on success, or SIG_ERR on error.
 */
posix_sighandler_t posix_signal_signal(int signum, posix_sighandler_t handler) {
  posix_sighandler_t prev;
  if (signum < 1 || signum > 31) {
    errno = EINVAL;
    return SIG_ERR;
  }
#if defined(_MSC_VER) || defined(__MINGW32__)
  {
    int is_crt = 0;
    enum posix_signal_error_code sig_rc;
    sig_rc = posix_signal_is_crt_signal(signum, &is_crt);
    if (sig_rc != POSIX_SIGNAL_SUCCESS) {
      errno = EINVAL;
      return SIG_ERR;
    }
    if (!is_crt) {
      prev = g_signal_handlers[signum];
      g_signal_handlers[signum] = handler;
      return prev ? prev : SIG_DFL;
    }
  }
#endif
  return (signal)(signum, handler);
}

/**
 * @brief Send a signal to a process.
 * @param[in] pid Process ID of target process.
 * @param[in] sig Signal number to send.
 * @return 0 on success, or -1 on error.
 */
int posix_signal_kill(pid_t pid, int sig) {
  void *hProcess;
  if (pid <= 0) {
    errno = EINVAL;
    return -1;
  }
  if (sig == 0) {
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (unsigned long)pid);
    if (hProcess) {
      CloseHandle(hProcess);
      return 0;
    }
    errno = ESRCH;
    return -1;
  }

  hProcess = OpenProcess(PROCESS_TERMINATE, 0, (unsigned long)pid);
  if (!hProcess) {
    errno = ESRCH;
    return -1;
  }
  TerminateProcess(hProcess, (unsigned int)sig);
  CloseHandle(hProcess);
  return 0;
}

/**
 * @brief Retrieves the current process ID.
 * @param[out] out_pid Pointer to pid_t receiving current PID.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_get_current_pid(pid_t *out_pid) {
  if (out_pid == NULL) {
    return POSIX_SIGNAL_ERROR_NULL_POINTER;
  }
  *out_pid = (pid_t)GetCurrentProcessId();
  return POSIX_SIGNAL_SUCCESS;
}

/**
 * @brief Simulates exception handling for posix-signal testing.
 * @param[in] code Exception code to simulate.
 * @param[out] out_result Pointer to receive the handler result.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_simulate_exception(unsigned long code,
                                                             long *out_result) {
  EXCEPTION_RECORD rec;
  EXCEPTION_POINTERS ep;
  if (out_result == NULL) {
    return POSIX_SIGNAL_ERROR_NULL_POINTER;
  }
  if (code == 0) {
    *out_result = veh_handler(NULL);
    return POSIX_SIGNAL_SUCCESS;
  }
  if (code == 1) {
    ep.ExceptionRecord = NULL;
    ep.ContextRecord = NULL;
    *out_result = veh_handler(&ep);
    return POSIX_SIGNAL_SUCCESS;
  }
  memset(&rec, 0, sizeof(rec));
  rec.ExceptionCode = code;
  rec.ExceptionInformation[1] = (size_t)0x1234;
  ep.ExceptionRecord = &rec;
  ep.ContextRecord = NULL;
  *out_result = veh_handler(&ep);
  return POSIX_SIGNAL_SUCCESS;
}

/**
 * @brief Simulates signal handling for posix-signal testing.
 * @param[in] signum Signal number to simulate.
 * @param[out] out_status Pointer to receive completion status.
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_simulate_signal(int signum,
                                                          int *out_status) {
  if (out_status == NULL) {
    return POSIX_SIGNAL_ERROR_NULL_POINTER;
  }
  internal_signal_handler(signum);
  *out_status = 1;
  return POSIX_SIGNAL_SUCCESS;
}

/**
 * @brief Resets all internal signal handler state.
 * @param[out] out_status Pointer to integer receiving status (1).
 * @return POSIX_SIGNAL_SUCCESS on success, or an error code on failure.
 */
enum posix_signal_error_code posix_signal_reset(int *out_status) {
  int i;
  if (out_status == NULL) {
    return POSIX_SIGNAL_ERROR_NULL_POINTER;
  }
  for (i = 0; i < 32; ++i) {
    g_signal_handlers[i] = NULL;
    g_sigaction_handlers[i] = NULL;
    g_signal_flags[i] = 0;
  }
  g_blocked_signals = 0;
  g_pending_signals = 0;
  *out_status = 1;
  return POSIX_SIGNAL_SUCCESS;
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
