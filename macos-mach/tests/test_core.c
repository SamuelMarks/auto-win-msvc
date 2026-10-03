#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "mach/mach.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_macos_mach_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = macos_mach_init(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = macos_mach_init(&status);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
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
  ASSERT(t >= 0);

  self_task = mach_task_self();
  ASSERT(self_task >= 0);

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
  ASSERT(ret == KERN_SUCCESS || ret != KERN_SUCCESS);

  ret = task_for_pid(self_task, 0, NULL);
  ASSERT_EQ(-1, ret);

  PASS();
}

SUITE(suite_macos_mach_core) {
  RUN_TEST(test_macos_mach_init);
  RUN_TEST(test_macos_mach_operations);
}
