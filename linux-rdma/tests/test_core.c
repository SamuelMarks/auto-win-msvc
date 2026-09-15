#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "rdma/rdma_cma.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_linux_rdma_init(void) {
  enum linux_rdma_error_code rc;
  int status;

  status = 0;
  rc = linux_rdma_init(NULL);
  if (rc != LINUX_RDMA_ERROR_NULL_POINTER) {
    printf("Expected LINUX_RDMA_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_rdma_init(&status);
  if (rc != LINUX_RDMA_SUCCESS) {
    printf("linux_rdma_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_rdma_functions(void) {
  int ret;

  ret = rdma_create_event_channel();
  ASSERT_EQ(0, ret);

  ret = rdma_bind_addr(NULL, NULL);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ENOSYS, errno);

  ret = rdma_listen(NULL, 10);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ENOSYS, errno);

  ret = rdma_accept(NULL, NULL);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(ENOSYS, errno);

  PASS();
}

SUITE(suite_linux_rdma_core) {
  RUN_TEST(test_linux_rdma_init);
  RUN_TEST(test_rdma_functions);
}
