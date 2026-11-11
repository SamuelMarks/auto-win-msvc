#ifndef SYS_PTRACE_H
#define SYS_PTRACE_H

/**
 * @file ptrace.h
 * @brief FreeBSD process trace (<sys/ptrace.h>) definitions.
 */

/* clang-format off */
#include <sys/types.h>
#include <signal.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define PT_TRACE_ME 0
#define PT_READ_I 1
#define PT_READ_D 2
#define PT_WRITE_I 4
#define PT_WRITE_D 5
#define PT_CONTINUE 7
#define PT_KILL 8
#define PT_STEP 9
#define PT_ATTACH 10
#define PT_DETACH 11
#define PT_IO 12
#define PT_LWPINFO 13
#define PT_GETNUMLWPS 14
#define PT_GETLWPLIST 15
#define PT_CLEARSTEP 16
#define PT_SETSTEP 17
#define PT_SUSPEND 18
#define PT_RESUME 19
#define PT_TO_SCE 20
#define PT_TO_SCX 21
#define PT_SYSCALL 22
#define PT_FOLLOW_FORK 23
#define PT_LWP_EVENTS 24
#define PT_GET_EVENT_MASK 25
#define PT_SET_EVENT_MASK 26
#define PT_GET_SC_ARGS 27
#define PT_GET_SC_RET 28
#define PT_COREDUMP 29
#define PT_GETREGS 33
#define PT_SETREGS 34
#define PT_GETFPREGS 35
#define PT_SETFPREGS 36
#define PT_GETDBREGS 37
#define PT_SETDBREGS 38

/** @brief Lightweight process trace information structure. */
struct ptrace_lwpinfo {
  lwpid_t pl_lwpid;
  int pl_event;
  int pl_flags;
  sigset_t pl_sigmask;
  sigset_t pl_siglist;
  char pl_tdname[20];
  pid_t pl_child_pid;
  unsigned int pl_syscall_code;
  unsigned int pl_syscall_narg;
};

/** @brief Process trace control request. */
int ptrace(int request, pid_t pid, caddr_t addr, int data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_PTRACE_H */
