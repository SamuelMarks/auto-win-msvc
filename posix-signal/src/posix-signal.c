#include <errno.h>
#ifndef ENOSYS
#define ENOSYS 38
#endif
/* posix-signal.c - Strict C89 Implementation */
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

/** \brief posix_signal_sigaction function. */
int posix_signal_sigaction(int signum, const struct sigaction *act,
                           struct sigaction *oldact) {
  /* Dummy implementation for MSVC porting */
  signum = signum;
  act = act;
  oldact = oldact;
  errno = ENOSYS;
  return -1;
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

#else

typedef int make_iso_compilers_happy_tu_posix_signal;

#endif
