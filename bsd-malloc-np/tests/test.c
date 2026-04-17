/* clang-format off */
#include "bsd-malloc-np.h"
#include "greatest.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

static char g_cb_buffer[4096] = {0};

static void test_write_cb(void *opaque, const char *str) {
  (void)opaque;
  if (str) {
#if defined(_MSC_VER)
    strcat_s(g_cb_buffer, sizeof(g_cb_buffer), str);
#else
    strncat(g_cb_buffer, str, sizeof(g_cb_buffer) - strlen(g_cb_buffer) - 1);
#endif
  }
}

TEST test_je_malloc_stats_print(void) {
#if defined(_WIN32) || defined(_WIN64)
  g_cb_buffer[0] = '\0';
  je_malloc_stats_print(test_write_cb, NULL, NULL);

  /* Verify our callback was invoked and has Windows stats */
  ASSERT(strlen(g_cb_buffer) > 0);
  ASSERT(strstr(g_cb_buffer, "Begin Windows Native Memory Stats") != NULL);
  ASSERT(strstr(g_cb_buffer, "WorkingSetSize") != NULL);
  ASSERT(strstr(g_cb_buffer, "TotalPhys") != NULL);
  ASSERT(strstr(g_cb_buffer, "End Windows Native Memory Stats") != NULL);

  /* Verify it safely ignores NULL callback */
  je_malloc_stats_print(NULL, NULL, NULL);
#endif
  PASS();
}

extern int dummy_bsd_malloc_np(void);

TEST test_dummy(void) {
  dummy_bsd_malloc_np();
  PASS();
}

SUITE(suite) {
  RUN_TEST(test_je_malloc_stats_print);
  RUN_TEST(test_dummy);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}
