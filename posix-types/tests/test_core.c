#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-types.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_types_get_info(void) {
  enum posix_types_error_code rc;
  int info;

  info = 0;
  rc = posix_types_get_info(NULL);
  if (rc != POSIX_TYPES_ERROR_NULL_POINTER) {
    printf("Expected POSIX_TYPES_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_types_get_info(&info);
  if (rc != POSIX_TYPES_SUCCESS) {
    printf("posix_types_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_types_definitions(void) {
  pid_t p;
  ssize_t s;
  mode_t m;
  off_t o;
  uid_t u;
  gid_t g;
  useconds_t us;
  suseconds_t sus;
  dev_t d;
  ino_t i;
  nlink_t nl;
  clock_t clk;
  time_t tm;
  fsblkcnt_t fb;
  fsfilcnt_t ff;
  blksize_t bs;
  blkcnt_t bc;
#if defined(_WIN32) || defined(_OFF64_T_DEFINED)
  off64_t o64;
#endif
#if defined(_WIN32) || defined(_TIMER_T_DEFINED)
  timer_t tr;
#endif
#if defined(_WIN32) || defined(_UCHAR_T_DEFINED)
  uchar uc;
#endif

  p = 100;
  s = 200;
  m = 0755;
  o = 300;
  u = 10;
  g = 20;
  us = 50;
  sus = 60;
  d = 1;
  i = 2;
  nl = 3;
  clk = 10;
  tm = 1000;
  fb = 1024;
  ff = 2048;
  bs = 4096;
  bc = 8;

  ASSERT_EQ(100, (int)p);
  ASSERT_EQ(200, (int)s);
  ASSERT_EQ(0755, (int)m);
  ASSERT_EQ(300, (int)o);
  ASSERT_EQ(10, (int)u);
  ASSERT_EQ(20, (int)g);
  ASSERT_EQ(50, (unsigned int)us);
  ASSERT_EQ(60, (long)sus);
  ASSERT_EQ(1, (unsigned int)d);
  ASSERT_EQ(2, (int)i);
  ASSERT_EQ(3, (int)nl);
  ASSERT_EQ(10L, (long)clk);
  ASSERT_EQ(1000L, (long)tm);
  ASSERT(fb > 0);
  ASSERT(ff > 0);
  ASSERT_EQ(4096L, (long)bs);
  ASSERT_EQ(8L, (long)bc);

#if defined(_WIN32) || defined(_OFF64_T_DEFINED)
  o64 = 400;
  ASSERT_EQ(400, (int)o64);
#endif
#if defined(_WIN32) || defined(_TIMER_T_DEFINED)
  tr = NULL;
  ASSERT_EQ(NULL, tr);
#endif
#if defined(_WIN32) || defined(_UCHAR_T_DEFINED)
  uc = (uchar)0xAA;
  ASSERT_EQ(0xAA, (int)uc);
#endif

  PASS();
}

SUITE(suite_posix_types_core) {
  RUN_TEST(test_posix_types_get_info);
  RUN_TEST(test_types_definitions);
}
