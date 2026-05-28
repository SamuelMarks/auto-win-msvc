#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <process.h> /* For _getpid if needed */
/* clang-format on */

TEST test__creat(void) { SKIP(); /* Generated stub for _creat */ }

TEST test_fcntl(void) { SKIP(); /* Generated stub for fcntl */ }

TEST test_alarm(void) { SKIP(); /* Generated stub for alarm */ }

TEST test_confstr(void) { SKIP(); /* Generated stub for confstr */ }

TEST test_crypt(void) { SKIP(); /* Generated stub for crypt */ }

TEST test_encrypt(void) { SKIP(); /* Generated stub for encrypt */ }

TEST test_fpathconf(void) { SKIP(); /* Generated stub for fpathconf */ }

TEST test_getgroups(void) { SKIP(); /* Generated stub for getgroups */ }

TEST test_gethostid(void) { SKIP(); /* Generated stub for gethostid */ }

TEST test_gethostname(void) { SKIP(); /* Generated stub for gethostname */ }

TEST test_getlogin(void) { SKIP(); /* Generated stub for getlogin */ }

TEST test_getlogin_r(void) { SKIP(); /* Generated stub for getlogin_r */ }

TEST test_getopt(void) { SKIP(); /* Generated stub for getopt */ }

TEST test_getpgrp(void) { SKIP(); /* Generated stub for getpgrp */ }

TEST test_getsid(void) { SKIP(); /* Generated stub for getsid */ }

TEST test_lockf(void) { SKIP(); /* Generated stub for lockf */ }

TEST test_pathconf(void) { SKIP(); /* Generated stub for pathconf */ }

TEST test_pause(void) { SKIP(); /* Generated stub for pause */ }

TEST test_setpgrp(void) { SKIP(); /* Generated stub for setpgrp */ }

TEST test_setsid(void) { SKIP(); /* Generated stub for setsid */ }

TEST test_sysconf(void) { SKIP(); /* Generated stub for sysconf */ }

TEST test_tcgetpgrp(void) { SKIP(); /* Generated stub for tcgetpgrp */ }

TEST test_tcsetpgrp(void) { SKIP(); /* Generated stub for tcsetpgrp */ }

TEST test_truncate(void) { SKIP(); /* Generated stub for truncate */ }

TEST test_ttyname(void) { SKIP(); /* Generated stub for ttyname */ }

TEST test_ttyname_r(void) { SKIP(); /* Generated stub for ttyname_r */ }

TEST test_ualarm(void) { SKIP(); /* Generated stub for ualarm */ }

SUITE(suite_posix_core_misc) {
  RUN_TEST(test__creat);
  RUN_TEST(test_fcntl);
  RUN_TEST(test_alarm);
  RUN_TEST(test_confstr);
  RUN_TEST(test_crypt);
  RUN_TEST(test_encrypt);
  RUN_TEST(test_fpathconf);
  RUN_TEST(test_getgroups);
  RUN_TEST(test_gethostid);
  RUN_TEST(test_gethostname);
  RUN_TEST(test_getlogin);
  RUN_TEST(test_getlogin_r);
  RUN_TEST(test_getopt);
  RUN_TEST(test_getpgrp);
  RUN_TEST(test_getsid);
  RUN_TEST(test_lockf);
  RUN_TEST(test_pathconf);
  RUN_TEST(test_pause);
  RUN_TEST(test_setpgrp);
  RUN_TEST(test_setsid);
  RUN_TEST(test_sysconf);
  RUN_TEST(test_tcgetpgrp);
  RUN_TEST(test_tcsetpgrp);
  RUN_TEST(test_truncate);
  RUN_TEST(test_ttyname);
  RUN_TEST(test_ttyname_r);
  RUN_TEST(test_ualarm);
}
