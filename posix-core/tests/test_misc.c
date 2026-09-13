#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include "sysexits.h"
#if defined(_MSC_VER) || defined(_WIN32)
#include <process.h> /* For _getpid if needed */
#endif
/* clang-format on */

TEST test_sysexits(void) {
  ASSERT_EQ(0, EX_OK);
  ASSERT_EQ(64, EX_USAGE);
  ASSERT_EQ(65, EX_DATAERR);
  ASSERT_EQ(66, EX_NOINPUT);
  ASSERT_EQ(67, EX_NOUSER);
  ASSERT_EQ(68, EX_NOHOST);
  ASSERT_EQ(69, EX_UNAVAILABLE);
  ASSERT_EQ(70, EX_SOFTWARE);
  ASSERT_EQ(71, EX_OSERR);
  ASSERT_EQ(72, EX_OSFILE);
  ASSERT_EQ(73, EX_CANTCREAT);
  ASSERT_EQ(74, EX_IOERR);
  ASSERT_EQ(75, EX_TEMPFAIL);
  ASSERT_EQ(76, EX_PROTOCOL);
  ASSERT_EQ(77, EX_NOPERM);
  ASSERT_EQ(78, EX_CONFIG);
  ASSERT_EQ(64, EX__BASE);
  ASSERT_EQ(78, EX__MAX);
  PASS();
}

TEST test__creat(void) { SKIP(); /* Generated stub for _creat */ }

TEST test_fcntl(void) { SKIP(); /* Generated stub for fcntl */ }

TEST test_alarm(void) { SKIP(); /* Generated stub for alarm */ }

TEST test_confstr(void) { SKIP(); /* Generated stub for confstr */ }

TEST test_crypt(void) { SKIP(); /* Generated stub for crypt */ }

TEST test_encrypt(void) { SKIP(); /* Generated stub for encrypt */ }

TEST test_fpathconf(void) { SKIP(); /* Generated stub for fpathconf */ }

TEST test_getgroups(void) {
  gid_t grps[64];
  int rc;
  rc = getgroups(0, NULL);
  if (rc > 0 && rc <= 64) {
    rc = getgroups(64, grps);
  } else if (rc == 0) {
    rc = getgroups(64, grps);
  }
  ASSERT(rc >= 0);
  PASS();
}

TEST test_setgroups(void) {
  gid_t grps[1];
  int res;
  grps[0] = 0;
  res = setgroups(1, grps);
  if (res == -1 && (errno == EPERM || errno == EACCES)) {
    /* Need root permissions to change group set on UNIX */
    SKIP();
  }
  ASSERT_EQ(0, res);
  ASSERT_EQ(-1, setgroups(1, NULL));
  PASS();
}

TEST test_getsubopt(void) {
  char str[64];
  char *opt;
  char *val;
  char *tokens[5];

  val = NULL;
#if defined(_MSC_VER)
  strcpy_s(str, sizeof(str), "ro,size=1024,name=foo,rw");
#else
  strcpy(str, "ro,size=1024,name=foo,rw");
#endif
  opt = str;
  tokens[0] = "ro";
  tokens[1] = "size";
  tokens[2] = "name";
  tokens[3] = "rw";
  tokens[4] = NULL;

  ASSERT_EQ(0, getsubopt(&opt, tokens, &val));
  ASSERT_EQ(NULL, val);

  ASSERT_EQ(1, getsubopt(&opt, tokens, &val));
  ASSERT_STR_EQ("1024", val);

  ASSERT_EQ(2, getsubopt(&opt, tokens, &val));
  ASSERT_STR_EQ("foo", val);

  ASSERT_EQ(3, getsubopt(&opt, tokens, &val));
  ASSERT_EQ(NULL, val);

  ASSERT_EQ(-1, getsubopt(&opt, tokens, &val));
  PASS();
}

TEST test_getline(void) {
  FILE *f;
  char *line;
  size_t n;
  ssize_t nread;

  line = NULL;
  n = 0;
  f = fopen("test_getline_tmp.txt", "w");
  ASSERT(f != NULL);
  fputs("hello world\nsecond line\n", f);
  fclose(f);

  f = fopen("test_getline_tmp.txt", "r");
  ASSERT(f != NULL);

  nread = getline(&line, &n, f);
  ASSERT_EQ(12, nread);
  ASSERT_STR_EQ("hello world\n", line);

  nread = getline(&line, &n, f);
  ASSERT_EQ(12, nread);
  ASSERT_STR_EQ("second line\n", line);

  nread = getline(&line, &n, f);
  ASSERT_EQ(-1, nread);

  free(line);
  fclose(f);
  remove("test_getline_tmp.txt");
  PASS();
}

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
  RUN_TEST(test_sysexits);
  RUN_TEST(test__creat);
  RUN_TEST(test_fcntl);
  RUN_TEST(test_alarm);
  RUN_TEST(test_confstr);
  RUN_TEST(test_crypt);
  RUN_TEST(test_encrypt);
  RUN_TEST(test_fpathconf);
  RUN_TEST(test_getgroups);
  RUN_TEST(test_setgroups);
  RUN_TEST(test_getsubopt);
  RUN_TEST(test_getline);
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
