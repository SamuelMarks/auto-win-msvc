#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include "sysexits.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER) || defined(_WIN32)
#include <process.h>
#include <io.h>
#include <winsock2.h>
#else
#include <unistd.h>
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

TEST test__creat(void) {
  int fd = creat("test_creat.tmp", 0666);
  ASSERT(fd >= 0);
  close(fd);
  remove("test_creat.tmp");
  PASS();
}

TEST test_fcntl(void) {
  int fd;

  ASSERT_EQ(-1, fcntl(-1, 99999));

  fd = open("test_fcntl.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    ASSERT_EQ(0, fcntl(fd, F_GETFD));
    ASSERT_EQ(0, fcntl(fd, F_SETFD, 0));
    close(fd);
    remove("test_fcntl.tmp");
  }
  PASS();
}

TEST test_alarm(void) {
  unsigned int rem;
  rem = alarm(5);
  (void)rem;
  rem = alarm(0);
  (void)rem;
  PASS();
}

TEST test_confstr(void) {
  char cbuf[256];
  size_t n;

  ASSERT_EQ(0, confstr(-1, NULL, 0));
  n = confstr(1, cbuf, sizeof(cbuf));
  (void)n;
  PASS();
}

TEST test_crypt(void) {
  char *c;

#if defined(_WIN32)
  ASSERT_EQ(NULL, crypt(NULL, NULL));
#endif
  c = crypt("key", "salt");
  (void)c;
  PASS();
}

TEST test_encrypt(void) {
  char block[64];
  memset(block, 0, sizeof(block));
  encrypt(block, 0);
  encrypt(block, 1);
  PASS();
}

TEST test_fpathconf(void) {
  int fd;

  ASSERT_EQ(-1, fpathconf(-1, 0));

  fd = open("test_fpc.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    long val = fpathconf(fd, 1);
    (void)val;
    close(fd);
    remove("test_fpc.tmp");
  }
  PASS();
}

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
    PASS();
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

TEST test_gethostid(void) {
  long hid = gethostid();
  (void)hid;
  PASS();
}

TEST test_gethostname(void) {
  char name[256];
  int rc = gethostname(name, sizeof(name));
  (void)rc;
  PASS();
}

TEST test_getlogin(void) {
  char *l = getlogin();
  (void)l;
  PASS();
}

TEST test_getlogin_r(void) {
  char lbuf[256];
  int rc;

  rc = getlogin_r(NULL, 0);
  (void)rc;
  rc = getlogin_r(lbuf, sizeof(lbuf));
  (void)rc;
  PASS();
}

TEST test_getopt(void) {
  char *argv[5];
  int opt;

  argv[0] = "prog";
  argv[1] = "-a";
  argv[2] = "-b";
  argv[3] = "val";
  argv[4] = NULL;

  optind = 1;
  opt = getopt(4, argv, "ab:");
  ASSERT_EQ('a', opt);
  opt = getopt(4, argv, "ab:");
  ASSERT_EQ('b', opt);
  ASSERT_STR_EQ("val", optarg);
  opt = getopt(4, argv, "ab:");
  ASSERT_EQ(-1, opt);
  PASS();
}

TEST test_getpgrp(void) {
  pid_t pgrp = getpgrp();
  (void)pgrp;
  PASS();
}

TEST test_getsid(void) {
  pid_t sid = getsid(0);
  (void)sid;
  PASS();
}

TEST test_lockf(void) {
  int fd;

  ASSERT_EQ(-1, lockf(-1, 0, -1));

  fd = open("test_lockf.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    (void)lockf(fd, 1, 10);
    (void)lockf(fd, 0, 10);
    close(fd);
    remove("test_lockf.tmp");
  }
  PASS();
}

TEST test_pathconf(void) {
  long pc;

  ASSERT_EQ(-1, pathconf(NULL, 0));
  pc = pathconf(".", 1);
  (void)pc;
  PASS();
}

TEST test_pause(void) { PASS(); }

TEST test_setpgrp(void) {
  pid_t sp = setpgrp();
  (void)sp;
  PASS();
}

TEST test_setsid(void) {
  pid_t ss = setsid();
  (void)ss;
  PASS();
}

TEST test_sysconf(void) {
  long sc;

  ASSERT_EQ(-1, sysconf(-1));
  sc = sysconf(1);
  (void)sc;
  PASS();
}

TEST test_tcgetpgrp(void) {
  pid_t tcp = tcgetpgrp(-1);
  (void)tcp;
  PASS();
}

TEST test_tcsetpgrp(void) {
  int trc = tcsetpgrp(-1, 0);
  (void)trc;
  PASS();
}

TEST test_truncate(void) {
  FILE *f;

  ASSERT_EQ(-1, truncate(NULL, 0));
  ASSERT_EQ(-1, truncate("test_trunc.tmp", -1));

  f = fopen("test_trunc.tmp", "w");
  ASSERT(f != NULL);
  fputs("1234567890", f);
  fclose(f);

  ASSERT_EQ(0, truncate("test_trunc.tmp", 5));
  remove("test_trunc.tmp");
  PASS();
}

TEST test_ttyname(void) {
  char *t = ttyname(-1);
  (void)t;
  PASS();
}

TEST test_ttyname_r(void) {
  char tbuf[64];
  int trc = ttyname_r(-1, tbuf, sizeof(tbuf));
  (void)trc;
  PASS();
}

TEST test_ualarm(void) {
  useconds_t rem;
  rem = ualarm(500000, 0);
  (void)rem;
  rem = ualarm(0, 0);
  (void)rem;
  PASS();
}

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
