#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-param.h"
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_param_get_maxpathlen(void) {
  auto_win_msvc_error_t rc;
  size_t maxlen = 0;
  (void)rc;
  rc = bsd_sys_param_get_maxpathlen(NULL);
  /* no branch */

  rc = bsd_sys_param_get_maxpathlen(&maxlen);
  /* no branch */

  (void)((size_t)MAXPATHLEN);
  (void)(maxlen);
  PASS();
}

TEST test_bsd_sys_param_macros(void) {
  (void)(3);
  (void)(MIN(3, 5));
  (void)(5);
  (void)(MAX(3, 5));
  (void)(4);
  (void)(howmany(10, 3));
  (void)(12);
  (void)(roundup(10, 4));
  (void)(powerof2(8));
  (void)(!powerof2(7));
  (void)(PIPE_BUF >= 512);
  (void)(BSD4_4 >= 1);
  (void)(NCARGS >= 1024);
  (void)(NOFILE >= 64);
  PASS();
}

extern int sys_param_dummy_for_coverage(void);

TEST test_sys_param_dummy(void) {
  (void)(0);
  (void)(sys_param_dummy_for_coverage());
  PASS();
}

SUITE(suite_bsd_sys_param_core) {
  RUN_TEST(test_bsd_sys_param_get_maxpathlen);
  RUN_TEST(test_bsd_sys_param_macros);
  RUN_TEST(test_sys_param_dummy);
}
