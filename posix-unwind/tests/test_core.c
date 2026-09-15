#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-unwind.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_unwind_get_info(void) {
  enum posix_unwind_error_code rc;
  int info;

  info = 0;
  rc = posix_unwind_get_info(NULL);
  if (rc != POSIX_UNWIND_ERROR_NULL_POINTER) {
    printf("Expected POSIX_UNWIND_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_unwind_get_info(&info);
  if (rc != POSIX_UNWIND_SUCCESS) {
    printf("posix_unwind_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test__Unwind_Reason_Code(void) {
  _Unwind_Reason_Code code;

  code = _URC_NO_REASON;
  ASSERT_EQ(0, (int)code);
  code = _URC_NORMAL_STOP;
  ASSERT_EQ(4, (int)code);
  code = _URC_END_OF_STACK;
  ASSERT_EQ(5, (int)code);

  PASS();
}

static _Unwind_Reason_Code sample_trace(struct _Unwind_Context *ctx,
                                        void *arg) {
  int *frame_count;
  size_t ip;

  frame_count = (int *)arg;
  ip = _Unwind_GetIP(ctx);
  (void)ip;

  (*frame_count)++;
  if (*frame_count >= 2) {
    return _URC_NORMAL_STOP;
  }
  return _URC_NO_REASON;
}

TEST test__Unwind_Backtrace(void) {
  int count;
  _Unwind_Reason_Code code;

#if defined(_MSC_VER)
  code = _Unwind_Backtrace(NULL, NULL);
  ASSERT_EQ((int)_URC_FATAL_PHASE1_ERROR, (int)code);
#endif

  count = 0;
  code = _Unwind_Backtrace(sample_trace, &count);
  (void)code;
  ASSERT(count > 0);

  PASS();
}

SUITE(suite_posix_unwind_core) {
  RUN_TEST(test_posix_unwind_get_info);
  RUN_TEST(test__Unwind_Reason_Code);
  RUN_TEST(test__Unwind_Backtrace);
}
