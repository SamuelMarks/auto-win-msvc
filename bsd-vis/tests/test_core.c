/* clang-format off */
#include "greatest.h"
#include <vis.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

extern void *(*g_bsd_vis_malloc)(size_t);
extern void bsd_vis_reset_malloc_hook(void);

static void *mock_vis_malloc_oom(size_t size) {
  (void)size;
  return NULL;
}

TEST test_bsd_vis_char_and_flags(void) {
  char out[16];

  /* snvis invalid arguments */
  ASSERT(snvis(NULL, sizeof(out), 'a', 0, 0, NULL) == NULL);
  ASSERT(snvis(out, 4, 'a', 0, 0, NULL) == NULL);

  /* extra == NULL, extra != NULL match, extra != NULL non-match */
  ASSERT(snvis(out, sizeof(out), 'a', 0, 0, NULL) != NULL);
  ASSERT_STR_EQ("a", out);
  ASSERT(snvis(out, sizeof(out), 'a', 0, 0, "xyz") != NULL);
  ASSERT_STR_EQ("a", out);
  ASSERT(snvis(out, sizeof(out), 'x', 0, 0, "xyz") != NULL);
  ASSERT_STR_EQ("\\170", out);

  /* High-bit non-ASCII (>= 0x80) */
  ASSERT(snvis(out, sizeof(out), 0x80, 0, 0, "") != NULL);
  ASSERT_STR_EQ("\\200", out);

  /* Whitespace rules: ' ', '\t', '\n' with VIS_SAFE / VIS_SP / VIS_TAB / VIS_NL
   */
  ASSERT(vis(out, ' ', 0, 0) != NULL);
  ASSERT_STR_EQ(" ", out);
  ASSERT(vis(out, ' ', VIS_SP | VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\s", out);

  ASSERT(vis(out, '\t', VIS_SAFE, 0) != NULL);
  ASSERT_STR_EQ("\t", out);
  ASSERT(vis(out, '\t', 0, 0) != NULL);
  ASSERT_STR_EQ("\\011", out);
  ASSERT(vis(out, '\t', VIS_TAB | VIS_SAFE | VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\t", out);

  ASSERT(vis(out, '\n', VIS_SAFE, 0) != NULL);
  ASSERT_STR_EQ("\n", out);
  ASSERT(vis(out, '\n', 0, 0) != NULL);
  ASSERT_STR_EQ("\\012", out);
  ASSERT(vis(out, '\n', VIS_NL | VIS_SAFE | VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\n", out);

  /* Backslash with and without VIS_NOSLASH */
  ASSERT(vis(out, '\\', 0, 0) != NULL);
  ASSERT_STR_EQ("\\\\", out);
  ASSERT(vis(out, '\\', VIS_NOSLASH, 0) != NULL);
  ASSERT_STR_EQ("\\", out);

  /* All VIS_CSTYLE switch cases + default fallthrough */
  ASSERT(vis(out, '\r', VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\r", out);
  ASSERT(vis(out, '\b', VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\b", out);
  ASSERT(vis(out, '\a', VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\a", out);
  ASSERT(vis(out, '\v', VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\v", out);
  ASSERT(vis(out, '\f', VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\f", out);
  ASSERT(vis(out, '\0', VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\0", out);
  ASSERT(vis(out, 0x01, VIS_CSTYLE, 0) != NULL);
  ASSERT_STR_EQ("\\001", out);

  /* svis and nvis wrappers */
  ASSERT(svis(out, '!', 0, 0, "!") != NULL);
  ASSERT_STR_EQ("\\041", out);
  ASSERT(nvis(out, sizeof(out), 'A', 0, 0) != NULL);
  ASSERT_STR_EQ("A", out);

  PASS();
}

TEST test_bsd_vis_strings_unvis_and_oom(void) {
  char enc[64];
  char dec[64];
  char *alloc_enc = NULL;
  int cerr = -1;
  int st = 0;
  char ch = 0;

  /* strsnvisx NULL / zero-dlen / NULL extra / overflow */
  ASSERT_EQ(-1, strsnvisx(NULL, sizeof(enc), "a", 1, 0, ""));
  ASSERT_EQ(-1, strsnvisx(enc, 0, "a", 1, 0, ""));
  ASSERT_EQ(2, strsnvisx(enc, sizeof(enc), "ab", 2, 0, NULL));
  ASSERT_STR_EQ("ab", enc);
  ASSERT_EQ(-1, strsnvisx(enc, 2, "ab", 2, 0, ""));
  ASSERT_STR_EQ("a", enc);

  /* Wrapper variants including NULL src and NULL/non-NULL cerr_ptr */
  ASSERT_EQ(5, strsvisx(enc, "a!", 2, 0, "!"));
  ASSERT_STR_EQ("a\\041", enc);
  ASSERT_EQ(2, strnvisx(enc, sizeof(enc), "ab", 2, 0));
  ASSERT_EQ(2, strvisx(enc, "ab", 2, 0));

  ASSERT_EQ(4, strsenvisx(enc, sizeof(enc), "a b", 3, VIS_SP | VIS_CSTYLE, "",
                          &cerr));
  ASSERT_STR_EQ("a\\sb", enc);
  ASSERT_EQ(0, cerr);
  ASSERT_EQ(2, strsenvisx(enc, sizeof(enc), "ab", 2, 0, "", NULL));

  cerr = -1;
  ASSERT_EQ(2, strenvisx(enc, sizeof(enc), "ab", 2, 0, &cerr));
  ASSERT_EQ(0, cerr);
  ASSERT_EQ(2, strenvisx(enc, sizeof(enc), "ab", 2, 0, NULL));

  ASSERT_EQ(0, strsnvis(enc, sizeof(enc), NULL, 0, ""));
  ASSERT_EQ(2, strsnvis(enc, sizeof(enc), "ab", 0, ""));
  ASSERT_EQ(0, strsvis(enc, NULL, 0, ""));
  ASSERT_EQ(2, strsvis(enc, "ab", 0, ""));
  ASSERT_EQ(0, strnvis(enc, NULL, sizeof(enc), 0));
  ASSERT_EQ(2, strnvis(enc, "ab", sizeof(enc), 0));
  ASSERT_EQ(0, strvis(enc, NULL, 0));
  ASSERT_EQ(4, strvis(enc, "a\nb", VIS_CSTYLE));
  ASSERT_STR_EQ("a\\nb", enc);

  /* stravis: NULL outp, NULL src, valid src, and OOM */
  ASSERT_EQ(-1, stravis(NULL, "abc", 0));
  ASSERT_EQ(0, stravis(&alloc_enc, NULL, 0));
  ASSERT(alloc_enc != NULL);
  free(alloc_enc);

  ASSERT(stravis(&alloc_enc, "hello\tworld", VIS_CSTYLE | VIS_TAB) > 0);
  ASSERT(alloc_enc != NULL);
  free(alloc_enc);

  g_bsd_vis_malloc = mock_vis_malloc_oom;
  alloc_enc = enc;
  ASSERT_EQ(-1, stravis(&alloc_enc, "oom", 0));
  ASSERT(alloc_enc == NULL);
  bsd_vis_reset_malloc_hook();

  /* unvis direct calls: NULL args, bad flag, UNVIS_END, and all escapes */
  ASSERT_EQ(UNVIS_SYNBAD, unvis(NULL, 'a', &st, 0));
  ASSERT_EQ(UNVIS_SYNBAD, unvis(&ch, 'a', NULL, 0));
  ASSERT_EQ(UNVIS_SYNBAD, unvis(&ch, 'a', &st, -1));
  st = 0;
  ASSERT_EQ(UNVIS_NOCHAR, unvis(&ch, 0, &st, UNVIS_END));
  st = 1;
  ASSERT_EQ(UNVIS_SYNBAD, unvis(&ch, 0, &st, UNVIS_END));

  /* strnunvisx / strunvisx / strnunvis / strunvis */
  ASSERT_EQ(-1, strnunvisx(NULL, sizeof(dec), "a", 0));
  ASSERT_EQ(-1, strnunvisx(dec, sizeof(dec), NULL, 0));
  ASSERT_EQ(-1, strnunvisx(dec, 0, "a", 0));
  /* Bad flag triggers r < 0 inside loop */
  ASSERT_EQ(-1, strnunvisx(dec, sizeof(dec), "a", -1));
  /* Destination buffer too small */
  ASSERT_EQ(-1, strnunvisx(dec, 2, "ab", 0));
  /* Dangling trailing backslash fails UNVIS_END check */
  ASSERT_EQ(-1, strunvis(dec, "abc\\"));

  /* All escape sequences: \n, \r, \t, \s, \\, and default (\x) */
  ASSERT_EQ(6, strunvis(dec, "\\n\\r\\t\\s\\\\\\x"));
  ASSERT_EQ('\n', dec[0]);
  ASSERT_EQ('\r', dec[1]);
  ASSERT_EQ('\t', dec[2]);
  ASSERT_EQ(' ', dec[3]);
  ASSERT_EQ('\\', dec[4]);
  ASSERT_EQ('x', dec[5]);

  ASSERT_EQ(-1, strunvisx(dec, NULL, 0));
  ASSERT_EQ(2, strnunvis(dec, "ok", sizeof(dec)));
  ASSERT_STR_EQ("ok", dec);

  PASS();
}

SUITE(suite_bsd_vis_core) {
  RUN_TEST(test_bsd_vis_char_and_flags);
  RUN_TEST(test_bsd_vis_strings_unvis_and_oom);
}
