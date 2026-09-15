#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "vm/vm_param.h"
#include <stdio.h>
/* clang-format on */

TEST test_bsd_vm_param(void) {
  enum bsd_vm_param_error_code rc;
  int info;

  info = 0;
  rc = bsd_vm_param_get_host_vm_info(NULL);
  if (rc != BSD_VM_PARAM_ERROR_NULL_POINTER) {
    printf("Expected BSD_VM_PARAM_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_vm_param_get_host_vm_info(&info);
  if (rc != BSD_VM_PARAM_SUCCESS) {
    printf("bsd_vm_param_get_host_vm_info failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(HOST_VM_INFO, info);
  PASS();
}

SUITE(suite_bsd_vm_param_core) { RUN_TEST(test_bsd_vm_param); }
