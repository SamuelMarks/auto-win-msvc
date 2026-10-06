#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_MSC_VER) || defined(_WIN32)
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif
/* clang-format on */

TEST test_core_getpid(void) {
  pid_t pid = getpid();
  (void)pid;

  PASS();
}

TEST test_fexecve(void) {
  char *argv[2];
  char *envp[2];
  envp[0] = "PATH=/bin";
  envp[1] = NULL;
  argv[0] = "test";
  argv[1] = NULL;

  /* Hit the error branch */
  fexecve(-1, NULL, NULL);
  fexecve(0, NULL, envp);
  fexecve(0, argv, NULL);

#if defined(_WIN32) || defined(_MSC_VER)
  /* Windows implementation testing */
#else
  /* Hit the ENOSYS branch on macOS */
  fexecve(0, argv, envp);
#endif

  PASS();
}

TEST test_fork(void) {
#if defined(_WIN32) && !defined(BUILD_SHARED_LIBS) &&                          \
    !defined(posix_core_SHARED)
  /* RtlCloneUserProcess requires dynamic CRT */
  PASS();
#elif defined(_WIN32)
  int pid = fork();
  if (pid == 0) {
    _exit(0);
  } else if (pid > 0) {

  } else {
  }
  PASS();
#else
  pid_t pid = fork();
  if (pid == 0) {
    _exit(0);
  } else if (pid > 0) {
    int status = 0;
    waitpid(pid, &status, 0);
  }
  PASS();
#endif
}

TEST test_getppid(void) {
  pid_t ppid = getppid();
  (void)ppid;

  PASS();
}

TEST test_vfork(void) {
#if defined(_WIN32) && !defined(BUILD_SHARED_LIBS) &&                          \
    !defined(posix_core_SHARED)
  /* RtlCloneUserProcess requires dynamic CRT */
  PASS();
#elif defined(_WIN32)
  int pid = vfork();
  if (pid == 0) {
    _exit(0);
  } else if (pid > 0) {

  } else {
  }
  PASS();
#else
  pid_t pid = fork();
  if (pid == 0) {
    _exit(0);
  } else if (pid > 0) {
    int status = 0;
    waitpid(pid, &status, 0);
  }
  PASS();
#endif
}

SUITE(suite_posix_core_process) {
  RUN_TEST(test_core_getpid);
  RUN_TEST(test_fexecve);
  RUN_TEST(test_fork);
  RUN_TEST(test_getppid);
  RUN_TEST(test_vfork);
}
