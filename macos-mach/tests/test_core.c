#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "mach/mach.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_macos_mach_init(void) {
  enum macos_mach_error_code rc;
  int status;

  status = 0;
  rc = macos_mach_init(NULL);
  if (rc != MACOS_MACH_ERROR_NULL_POINTER) {
    printf("Expected MACOS_MACH_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = macos_mach_init(&status);
  if (rc != MACOS_MACH_SUCCESS) {
    printf("macos_mach_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_macos_mach_operations(void) {
  task_t self_task;
  struct task_basic_info info;
  mach_msg_type_number_t count;
  kern_return_t ret;
  mach_port_t out_port;
  uint64_t t;

  t = mach_absolute_time();
  (void)t;

  self_task = mach_task_self();
  (void)self_task;

  count = sizeof(struct task_basic_info) / sizeof(int);
  ret =
      task_info(self_task, TASK_BASIC_INFO, (task_info_t)(void *)&info, &count);
#if defined(_WIN32)
  ASSERT_EQ(KERN_SUCCESS, ret);
#else
  ASSERT_EQ(-1, ret);
#endif

  out_port = 0;
  ret = task_for_pid(self_task, 0, &out_port);
  (void)ret;

  ret = task_for_pid(self_task, 0, NULL);
  ASSERT_EQ(-1, ret);

  PASS();
}

SUITE(suite_macos_mach_core) {
  RUN_TEST(test_macos_mach_init);
  RUN_TEST(test_macos_mach_operations);
}
