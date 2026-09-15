#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-langinfo.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_langinfo_get_info(void) {
  enum posix_langinfo_error_code rc;
  int info;

  info = 0;
  rc = posix_langinfo_get_info(NULL);
  if (rc != POSIX_LANGINFO_ERROR_NULL_POINTER) {
    printf("Expected POSIX_LANGINFO_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_langinfo_get_info(&info);
  if (rc != POSIX_LANGINFO_SUCCESS) {
    printf("posix_langinfo_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_langinfo_items(void) {
  char *str;

  /* First call tests initial population */
  str = posix_langinfo(CODESET);
  ASSERT(str != NULL);
  ASSERT(strlen(str) > 0);

  /* Second call tests cached path */
  str = posix_langinfo(CODESET);
  ASSERT(str != NULL);
  ASSERT(strlen(str) > 0);

  str = posix_langinfo(D_T_FMT);
  ASSERT_STR_EQ("%a %b %e %H:%M:%S %Y", str);

  str = posix_langinfo(D_FMT);
  ASSERT_STR_EQ("%m/%d/%y", str);

  str = posix_langinfo(T_FMT);
  ASSERT_STR_EQ("%H:%M:%S", str);

  str = posix_langinfo(T_FMT_AMPM);
  ASSERT_STR_EQ("%I:%M:%S %p", str);

  str = posix_langinfo(AM_STR);
  ASSERT_STR_EQ("AM", str);

  str = posix_langinfo(PM_STR);
  ASSERT_STR_EQ("PM", str);

  str = posix_langinfo(DAY_1);
  ASSERT_STR_EQ("Sunday", str);
  str = posix_langinfo(DAY_2);
  ASSERT_STR_EQ("Monday", str);
  str = posix_langinfo(DAY_3);
  ASSERT_STR_EQ("Tuesday", str);
  str = posix_langinfo(DAY_4);
  ASSERT_STR_EQ("Wednesday", str);
  str = posix_langinfo(DAY_5);
  ASSERT_STR_EQ("Thursday", str);
  str = posix_langinfo(DAY_6);
  ASSERT_STR_EQ("Friday", str);
  str = posix_langinfo(DAY_7);
  ASSERT_STR_EQ("Saturday", str);

  str = posix_langinfo(ABDAY_1);
  ASSERT_STR_EQ("Sun", str);
  str = posix_langinfo(ABDAY_2);
  ASSERT_STR_EQ("Mon", str);
  str = posix_langinfo(ABDAY_3);
  ASSERT_STR_EQ("Tue", str);
  str = posix_langinfo(ABDAY_4);
  ASSERT_STR_EQ("Wed", str);
  str = posix_langinfo(ABDAY_5);
  ASSERT_STR_EQ("Thu", str);
  str = posix_langinfo(ABDAY_6);
  ASSERT_STR_EQ("Fri", str);
  str = posix_langinfo(ABDAY_7);
  ASSERT_STR_EQ("Sat", str);

  str = posix_langinfo(MON_1);
  ASSERT_STR_EQ("January", str);
  str = posix_langinfo(MON_2);
  ASSERT_STR_EQ("February", str);
  str = posix_langinfo(MON_3);
  ASSERT_STR_EQ("March", str);
  str = posix_langinfo(MON_4);
  ASSERT_STR_EQ("April", str);
  str = posix_langinfo(MON_5);
  ASSERT_STR_EQ("May", str);
  str = posix_langinfo(MON_6);
  ASSERT_STR_EQ("June", str);
  str = posix_langinfo(MON_7);
  ASSERT_STR_EQ("July", str);
  str = posix_langinfo(MON_8);
  ASSERT_STR_EQ("August", str);
  str = posix_langinfo(MON_9);
  ASSERT_STR_EQ("September", str);
  str = posix_langinfo(MON_10);
  ASSERT_STR_EQ("October", str);
  str = posix_langinfo(MON_11);
  ASSERT_STR_EQ("November", str);
  str = posix_langinfo(MON_12);
  ASSERT_STR_EQ("December", str);

  str = posix_langinfo(ABMON_1);
  ASSERT_STR_EQ("Jan", str);
  str = posix_langinfo(ABMON_2);
  ASSERT_STR_EQ("Feb", str);
  str = posix_langinfo(ABMON_3);
  ASSERT_STR_EQ("Mar", str);
  str = posix_langinfo(ABMON_4);
  ASSERT_STR_EQ("Apr", str);
  str = posix_langinfo(ABMON_5);
  ASSERT_STR_EQ("May", str);
  str = posix_langinfo(ABMON_6);
  ASSERT_STR_EQ("Jun", str);
  str = posix_langinfo(ABMON_7);
  ASSERT_STR_EQ("Jul", str);
  str = posix_langinfo(ABMON_8);
  ASSERT_STR_EQ("Aug", str);
  str = posix_langinfo(ABMON_9);
  ASSERT_STR_EQ("Sep", str);
  str = posix_langinfo(ABMON_10);
  ASSERT_STR_EQ("Oct", str);
  str = posix_langinfo(ABMON_11);
  ASSERT_STR_EQ("Nov", str);
  str = posix_langinfo(ABMON_12);
  ASSERT_STR_EQ("Dec", str);

  str = posix_langinfo(ERA);
  ASSERT_STR_EQ("", str);

  str = posix_langinfo(ERA_D_FMT);
  ASSERT_STR_EQ("", str);

  str = posix_langinfo(ERA_D_T_FMT);
  ASSERT_STR_EQ("", str);

  str = posix_langinfo(ERA_T_FMT);
  ASSERT_STR_EQ("", str);

  str = posix_langinfo(ALT_DIGITS);
  ASSERT_STR_EQ("", str);

  str = posix_langinfo(RADIXCHAR);
  ASSERT_STR_EQ(".", str);

  str = posix_langinfo(THOUSEP);
  ASSERT_STR_EQ("", str);

  str = posix_langinfo(YESEXPR);
  ASSERT_STR_EQ("^[yY]", str);

  str = posix_langinfo(NOEXPR);
  ASSERT_STR_EQ("^[nN]", str);

  str = posix_langinfo(CRNCYSTR);
  ASSERT_STR_EQ("-", str);

  /* Test default invalid item */
  str = posix_langinfo((nl_item)9999);
  ASSERT_STR_EQ("", str);

  /* Test nl_langinfo macro or function */
  str = nl_langinfo(CODESET);
  ASSERT(str != NULL);

  PASS();
}

SUITE(suite_posix_langinfo_core) {
  RUN_TEST(test_posix_langinfo_get_info);
  RUN_TEST(test_posix_langinfo_items);
}
