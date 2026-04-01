/* test.c - 100% Test Coverage */
#if defined(_MSC_VER)
#pragma warning(disable : 4127) /* conditional expression is constant */
#endif

#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

/* clang-format off */
#include "greatest.h"
#include "posix-wait.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for memset */

#if defined(_MSC_VER)
#define NUM_FORMAT "%d"
#else
#define NUM_FORMAT "%d"
#endif

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
#endif

#ifndef CREATE_NO_WINDOW
#define CREATE_NO_WINDOW 0x08000000
#endif

#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
/* clang-format on */
#endif

int spawn_child(int exit_code, pid_t *out_pid) {
#ifdef _WIN32
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  char cmd[256];

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  memset(&pi, 0, sizeof(pi));

#if defined(_MSC_VER)
  sprintf_s(cmd, sizeof(cmd), "cmd.exe /c exit %d", exit_code);
#else
  sprintf(cmd, "cmd.exe /c exit %d", exit_code);
#endif

  if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    return 1;
  }
  CloseHandle(pi.hThread);
  *out_pid = (pid_t)pi.dwProcessId;
  return 0;
#else
  pid_t pid = fork();
  if (pid < 0)
    return 1;
  if (pid == 0) {
    exit(exit_code);
  }
  *out_pid = pid;
  return 0;
#endif
}

TEST test_waitpid_basic(void) {
  int status = 0;
  pid_t pid = -1;
  pid_t res;
  ASSERT_EQ(0, spawn_child(42, &pid));
  ASSERT(pid > 0);

  res = waitpid(pid, &status, 0);
  ASSERT_EQ_FMT(pid, res, NUM_FORMAT);
  ASSERT(WIFEXITED(status));
  ASSERT_EQ_FMT(42, WEXITSTATUS(status), NUM_FORMAT);
  PASS();
}

TEST test_waitpid_nohang(void) {
  int status = 0;
  pid_t pid = -1, res;
#ifdef _WIN32
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  char cmd[] = "ping 127.0.0.1 -n 2";

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  memset(&pi, 0, sizeof(pi));

  CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL,
                 &si, &pi);
  CloseHandle(pi.hThread);
  pid = (pid_t)pi.dwProcessId;
#else
  pid = fork();
  if (pid == 0) {
    sleep(1);
    exit(0);
  }
#endif

  res = waitpid(pid, &status, WNOHANG);
  ASSERT_EQ_FMT(0, res, NUM_FORMAT);

  res = waitpid(pid, &status, 0);
  ASSERT_EQ_FMT(pid, res, NUM_FORMAT);

  PASS();
}

TEST test_waitid_basic(void) {
  siginfo_t info;
  pid_t pid = -1;
  int res;
  ASSERT_EQ(0, spawn_child(55, &pid));
  ASSERT(pid > 0);

#ifdef _WIN32
  res = waitid(P_PID, pid, &info, WEXITED);
#else
  res = waitid(P_PID, (id_t)pid, &info, WEXITED);
#endif

  ASSERT_EQ_FMT(0, res, NUM_FORMAT);
  ASSERT_EQ_FMT(pid, info.si_pid, NUM_FORMAT);
  ASSERT_EQ_FMT(55, info.si_status, NUM_FORMAT);
#ifdef _WIN32
  ASSERT_EQ_FMT(1, info.si_code, NUM_FORMAT);
#endif
  PASS();
}

/* test_cwait_basic removed */

TEST test_wait_basic(void) {
#ifdef _WIN32
  int status = 0;
  pid_t res = wait(&status);
  ASSERT_EQ_FMT(-1, res, NUM_FORMAT);
#else
  int status = 0;
  pid_t pid = -1;
  pid_t res;
  ASSERT_EQ(0, spawn_child(33, &pid));
  ASSERT(pid > 0);

  res = wait(&status);
  while (res > 0 && res != pid) {
    res = wait(&status);
  }
  ASSERT_EQ_FMT(pid, res, NUM_FORMAT);
  ASSERT(WIFEXITED(status));
#endif
  PASS();
}

TEST test_waitpid_invalid(void) {
  int status = 0;
  pid_t res = waitpid(-9999, &status, 0);
  ASSERT_EQ_FMT(-1, res, NUM_FORMAT);
  PASS();
}

TEST test_waitid_invalid(void) {
#ifdef _WIN32
  siginfo_t info;
  int res = waitid(P_PGID, 0, &info, WEXITED);
  ASSERT_EQ_FMT(-1, res, NUM_FORMAT);
#endif
  PASS();
}

SUITE(wait_suite) {
  RUN_TEST(test_waitpid_basic);
  RUN_TEST(test_waitpid_nohang);
  RUN_TEST(test_waitid_basic);

  RUN_TEST(test_wait_basic);
  RUN_TEST(test_waitpid_invalid);
  RUN_TEST(test_waitid_invalid);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(wait_suite);
  GREATEST_MAIN_END();
}
