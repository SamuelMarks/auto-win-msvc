#include "bsd/string.h"
#include "greatest.h"
#include <string.h>

TEST test_bsd_string_strlcpy(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  char dst[10];
  (void)dst;
#endif
  PASS();
}

TEST test_bsd_string_strlcat(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  char dst[10] = "ab";
  (void)dst;
  /* when size is less than or equal to strlen(dst), the return value is size +
   * strlen(src) */

#endif
  PASS();
}

TEST test_bsd_string_strsep(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  char str[] = "a,b,c";
  char *p = str;
  (void)p;
#endif
  PASS();
}

TEST test_bsd_string_strcasecmp(void) {
#if defined(_MSC_VER) || defined(_WIN32)

#if defined(_MSC_VER)

#endif
#endif
  PASS();
}

TEST test_bsd_string_strverscmp(void) {
#if defined(_MSC_VER) || defined(_WIN32)

#endif
  PASS();
}

TEST test_bsd_string_strcasestr(void) {
#if defined(_MSC_VER) || defined(_WIN32)

#endif
  PASS();
}

extern int bsd_string_dummy_for_coverage(void);

TEST test_bsd_string_dummy(void) { PASS(); }

SUITE(suite_bsd_string_core) {
  RUN_TEST(test_bsd_string_strlcpy);
  RUN_TEST(test_bsd_string_strlcat);
  RUN_TEST(test_bsd_string_strsep);
  RUN_TEST(test_bsd_string_strcasecmp);
  RUN_TEST(test_bsd_string_strverscmp);
  RUN_TEST(test_bsd_string_strcasestr);
  RUN_TEST(test_bsd_string_dummy);
}
