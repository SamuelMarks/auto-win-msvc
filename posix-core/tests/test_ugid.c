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
#endif
/* clang-format on */

TEST test_getegid(void) {
  gid_t egid = getegid();
  (void)egid;

  PASS();
}

TEST test_geteuid(void) {
  uid_t euid = geteuid();
  (void)euid;

  PASS();
}

TEST test_getgid(void) {
  gid_t gid = getgid();
  (void)gid;

  PASS();
}

TEST test_getpgid(void) {
  pid_t pgid = getpgid(0);
  (void)pgid;

  PASS();
}

TEST test_getuid(void) {
  uid_t uid = getuid();
  (void)uid;

  PASS();
}

TEST test_setegid(void) {
  int rc;
  gid_t egid = getegid();
  (void)egid;
  rc = setegid(egid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }

  PASS();
}

TEST test_seteuid(void) {
  int rc;
  uid_t euid = geteuid();
  (void)euid;
  rc = seteuid(euid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }

  PASS();
}

TEST test_setgid(void) {
  int rc;
  gid_t gid = getgid();
  (void)gid;
  rc = setgid(gid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }

  PASS();
}

TEST test_setpgid(void) {
  int rc;
  rc = setpgid(0, 0);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }

  PASS();
}

TEST test_setregid(void) {
  int rc;
  gid_t gid = getgid();
  (void)gid;
  rc = setregid(gid, gid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }

  PASS();
}

TEST test_setreuid(void) {
  int rc;
  uid_t uid = getuid();
  (void)uid;
  rc = setreuid(uid, uid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }

  PASS();
}

TEST test_setuid(void) {
  int rc;
  uid_t uid = getuid();
  (void)uid;
  rc = setuid(uid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }

  PASS();
}

SUITE(suite_posix_core_ugid) {
  RUN_TEST(test_getegid);
  RUN_TEST(test_geteuid);
  RUN_TEST(test_getgid);
  RUN_TEST(test_getpgid);
  RUN_TEST(test_getuid);
  RUN_TEST(test_setegid);
  RUN_TEST(test_seteuid);
  RUN_TEST(test_setgid);
  RUN_TEST(test_setpgid);
  RUN_TEST(test_setregid);
  RUN_TEST(test_setreuid);
  RUN_TEST(test_setuid);
}
