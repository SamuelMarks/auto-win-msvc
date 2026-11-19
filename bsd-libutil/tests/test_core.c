/* clang-format off */
#include "greatest.h"
#include <libutil.h>
#include <login_cap.h>
#include <pwd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

static int g_libutil_oom_countdown = 0;

static void *mock_libutil_calloc(size_t n, size_t sz) {
  if (g_libutil_oom_countdown > 0) {
    g_libutil_oom_countdown--;
    if (g_libutil_oom_countdown == 0) {
      return NULL;
    }
  }
  return calloc(n, sz);
}

static char *mock_libutil_strdup(const char *s) {
  size_t len;
  char *p;
  if (g_libutil_oom_countdown > 0) {
    g_libutil_oom_countdown--;
    if (g_libutil_oom_countdown == 0) {
      return NULL;
    }
  }
  len = strlen(s) + 1;
  p = (char *)malloc(len);
  if (p != NULL) {
    memcpy(p, s, len);
  }
  return p;
}

static int mock_libutil_write_fail(int fd, const void *buf, unsigned int len) {
  (void)fd;
  (void)buf;
  (void)len;
  return -1;
}

TEST test_bsd_libutil_humanize_number(void) {
  char buf[64];
  int64_t exa = (int64_t)1000 * 1000 * 1000;
  exa = exa * 1000 * 1000 * 1000 * 2;

  /* Error branches: NULL buf and zero len */
  ASSERT_EQ(-1, humanize_number(NULL, sizeof(buf), 100, "B", HN_AUTOSCALE, 0));
  ASSERT_EQ(-1, humanize_number(buf, 0, 100, "B", HN_AUTOSCALE, 0));

  /* Autoscale, negative, space vs nospace, NULL suffix, HN_B, HN_IEC_PREFIXES
   */
  ASSERT(humanize_number(buf, sizeof(buf), 512, NULL, HN_AUTOSCALE, HN_B) > 0);
  ASSERT_STR_EQ("512 B", buf);

  ASSERT(humanize_number(buf, sizeof(buf), 512, "B", 0, HN_NOSPACE) > 0);
  ASSERT_STR_EQ("512B", buf);

  ASSERT(humanize_number(buf, sizeof(buf), -2048, "B", HN_AUTOSCALE,
                         HN_IEC_PREFIXES) > 0);
  ASSERT_STR_EQ("-2 KiB", buf);

  /* HN_DIVISOR_1000 and max scale clamping (i == 6) */
  ASSERT(humanize_number(buf, sizeof(buf), exa, "B", HN_AUTOSCALE,
                         HN_DIVISOR_1000 | HN_NOSPACE) > 0);
  ASSERT_STR_EQ("2EB", buf);

  /* Explicit positive scale (< 6 and >= 6) */
  ASSERT(humanize_number(buf, sizeof(buf), 1048576, "B", 2, HN_NOSPACE) > 0);
  ASSERT_STR_EQ("1MB", buf);
  ASSERT(humanize_number(buf, sizeof(buf), exa, "B", 9,
                         HN_DIVISOR_1000 | HN_NOSPACE) > 0);
  ASSERT_STR_EQ("2EB", buf);

  /* HN_GETSCALE */
  ASSERT_EQ(1, humanize_number(buf, sizeof(buf), 2048, "B", HN_GETSCALE,
                               HN_GETSCALE));
  /* scale == HN_GETSCALE without HN_GETSCALE flag */
  ASSERT(humanize_number(buf, sizeof(buf), 2048, "B", HN_GETSCALE, 0) > 0);

  /* Truncation (total >= len) */
  ASSERT_EQ(-1, humanize_number(buf, 2, 1024, "B", HN_AUTOSCALE, 0));

  PASS();
}

TEST test_bsd_libutil_expand_number(void) {
  uint64_t val = 0;

  /* Error branches: NULL args, empty, whitespace-only, negative, non-numeric */
  ASSERT_EQ(-1, expand_number(NULL, &val));
  ASSERT_EQ(-1, expand_number("10", NULL));
  ASSERT_EQ(-1, expand_number("", &val));
  ASSERT_EQ(-1, expand_number("   ", &val));
  ASSERT_EQ(-1, expand_number("  -10", &val));
  ASSERT_EQ(-1, expand_number("xyz", &val));
  ASSERT_EQ(-1, expand_number("999999999999999999999999999999", &val));

  /* Plain integer without suffix */
  ASSERT_EQ(0, expand_number("  42", &val));
  ASSERT_EQ((uint64_t)42U, val);

  /* All valid unit prefixes and suffix forms ('', 'B', 'iB') */
  ASSERT_EQ(0, expand_number("7B", &val));
  ASSERT_EQ((uint64_t)7U, val);
  ASSERT_EQ(0, expand_number("2K", &val));
  ASSERT_EQ((uint64_t)2048U, val);
  ASSERT_EQ(0, expand_number("2KB", &val));
  ASSERT_EQ((uint64_t)2048U, val);
  ASSERT_EQ(0, expand_number("2KiB", &val));
  ASSERT_EQ((uint64_t)2048U, val);
  ASSERT_EQ(0, expand_number("1M", &val));
  ASSERT_EQ((uint64_t)1048576U, val);
  ASSERT_EQ(0, expand_number("1G", &val));
  ASSERT_EQ((uint64_t)1073741824U, val);
  ASSERT_EQ(0, expand_number("1T", &val));
  ASSERT_EQ(0, expand_number("1P", &val));
  ASSERT_EQ(0, expand_number("1E", &val));

  /* Invalid unit and malformed suffix tails */
  ASSERT_EQ(-1, expand_number("1Z", &val));
  ASSERT_EQ(-1, expand_number("1KBX", &val));
  ASSERT_EQ(-1, expand_number("1KiX", &val));
  ASSERT_EQ(-1, expand_number("1KiBX", &val));
  ASSERT_EQ(-1, expand_number("1KX", &val));

  /* Shift overflow */
  ASSERT_EQ(-1, expand_number("32E", &val));

  PASS();
}

TEST test_bsd_libutil_hexdump_and_pidfile(void) {
  unsigned char data[6];
  struct pidfh *pfh;
  pid_t pid = 99;
  int fd;

  data[0] = 'A';
  data[1] = 1;
  data[2] = 'B';
  data[3] = 2;
  data[4] = 'C';
  data[5] = 0x7f;

  /* length == 0, default cols/delim, custom cols/delim/hdr, omit flags */
  hexdump(data, 0, NULL, 0);
  hexdump(data, 6, "PFX: ", 4 | ('[' << 8));
  hexdump(data, 4, NULL, HD_OMIT_COUNT | HD_OMIT_HEX | HD_OMIT_CHARS);

  /* pidfile error and OOM branches */
  bsd_libutil_set_alloc_hooks(mock_libutil_calloc, mock_libutil_strdup);
  g_libutil_oom_countdown = 1;
  ASSERT(pidfile_open("test_libutil_oom.pid", 0600, &pid) == NULL);
  ASSERT_EQ((pid_t)0, pid);
  bsd_libutil_set_alloc_hooks(NULL, NULL);

  ASSERT(pidfile_open("/nonexistent_dir_awm_99999/bad.pid", 0, NULL) == NULL);
  ASSERT_EQ(-1, pidfile_write(NULL));
  ASSERT_EQ(-1, pidfile_fileno(NULL));
  ASSERT_EQ(0, pidfile_close(NULL));
  ASSERT_EQ(0, pidfile_remove(NULL));

  /* pidfile normal open, write, write failure, fileno, close, remove */
  pfh = pidfile_open(NULL, 0, NULL);
  ASSERT(pfh != NULL);
  ASSERT(pidfile_fileno(pfh) >= 0);
  ASSERT_EQ(0, pidfile_write(pfh));

  bsd_libutil_set_write_hook(mock_libutil_write_fail);
  ASSERT_EQ(-1, pidfile_write(pfh));
  bsd_libutil_set_write_hook(NULL);

  ASSERT_EQ(0, pidfile_remove(pfh));

  /* pidfile_close with valid fd and with negative fd */
  pfh = pidfile_open("test_libutil_custom.pid", 0600, &pid);
  ASSERT(pfh != NULL);
  ASSERT_EQ(0, pidfile_close(pfh));
  (void)remove("test_libutil_custom.pid");

  pfh = pidfile_open("test_libutil_negfd.pid", 0600, NULL);
  ASSERT(pfh != NULL);
#if defined(_MSC_VER) || defined(_WIN32)
  _close(pfh->pf_fd);
#else
  close(pfh->pf_fd);
#endif
  pfh->pf_fd = -1;
  ASSERT_EQ(-1, pidfile_write(pfh));
  ASSERT_EQ(0, pidfile_close(pfh));
  (void)remove("test_libutil_negfd.pid");

  pfh = pidfile_open("test_libutil_negfd2.pid", 0600, NULL);
  ASSERT(pfh != NULL);
#if defined(_MSC_VER) || defined(_WIN32)
  _close(pfh->pf_fd);
#else
  close(pfh->pf_fd);
#endif
  pfh->pf_fd = -1;
  ASSERT_EQ(0, pidfile_remove(pfh));

  /* flopen and flopenat */
  ASSERT_EQ(-1, flopen(NULL, O_RDONLY));
  ASSERT_EQ(-1, flopen("/nonexistent_dir_awm_99999/flopen.tmp", O_RDONLY));

  fd = flopen("test_libutil_flopen.tmp", O_RDWR | O_CREAT | O_TRUNC, 0600);
  ASSERT(fd >= 0);
#if defined(_MSC_VER) || defined(_WIN32)
  _close(fd);
#else
  close(fd);
#endif

  fd = flopenat(-1, "test_libutil_flopen.tmp", O_RDONLY);
  ASSERT(fd >= 0);
#if defined(_MSC_VER) || defined(_WIN32)
  _close(fd);
#else
  close(fd);
#endif

  fd = flopenat(-1, "test_libutil_flopen2.tmp", O_RDWR | O_CREAT | O_TRUNC,
                0600);
  ASSERT(fd >= 0);
#if defined(_MSC_VER) || defined(_WIN32)
  _close(fd);
#else
  close(fd);
#endif

  (void)remove("test_libutil_flopen.tmp");
  (void)remove("test_libutil_flopen2.tmp");

  PASS();
}

TEST test_bsd_libutil_login_pw_and_misc(void) {
  char host1[32] = "myhost.example.com";
  char host2[32] = "nodot";
  char host3[32] = "longhost.example.com";
  char outhost[16];
  char scanbuf[64] = "root:*:0:0::0:0:root:/root:/bin/sh";
  login_cap_t *lc;
  struct passwd pw1;
  struct passwd pw2;
  struct passwd *pw_copy_ptr;

  /* UUCP locks */
  ASSERT_EQ(UU_LOCK_OK, uu_lock("ttyS0"));
  ASSERT_EQ(0, uu_unlock("ttyS0"));
  ASSERT_EQ(UU_LOCK_OK, uu_lock_txfr("ttyS0", (pid_t)1));
  ASSERT_STR_EQ("No error", uu_lockerr(UU_LOCK_OK));

  /* trimdomain & realhostname */
  trimdomain(NULL, 10);
  trimdomain(host1, 0);
  trimdomain(host1, (int)sizeof(host1));
  ASSERT_STR_EQ("myhost", host1);
  trimdomain(host2, (int)sizeof(host2));
  ASSERT_STR_EQ("nodot", host2);
  trimdomain(host3, 4);
  ASSERT_STR_EQ("longhost.example.com", host3);

  ASSERT_EQ(HOSTNAME_FOUND, realhostname(NULL, 10, NULL));
  ASSERT_EQ(HOSTNAME_FOUND, realhostname(outhost, 0, NULL));
  ASSERT_EQ(HOSTNAME_FOUND, realhostname_sa(outhost, sizeof(outhost), NULL, 0));
  ASSERT_STR_EQ("localhost", outhost);

  /* login_cap OOM and normal lifecycle */
  bsd_libutil_set_alloc_hooks(mock_libutil_calloc, mock_libutil_strdup);
  g_libutil_oom_countdown = 1;
  ASSERT(login_getclass("staff") == NULL);
  g_libutil_oom_countdown = 2;
  ASSERT(login_getclass("staff") == NULL);
  bsd_libutil_set_alloc_hooks(NULL, NULL);

  login_close(NULL);

  lc = login_getclass(NULL);
  ASSERT(lc != NULL);
  ASSERT_STR_EQ(LOGIN_DEFCLASS, lc->lc_class);
  lc->lc_cap = (char *)malloc(4);
  lc->lc_style = (char *)malloc(4);
  ASSERT_EQ(1, login_getcapbool(lc, "bool", 1));
  ASSERT_EQ((rlim_t)10, login_getcapnum(lc, "num", (rlim_t)10, (rlim_t)0));
  ASSERT_EQ((rlim_t)20, login_getcapsize(lc, "sz", (rlim_t)20, (rlim_t)0));
  ASSERT_STR_EQ("v", login_getcapstr(lc, "str", "v", NULL));
  ASSERT_EQ((rlim_t)30, login_getcaptime(lc, "tm", (rlim_t)30, (rlim_t)0));
  ASSERT_STR_EQ("/bin", login_getpath(lc, "path", "/bin", NULL));
  ASSERT_EQ(0, setclasscontext("default", LOGIN_SETALL));
  ASSERT_EQ(0, setusercontext(lc, NULL, (uid_t)0, LOGIN_SETALL));
  ASSERT_EQ(0, setclassenvironment(lc, NULL, 1));
  login_close(lc);

  lc = login_getpwclass(NULL);
  ASSERT(lc != NULL);
  free(lc->lc_class);
  lc->lc_class = NULL;
  login_close(lc);

  lc = login_getuserclass(NULL);
  ASSERT(lc != NULL);
  login_close(lc);

  lc = login_getclassbyname("daemon", NULL);
  ASSERT(lc != NULL);
  ASSERT_STR_EQ("daemon", lc->lc_class);
  login_close(lc);

  /* getlocalbase branches */
#if defined(_MSC_VER)
  {
    char huge_env[600];
    memset(huge_env, 'A', sizeof(huge_env) - 1);
    huge_env[sizeof(huge_env) - 1] = '\0';
    (void)_putenv_s("LOCALBASE", "");
    ASSERT_STR_EQ("C:\134usr\134local", getlocalbase());
    (void)_putenv_s("LOCALBASE", huge_env);
    ASSERT_STR_EQ("C:\134usr\134local", getlocalbase());
    (void)_putenv_s("LOCALBASE", "C:\134custom\134local");
    ASSERT_STR_EQ("C:\134custom\134local", getlocalbase());
    (void)_putenv_s("LOCALBASE", "");
  }
#elif defined(_WIN32)
  (void)_putenv("LOCALBASE=");
  ASSERT_STR_EQ("C:\134usr\134local", getlocalbase());
  (void)_putenv("LOCALBASE=C:\134custom\134local");
  ASSERT_STR_EQ("C:\134custom\134local", getlocalbase());
  (void)_putenv("LOCALBASE=");
#else
  (void)unsetenv("LOCALBASE");
  ASSERT_STR_EQ("/usr/local", getlocalbase());
  (void)setenv("LOCALBASE", "", 1);
  ASSERT_STR_EQ("/usr/local", getlocalbase());
  (void)setenv("LOCALBASE", "/custom/local", 1);
  ASSERT_STR_EQ("/custom/local", getlocalbase());
  (void)unsetenv("LOCALBASE");
#endif

  /* quota & pw_* helpers */
  ASSERT(quota_open(NULL, 0, 0) == NULL);
  ASSERT_EQ(-1, quota_read(NULL, NULL, 0));
  quota_close(NULL);

  pw_initpwd(NULL);
  pw_initpwd(&pw1);
  pw_initpwd(&pw2);
  pw1.pw_uid = (uid_t)100;
  pw1.pw_gid = (gid_t)200;
  pw2.pw_uid = (uid_t)100;
  pw2.pw_gid = (gid_t)200;

  ASSERT_EQ(0, pw_equal(NULL, &pw2));
  ASSERT_EQ(0, pw_equal(&pw1, NULL));
  ASSERT_EQ(1, pw_equal(&pw1, &pw2));
  pw2.pw_gid = (gid_t)201;
  ASSERT_EQ(0, pw_equal(&pw1, &pw2));
  pw2.pw_uid = (uid_t)101;
  ASSERT_EQ(0, pw_equal(&pw1, &pw2));

  ASSERT(pw_dup(NULL) == NULL);
  bsd_libutil_set_alloc_hooks(mock_libutil_calloc, mock_libutil_strdup);
  g_libutil_oom_countdown = 1;
  ASSERT(pw_dup(&pw1) == NULL);
  bsd_libutil_set_alloc_hooks(NULL, NULL);

  pw_copy_ptr = pw_dup(&pw1);
  ASSERT(pw_copy_ptr != NULL);
  ASSERT_EQ((uid_t)100, pw_copy_ptr->pw_uid);
  free(pw_copy_ptr);

  ASSERT_EQ(-1, pw_copy(-1, -1, &pw1, &pw2));
  ASSERT_EQ(-1, pw_edit(0));
  pw_fini();
  ASSERT_EQ(0, pw_init("/etc", "master.passwd"));
  ASSERT(pw_make(&pw1) == NULL);
  ASSERT(pw_make_v7(&pw1) == NULL);
  ASSERT_EQ(-1, pw_mkdb(NULL));
  ASSERT_EQ(-1, pw_lock());
  ASSERT_EQ(0, __pw_scan(scanbuf, &pw1, PWSCAN_MASTER));
  ASSERT(pw_scan("root:*:0:0::0:0:root:/root:/bin/sh", 0) == NULL);
  ASSERT_STR_EQ("/etc/master.passwd.tmp", pw_tempname());
  ASSERT_EQ(-1, pw_tmp(-1));

  PASS();
}

SUITE(suite_bsd_libutil_core) {
  RUN_TEST(test_bsd_libutil_humanize_number);
  RUN_TEST(test_bsd_libutil_expand_number);
  RUN_TEST(test_bsd_libutil_hexdump_and_pidfile);
  RUN_TEST(test_bsd_libutil_login_pw_and_misc);
}
