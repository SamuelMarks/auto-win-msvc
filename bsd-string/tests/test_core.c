/* clang-format off */
#include "bsd/string.h"
#include "greatest.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
/* clang-format on */

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
extern void *(*g_bsd_string_malloc)(size_t);
extern void bsd_string_reset_malloc_hook(void);

static int g_mock_bsd_str_oom = 0;

static void *mock_bsd_string_malloc(size_t sz) {
  if (g_mock_bsd_str_oom) {
    return NULL;
  }
  return malloc(sz);
}
#endif /* !BSD && !__APPLE__ */

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 38)
#define TEST_HAVE_GLIBC_STRLCPY 1
#endif
#if __GLIBC_PREREQ(2, 25)
#define TEST_HAVE_GLIBC_EXPLICIT_BZERO 1
#endif
#endif

TEST test_bsd_string_strlcpy(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__) && !defined(TEST_HAVE_GLIBC_STRLCPY)
  char dst[10];

  ASSERT_EQ(0U, strlcpy(dst, NULL, sizeof(dst)));
  ASSERT_EQ(5U, strlcpy(NULL, "hello", 0));
  ASSERT_EQ(5U, strlcpy(dst, "hello", 0));
  ASSERT_EQ(5U, strlcpy(dst, "hello", sizeof(dst)));
  ASSERT_STR_EQ("hello", dst);
  ASSERT_EQ(11U, strlcpy(dst, "hello world", sizeof(dst)));
  ASSERT_STR_EQ("hello wor", dst);
#endif
  PASS();
}

TEST test_bsd_string_strlcat(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__) && !defined(TEST_HAVE_GLIBC_STRLCPY)
  char dst[10];

  ASSERT_EQ(0U, strlcat(NULL, "cde", 10));
  memcpy(dst, "ab", 3);
  ASSERT_EQ(0U, strlcat(dst, NULL, sizeof(dst)));
  ASSERT_EQ(4U, strlcat(dst, "cde", 1));
  ASSERT_EQ(5U, strlcat(dst, "cde", sizeof(dst)));
  ASSERT_STR_EQ("abcde", dst);
  ASSERT_EQ(12U, strlcat(dst, "1234567", sizeof(dst)));
  ASSERT_STR_EQ("abcde1234", dst);
#endif
  PASS();
}

TEST test_bsd_string_strnstr_strmode_bzero(void) {
  char buf[8];

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__) &&  \
    !defined(TEST_HAVE_GLIBC_EXPLICIT_BZERO)
  memcpy(buf, "secret!", 8);
  explicit_bzero(NULL, 4);
  explicit_bzero(buf, 0);
  explicit_bzero(buf, sizeof(buf));
  ASSERT_EQ(0, buf[0]);
#else
  (void)buf;
#endif

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
  ASSERT_EQ(0, timingsafe_memcmp("abc", "abc", 0));
  ASSERT_EQ(0, timingsafe_memcmp("abc", "abc", 3));
  ASSERT(timingsafe_memcmp("abc", "abd", 3) < 0);
  ASSERT(timingsafe_memcmp("abd", "abc", 3) > 0);
#endif

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  (void)0;
  {
    char modebuf[16];
    char *ffstr;
    char ffin1[] = "nodump,,nouchg";
    char ffin2[] = "bogusflag";
    char *ffp = NULL;
    unsigned long setf = 0;
    unsigned long clrf = 0;
    int types[8];
    int perms[8];
    size_t i;

    types[0] = S_IFDIR;
    types[1] = S_IFCHR;
#ifdef S_IFBLK
    types[2] = S_IFBLK;
#else
    types[2] = 0060000;
#endif
    types[3] = S_IFREG;
    types[4] = 0120000; /* S_IFLNK */
    types[5] = 0140000; /* S_IFSOCK */
    types[6] = 0010000; /* S_IFIFO */
    types[7] = 0;

    perms[0] = 0;
    perms[1] = 0777;
    perms[2] = 04000;
    perms[3] = 04700;
    perms[4] = 02000;
    perms[5] = 02070;
    perms[6] = 01000;
    perms[7] = 01007;

    ASSERT(strnstr(NULL, "a", 1) == NULL);
    ASSERT(strnstr("abc", NULL, 3) == NULL);
    ASSERT_STR_EQ("abc", strnstr("abc", "", 0));
    ASSERT(strnstr("hello world", "world", 11) != NULL);
    ASSERT(strnstr("hello world", "world", 5) == NULL);
    ASSERT(strnstr("hi", "xyz", 10) == NULL);

    strmode(0755, NULL);
    for (i = 0; i < 8; ++i) {
      strmode(types[i] | perms[i], modebuf);
    }

    ASSERT_EQ(0, timingsafe_bcmp("abc", "abc", 0));
    ASSERT_EQ(0, timingsafe_bcmp("abc", "abc", 3));
    ASSERT_NEQ(0, timingsafe_bcmp("abc", "abd", 3));

    /* OOM in fflagstostr via g_bsd_string_malloc */
    g_bsd_string_malloc = mock_bsd_string_malloc;
    g_mock_bsd_str_oom = 1;
    ASSERT(fflagstostr(1UL) == NULL);
    g_mock_bsd_str_oom = 0;
    bsd_string_reset_malloc_hook();

    ffstr = fflagstostr(0UL);
    ASSERT(ffstr != NULL);
    ASSERT_STR_EQ("", ffstr);
    free(ffstr);

    ffstr = fflagstostr(0x00000001UL | 0x00010000UL);
    ASSERT(ffstr != NULL);
    free(ffstr);

    ASSERT_EQ(0, strtofflags(NULL, NULL, NULL));
    ffp = NULL;
    ASSERT_EQ(0, strtofflags(&ffp, &setf, &clrf));
    ffp = ffin1;
    ASSERT_EQ(0, strtofflags(&ffp, &setf, &clrf));
    ASSERT_EQ(0x00000001UL, setf);
    ASSERT_EQ(0x00000002UL, clrf);
    memcpy(ffin1, "nodump", 7);
    ffp = ffin1;
    ASSERT_EQ(0, strtofflags(&ffp, NULL, NULL));
    ffp = ffin2;
    ASSERT_EQ(1, strtofflags(&ffp, &setf, &clrf));
  }
#endif
  PASS();
}

TEST test_bsd_string_win_extensions(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  char lstr[] = "a,b";
  char *lp = lstr;
  char *lnull = NULL;

  ASSERT(strsep(NULL, ",") == NULL);
  ASSERT(strsep(&lp, NULL) == NULL);
  ASSERT(strsep(&lnull, ",") == NULL);
  ASSERT_STR_EQ("a", strsep(&lp, ","));
  ASSERT_STR_EQ("b", strsep(&lp, ","));
  ASSERT(strsep(&lp, ",") == NULL);

#if defined(_MSC_VER)
  ASSERT_EQ(0, strcasecmp(NULL, NULL));
  ASSERT_EQ(1, strcasecmp("a", NULL));
  ASSERT_EQ(-1, strcasecmp(NULL, "a"));
  ASSERT_EQ(0, strncasecmp(NULL, NULL, 3));
  ASSERT_EQ(1, strncasecmp("a", NULL, 3));
  ASSERT_EQ(-1, strncasecmp(NULL, "a", 3));
#endif
  ASSERT_EQ(0, strcasecmp("abc", "ABC"));
  ASSERT_EQ(0, strncasecmp("abcdef", "ABCxyz", 3));

  ASSERT_EQ(0, strverscmp(NULL, NULL));
  ASSERT_EQ(1, strverscmp("a", NULL));
  ASSERT_EQ(-1, strverscmp(NULL, "a"));
  ASSERT_EQ(0, strverscmp("1.0", "1.0"));

  ASSERT(strcasestr(NULL, "a") == NULL);
  ASSERT(strcasestr("a", NULL) == NULL);
  ASSERT_STR_EQ("Hello", strcasestr("Hello", ""));
  ASSERT(strcasestr("Hello World", "world") != NULL);
  ASSERT(strcasestr("Hello", "xyz") == NULL);
#endif
  PASS();
}

TEST test_bsd_string_dummy(void) {
  ASSERT_EQ(0, bsd_string_dummy_for_coverage());
  PASS();
}

SUITE(suite_bsd_string_core) {
  RUN_TEST(test_bsd_string_strlcpy);
  RUN_TEST(test_bsd_string_strlcat);
  RUN_TEST(test_bsd_string_strnstr_strmode_bzero);
  RUN_TEST(test_bsd_string_win_extensions);
  RUN_TEST(test_bsd_string_dummy);
}
