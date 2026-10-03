#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-stdatomic.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_stdatomic_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_stdatomic_get_info(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_stdatomic_get_info(&info);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_stdatomic_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_stdatomic_operations(void) {
  atomic_int a;
  long old_val;

  atomic_init(&a, 10);
  ASSERT_EQ(10, atomic_load(&a));

  atomic_store(&a, 20);
  ASSERT_EQ(20, atomic_load(&a));

  old_val = atomic_fetch_add(&a, 5);
  ASSERT_EQ(20, old_val);
  ASSERT_EQ(25, atomic_load(&a));

  old_val = atomic_fetch_sub(&a, 3);
  ASSERT_EQ(25, old_val);
  ASSERT_EQ(22, atomic_load(&a));

  old_val = atomic_exchange(&a, 100);
  ASSERT_EQ(22, old_val);
  ASSERT_EQ(100, atomic_load(&a));

  atomic_thread_fence(memory_order_seq_cst);

  PASS();
}

SUITE(suite_posix_stdatomic_core) {
  RUN_TEST(test_posix_stdatomic_get_info);
  RUN_TEST(test_stdatomic_operations);
}
