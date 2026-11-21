#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file posix-wait.c
 * @brief POSIX sys/wait.h implementation and compatibility layer.
 */
#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

/* clang-format off */
#if defined(__MSDOS__) || defined(__DOS__) || defined(__WATCOMC__)
#include <process.h>
#endif
#include "posix-wait.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#if defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)
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
#include <winsock2.h>
#else
#include <handleapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#endif
#endif
/* clang-format on */

#if defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0x00000000L
#endif

#ifndef WAIT_TIMEOUT
#define WAIT_TIMEOUT 258L
#endif

typedef unsigned long WIN_DWORD;

__declspec(dllimport) void __stdcall Sleep(WIN_DWORD dwMilliseconds);

__declspec(selectany) void *g_posix_child_handles[1024] = {0};
__declspec(selectany) unsigned long g_posix_child_pids[1024] = {0};
__declspec(selectany) int g_posix_child_count = 0;

#if defined(_MSC_VER)
#if defined(_M_IX86)
#pragma comment(linker,                                                        \
                "/alternatename:_awm_vfork_try_reap=_awm_vfork_try_reap_stub")
#else
#pragma comment(linker,                                                        \
                "/alternatename:awm_vfork_try_reap=awm_vfork_try_reap_stub")
#endif
/** @brief Fallback stub when posix-core is not linked. */
int awm_vfork_try_reap_stub(int pid, int *stat_loc) {
  (void)pid;
  (void)stat_loc;
  return 0;
}
#elif defined(__GNUC__) || defined(__clang__)
/** @brief Weak fallback stub when posix-core is not linked. */
int awm_vfork_try_reap(int pid, int *stat_loc);
int __attribute__((weak)) awm_vfork_try_reap(int pid, int *stat_loc) {
  (void)pid;
  (void)stat_loc;
  return 0;
}
#endif

extern int awm_vfork_try_reap(int pid, int *stat_loc);

static HANDLE default_wait_open(DWORD access, BOOL inherit, DWORD pid) {
  return OpenProcess(access, inherit, pid);
}
static DWORD default_wait_single(HANDLE h, DWORD ms) {
  return WaitForSingleObject(h, ms);
}
static BOOL default_wait_exit_code(HANDLE h, DWORD *code) {
  return GetExitCodeProcess(h, code);
}
static BOOL default_wait_terminate(HANDLE h, UINT sig) {
  return TerminateProcess(h, sig);
}
static BOOL default_wait_close(HANDLE h) { return CloseHandle(h); }
static int default_wait_reap(int pid, int *stat_loc) {
  return awm_vfork_try_reap(pid, stat_loc);
}

static HANDLE (*g_wait_open)(DWORD, BOOL, DWORD) = default_wait_open;
static DWORD (*g_wait_single)(HANDLE, DWORD) = default_wait_single;
static BOOL (*g_wait_exit_code)(HANDLE, DWORD *) = default_wait_exit_code;
static BOOL (*g_wait_terminate)(HANDLE, UINT) = default_wait_terminate;
static BOOL (*g_wait_close)(HANDLE) = default_wait_close;
static int (*g_wait_reap)(int, int *) = default_wait_reap;

/**
 * @brief Overrides OpenProcess and WaitForSingleObject hooks for testing.
 */
void posix_wait_set_open_wait_hooks(void *(*open_fn)(unsigned long, int,
                                                     unsigned long),
                                    unsigned long (*single_fn)(void *,
                                                               unsigned long));
void posix_wait_set_open_wait_hooks(void *(*open_fn)(unsigned long, int,
                                                     unsigned long),
                                    unsigned long (*single_fn)(void *,
                                                               unsigned long)) {
  g_wait_open = (HANDLE (*)(DWORD, BOOL, DWORD))open_fn;
  g_wait_single = (DWORD (*)(HANDLE, DWORD))single_fn;
}

/**
 * @brief Overrides process status, termination, close, and vfork hooks.
 */
void posix_wait_set_proc_hooks(int (*exit_code_fn)(void *, unsigned long *),
                               int (*terminate_fn)(void *, unsigned int),
                               int (*close_fn)(void *),
                               int (*reap_fn)(int, int *));
void posix_wait_set_proc_hooks(int (*exit_code_fn)(void *, unsigned long *),
                               int (*terminate_fn)(void *, unsigned int),
                               int (*close_fn)(void *),
                               int (*reap_fn)(int, int *)) {
  g_wait_exit_code = (BOOL (*)(HANDLE, DWORD *))exit_code_fn;
  g_wait_terminate = (BOOL (*)(HANDLE, UINT))terminate_fn;
  g_wait_close = (BOOL (*)(HANDLE))close_fn;
  g_wait_reap = reap_fn;
}

/**
 * @brief Resets all posix-wait Win32 hooks to defaults.
 */
void posix_wait_reset_hooks(void);
void posix_wait_reset_hooks(void) {
  g_wait_open = default_wait_open;
  g_wait_single = default_wait_single;
  g_wait_exit_code = default_wait_exit_code;
  g_wait_terminate = default_wait_terminate;
  g_wait_close = default_wait_close;
  g_wait_reap = default_wait_reap;
}

/**
 * @brief Populates or clears child tracking table entries for unit testing.
 */
void posix_wait_test_set_children(int count, void *h0, unsigned long pid0);
void posix_wait_test_set_children(int count, void *h0, unsigned long pid0) {
  g_posix_child_count = count;
  if (count > 0) {
    g_posix_child_handles[0] = h0;
    g_posix_child_pids[0] = pid0;
  }
}

/** @brief wait function. */
pid_t wait(int *stat_loc) { return waitpid(-1, stat_loc, 0); }

/** @brief wait3 function. */
pid_t wait3(int *stat_loc, int options, struct rusage *rusage) {
  pid_t ret;
  if (rusage != NULL) {
    unsigned char *p = (unsigned char *)rusage;
    size_t k;
    for (k = 0; k < 72; k++) {
      p[k] = 0;
    }
  }
  ret = waitpid(-1, stat_loc, options);
  return ret;
}

/** @brief waitpid function. */
pid_t waitpid(pid_t pid, int *stat_loc, int options) {
  DWORD wait_res;
  DWORD exit_code;
  int i;
  {
    int rpid = g_wait_reap((int)pid, stat_loc);
    if (rpid > 0) {
      return (pid_t)rpid;
    }
  }

  if (pid <= 0) {
    if (pid != -1) {
      errno = EINVAL;
      return (pid_t)-1;
    }

    /* Wait for any child (-1) */
    for (;;) {
      if (g_posix_child_count == 0) {
        errno = ECHILD;
        return (pid_t)-1;
      }
      for (i = 0; i < g_posix_child_count; i++) {
        if (g_wait_single(g_posix_child_handles[i], 0) == WAIT_OBJECT_0) {
          pid_t ret;
          if (stat_loc != NULL) {
            if (g_wait_exit_code(g_posix_child_handles[i], &exit_code)) {
              *stat_loc = ((int)(exit_code & 0xFF) << 8);
            } else {
              *stat_loc = 0;
            }
          }
          ret = (pid_t)g_posix_child_pids[i];
          g_wait_close(g_posix_child_handles[i]);
          g_posix_child_handles[i] =
              g_posix_child_handles[g_posix_child_count - 1];
          g_posix_child_pids[i] = g_posix_child_pids[g_posix_child_count - 1];
          g_posix_child_count--;
          return ret;
        }
      }
      if (options & WNOHANG) {
        return 0;
      }
      Sleep(10);
    }
  }

  {
    int found_idx = -1;
    HANDLE hProcess = NULL;
    for (i = 0; i < g_posix_child_count; i++) {
      if (g_posix_child_pids[i] == (DWORD)pid) {
        found_idx = i;
        hProcess = g_posix_child_handles[i];
        break;
      }
    }

    if (found_idx == -1) {
      hProcess = g_wait_open(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE,
                             (DWORD)pid);
    }

    if (hProcess == NULL) {
      errno = ECHILD;
      return (pid_t)-1;
    }

    wait_res = g_wait_single(hProcess, (options & WNOHANG) ? 0 : INFINITE);
    if (wait_res == WAIT_TIMEOUT) {
      if (found_idx == -1)
        g_wait_close(hProcess);
      return 0;
    } else if (wait_res == WAIT_OBJECT_0) {
      if (stat_loc != NULL) {
        if (g_wait_exit_code(hProcess, &exit_code)) {
          *stat_loc = ((int)(exit_code & 0xFF) << 8);
        } else {
          *stat_loc = 0;
        }
      }
      if (found_idx != -1) {
        g_wait_close(g_posix_child_handles[found_idx]);
        g_posix_child_handles[found_idx] =
            g_posix_child_handles[g_posix_child_count - 1];
        g_posix_child_pids[found_idx] =
            g_posix_child_pids[g_posix_child_count - 1];
        g_posix_child_count--;
      } else {
        g_wait_close(hProcess);
      }
      return pid;
    }

    if (found_idx == -1)
      g_wait_close(hProcess);
    errno = EINVAL;
    return (pid_t)-1;
  }
}

/** @brief waitid function. */
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
  int status = 0;
  pid_t res;
  pid_t pid_to_wait = -1;

  if (infop != NULL) {
    infop->si_pid = 0;
    infop->si_status = 0;
    infop->si_signo = 17; /* SIGCHLD */
    infop->si_code = 0;
    infop->si_uid = 0;
  }

  if (idtype == P_PID) {
    pid_to_wait = (pid_t)id;
  } else if (idtype == P_ALL) {
    pid_to_wait = (pid_t)-1;
  } else {
    errno = EINVAL;
    return -1;
  }

  res = waitpid(pid_to_wait, &status, options);
  if (res > 0) {
    if (infop != NULL) {
      infop->si_pid = (int)res;
      infop->si_status = WEXITSTATUS(status);
      infop->si_code = 1; /* CLD_EXITED */
    }
    return 0;
  } else if (res == 0) {
    return 0;
  }

  return -1;
}

/** @brief posix_wait_kill function. */
int posix_wait_kill(pid_t pid, int sig) {
  HANDLE hProcess;
  if (pid <= 0) {
    errno = EINVAL;
    return -1;
  }
  if (sig == 0) {
    hProcess = g_wait_open(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
    if (hProcess) {
      g_wait_close(hProcess);
      return 0;
    }
    errno = ESRCH;
    return -1;
  }
  hProcess = g_wait_open(PROCESS_TERMINATE, FALSE, (DWORD)pid);
  if (!hProcess) {
    errno = ESRCH;
    return -1;
  }
  if (!g_wait_terminate(hProcess, (UINT)sig)) {
    g_wait_close(hProcess);
    errno = EPERM;
    return -1;
  }
  g_wait_close(hProcess);
  return 0;
}

#endif /* _WIN32 && !DOS */

#if defined(__MSDOS__) || defined(__WATCOMC__)

extern int awm_vfork_try_reap(int pid, int *stat_loc);

/** @brief Wait for a child process to stop or terminate on DOS. */
pid_t wait(int *stat_loc) {
  int rpid = awm_vfork_try_reap(-1, stat_loc);
  if (rpid > 0) {
    return (pid_t)rpid;
  }
  errno = ECHILD;
  return -1;
}

/** @brief Wait for a specific child process to stop or terminate on DOS. */
pid_t waitpid(pid_t pid, int *stat_loc, int options) {
  int rpid;
  (void)options;
  rpid = awm_vfork_try_reap((int)pid, stat_loc);
  if (rpid > 0) {
    return (pid_t)rpid;
  }
  errno = ECHILD;
  return -1;
}

/** @brief Wait for a child process to change state on DOS. */
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
  if (idtype || id || infop || options) {
  }
  errno = EINVAL;
  return -1;
}

/** @brief Wait for process termination with resource usage on DOS. */
pid_t wait3(int *stat_loc, int options, struct rusage *rusage) {
  if (stat_loc || options || rusage) {
  }
  errno = ECHILD;
  return -1;
}

/** @brief Send a signal to a process on DOS. */
int posix_wait_kill(pid_t pid, int sig) {
  if (pid <= 0 || sig < 0) {
    errno = EINVAL;
    return -1;
  }
  if (sig == 0 && pid == (pid_t)getpid()) {
    return 0;
  }
  errno = ESRCH;
  return -1;
}

#endif /* MSDOS || WATCOMC */

/** @brief Non-standard Microsoft-compatible cwait. */
intptr_t posix_cwait(int *termstat, intptr_t pid, int action) {
  (void)action;
  return waitpid((pid_t)pid, termstat, 0);
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

#if !defined(_WIN32) && !defined(__MSDOS__) && !defined(__WATCOMC__)
int posix_wait_kill(pid_t pid, int sig) { return kill(pid, sig); }
#endif

/**
 * @brief Retrieves information on posix-wait polyfill availability.
 */
auto_win_msvc_error_t posix_wait_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_wait;

#endif
typedef int make_iso_compilers_happy_mega_batch3_26;
