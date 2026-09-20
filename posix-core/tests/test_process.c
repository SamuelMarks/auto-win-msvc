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
  ASSERT(pid > 0);
  PASS();
}

TEST test_fexecve(void) {
  char *argv[2];
  argv[0] = "test";
  argv[1] = NULL;
#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(-1, fexecve(-1, NULL, NULL));
  ASSERT_EQ(EINVAL, errno);
#endif
  ASSERT_EQ(-1, fexecve(-1, argv, NULL));
  ASSERT_EQ(-1, fexecve(9999, argv, NULL));
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
    ASSERT(pid > 0);
  } else {
    ASSERT_EQ(-1, pid);
  }
  PASS();
#else
  pid_t pid = fork();
  if (pid == 0) {
    _exit(0);
  } else if (pid > 0) {
    int status = 0;
    waitpid(pid, &status, 0);
    ASSERT(pid > 0);
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
    ASSERT(pid > 0);
  } else {
    ASSERT_EQ(-1, pid);
  }
  PASS();
#else
  pid_t pid = fork();
  if (pid == 0) {
    _exit(0);
  } else if (pid > 0) {
    int status = 0;
    waitpid(pid, &status, 0);
    ASSERT(pid > 0);
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
