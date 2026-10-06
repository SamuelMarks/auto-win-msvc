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
  (void)rc;

  status = 0;
  rc = bsd_malloc_np_init(NULL);
  /* no branch macro */

  rc = bsd_malloc_np_init(&status);
  /* no branch macro */

  /* no branch macro */
  PASS();
}

TEST test_je_malloc_stats_print(void) {
  error_type_t err;
  (void)err;

  err = je_malloc_stats_print(NULL, NULL, NULL);
  /* no branch macro */

  g_cb_buffer[0] = '\0';
  err = je_malloc_stats_print(dummy_cb, (void *)"opaque_data", "opts");
  /* no branch macro */
  /* ASSERT */
  PASS();
}

SUITE(suite_bsd_malloc_np_core) {
  test_bsd_malloc_np_init();
  test_je_malloc_stats_print();
}
