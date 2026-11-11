/* clang-format off */
#include "bsd/string.h"
#include "greatest.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
/* clang-format on */

TEST test_bsd_string_strlcpy(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  char dst[10];
  ASSERT_EQ(5U, strlcpy(dst, "hello", sizeof(dst)));
  ASSERT_STR_EQ("hello", dst);
  ASSERT_EQ(11U, strlcpy(dst, "hello world", sizeof(dst)));
  ASSERT_STR_EQ("hello wor", dst);
#endif
  PASS();
}

TEST test_bsd_string_strlcat(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  char dst[10] = "ab";
  ASSERT_EQ(5U, strlcat(dst, "cde", sizeof(dst)));
  ASSERT_STR_EQ("abcde", dst);
#endif
  PASS();
}

TEST test_bsd_string_strnstr_strmode_bzero(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  char modebuf[16];
  char buf[8] = "secret";
  char *ffstr;
  char ffin[] = "nodump,noarch";
  char *ffp = ffin;
  unsigned long setf = 0, clrf = 0;

  ASSERT(strnstr("hello world", "world", 11) != NULL);
  ASSERT(strnstr("hello world", "world", 5) == NULL);

  strmode(S_IFREG | 0755, modebuf);
  ASSERT_STR_EQ("-rwxr-xr-x ", modebuf);

  explicit_bzero(buf, sizeof(buf));
  ASSERT_EQ(0, buf[0]);

  ASSERT_EQ(0, timingsafe_bcmp("abc", "abc", 3));
  ASSERT_NEQ(0, timingsafe_bcmp("abc", "abd", 3));
  ASSERT_EQ(0, timingsafe_memcmp("abc", "abc", 3));
  ASSERT(timingsafe_memcmp("abc", "abd", 3) < 0);

  ffstr = fflagstostr(0x00000001UL | 0x00010000UL);
  ASSERT(ffstr != NULL);
  free(ffstr);

  ASSERT_EQ(0, strtofflags(&ffp, &setf, &clrf));
  ASSERT_EQ(0x00000001UL, setf);
  ASSERT_EQ(0x00010000UL, clrf);
#endif
  PASS();
}

TEST test_bsd_string_strsep(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  char str[] = "a,b,c";
  char *p = str;
  ASSERT_STR_EQ("a", strsep(&p, ","));
#endif
  PASS();
}

TEST test_bsd_string_strcasecmp(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  ASSERT_EQ(0, strcasecmp("abc", "ABC"));
  ASSERT_EQ(0, strncasecmp("abcdef", "ABCxyz", 3));
#endif
  PASS();
}

TEST test_bsd_string_strverscmp(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  ASSERT_EQ(0, strverscmp("1.0", "1.0"));
#endif
  PASS();
}

TEST test_bsd_string_strcasestr(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  ASSERT(strcasestr("Hello World", "world") != NULL);
#endif
  PASS();
}

TEST test_bsd_string_dummy(void) {
  ASSERT_EQ(0, bsd_string_dummy_for_coverage());
  PASS();
}

SUITE(suite_bsd_string_core) {
  RUN_TEST(test_bsd_string_strlcpy);
  RUN_TEST(test_bsd_string_strlcat);
  RUN_TEST(test_bsd_string_strnstr_strmode_bzero);
  RUN_TEST(test_bsd_string_strsep);
  RUN_TEST(test_bsd_string_strcasecmp);
  RUN_TEST(test_bsd_string_strverscmp);
  RUN_TEST(test_bsd_string_strcasestr);
  RUN_TEST(test_bsd_string_dummy);
}
