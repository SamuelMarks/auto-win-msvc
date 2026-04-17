/* test.c for posix-langinfo */
/* clang-format off */
#include "langinfo.h"
#include "greatest.h"
#include <string.h>
/* clang-format on */

extern int dummy_posix_langinfo(void);

TEST test_nl_langinfo_codeset(void) {
#if defined(POSIX_LANGINFO_MSVC) || defined(_WIN32)
  char *res = nl_langinfo(CODESET);
  ASSERT(res != NULL);
  ASSERT(strlen(res) > 0);
#endif
  PASS();
}

TEST test_nl_langinfo_days(void) {
#if defined(POSIX_LANGINFO_MSVC) || defined(_WIN32)
  ASSERT_STR_EQ("Sunday", nl_langinfo(DAY_1));
  ASSERT_STR_EQ("Monday", nl_langinfo(DAY_2));
  ASSERT_STR_EQ("Tuesday", nl_langinfo(DAY_3));
  ASSERT_STR_EQ("Wednesday", nl_langinfo(DAY_4));
  ASSERT_STR_EQ("Thursday", nl_langinfo(DAY_5));
  ASSERT_STR_EQ("Friday", nl_langinfo(DAY_6));
  ASSERT_STR_EQ("Saturday", nl_langinfo(DAY_7));

  ASSERT_STR_EQ("Sun", nl_langinfo(ABDAY_1));
  ASSERT_STR_EQ("Mon", nl_langinfo(ABDAY_2));
  ASSERT_STR_EQ("Tue", nl_langinfo(ABDAY_3));
  ASSERT_STR_EQ("Wed", nl_langinfo(ABDAY_4));
  ASSERT_STR_EQ("Thu", nl_langinfo(ABDAY_5));
  ASSERT_STR_EQ("Fri", nl_langinfo(ABDAY_6));
  ASSERT_STR_EQ("Sat", nl_langinfo(ABDAY_7));
#endif
  PASS();
}

TEST test_nl_langinfo_months(void) {
#if defined(POSIX_LANGINFO_MSVC) || defined(_WIN32)
  ASSERT_STR_EQ("January", nl_langinfo(MON_1));
  ASSERT_STR_EQ("February", nl_langinfo(MON_2));
  ASSERT_STR_EQ("March", nl_langinfo(MON_3));
  ASSERT_STR_EQ("April", nl_langinfo(MON_4));
  ASSERT_STR_EQ("May", nl_langinfo(MON_5));
  ASSERT_STR_EQ("June", nl_langinfo(MON_6));
  ASSERT_STR_EQ("July", nl_langinfo(MON_7));
  ASSERT_STR_EQ("August", nl_langinfo(MON_8));
  ASSERT_STR_EQ("September", nl_langinfo(MON_9));
  ASSERT_STR_EQ("October", nl_langinfo(MON_10));
  ASSERT_STR_EQ("November", nl_langinfo(MON_11));
  ASSERT_STR_EQ("December", nl_langinfo(MON_12));

  ASSERT_STR_EQ("Jan", nl_langinfo(ABMON_1));
  ASSERT_STR_EQ("Feb", nl_langinfo(ABMON_2));
  ASSERT_STR_EQ("Mar", nl_langinfo(ABMON_3));
  ASSERT_STR_EQ("Apr", nl_langinfo(ABMON_4));
  ASSERT_STR_EQ("May", nl_langinfo(ABMON_5));
  ASSERT_STR_EQ("Jun", nl_langinfo(ABMON_6));
  ASSERT_STR_EQ("Jul", nl_langinfo(ABMON_7));
  ASSERT_STR_EQ("Aug", nl_langinfo(ABMON_8));
  ASSERT_STR_EQ("Sep", nl_langinfo(ABMON_9));
  ASSERT_STR_EQ("Oct", nl_langinfo(ABMON_10));
  ASSERT_STR_EQ("Nov", nl_langinfo(ABMON_11));
  ASSERT_STR_EQ("Dec", nl_langinfo(ABMON_12));
#endif
  PASS();
}

TEST test_nl_langinfo_misc(void) {
#if defined(POSIX_LANGINFO_MSVC) || defined(_WIN32)
  ASSERT_STR_EQ("%a %b %e %H:%M:%S %Y", nl_langinfo(D_T_FMT));
  ASSERT_STR_EQ("%m/%d/%y", nl_langinfo(D_FMT));
  ASSERT_STR_EQ("%H:%M:%S", nl_langinfo(T_FMT));
  ASSERT_STR_EQ("%I:%M:%S %p", nl_langinfo(T_FMT_AMPM));
  ASSERT_STR_EQ("AM", nl_langinfo(AM_STR));
  ASSERT_STR_EQ("PM", nl_langinfo(PM_STR));
  ASSERT_STR_EQ("", nl_langinfo(ERA));
  ASSERT_STR_EQ("", nl_langinfo(ERA_D_FMT));
  ASSERT_STR_EQ("", nl_langinfo(ERA_D_T_FMT));
  ASSERT_STR_EQ("", nl_langinfo(ERA_T_FMT));
  ASSERT_STR_EQ("", nl_langinfo(ALT_DIGITS));
  ASSERT_STR_EQ(".", nl_langinfo(RADIXCHAR));
  ASSERT_STR_EQ("", nl_langinfo(THOUSEP));
  ASSERT_STR_EQ("^[yY]", nl_langinfo(YESEXPR));
  ASSERT_STR_EQ("^[nN]", nl_langinfo(NOEXPR));
  ASSERT_STR_EQ("-", nl_langinfo(CRNCYSTR));

  /* invalid or unsupported item */
  ASSERT_STR_EQ("", nl_langinfo(9999));
#endif
  PASS();
}

TEST test_dummy(void) {
  dummy_posix_langinfo();
  PASS();
}

SUITE(suite) {
  RUN_TEST(test_nl_langinfo_codeset);
  RUN_TEST(test_nl_langinfo_days);
  RUN_TEST(test_nl_langinfo_months);
  RUN_TEST(test_nl_langinfo_misc);
  RUN_TEST(test_dummy);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}
