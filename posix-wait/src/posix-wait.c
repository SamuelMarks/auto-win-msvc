/* posix-wait.c - Strict C89 Implementation */
#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

/* clang-format off */
#include "posix-wait.h"
#include <errno.h>
#include <stddef.h>

#ifdef _WIN32

#if defined(_M_AMD64) && !defined(_AMD64_)
#define _AMD64_
#elif defined(_M_IX86) && !defined(_X86_)
#define _X86_
#elif defined(_M_ARM64) && !defined(_ARM64_)
#define _ARM64_
#elif defined(_M_ARM) && !defined(_ARM_)
#define _ARM_
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#else
#include <handleapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <synchapi.h>
/* clang-format on */
#endif

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0x00000000L
#endif

#ifndef WAIT_TIMEOUT
#define WAIT_TIMEOUT 258L
#endif

/** \brief wait function. */
pid_t wait(int *stat_loc) { return waitpid(-1, stat_loc, 0); }

/* Missing functions from TlHelp32 */
typedef void *WIN_HANDLE;
typedef unsigned long WIN_DWORD;
typedef const char *WIN_LPCSTR;

#define WIN_TH32CS_SNAPPROCESS 0x00000002
typedef struct tagWIN_PROCESSENTRY32 {
  WIN_DWORD dwSize;
  WIN_DWORD cntUsage;
  WIN_DWORD th32ProcessID;
  WIN_DWORD th32DefaultHeapID;
  WIN_DWORD th32ModuleID;
  WIN_DWORD cntThreads;
  WIN_DWORD th32ParentProcessID;
  long pcPriClassBase;
  WIN_DWORD dwFlags;
  char szExeFile[260];
} WIN_PROCESSENTRY32;

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport)
WIN_HANDLE __stdcall CreateToolhelp32Snapshot(WIN_DWORD dwFlags,
                                              WIN_DWORD th32ProcessID);
__declspec(dllimport) int __stdcall Process32First(WIN_HANDLE hSnapshot,
                                                   WIN_PROCESSENTRY32 *lppe);
__declspec(dllimport) int __stdcall Process32Next(WIN_HANDLE hSnapshot,
                                                  WIN_PROCESSENTRY32 *lppe);
__declspec(dllimport) WIN_DWORD __stdcall GetCurrentProcessId(void);
__declspec(dllimport) void __stdcall Sleep(WIN_DWORD dwMilliseconds);
#ifdef __cplusplus
}
#endif

/** \brief waitpid function. */
pid_t waitpid(pid_t pid, int *stat_loc, int options) {
  HANDLE hProcess;
  DWORD wait_res;
  DWORD exit_code;

  if (pid <= 0) {
    if (pid != -1) {
      errno = EINVAL;
      return (pid_t)-1;
    }

    /* Wait for any child (-1) */
    while (1) {
      HANDLE hSnap;
      WIN_PROCESSENTRY32 pe32;
      DWORD myPid = GetCurrentProcessId();
      HANDLE childHandles[64];
      DWORD childPids[64];
      DWORD childCount = 0;
      DWORD i;
      int exited_found = 0;
      pid_t ret = -1;

      hSnap = CreateToolhelp32Snapshot(WIN_TH32CS_SNAPPROCESS, 0);
      if (hSnap == (HANDLE)(size_t)-1) {
        errno = ECHILD;
        return (pid_t)-1;
      }

      pe32.dwSize = sizeof(WIN_PROCESSENTRY32);
      if (Process32First(hSnap, &pe32)) {
        do {
          if (pe32.th32ParentProcessID == myPid) {
            HANDLE hProc = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION,
                                       FALSE, pe32.th32ProcessID);
            if (hProc) {
              if (childCount < 64) {
                childHandles[childCount] = hProc;
                childPids[childCount] = pe32.th32ProcessID;
                childCount++;
              } else {
                CloseHandle(hProc);
              }
            }
          }
        } while (Process32Next(hSnap, &pe32));
      }
      CloseHandle(hSnap);

      if (childCount == 0) {
        errno = ECHILD;
        return (pid_t)-1;
      }

      for (i = 0; i < childCount; i++) {
        if (WaitForSingleObject(childHandles[i], 0) == WAIT_OBJECT_0) {
          if (stat_loc != NULL) {
            if (GetExitCodeProcess(childHandles[i], &exit_code)) {
              *stat_loc = ((int)(exit_code & 0xFF) << 8);
            } else {
              *stat_loc = 0;
            }
          }
          ret = (pid_t)childPids[i];
          exited_found = 1;
          break;
        }
      }

      if (exited_found) {
        for (i = 0; i < childCount; i++) {
          CloseHandle(childHandles[i]);
        }
        return ret;
      }

      if (options & WNOHANG) {
        for (i = 0; i < childCount; i++) {
          CloseHandle(childHandles[i]);
        }
        return 0;
      }

      /* Blocking wait, sleep to prevent spinning too fast */
      for (i = 0; i < childCount; i++) {
        CloseHandle(childHandles[i]);
      }
      Sleep(50);
    }
  }

  hProcess =
      OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
  if (hProcess == NULL) {
    errno = ECHILD;
    return (pid_t)-1;
  }

  wait_res = WaitForSingleObject(hProcess, (options & WNOHANG) ? 0 : INFINITE);
  if (wait_res == WAIT_TIMEOUT) {
    CloseHandle(hProcess);
    return 0;
  } else if (wait_res == WAIT_OBJECT_0) {
    if (stat_loc != NULL) {
      if (GetExitCodeProcess(hProcess, &exit_code)) {
        /* Simulate POSIX status byte shifting */
        *stat_loc = ((int)(exit_code & 0xFF) << 8);
      } else {
        *stat_loc = 0;
      }
    }
    CloseHandle(hProcess);
    return pid;
  }

  CloseHandle(hProcess);
  errno = EINVAL;
  return (pid_t)-1;
}

/** \brief waitid function. */
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
  pid_t res;
  int status = 0;
  int pid_to_wait = -1;

  if (infop != NULL) {
    infop->si_signo = 0;
    infop->si_code = 0;
    infop->si_pid = 0;
    infop->si_status = 0;
    infop->si_uid = 0;
  }

  if (idtype == P_PID) {
    pid_to_wait = (int)id;
  } else if (idtype == P_ALL) {
    pid_to_wait = -1;
  } else {
    errno = EINVAL;
    return -1;
  }

  res = waitpid((pid_t)pid_to_wait, &status, options);
  if (res > 0) {
    if (infop != NULL) {
      infop->si_pid = res;
      infop->si_status = WEXITSTATUS(status);
      infop->si_code = 1; /* CLD_EXITED */
    }
    return 0;
  } else if (res == 0) {
    return 0;
  }

  return -1;
}

#endif /* _WIN32 */

/* Non-standard Microsoft-compatible cwait */
pid_t cwait(int *termstat, pid_t pid, int action) {
  (void)action; /* Unused */
  return waitpid(pid, termstat, 0);
}

#ifdef __CYGWIN__
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
  pid_t res;
  int status = 0;
  int pid_to_wait = -1;
  int waitpid_options = 0;

  if (infop != NULL) {
    infop->si_signo = 0;
    infop->si_code = 0;
    infop->si_pid = 0;
    infop->si_status = 0;
    infop->si_uid = 0;
  }

  if (idtype == P_PID) {
    pid_to_wait = (int)id;
  } else if (idtype == P_ALL) {
    pid_to_wait = -1;
  } else {
    errno = EINVAL;
    return -1;
  }

  if (options & WNOHANG)
    waitpid_options |= WNOHANG;
  if (options & WSTOPPED)
    waitpid_options |= WUNTRACED;
  if (options & WCONTINUED)
    waitpid_options |= WCONTINUED;

  res = waitpid((pid_t)pid_to_wait, &status, waitpid_options);
  if (res > 0) {
    if (infop != NULL) {
      infop->si_pid = res;
      infop->si_status = WEXITSTATUS(status);
      infop->si_code = 1; /* CLD_EXITED */
    }
    return 0;
  } else if (res == 0) {
    return 0;
  }

  return -1;
}
#endif /* __CYGWIN__ */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_wait(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_wait;
