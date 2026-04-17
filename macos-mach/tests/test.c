/* test.c - Strict C89 Implementation */

/* clang-format off */
#include "mach/mach.h"
#include "greatest.h"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_mach_absolute_time(void) {
  uint64_t t1 = mach_absolute_time();
  uint64_t t2;
#if defined(_WIN32)
  Sleep(10);
#else
  sleep(1);
#endif
  t2 = mach_absolute_time();
#if defined(_WIN32)
  ASSERT_GT(t2, t1);
#else
  ASSERT(t2 >= t1);
#endif
  PASS();
}

TEST test_mach_task_self(void) {
  task_t t = mach_task_self();
#if defined(_WIN32)
  ASSERT_EQ((task_t)-1, t);
#else
  ASSERT_EQ(0, (int)t);
#endif
  PASS();
}

TEST test_task_info(void) {
  task_t t = mach_task_self();
  struct task_basic_info info;
  mach_msg_type_number_t count = sizeof(info) / sizeof(int);
  kern_return_t kr = task_info(t, TASK_BASIC_INFO, (task_info_t)&info, &count);
#if defined(_WIN32)
  ASSERT_EQ(KERN_SUCCESS, kr);
  /* Process should have some resident size */
  ASSERT(info.resident_size > 0);
#else
  ASSERT_EQ(-1, kr); /* stubbed for POSIX currently */
#endif
  PASS();
}

TEST test_task_for_pid(void) {
  task_t self = mach_task_self();
  mach_port_t port = 0;
  kern_return_t kr;
#if defined(_WIN32)
  kr = task_for_pid(self, (int)GetCurrentProcessId(), &port);
  ASSERT_EQ(KERN_SUCCESS, kr);
  ASSERT(port != 0);
#else
  kr = task_for_pid(self, getpid(), &port);
  ASSERT_EQ(-1, kr);
#endif
  PASS();
}

TEST test_task_for_pid_invalid(void) {
  task_t self = mach_task_self();
  kern_return_t kr;
  kr = task_for_pid(self, -1, NULL);
  ASSERT_EQ(-1, kr);
  PASS();
}

SUITE(mach_suite) {
  RUN_TEST(test_mach_absolute_time);
  RUN_TEST(test_mach_task_self);
  RUN_TEST(test_task_info);
  RUN_TEST(test_task_for_pid);
  RUN_TEST(test_task_for_pid_invalid);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(mach_suite);
  GREATEST_MAIN_END();
}
