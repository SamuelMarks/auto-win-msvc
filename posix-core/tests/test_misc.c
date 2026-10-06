#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include "sysexits.h"
#include "paths.h"
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
#include <grp.h>
#endif
/* clang-format on */

extern int posix_isatty(intptr_t fd);
extern void ASSIGN_CONST_PTR(const void *pptr, void *v);
extern void XZALLOC_CONST_PTR(const void *pptr, size_t size);

TEST test_paths(void) { PASS(); }

TEST test_sysexits(void) { PASS(); }

TEST test__creat(void) {
  int fd = creat("test_creat.tmp", 0666);

  close(fd);
  remove("test_creat.tmp");
  PASS();
}

TEST test_fcntl(void) {
  int fd;

  fd = open("test_fcntl.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    int dup_fd;

    dup_fd = fcntl(fd, F_DUPFD, 10);

    if (dup_fd >= 0) {
      close(dup_fd);
    }
    dup_fd = fcntl(fd, F_DUPFD_CLOEXEC, 12);

    if (dup_fd >= 0) {
      close(dup_fd);
    }
    close(fd);
    remove("test_fcntl.tmp");
  }
  PASS();
}

TEST test_alarm(void) {
  unsigned int rem;
  (void)rem;
  rem = alarm(5);

  rem = alarm(0);

  PASS();
}

TEST test_confstr(void) {
  char cbuf[256];
  size_t n;
  (void)n;

  n = confstr(1, cbuf, sizeof(cbuf));

  PASS();
}

TEST test_crypt(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  char *c;

  c = crypt("key", "salt");

#endif
  PASS();
}

TEST test_encrypt(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  char block[64];
  memset(block, 0, sizeof(block));
  encrypt(block, 0);
  encrypt(block, 1);
#endif
  PASS();
}

TEST test_fpathconf(void) {
  int fd;

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
  (void)rc;
  rc = getgroups(0, NULL);
  if (rc > 0 && rc <= 64) {
    rc = getgroups(64, grps);
  } else if (rc == 0) {
    rc = getgroups(64, grps);
  }

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

  PASS();
}

TEST test_getsubopt(void) {
  char str[64];
  char *opt = NULL;
  char *val = NULL;
  char *tokens[5];
  (void)opt;
  (void)val;

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

  PASS();
}

TEST test_getline(void) {
  FILE *f;
  char *line;
  size_t n;
  ssize_t nread;
  (void)n;

  (void)nread;

  line = NULL;
  n = 0;
  f = fopen("test_getline_tmp.txt", "w");

  fputs("hello world\nsecond line\n", f);
  fclose(f);

  f = fopen("test_getline_tmp.txt", "r");

  nread = getline(&line, &n, f);

  nread = getline(&line, &n, f);

  nread = getline(&line, &n, f);

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
  (void)rc;

#if defined(_WIN32) || defined(_MSC_VER)
  rc = getlogin_r(NULL, 0);

#endif
  rc = getlogin_r(lbuf, sizeof(lbuf));

  PASS();
}

TEST test_getopt(void) {
  char *argv[5];
  int opt;
  (void)opt;

  argv[0] = "prog";
  argv[1] = "-a";
  argv[2] = "-b";
  argv[3] = "val";
  argv[4] = NULL;

  optind = 1;
  opt = getopt(4, argv, "ab:");

  opt = getopt(4, argv, "ab:");

  opt = getopt(4, argv, "ab:");

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

  fd = open("test_lockf.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {

    close(fd);
    remove("test_lockf.tmp");
  }
  PASS();
}

TEST test_pathconf(void) {
  long pc;
  (void)pc;

#if defined(_WIN32) || defined(_MSC_VER)

#endif
  pc = pathconf(".", 1);

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
  (void)sc;

  sc = sysconf(1);

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

#if defined(_WIN32) || defined(_MSC_VER)

#endif

  f = fopen("test_trunc.tmp", "w");

  fputs("1234567890", f);
  fclose(f);

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
  (void)rem;
  rem = ualarm(500000, 0);

  rem = ualarm(0, 0);

  PASS();
}

TEST test_posix_isatty(void) {
  posix_isatty(-1);
  posix_isatty(0);
  posix_isatty(2048);
  posix_isatty(3000);
  PASS();
}

TEST test_posix_mkdtemp(void) {
  char tmpl[] = "test_posix_mkdtemp_XXXXXX";
  char *res = posix_mkdtemp(tmpl);

  rmdir(res);
  PASS();
}

TEST test_ptr_assignment(void) {
  const void *c_ptr = NULL;
  void *ptr = (void *)(intptr_t)0xdeadbeef;
  ASSIGN_CONST_PTR(&c_ptr, ptr);

  XZALLOC_CONST_PTR(&c_ptr, 16);

  free((void *)(intptr_t)c_ptr);
  PASS();
}

SUITE(suite_posix_core_misc) {
  RUN_TEST(test_paths);
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
  RUN_TEST(test_posix_isatty);
  RUN_TEST(test_posix_mkdtemp);
  RUN_TEST(test_ptr_assignment);
}
