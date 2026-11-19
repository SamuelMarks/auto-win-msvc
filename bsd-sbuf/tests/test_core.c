/* clang-format off */
#include "greatest.h"
#include <sys/sbuf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

extern void *(*g_bsd_sbuf_calloc)(size_t, size_t);
extern void *(*g_bsd_sbuf_malloc)(size_t);
extern void *(*g_bsd_sbuf_realloc)(void *, size_t);
extern int (*g_bsd_sbuf_vsnprintf)(char *, size_t, const char *, va_list);
extern void bsd_sbuf_reset_hooks(void);

static int g_mock_sbuf_oom = 0;
static int g_mock_vsnprintf_fail = 0;
static int g_drain_ret = 0;
static size_t g_drained_bytes = 0;

static void *mock_sbuf_calloc(size_t nmemb, size_t size) {
  if (g_mock_sbuf_oom) {
    return NULL;
  }
  return calloc(nmemb, size);
}

static void *mock_sbuf_malloc(size_t size) {
  if (g_mock_sbuf_oom) {
    return NULL;
  }
  return malloc(size);
}

static void *mock_sbuf_realloc(void *ptr, size_t size) {
  if (g_mock_sbuf_oom) {
    return NULL;
  }
  return realloc(ptr, size);
}

static int mock_sbuf_vsnprintf(char *buf, size_t size, const char *fmt,
                               va_list ap) {
  (void)buf;
  (void)size;
  (void)fmt;
  (void)ap;
  if (g_mock_vsnprintf_fail) {
    return -1;
  }
  return 0;
}

static int test_drain_cb(void *arg, const char *data, int len) {
  int *calls = (int *)arg;
  (void)data;
  if (calls != NULL) {
    (*calls)++;
  }
  if (g_drain_ret < 0) {
    return -1;
  }
  g_drained_bytes += (size_t)len;
  return len;
}

TEST test_bsd_sbuf_ops_and_branches(void) {
  struct sbuf *sb;
  struct sbuf stack_sb;
  char fixed[8];
  char long_str[1100];
  unsigned char raw[3] = {0xaa, 0xbb, 0x01};

  /* NULL argument handling across all APIs */
  sbuf_clear(NULL);
  ASSERT_EQ(-1, sbuf_setpos(NULL, 0));
  ASSERT_EQ(-1, sbuf_bcat(NULL, "a", 1));
  ASSERT_EQ(-1, sbuf_cat(NULL, NULL));
  ASSERT_EQ(-1, sbuf_printf(NULL, "%d", 1));
  ASSERT_EQ(-1, sbuf_nl_terminate(NULL));
  ASSERT_EQ(-1, sbuf_trim(NULL));
  ASSERT_EQ(EINVAL, sbuf_error(NULL));
  ASSERT_EQ(-1, sbuf_finish(NULL));
  ASSERT(sbuf_data(NULL) == NULL);
  ASSERT_EQ(-1, (int)sbuf_len(NULL));
  ASSERT_EQ(0, sbuf_done(NULL));
  sbuf_delete(NULL);
  sbuf_set_drain(NULL, test_drain_cb, NULL);
  sbuf_hexdump(NULL, raw, 3, "hdr", 0);

  /* Basic auto-extending sbuf */
  sb = sbuf_new_auto();
  ASSERT(sb != NULL);
  ASSERT_EQ(0, sbuf_done(sb));
  ASSERT_EQ(0, sbuf_error(sb));
  ASSERT_EQ(-1, sbuf_bcat(sb, NULL, 1));
  ASSERT_EQ(-1, sbuf_printf(sb, NULL));

  /* nl_terminate on empty -> adds '\n', then second call is no-op */
  ASSERT_EQ(0, sbuf_nl_terminate(sb));
  ASSERT_STR_EQ("\n", sbuf_data(sb));
  ASSERT_EQ(0, sbuf_nl_terminate(sb));
  ASSERT_STR_EQ("\n", sbuf_data(sb));

  /* trim all-whitespace down to 0 length */
  ASSERT_EQ(0, sbuf_cpy(sb, "   \t\n"));
  ASSERT_EQ(0, sbuf_trim(sb));
  ASSERT_EQ(0, (int)sbuf_len(sb));

  /* trim trailing whitespace preserving prefix + nl_terminate on non-'\n' */
  ASSERT_EQ(0, sbuf_cpy(sb, "hello  \t"));
  ASSERT_EQ(0, sbuf_trim(sb));
  ASSERT_STR_EQ("hello", sbuf_data(sb));
  ASSERT_EQ(0, sbuf_nl_terminate(sb));
  ASSERT_STR_EQ("hello\n", sbuf_data(sb));

  /* setpos valid and out-of-range */
  ASSERT_EQ(-1, sbuf_setpos(sb, -1));
  ASSERT_EQ(-1, sbuf_setpos(sb, 99));
  ASSERT_EQ(0, sbuf_setpos(sb, 5));
  ASSERT_STR_EQ("hello", sbuf_data(sb));

  /* bcpy and printf with >1024 truncation and multi-doubling autoextend */
  ASSERT_EQ(0, sbuf_bcpy(sb, "xy", 2));
  ASSERT_STR_EQ("xy", sbuf_data(sb));
  memset(long_str, 'A', sizeof(long_str) - 1);
  long_str[sizeof(long_str) - 1] = '\0';
  ASSERT_EQ(0, sbuf_printf(sb, "%s", long_str));
  ASSERT_EQ(2 + 1023, (int)sbuf_len(sb));

  /* hexdump with NULL ptr, non-NULL hdr, NULL hdr, and 0 length */
  sbuf_clear(sb);
  sbuf_hexdump(sb, NULL, 3, "hdr", 0);
  sbuf_hexdump(sb, raw, 2, "HEX: ", 0);
  sbuf_hexdump(sb, raw, 0, NULL, 0);
  ASSERT_EQ(0, sbuf_finish(sb));
  ASSERT(sbuf_done(sb) != 0);
  ASSERT_STR_EQ("HEX: aa bb \n\n", sbuf_data(sb));
  sbuf_delete(sb);

  /* Stack sbuf with fixed buffer: overflow sets s_error and blocks further
   * writes and sbuf_finish */
  ASSERT(sbuf_new(&stack_sb, fixed, (int)sizeof(fixed), SBUF_FIXEDLEN) ==
         &stack_sb);
  ASSERT_EQ(0, sbuf_cat(&stack_sb, "1234567"));
  ASSERT_EQ(-1, sbuf_putc(&stack_sb, '8'));
  ASSERT_EQ(ENOMEM, sbuf_error(&stack_sb));
  /* Subsequent extend immediately returns -1 because s_error != 0 */
  ASSERT_EQ(-1, sbuf_putc(&stack_sb, '9'));
  ASSERT_EQ(-1, sbuf_finish(&stack_sb));
  sbuf_delete(&stack_sb);

  /* Stack sbuf with 0-length fixed buffer (exercises s_size == 0 branches) */
  ASSERT(sbuf_new(&stack_sb, fixed, 0, SBUF_FIXEDLEN) == &stack_sb);
  sbuf_clear(&stack_sb);
  sbuf_delete(&stack_sb);

  /* Stack sbuf with dynamic small buffer (length > 0) + manual s_buf == NULL */
  ASSERT(sbuf_new(&stack_sb, NULL, 4, SBUF_AUTOEXTEND) == &stack_sb);
  ASSERT_EQ(0, sbuf_cat(&stack_sb, "abcdef"));
  sbuf_delete(&stack_sb);

  memset(&stack_sb, 0, sizeof(stack_sb));
  stack_sb.s_flags = SBUF_AUTOEXTEND | SBUF_DYNAMIC;
  sbuf_clear(&stack_sb);
  /* s_size == 0 && s_buf == NULL -> newsize defaults to 64 without loop */
  ASSERT_EQ(0, sbuf_cat(&stack_sb, "ok"));
  ASSERT_STR_EQ("ok", sbuf_data(&stack_sb));
  free(stack_sb.s_buf);
  stack_sb.s_buf = NULL;
  /* sbuf_delete when SBUF_DYNAMIC is set but s_buf is NULL */
  sbuf_delete(&stack_sb);

  PASS();
}

TEST test_bsd_sbuf_drain_and_oom(void) {
  struct sbuf stack_sb;
  struct sbuf *sb;
  char fixed[8];
  int drain_calls = 0;

  /* 1. Drain succeeds and freed space fits incoming write */
  g_drain_ret = 0;
  g_drained_bytes = 0;
  sbuf_new(&stack_sb, fixed, (int)sizeof(fixed), SBUF_FIXEDLEN);
  sbuf_set_drain(&stack_sb, test_drain_cb, &drain_calls);
  /* Initial write when s_len == 0 that exceeds fixed buffer -> skips drain */
  ASSERT_EQ(-1, sbuf_cat(&stack_sb, "123456789"));
  sbuf_clear(&stack_sb);
  ASSERT_EQ(0, sbuf_cat(&stack_sb, "12345"));
  /* Adding "abc" (5 + 3 + 1 > 8) triggers drain of "12345", then "abc" fits */
  ASSERT_EQ(0, sbuf_cat(&stack_sb, "abc"));
  ASSERT_EQ(1, drain_calls);
  ASSERT_EQ(5, (int)g_drained_bytes);
  /* Adding 10-byte string triggers drain of "abc", still > 8 bytes -> fails */
  ASSERT_EQ(-1, sbuf_cat(&stack_sb, "0123456789"));
  ASSERT_EQ(2, drain_calls);

  /* 2. Drain returns < 0 during sbuf_extend */
  sbuf_clear(&stack_sb);
  ASSERT_EQ(0, sbuf_cat(&stack_sb, "12345"));
  g_drain_ret = -1;
  ASSERT_EQ(-1, sbuf_cat(&stack_sb, "6789"));
  ASSERT_EQ(ENOMEM, sbuf_error(&stack_sb));

  /* 3. Drain during sbuf_finish: success, s_len == 0 skip, and error (< 0) */
  sbuf_clear(&stack_sb);
  g_drain_ret = 0;
  /* s_len == 0 with drain attached -> skips calling drain */
  ASSERT_EQ(0, sbuf_finish(&stack_sb));
  sbuf_clear(&stack_sb);
  ASSERT_EQ(0, sbuf_cat(&stack_sb, "xyz"));
  ASSERT_EQ(0, sbuf_finish(&stack_sb));
  ASSERT_EQ(0, (int)sbuf_len(&stack_sb));

  sbuf_clear(&stack_sb);
  ASSERT_EQ(0, sbuf_cat(&stack_sb, "xyz"));
  g_drain_ret = -1;
  ASSERT_EQ(-1, sbuf_finish(&stack_sb));
  sbuf_delete(&stack_sb);

  /* 4. OOM tests via allocator & vsnprintf hooks */
  g_bsd_sbuf_calloc = mock_sbuf_calloc;
  g_bsd_sbuf_malloc = mock_sbuf_malloc;
  g_bsd_sbuf_realloc = mock_sbuf_realloc;

  /* calloc fails in sbuf_new(NULL, ...) */
  g_mock_sbuf_oom = 1;
  ASSERT(sbuf_new_auto() == NULL);

  /* malloc fails when s == NULL (dynstruct == 1 -> frees s) */
  g_bsd_sbuf_calloc = calloc;
  ASSERT(sbuf_new_auto() == NULL);

  /* malloc fails when s != NULL (dynstruct == 0 -> does not free &stack_sb) */
  ASSERT(sbuf_new(&stack_sb, NULL, 16, SBUF_AUTOEXTEND) == NULL);

  /* realloc fails in sbuf_extend */
  g_mock_sbuf_oom = 0;
  sb = sbuf_new(NULL, NULL, 4, SBUF_AUTOEXTEND);
  ASSERT(sb != NULL);
  g_mock_sbuf_oom = 1;
  ASSERT_EQ(-1, sbuf_cat(sb, "overflow_realloc_test"));
  ASSERT_EQ(ENOMEM, sbuf_error(sb));
  g_mock_sbuf_oom = 0;

  /* vsnprintf returns < 0 in sbuf_vprintf */
  sbuf_clear(sb);
  g_bsd_sbuf_vsnprintf = mock_sbuf_vsnprintf;
  g_mock_vsnprintf_fail = 1;
  ASSERT_EQ(-1, sbuf_printf(sb, "%d", 123));
  ASSERT_EQ(EINVAL, sbuf_error(sb));
  g_mock_vsnprintf_fail = 0;

  bsd_sbuf_reset_hooks();
  sbuf_delete(sb);

  PASS();
}

SUITE(suite_bsd_sbuf_core) {
  RUN_TEST(test_bsd_sbuf_ops_and_branches);
  RUN_TEST(test_bsd_sbuf_drain_and_oom);
}
