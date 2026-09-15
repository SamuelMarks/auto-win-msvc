#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "macos-malloc.h"
#include <stdio.h>
/* clang-format on */

TEST test_macos_malloc_init(void) {
  enum macos_malloc_error_code rc;
  int status;

  status = 0;
  rc = macos_malloc_init(NULL);
  if (rc != MACOS_MALLOC_ERROR_NULL_POINTER) {
    printf("Expected MACOS_MALLOC_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = macos_malloc_init(&status);
  if (rc != MACOS_MALLOC_SUCCESS) {
    printf("macos_malloc_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_macos_malloc_good_size(void) {
  enum macos_malloc_error_code rc;
  size_t good_sz;

  good_sz = 0;
  rc = macos_malloc_good_size(24, NULL);
  if (rc != MACOS_MALLOC_ERROR_NULL_POINTER) {
    printf("Expected MACOS_MALLOC_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = macos_malloc_good_size(24, &good_sz);
  if (rc != MACOS_MALLOC_SUCCESS) {
    printf("macos_malloc_good_size failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(32, good_sz);
  PASS();
}

SUITE(suite_macos_malloc_core) {
  RUN_TEST(test_macos_malloc_init);
  RUN_TEST(test_macos_malloc_good_size);
}
