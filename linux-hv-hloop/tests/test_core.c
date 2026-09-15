#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-hv-hloop.h"
#include "hv/hloop.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_hv_hloop_init(void) {
  enum linux_hv_hloop_error_code rc;
  int status;

  status = 0;
  rc = linux_hv_hloop_init(NULL);
  if (rc != LINUX_HV_HLOOP_ERROR_NULL_POINTER) {
    printf("Expected LINUX_HV_HLOOP_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_hv_hloop_init(&status);
  if (rc != LINUX_HV_HLOOP_SUCCESS) {
    printf("linux_hv_hloop_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

SUITE(suite_linux_hv_hloop_core) { RUN_TEST(test_linux_hv_hloop_init); }
