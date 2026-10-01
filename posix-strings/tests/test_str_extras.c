#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-strings.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_stpcpy_and_stpncpy(void) {
  char buf[32];
  char *ret;

  ret = stpcpy(buf, "hello");
  ASSERT_EQ(buf + 5, ret);
  ASSERT_EQ('\0', *ret);
  ASSERT_STR_EQ("hello", buf);

  memset(buf, 'X', sizeof(buf));
  ret = stpncpy(buf, "hi", 5);
  ASSERT_EQ(buf + 2, ret);
  ASSERT_EQ('\0', buf[2]);
  ASSERT_EQ('\0', buf[3]);
  ASSERT_EQ('\0', buf[4]);

  ret = stpncpy(buf, "toolongstring", 4);
  ASSERT_EQ(buf + 4, ret);
  ASSERT_EQ('t', buf[0]);
  ASSERT_EQ('o', buf[1]);
  ASSERT_EQ('o', buf[2]);
  ASSERT_EQ('l', buf[3]);

  PASS();
}

TEST test_strchrnul_and_memops(void) {
  char str[] = "abcde";
  char buf[16];
  void *mret;

  ASSERT_EQ(str + 2, strchrnul(str, 'c'));
  ASSERT_EQ(str + 5, strchrnul(str, 'z'));

  mret = mempcpy(buf, "1234", 4);
  ASSERT_EQ((void *)(buf + 4), mret);
  buf[4] = '\0';
  ASSERT_STR_EQ("1234", buf);

  ASSERT_EQ((void *)(str + 3), memrchr(str, 'd', 5));
  ASSERT_EQ(NULL, memrchr(str, 'z', 5));

  PASS();
}

TEST test_strsignal(void) {
  char *s;
  s = strsignal(9);
  ASSERT(s != NULL);
  ASSERT_STR_EQ("Killed", s);

  s = strsignal(999);
  ASSERT(s != NULL);
  ASSERT(strstr(s, "999") != NULL);

  PASS();
}

SUITE(suite_posix_strings_extras) {
  RUN_TEST(test_stpcpy_and_stpncpy);
  RUN_TEST(test_strchrnul_and_memops);
  RUN_TEST(test_strsignal);
}
