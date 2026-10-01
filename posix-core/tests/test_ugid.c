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
  ASSERT(egid >= 0);
  PASS();
}

TEST test_geteuid(void) {
  uid_t euid = geteuid();
  ASSERT(euid >= 0);
  PASS();
}

TEST test_getgid(void) {
  gid_t gid = getgid();
  ASSERT(gid >= 0);
  PASS();
}

TEST test_getpgid(void) {
  pid_t pgid = getpgid(0);
  ASSERT(pgid >= 0);
  PASS();
}

TEST test_getuid(void) {
  uid_t uid = getuid();
  ASSERT(uid >= 0);
  PASS();
}

TEST test_setegid(void) {
  gid_t egid = getegid();
  int rc = setegid(egid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }
  ASSERT_EQ(0, rc);
  PASS();
}

TEST test_seteuid(void) {
  uid_t euid = geteuid();
  int rc = seteuid(euid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }
  ASSERT_EQ(0, rc);
  PASS();
}

TEST test_setgid(void) {
  gid_t gid = getgid();
  int rc = setgid(gid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }
  ASSERT_EQ(0, rc);
  PASS();
}

TEST test_setpgid(void) {
  int rc = setpgid(0, 0);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }
  ASSERT(rc == 0 || rc == -1);
  PASS();
}

TEST test_setregid(void) {
  gid_t gid = getgid();
  int rc = setregid(gid, gid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }
  ASSERT_EQ(0, rc);
  PASS();
}

TEST test_setreuid(void) {
  uid_t uid = getuid();
  int rc = setreuid(uid, uid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }
  ASSERT_EQ(0, rc);
  PASS();
}

TEST test_setuid(void) {
  uid_t uid = getuid();
  int rc = setuid(uid);
  if (rc == -1 && (errno == EPERM || errno == EACCES)) {
    PASS();
  }
  ASSERT_EQ(0, rc);
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
