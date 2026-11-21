/* clang-format off */
#include "bsd/stdlib.h"
#include "greatest.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
/* clang-format on */

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 34)
#define TEST_HAVE_GLIBC_CLOSEFROM 1
#endif
#if __GLIBC_PREREQ(2, 26)
#define TEST_HAVE_GLIBC_REALLOCARRAY 1
#endif
#if __GLIBC_PREREQ(2, 36)
#define TEST_HAVE_GLIBC_ARC4RANDOM 1
#endif
#endif

static int g_oom_countdown = 0;
static int g_closed_count = 0;
static long g_mock_sc_max = 16;
static time_t g_mock_time_val = 0;
static unsigned long g_mock_mod_len = 0;
static const char *g_mock_mod_str = "C:\\tools\\demo.EXE";

static void *mock_malloc(size_t sz) {
  if (g_oom_countdown > 0) {
    g_oom_countdown--;
    if (g_oom_countdown == 0) {
      return NULL;
    }
  }
  return malloc(sz);
}

static void *mock_calloc(size_t n, size_t sz) {
  if (g_oom_countdown > 0) {
    g_oom_countdown--;
    if (g_oom_countdown == 0) {
      return NULL;
    }
  }
  return calloc(n, sz);
}

static void *mock_realloc(void *ptr, size_t sz) {
  if (g_oom_countdown > 0) {
    g_oom_countdown--;
    if (g_oom_countdown == 0) {
      return NULL;
    }
  }
  return realloc(ptr, sz);
}

static int mock_close_fd(int fd) {
  (void)fd;
  g_closed_count++;
  return 0;
}

static long mock_sysconf(int name) {
  (void)name;
  return g_mock_sc_max;
}

static time_t mock_time_fn(time_t *t) {
  if (t != NULL) {
    *t = g_mock_time_val;
  }
  return g_mock_time_val;
}

static unsigned long mock_mod_fname(char *buf, unsigned long sz) {
  if (g_mock_mod_len > 0 && g_mock_mod_len < sz && g_mock_mod_str != NULL) {
    size_t slen = strlen(g_mock_mod_str);
    if (slen + 1 <= (size_t)sz) {
      memcpy(buf, g_mock_mod_str, slen + 1);
    }
  }
  return g_mock_mod_len;
}

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
static int cmp_int(const void *a, const void *b) {
  int ia = *(const int *)a;
  int ib = *(const int *)b;
  return (ia > ib) - (ia < ib);
}

static FILE *open_tmp_stream(void) {
  FILE *fp = NULL;
#if defined(_MSC_VER)
  if (fopen_s(&fp, "bsd_stdlib_tmp.dat", "wb+") != 0) {
    fp = NULL;
  }
#else
  fp = fopen("bsd_stdlib_tmp.dat", "wb+");
#endif
  return fp;
}
#endif

TEST test_bsd_stdlib_alloc_and_closefrom(void) {
#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
  void *p;
  void *p2;
  size_t huge_sz = ((size_t)-1) / 2 + 2;

#if !defined(TEST_HAVE_GLIBC_CLOSEFROM)
  /* Default closefrom with high lowfd (closes non-open high fds safely) */
  closefrom(250);
  closefrom(300);

  /* Hooked closefrom covering lowfd < 0 and sysconf branches */
  bsd_stdlib_set_sys_hooks(mock_close_fd, mock_sysconf, mock_time_fn,
                           mock_mod_fname);
  g_closed_count = 0;
  g_mock_sc_max = -1;
  closefrom(-5);
  ASSERT_EQ(256, g_closed_count);

  g_closed_count = 0;
  g_mock_sc_max = 8192;
  closefrom(250);
  ASSERT_EQ(6, g_closed_count);

  g_closed_count = 0;
  g_mock_sc_max = 10;
  closefrom(8);
  ASSERT(g_closed_count > 0);

  bsd_stdlib_set_sys_hooks(NULL, NULL, NULL, NULL);
#else
  bsd_stdlib_set_sys_hooks(mock_close_fd, mock_sysconf, mock_time_fn,
                           mock_mod_fname);
  bsd_stdlib_set_sys_hooks(NULL, NULL, NULL, NULL);
#endif

#if !defined(TEST_HAVE_GLIBC_REALLOCARRAY)
  errno = 0;
  ASSERT(reallocarray(NULL, huge_sz, 4) == NULL);
  ASSERT_EQ(ENOMEM, errno);

  p = reallocarray(NULL, 0, 0);
  free(p);

  p = reallocarray(NULL, 4, sizeof(int));
  ASSERT(p != NULL);
  free(p);

  bsd_stdlib_set_alloc_hooks(mock_malloc, mock_calloc, mock_realloc);
  g_oom_countdown = 1;
  ASSERT(reallocarray(NULL, 4, sizeof(int)) == NULL);
  bsd_stdlib_set_alloc_hooks(NULL, NULL, NULL);
#endif

  /* recallocarray overflow & size == 0 & NULL ptr & shrink & same & grow & OOM
   */
  errno = 0;
  ASSERT(recallocarray(NULL, huge_sz, 1, 4) == NULL);
  ASSERT_EQ(ENOMEM, errno);

  errno = 0;
  ASSERT(recallocarray(NULL, 1, huge_sz, 4) == NULL);
  ASSERT_EQ(ENOMEM, errno);

  p = recallocarray(NULL, 0, 0, 0);
  free(p);

  p = recallocarray(NULL, 0, 4, sizeof(int));
  ASSERT(p != NULL);

  /* Grow */
  p2 = recallocarray(p, 4, 8, sizeof(int));
  ASSERT(p2 != NULL);

  /* Same size (diff == 0) */
  p = recallocarray(p2, 8, 8, sizeof(int));
  ASSERT(p != NULL);

  /* Shrink (diff > 0) */
  p2 = recallocarray(p, 8, 2, sizeof(int));
  ASSERT(p2 != NULL);

  /* OOM on malloc inside recallocarray */
  bsd_stdlib_set_alloc_hooks(mock_malloc, mock_calloc, mock_realloc);
  g_oom_countdown = 1;
  ASSERT(recallocarray(p2, 2, 4, sizeof(int)) == NULL);
  g_oom_countdown = 1;
  ASSERT(recallocarray(NULL, 0, 4, sizeof(int)) == NULL);
  bsd_stdlib_set_alloc_hooks(NULL, NULL, NULL);

  freezero(NULL, 16);
  freezero(p2, 2 * sizeof(int));
#endif

#if defined(__APPLE__)
  {
    void *ms = bsd_setmode("0755");
    ASSERT(ms != NULL);
    ASSERT_EQ((mode_t)0755, bsd_getmode(ms, 0));
    free(ms);
  }
#endif

  bsd_stdlib_reset_state();
  ASSERT_EQ(0, bsd_stdlib_dummy_for_coverage());
  PASS();
}

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
TEST test_bsd_stdlib_progname_strtonum_setmode(void) {
  const char *errstr = NULL;
  void *ms;

  bsd_stdlib_reset_state();
#if defined(_WIN32)
  /* Exercise default GetModuleFileNameA path first */
  ASSERT(getprogname() != NULL);
  /* Cached branch */
  ASSERT(getprogname() != NULL);

  /* Mocked GetModuleFileNameA: len == 0 */
  bsd_stdlib_reset_state();
  bsd_stdlib_set_sys_hooks(NULL, NULL, NULL, mock_mod_fname);
  g_mock_mod_len = 0;
  ASSERT_STR_EQ("prog", getprogname());

  /* Mocked GetModuleFileNameA: len == sizeof(modpath) (260) */
  bsd_stdlib_reset_state();
  g_mock_mod_len = 260;
  ASSERT_STR_EQ("prog", getprogname());

  /* Mocked GetModuleFileNameA: valid path */
  bsd_stdlib_reset_state();
  g_mock_mod_str = "C:\\tools\\demo.EXE";
  g_mock_mod_len = (unsigned long)strlen(g_mock_mod_str);
  ASSERT_STR_EQ("demo", getprogname());
  bsd_stdlib_set_sys_hooks(NULL, NULL, NULL, NULL);
#else
  ASSERT_STR_EQ("prog", getprogname());
  ASSERT_STR_EQ("prog", getprogname());
#endif

  setprogname(NULL);
  setprogname("ab");
  ASSERT_STR_EQ("ab", getprogname());
  setprogname("/usr/bin/myprog.exe");
  ASSERT_STR_EQ("myprog", getprogname());
  setprogname("C:\\bin\\tool.EXE");
  ASSERT_STR_EQ("tool", getprogname());
  setprogname("a\\b/forward.bin");
  ASSERT_STR_EQ("forward.bin", getprogname());
  setprogname("a/b\\back");
  ASSERT_STR_EQ("back", getprogname());

  /* strtonum branches */
  ASSERT_EQ(0, strtonum("10", 20, 10, &errstr));
  ASSERT_STR_EQ("invalid", errstr);
  ASSERT_EQ(0, strtonum("10", 20, 10, NULL));

  ASSERT_EQ(0, strtonum(NULL, 0, 10, &errstr));
  ASSERT_STR_EQ("invalid", errstr);
  ASSERT_EQ(0, strtonum("", 0, 10, NULL));

  ASSERT_EQ(0, strtonum("abc", 0, 10, &errstr));
  ASSERT_STR_EQ("invalid", errstr);
  ASSERT_EQ(0, strtonum("12abc", 0, 10, NULL));

  ASSERT_EQ(0, strtonum("-5", 0, 10, &errstr));
  ASSERT_STR_EQ("too small", errstr);
  ASSERT_EQ(0, strtonum("-5", 0, 10, NULL));
  ASSERT_EQ(0, strtonum("-99999999999999999999999999999", -100, 100, &errstr));
  ASSERT_STR_EQ("too small", errstr);

  ASSERT_EQ(0, strtonum("50", 0, 10, &errstr));
  ASSERT_STR_EQ("too large", errstr);
  ASSERT_EQ(0, strtonum("50", 0, 10, NULL));
  ASSERT_EQ(0, strtonum("99999999999999999999999999999", -100, 100, &errstr));
  ASSERT_STR_EQ("too large", errstr);

  ASSERT_EQ(42, strtonum("42", 0, 100, &errstr));
  ASSERT(errstr == NULL);
  ASSERT_EQ(42, strtonum("42", 0, 100, NULL));

  /* bsd_setmode / getmode / bsd_getmode */
  ASSERT(bsd_setmode(NULL) == NULL);
  ASSERT(bsd_setmode("") == NULL);

  bsd_stdlib_set_alloc_hooks(mock_malloc, mock_calloc, mock_realloc);
  g_oom_countdown = 1;
  ASSERT(bsd_setmode("0755") == NULL);
  bsd_stdlib_set_alloc_hooks(NULL, NULL, NULL);

  ASSERT(bsd_setmode("078") == NULL);
  ASSERT(bsd_setmode("010000") == NULL);
  ms = bsd_setmode("0755");
  ASSERT(ms != NULL);
  ASSERT_EQ((mode_t)(0100000 | 0755), getmode(ms, 0100000));
  ASSERT_EQ((mode_t)0755, bsd_getmode(ms, 0));
  ASSERT_EQ((mode_t)0600, getmode(NULL, 0600));
  free(ms);

  /* Symbolic modes: u, g, o, a, default who, +, -, =, r, w, x, X, s, t, ',' and
   * invalid */
  ms = bsd_setmode("+x");
  ASSERT(ms != NULL);
  free(ms);

  ms = bsd_setmode("u+rwx,g-w,o=rx,a+st,+X");
  ASSERT(ms != NULL);
  (void)getmode(ms, 0666);
  free(ms);

  ASSERT(bsd_setmode("u+x?bad") == NULL);
  ASSERT(bsd_setmode("8bad") == NULL);
  PASS();
}

TEST test_bsd_stdlib_io_and_misc(void) {
  FILE *fp;
  size_t len = 99;
  size_t lineno = 0;
  char *line;
  wchar_t *wline;
  char longbuf[300];
  int hlen = 0;
  long bsize = 0;
  int arr[4] = {4, 1, 3, 2};
  const unsigned char *sarr[4];
  unsigned char weight_tbl[256];
  char devbuf[16];
  char *cap_out = (char *)0x1;
  size_t i;

  /* fgetln / fgetwln / fparseln NULL stream */
  ASSERT(fgetln(NULL, &len) == NULL);
  ASSERT_EQ(0U, len);
  ASSERT(fgetln(NULL, NULL) == NULL);

  ASSERT(fgetwln(NULL, &len) == NULL);
  ASSERT_EQ(0U, len);
  ASSERT(fgetwln(NULL, NULL) == NULL);

  ASSERT(fparseln(NULL, &len, &lineno, NULL, 0) == NULL);

  /* Empty file */
  fp = open_tmp_stream();
  ASSERT(fp != NULL);
  ASSERT(fgetln(fp, &len) == NULL);
  ASSERT_EQ(0U, len);
  rewind(fp);
  ASSERT(fgetln(fp, NULL) == NULL);
  rewind(fp);
  ASSERT(fgetwln(fp, &len) == NULL);
  ASSERT_EQ(0U, len);
  rewind(fp);
  ASSERT(fgetwln(fp, NULL) == NULL);
  rewind(fp);
  ASSERT(fparseln(fp, &len, &lineno, NULL, 0) == NULL);
  fclose(fp);

  /* Long line (> 256 chars) + second line without trailing newline + OOM */
  bsd_stdlib_reset_state();
  fp = open_tmp_stream();
  ASSERT(fp != NULL);
  memset(longbuf, 'A', 280);
  longbuf[280] = '\n';
  longbuf[281] = '\0';
  fputs(longbuf, fp);
  fputs("tail_no_nl", fp);
  rewind(fp);

  bsd_stdlib_set_alloc_hooks(mock_malloc, mock_calloc, mock_realloc);
  g_oom_countdown = 1;
  ASSERT(fgetln(fp, &len) == NULL);
  ASSERT_EQ(0U, len);
  rewind(fp);
  g_oom_countdown = 1;
  ASSERT(fgetln(fp, NULL) == NULL);
  bsd_stdlib_set_alloc_hooks(NULL, NULL, NULL);

  rewind(fp);
  line = fgetln(fp, &len);
  ASSERT(line != NULL);
  ASSERT_EQ(281U, len);
  line = fgetln(fp, NULL);
  ASSERT(line != NULL);
  ASSERT_STR_EQ("tail_no_nl", line);
  fclose(fp);

  /* fgetwln: > 128 wide chars + line without newline + OOM */
  bsd_stdlib_reset_state();
  fp = open_tmp_stream();
  ASSERT(fp != NULL);
  for (i = 0; i < 140; i++) {
    fputwc(L'W', fp);
  }
  fputwc(L'\n', fp);
  fputwc(L'Z', fp);
  rewind(fp);

  bsd_stdlib_set_alloc_hooks(mock_malloc, mock_calloc, mock_realloc);
  g_oom_countdown = 1;
  ASSERT(fgetwln(fp, &len) == NULL);
  ASSERT_EQ(0U, len);
  rewind(fp);
  g_oom_countdown = 1;
  ASSERT(fgetwln(fp, NULL) == NULL);
  bsd_stdlib_set_alloc_hooks(NULL, NULL, NULL);

  rewind(fp);
  wline = fgetwln(fp, &len);
  ASSERT(wline != NULL);
  ASSERT_EQ(141U, len);
  wline = fgetwln(fp, NULL);
  ASSERT(wline != NULL);
  fclose(fp);

  /* fparseln: CRLF, comments, custom delim, disabled comment ('\0'), OOM */
  fp = open_tmp_stream();
  ASSERT(fp != NULL);
  fputs("key=val # comment\r\n\nplain;semi\n", fp);
  rewind(fp);

  line = fparseln(fp, &len, &lineno, NULL, 0);
  ASSERT(line != NULL);
  ASSERT_STR_EQ("key=val ", line);
  ASSERT_EQ(1U, lineno);
  free(line);

  /* Empty line (l == 0) with NULL size & NULL lineno */
  line = fparseln(fp, NULL, NULL, "\\\0\0", 0);
  ASSERT(line != NULL);
  ASSERT_STR_EQ("", line);
  free(line);

  /* Custom comment ';' */
  line = fparseln(fp, &len, &lineno, "\\\\;", 0);
  ASSERT(line != NULL);
  ASSERT_STR_EQ("plain", line);
  free(line);

  rewind(fp);
  bsd_stdlib_set_alloc_hooks(mock_malloc, mock_calloc, mock_realloc);
  g_oom_countdown = 1;
  ASSERT(fparseln(fp, &len, &lineno, NULL, 0) == NULL);
  bsd_stdlib_set_alloc_hooks(NULL, NULL, NULL);
  fclose(fp);

  /* No-comment custom str ("\0\0\0") on non-empty line */
  fp = open_tmp_stream();
  ASSERT(fp != NULL);
  fputs("abc#def", fp);
  rewind(fp);
  line = fparseln(fp, &len, NULL, "\0\0\0", 0);
  ASSERT(line != NULL);
  ASSERT_STR_EQ("abc#def", line);
  free(line);
  fclose(fp);

  /* getbsize */
  ASSERT_EQ(0, getbsize(&hlen, &bsize));
  ASSERT_EQ(9, hlen);
  ASSERT_EQ(512L, bsize);
  ASSERT_EQ(0, getbsize(NULL, NULL));

#if !defined(TEST_HAVE_GLIBC_ARC4RANDOM)
  {
    unsigned char rnd[7];
    bsd_stdlib_reset_state();
    /* Default time hook */
    (void)arc4random();
    /* Zero-seed fallback branch: time() ^ 0xa5a5a5a5 == 0 */
    bsd_stdlib_reset_state();
    bsd_stdlib_set_sys_hooks(NULL, NULL, mock_time_fn, NULL);
    g_mock_time_val = (time_t)0xa5a5a5a5UL;
    (void)arc4random();
    bsd_stdlib_set_sys_hooks(NULL, NULL, NULL, NULL);

    arc4random_buf(NULL, 4);
    arc4random_buf(rnd, 0);
    arc4random_buf(rnd, sizeof(rnd));
    ASSERT_EQ(0U, arc4random_uniform(0));
    ASSERT_EQ(0U, arc4random_uniform(1));
    ASSERT(arc4random_uniform(10) < 10U);
  }
#endif

  /* Sorting */
  ASSERT_EQ(-1, mergesort(NULL, 4, sizeof(int), cmp_int));
  ASSERT_EQ(-1, mergesort(arr, 4, sizeof(int), NULL));
  ASSERT_EQ(0, mergesort(arr, 4, sizeof(int), cmp_int));

  arr[0] = 9;
  ASSERT_EQ(-1, heapsort(NULL, 4, sizeof(int), cmp_int));
  ASSERT_EQ(-1, heapsort(arr, 4, sizeof(int), NULL));
  ASSERT_EQ(0, heapsort(arr, 4, sizeof(int), cmp_int));

  ASSERT_EQ(-1, radixsort(NULL, 2, NULL, 0));
  ASSERT_EQ(-1, radixsort(sarr, -1, NULL, 0));
  ASSERT_EQ(0, radixsort(sarr, 0, NULL, 0));

  sarr[0] = (const unsigned char *)"ab";
  sarr[1] = (const unsigned char *)"";
  sarr[2] = (const unsigned char *)"aa";
  sarr[3] = (const unsigned char *)"ab";
  ASSERT_EQ(0, radixsort(sarr, 4, NULL, 0));
  ASSERT_STR_EQ("", (const char *)sarr[0]);

  for (i = 0; i < 256; i++) {
    weight_tbl[i] = (unsigned char)(255 - i);
  }
  sarr[0] = (const unsigned char *)"";
  sarr[1] = (const unsigned char *)"ab";
  sarr[2] = (const unsigned char *)"aa";
  sarr[3] = (const unsigned char *)"";
  ASSERT_EQ(0, sradixsort(sarr, 4, weight_tbl, 0));

#if !defined(__GLIBC__)
  ASSERT_EQ(-1, rpmatch(NULL));
  ASSERT_EQ(-1, rpmatch(""));
  ASSERT_EQ(1, rpmatch("yes"));
  ASSERT_EQ(1, rpmatch("YES"));
  ASSERT_EQ(0, rpmatch("no"));
  ASSERT_EQ(0, rpmatch("NO"));
  ASSERT_EQ(-1, rpmatch("maybe"));
  ASSERT_EQ(0, daemon(1, 1));
#endif

  setproctitle("fmt %d", 1);
  setproctitle_fast("fmt %d", 1);

  ASSERT(devname_r(0, 0, NULL, 8) == NULL);
  ASSERT(devname_r(0, 0, devbuf, 0) == NULL);
  ASSERT_STR_EQ("tty", devname_r(0, 0, devbuf, (int)sizeof(devbuf)));
  ASSERT_STR_EQ("tty", devname(0, 0));

  /* cget* capability database stubs */
  ASSERT_EQ(-1, cgetent(&cap_out, NULL, "vt100"));
  ASSERT(cap_out == NULL);
  ASSERT_EQ(-1, cgetent(NULL, NULL, "vt100"));
  ASSERT(cgetcap(devbuf, "co", ':') == NULL);
  ASSERT_EQ(-1, cgetnum(devbuf, "co", &bsize));
  cap_out = (char *)0x1;
  ASSERT_EQ(-1, cgetstr(devbuf, "cl", &cap_out));
  ASSERT(cap_out == NULL);
  ASSERT_EQ(-1, cgetstr(devbuf, "cl", NULL));
  cap_out = (char *)0x1;
  ASSERT_EQ(-1, cgetustr(devbuf, "cl", &cap_out));
  ASSERT(cap_out == NULL);
  cap_out = (char *)0x1;
  ASSERT_EQ(0, cgetnext(&cap_out, NULL));
  ASSERT(cap_out == NULL);
  ASSERT_EQ(0, cgetnext(NULL, NULL));
  ASSERT_EQ(0, cgetclose());
  ASSERT_EQ(0, cgetset("ent"));
  ASSERT_EQ(-1, cgetmatch(devbuf, "vt100"));

  /* fmtcheck */
  ASSERT_STR_EQ("def", fmtcheck(NULL, "def"));
  ASSERT_STR_EQ("cand", fmtcheck("cand", NULL));
  ASSERT_STR_EQ("def", fmtcheck("%l", "def"));
  ASSERT_STR_EQ("%", fmtcheck("%d", "%"));
  ASSERT_STR_EQ("def", fmtcheck("%#", "def"));
  ASSERT_STR_EQ("%d", fmtcheck("%.4", "%d"));
  ASSERT_STR_EQ("%d", fmtcheck("plain", "%d"));
  ASSERT_STR_EQ("ok %% %#0- +'12.4lld",
                fmtcheck("ok %% %#0- +'12.4lld", "%lld"));
  ASSERT_STR_EQ("%*d %.*s %.s", fmtcheck("%*d %.*s %.s", "%*d %.*s %s"));
  ASSERT_STR_EQ("def", fmtcheck("%d", "def"));
  ASSERT_STR_EQ("%s", fmtcheck("%d", "%s"));
  ASSERT_STR_EQ("%d", fmtcheck("%ld", "%d"));
  ASSERT_STR_EQ("%hhhhhhhhd", fmtcheck("%hhhhhhhhd", "%hhhhhhhhhd"));

  remove("bsd_stdlib_tmp.dat");
  bsd_stdlib_reset_state();
  PASS();
}
#endif

SUITE(suite_bsd_stdlib_core) {
  RUN_TEST(test_bsd_stdlib_alloc_and_closefrom);
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  RUN_TEST(test_bsd_stdlib_progname_strtonum_setmode);
  RUN_TEST(test_bsd_stdlib_io_and_misc);
#endif
}
