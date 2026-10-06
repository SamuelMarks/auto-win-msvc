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

  ret = posix_stpcpy(buf, "hello");
  ASSERT_EQ(buf + 5, ret);
  ASSERT_EQ('\0', *ret);
  ASSERT_STR_EQ("hello", buf);

  memset(buf, 'X', sizeof(buf));
  ret = posix_stpncpy(buf, "hi", 5);
  ASSERT_EQ(buf + 2, ret);
  ASSERT_EQ('\0', buf[2]);
  ASSERT_EQ('\0', buf[3]);
  ASSERT_EQ('\0', buf[4]);

  ret = posix_stpncpy(buf, "toolongstring", 4);
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

  ASSERT_EQ(str + 2, posix_strchrnul(str, 'c'));
  ASSERT_EQ(str + 5, posix_strchrnul(str, 'z'));

  mret = posix_mempcpy(buf, "1234", 4);
  ASSERT_EQ((void *)(buf + 4), mret);
  buf[4] = '\0';
  ASSERT_STR_EQ("1234", buf);

  ASSERT_EQ((void *)(str + 3), posix_memrchr(str, 'd', 5));
  ASSERT_EQ(NULL, posix_memrchr(str, 'z', 5));

  PASS();
}

TEST test_strsignal(void) {
  char *s;
  s = posix_strsignal(1);
  ASSERT_STR_EQ("Hangup", s);
  s = posix_strsignal(2);
  ASSERT_STR_EQ("Interrupt", s);
  s = posix_strsignal(3);
  ASSERT_STR_EQ("Quit", s);
  s = posix_strsignal(4);
  ASSERT_STR_EQ("Illegal instruction", s);
  s = posix_strsignal(6);
  ASSERT_STR_EQ("Aborted", s);
  s = posix_strsignal(8);
  ASSERT_STR_EQ("Floating point exception", s);
  s = posix_strsignal(9);
  ASSERT_STR_EQ("Killed", s);
  s = posix_strsignal(11);
  ASSERT_STR_EQ("Segmentation fault", s);
  s = posix_strsignal(13);
  ASSERT_STR_EQ("Broken pipe", s);
  s = posix_strsignal(14);
  ASSERT_STR_EQ("Alarm clock", s);
  s = posix_strsignal(15);
  ASSERT_STR_EQ("Terminated", s);
  s = posix_strsignal(17);
  ASSERT_STR_EQ("Child exited", s);
  s = posix_strsignal(18);
  ASSERT_STR_EQ("Continued", s);
  s = posix_strsignal(19);
  ASSERT_STR_EQ("Stopped (signal)", s);
  s = posix_strsignal(20);
  ASSERT_STR_EQ("Stopped", s);
  s = posix_strsignal(21);
  ASSERT_STR_EQ("Stopped (tty input)", s);
  s = posix_strsignal(22);
  ASSERT_STR_EQ("Stopped (tty output)", s);
  s = posix_strsignal(28);
  ASSERT_STR_EQ("Window changed", s);

  s = posix_strsignal(999);
  ASSERT(s != NULL);
  ASSERT(strstr(s, "999") != NULL);

  PASS();
}

SUITE(suite_posix_strings_extras) {
  RUN_TEST(test_stpcpy_and_stpncpy);
  RUN_TEST(test_strchrnul_and_memops);
  RUN_TEST(test_strsignal);
}
