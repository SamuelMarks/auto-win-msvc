#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-strings.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_strings_get_info(void) {
  enum posix_strings_error_code rc;
  int info;

  info = 0;
  rc = posix_strings_get_info(NULL);
  if (rc != POSIX_STRINGS_ERROR_NULL_POINTER) {
    printf("Expected POSIX_STRINGS_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_strings_get_info(&info);
  if (rc != POSIX_STRINGS_SUCCESS) {
    printf("posix_strings_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_strings_case(void) {
  ASSERT_EQ(0, strcasecmp("hello", "HELLO"));
  ASSERT(strcasecmp("apple", "banana") < 0);
  ASSERT(strcasecmp("banana", "apple") > 0);

  ASSERT_EQ(0, strncasecmp("hello world", "HELLO there", 5));
  ASSERT(strncasecmp("abc", "abd", 3) < 0);
  PASS();
}

TEST test_strings_bops(void) {
  char buf1[16];
  char buf2[16];

  memset(buf1, 'A', sizeof(buf1));
  bzero(buf1, sizeof(buf1));
  ASSERT_EQ(0, buf1[0]);
  ASSERT_EQ(0, buf1[15]);

#if defined(_MSC_VER)
  strcpy_s(buf1, sizeof(buf1), "test1234");
#else
  strcpy(buf1, "test1234");
#endif
  bcopy(buf1, buf2, 9);
  ASSERT_STR_EQ("test1234", buf2);

  ASSERT_EQ(0, bcmp(buf1, buf2, 9));
  buf2[0] = 'x';
  ASSERT(bcmp(buf1, buf2, 9) != 0);

  PASS();
}

TEST test_strtok_r(void) {
  char str[32];
  char *saveptr;
  char *token;

  saveptr = NULL;
#if defined(_MSC_VER)
  strcpy_s(str, sizeof(str), "apple,banana,cherry");
#else
  strcpy(str, "apple,banana,cherry");
#endif

  token = strtok_r(str, ",", &saveptr);
  ASSERT_STR_EQ("apple", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_STR_EQ("banana", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_STR_EQ("cherry", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_EQ(NULL, token);

  PASS();
}

TEST test_ffs(void) {
  ASSERT_EQ(0, ffs(0));
  ASSERT_EQ(1, ffs(1));
  ASSERT_EQ(2, ffs(2));
  ASSERT_EQ(1, ffs(3));
  ASSERT_EQ(3, ffs(4));
  ASSERT_EQ(5, ffs(16));
  ASSERT_EQ(32, ffs((int)0x80000000));
  PASS();
}

TEST test_ffsl(void) {
  ASSERT_EQ(0, ffsl(0));
  ASSERT_EQ(1, ffsl(1));
  ASSERT_EQ(2, ffsl(2));
  ASSERT_EQ(4, ffsl(8));
  ASSERT_EQ(5, ffsl(16));
  PASS();
}

TEST test_ffsll(void) {
  posix_strings_llong val;
  ASSERT_EQ(0, ffsll(0));
  ASSERT_EQ(1, ffsll(1));
  ASSERT_EQ(2, ffsll(2));
  ASSERT_EQ(9, ffsll(256));

  /* Upper 32 bits test */
  val = (posix_strings_llong)1 << 40;
  ASSERT_EQ(41, ffsll(val));

  PASS();
}

SUITE(suite_posix_strings_core) {
  RUN_TEST(test_posix_strings_get_info);
  RUN_TEST(test_strings_case);
  RUN_TEST(test_strings_bops);
  RUN_TEST(test_strtok_r);
  RUN_TEST(test_ffs);
  RUN_TEST(test_ffsl);
  RUN_TEST(test_ffsll);
}
