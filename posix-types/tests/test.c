/* test.c - 100% Test Coverage Stubs */
#define _LARGEFILE64_SOURCE
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE
/* clang-format off */
#include "greatest.h"
#include "posix-types.h"
#include <stddef.h>
#include <stdio.h>

#ifdef _MSC_VER
#include <basetsd.h>
#include <time.h>
/* clang-format on */
#endif

#if defined(__APPLE__) || defined(__CYGWIN__)
#define SKIP_OFF64_T
#ifndef SKIP_TIMER_T
#if defined(__APPLE__)
#define SKIP_TIMER_T
#endif
#endif
#endif

TEST test_pid_t(void) {
  pid_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(int), sizeof(pid_t));
#endif
  PASS();
}

TEST test_ssize_t(void) {
  ssize_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(SSIZE_T), sizeof(ssize_t));
#endif
  PASS();
}

TEST test_mode_t(void) {
  mode_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(unsigned short), sizeof(mode_t));
#endif
  PASS();
}

TEST test_off_t(void) {
  off_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(long), sizeof(off_t));
#endif
  PASS();
}

#ifndef SKIP_OFF64_T
TEST test_off64_t(void) {
  off64_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(__int64), sizeof(off64_t));
#endif
  PASS();
}
#endif

TEST test_uid_t(void) {
  uid_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(int), sizeof(uid_t));
#endif
  PASS();
}

TEST test_gid_t(void) {
  gid_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(int), sizeof(gid_t));
#endif
  PASS();
}

TEST test_useconds_t(void) {
  useconds_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(unsigned int), sizeof(useconds_t));
#endif
  PASS();
}

TEST test_suseconds_t(void) {
  suseconds_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(long), sizeof(suseconds_t));
#endif
  PASS();
}

TEST test_dev_t(void) {
  dev_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(unsigned int), sizeof(dev_t));
#endif
  PASS();
}

TEST test_ino_t(void) {
  ino_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(unsigned short), sizeof(ino_t));
#endif
  PASS();
}

TEST test_nlink_t(void) {
  nlink_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(short), sizeof(nlink_t));
#endif
  PASS();
}

TEST test_socklen_t(void) {
  socklen_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(int), sizeof(socklen_t));
#endif
  PASS();
}

TEST test_sa_family_t(void) {
  sa_family_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(unsigned short), sizeof(sa_family_t));
#endif
  PASS();
}

TEST test_sigset_t(void) {
  sigset_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(unsigned long), sizeof(sigset_t));
#endif
  PASS();
}

TEST test_id_t(void) {
  id_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(int), sizeof(id_t));
#endif
  PASS();
}

TEST test_key_t(void) {
  key_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(int), sizeof(key_t));
#endif
  PASS();
}

TEST test_clock_t(void) {
  clock_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(long), sizeof(clock_t));
#endif
  PASS();
}

TEST test_time_t(void) {
  time_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(__time64_t), sizeof(time_t));
#endif
  PASS();
}

#ifndef SKIP_TIMER_T
TEST test_timer_t(void) {
  timer_t val = (timer_t)0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(void *), sizeof(timer_t));
#endif
  PASS();
}
#endif

TEST test_clockid_t(void) {
  clockid_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(int), sizeof(clockid_t));
#endif
  PASS();
}

TEST test_fsblkcnt_t(void) {
  fsblkcnt_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(unsigned long long), sizeof(fsblkcnt_t));
#endif
  PASS();
}

TEST test_fsfilcnt_t(void) {
  fsfilcnt_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(unsigned long long), sizeof(fsfilcnt_t));
#endif
  PASS();
}

TEST test_blksize_t(void) {
  blksize_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(long), sizeof(blksize_t));
#endif
  PASS();
}

TEST test_blkcnt_t(void) {
  blkcnt_t val = 0;
  ASSERT(sizeof(val) > 0);
#ifdef _MSC_VER
  ASSERT_EQ(sizeof(long), sizeof(blkcnt_t));
#endif
  PASS();
}

SUITE(posix_types_suite) {
  RUN_TEST(test_pid_t);
  RUN_TEST(test_ssize_t);
  RUN_TEST(test_mode_t);
  RUN_TEST(test_off_t);
#ifndef SKIP_OFF64_T
  RUN_TEST(test_off64_t);
#endif
  RUN_TEST(test_uid_t);
  RUN_TEST(test_gid_t);
  RUN_TEST(test_useconds_t);
  RUN_TEST(test_suseconds_t);
  RUN_TEST(test_dev_t);
  RUN_TEST(test_ino_t);
  RUN_TEST(test_nlink_t);
  RUN_TEST(test_socklen_t);
  RUN_TEST(test_sa_family_t);
  RUN_TEST(test_sigset_t);
  RUN_TEST(test_id_t);
  RUN_TEST(test_key_t);
  RUN_TEST(test_clock_t);
  RUN_TEST(test_time_t);
#ifndef SKIP_TIMER_T
  RUN_TEST(test_timer_t);
#endif
  RUN_TEST(test_clockid_t);
  RUN_TEST(test_fsblkcnt_t);
  RUN_TEST(test_fsfilcnt_t);
  RUN_TEST(test_blksize_t);
  RUN_TEST(test_blkcnt_t);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(posix_types_suite);
  GREATEST_MAIN_END();
}
