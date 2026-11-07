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
  (void)opaque;
  (void)str;
}

TEST test_bsd_malloc_np_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = bsd_malloc_np_init(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = bsd_malloc_np_init(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, status);

  PASS();
}

TEST test_je_malloc_stats_print(void) {
  error_type_t err;

  err = je_malloc_stats_print(NULL, NULL, NULL);
  ASSERT_EQ(-1, err);

  g_cb_buffer[0] = '\0';
  err = je_malloc_stats_print(dummy_cb, (void *)"opaque_data", "opts");
  ASSERT_EQ(ERR_NONE, err);
  PASS();
}

SUITE(suite_bsd_malloc_np_core) {
  test_bsd_malloc_np_init();
  test_je_malloc_stats_print();
}
