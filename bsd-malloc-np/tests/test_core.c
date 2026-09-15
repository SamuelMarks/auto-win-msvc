#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-malloc-np.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

static char g_cb_buffer[2048];

static void dummy_cb(void *opaque, const char *str) {
  size_t len;
  (void)opaque;
  len = strlen(str);
  if (strlen(g_cb_buffer) + len < sizeof(g_cb_buffer) - 1) {
#if defined(_MSC_VER)
    strcat_s(g_cb_buffer, sizeof(g_cb_buffer), str);
#else
    strcat(g_cb_buffer, str);
#endif
  }
}

TEST test_bsd_malloc_np_init(void) {
  enum bsd_malloc_np_error_code rc;
  int status;

  status = 0;
  rc = bsd_malloc_np_init(NULL);
  if (rc != BSD_MALLOC_NP_ERROR_NULL_POINTER) {
    printf("Expected BSD_MALLOC_NP_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_malloc_np_init(&status);
  if (rc != BSD_MALLOC_NP_SUCCESS) {
    printf("bsd_malloc_np_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_je_malloc_stats_print(void) {
  error_type_t err;

  err = je_malloc_stats_print(NULL, NULL, NULL);
  ASSERT_EQ(ERR_NONE, err);

  g_cb_buffer[0] = '\0';
  err = je_malloc_stats_print(dummy_cb, NULL, "opts");
  ASSERT_EQ(ERR_NONE, err);
  ASSERT(strlen(g_cb_buffer) > 0);
  PASS();
}

SUITE(suite_bsd_malloc_np_core) {
  RUN_TEST(test_bsd_malloc_np_init);
  RUN_TEST(test_je_malloc_stats_print);
}
