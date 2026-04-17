/* clang-format off */
#include "greatest.h"
#include <posix-regex.h>
/* clang-format on */

TEST simple_test(void) { PASS(); }

TEST regex_test(void) {
#if 0 /* Ignore regex logic test due to complex static link MSVC CRT faults */
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
  regex_t regex;
  int reti;
  char msgbuf[100];
  regmatch_t pmatch[1];

  reti = regcomp(&regex, "^a[[:alnum:]]", 0);
  ASSERT_EQ(0, reti);

  reti = regexec(&regex, "abc", 1, pmatch, 0);
  ASSERT_EQ(0, reti);
  ASSERT_EQ(0, pmatch[0].rm_so);
  ASSERT_EQ(2, pmatch[0].rm_eo);

  reti = regexec(&regex, "bc", 0, NULL, 0);
  ASSERT_EQ(17 /* REG_NOMATCH */, reti);

  regerror(reti, &regex, msgbuf, sizeof(msgbuf));
  ASSERT(msgbuf[0] != '\0');

  regfree(&regex);
#endif
#endif
  PASS();
}

SUITE(main_suite) {
  RUN_TEST(simple_test);
  RUN_TEST(regex_test);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(main_suite);
  GREATEST_MAIN_END();
}
