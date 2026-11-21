/* clang-format off */
#include "greatest.h"
#include "posix-strings.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_strings_get_info(void) {
  int info = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_strings_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_strings_get_info(&info));
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_strings_case(void) {
  ASSERT_EQ(0, strcasecmp("hello", "HELLO"));
  ASSERT(strcasecmp("apple", "banana") < 0);
  ASSERT(strcasecmp("banana", "apple") > 0);

  ASSERT_EQ(0, strncasecmp("hello world", "HELLO there", 5));
  ASSERT(strncasecmp("abc", "abd", 3) < 0);
  PASS();
}

TEST test_strings_bops(void) {
  char buf1[16];
  char buf2[16];

  memset(buf1, 'A', sizeof(buf1));
  bzero(buf1, sizeof(buf1));
  ASSERT_EQ(0, buf1[0]);
  ASSERT_EQ(0, buf1[15]);

#if defined(_MSC_VER)
  strcpy_s(buf1, sizeof(buf1), "test1234");
#else
  strcpy(buf1, "test1234");
#endif
  bcopy(buf1, buf2, 9);
  ASSERT_STR_EQ("test1234", buf2);

  ASSERT_EQ(0, bcmp(buf1, buf2, 9));
  buf2[0] = 'x';
  ASSERT(bcmp(buf1, buf2, 9) != 0);

  PASS();
}

TEST test_strtok_r(void) {
  char str[32];
  char *saveptr = NULL;
  char *token;

#if defined(_MSC_VER)
  strcpy_s(str, sizeof(str), "apple,banana,cherry");
#else
  strcpy(str, "apple,banana,cherry");
#endif

  token = strtok_r(str, ",", &saveptr);
  ASSERT_STR_EQ("apple", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_STR_EQ("banana", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_STR_EQ("cherry", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_EQ(NULL, token);

  PASS();
}

TEST test_ffs(void) {
  ASSERT_EQ(0, ffs(0));
  ASSERT_EQ(1, ffs(1));
  ASSERT_EQ(2, ffs(2));
  ASSERT_EQ(1, ffs(3));
  ASSERT_EQ(3, ffs(4));
  ASSERT_EQ(5, ffs(16));
#if defined(__I86__)
  ASSERT_EQ(16, ffs((int)0x8000U));
#else
  ASSERT_EQ(32, ffs((int)0x80000000U));
#endif
  PASS();
}

TEST test_ffsl(void) {
  ASSERT_EQ(0, ffsl(0));
  ASSERT_EQ(1, ffsl(1));
  ASSERT_EQ(2, ffsl(2));
  ASSERT_EQ(4, ffsl(8));
  ASSERT_EQ(5, ffsl(16));
  PASS();
}

TEST test_ffsll(void) {
  posix_strings_llong val;
  ASSERT_EQ(0, ffsll(0));
  ASSERT_EQ(1, ffsll(1));
  ASSERT_EQ(2, ffsll(2));
  ASSERT_EQ(9, ffsll(256));

  val = (posix_strings_llong)1 << 40;
  ASSERT_EQ(41, ffsll(val));

  PASS();
}

TEST test_posix_strings_extensions(void) {
  char dst[16];
  char *endp;
  void *mp;
  const char *sample = "abca";
  int sigs[] = {1,  2,  3,  4,  6,  8,  9,  11, 13,
                14, 15, 17, 18, 19, 20, 21, 22, 28};
  size_t i;

  /* posix_stpcpy: empty & non-empty */
  endp = posix_stpcpy(dst, "");
  ASSERT_EQ(dst, endp);
  ASSERT_STR_EQ("", dst);

  endp = posix_stpcpy(dst, "hi");
  ASSERT_EQ(dst + 2, endp);
  ASSERT_STR_EQ("hi", dst);

  /* posix_stpncpy: n==0, short src with NUL padding, exact/truncated src */
  memset(dst, 'X', sizeof(dst));
  endp = posix_stpncpy(dst, "hi", 0);
  ASSERT_EQ(dst, endp);

  memset(dst, 'X', sizeof(dst));
  endp = posix_stpncpy(dst, "hi", 5);
  ASSERT_EQ(dst + 2, endp);
  ASSERT_EQ('\0', dst[2]);
  ASSERT_EQ('\0', dst[3]);
  ASSERT_EQ('\0', dst[4]);

  memset(dst, 'X', sizeof(dst));
  endp = posix_stpncpy(dst, "abcdef", 3);
  ASSERT_EQ(dst + 3, endp);
  ASSERT_EQ('a', dst[0]);
  ASSERT_EQ('c', dst[2]);

  /* posix_strchrnul: match at start, match in middle, NUL search, miss */
  ASSERT_EQ(sample, posix_strchrnul(sample, 'a'));
  ASSERT_EQ(sample + 2, posix_strchrnul(sample, 'c'));
  ASSERT_EQ(sample + 4, posix_strchrnul(sample, '\0'));
  ASSERT_EQ(sample + 4, posix_strchrnul(sample, 'z'));

  /* posix_mempcpy & posix_memrchr */
  mp = posix_mempcpy(dst, "xyz", 4);
  ASSERT_EQ((void *)(dst + 4), mp);
  ASSERT_STR_EQ("xyz", dst);

  ASSERT_EQ(NULL, posix_memrchr(sample, 'a', 0));
  ASSERT_EQ((const void *)(sample + 3), posix_memrchr(sample, 'a', 4));
  ASSERT_EQ((const void *)(sample + 1), posix_memrchr(sample, 'b', 4));
  ASSERT_EQ(NULL, posix_memrchr(sample, 'z', 4));

  /* posix_strsignal: all known signals + unknown default */
  for (i = 0; i < sizeof(sigs) / sizeof(sigs[0]); ++i) {
    char *desc = posix_strsignal(sigs[i]);
    ASSERT(desc != NULL);
    ASSERT(strlen(desc) > 0);
  }
  ASSERT_STR_EQ("Unknown signal 999", posix_strsignal(999));

  PASS();
}

#if defined(_WIN32)
#undef mempcpy
#undef strchrnul
#undef stpcpy
#undef stpncpy
#undef memrchr
#undef strsignal
#undef strtok_r

extern void *mempcpy(void *dest, const void *src, size_t n);
extern char *strchrnul(const char *s, int c);
extern char *stpcpy(char *dest, const char *src);
extern char *stpncpy(char *dest, const char *src, size_t n);
extern void *memrchr(const void *s, int c, size_t n);
extern char *strsignal(int sig);
extern char *strtok_r(char *str, const char *delim, char **saveptr);

TEST test_win32_abi_wrappers(void) {
  char buf[16];
  char tokbuf[16];
  char *save = NULL;

  ASSERT_EQ((void *)(buf + 3), mempcpy(buf, "ab", 3));
  ASSERT_EQ(buf + 1, strchrnul(buf, 'b'));
  ASSERT_EQ(buf + 2, stpcpy(buf, "cd"));
  ASSERT_EQ(buf + 2, stpncpy(buf, "ef", 4));
  ASSERT_EQ((void *)(buf + 1), memrchr(buf, 'f', 2));
  ASSERT_STR_EQ("Terminated", strsignal(15));

#if defined(_MSC_VER)
  strcpy_s(tokbuf, sizeof(tokbuf), "x:y");
#else
  strcpy(tokbuf, "x:y");
#endif
  ASSERT_STR_EQ("x", strtok_r(tokbuf, ":", &save));
  ASSERT_STR_EQ("y", strtok_r(NULL, ":", &save));

  PASS();
}
#endif

SUITE(suite_posix_strings_core) {
  RUN_TEST(test_posix_strings_get_info);
  RUN_TEST(test_strings_case);
  RUN_TEST(test_strings_bops);
  RUN_TEST(test_strtok_r);
  RUN_TEST(test_ffs);
  RUN_TEST(test_ffsl);
  RUN_TEST(test_ffsll);
  RUN_TEST(test_posix_strings_extensions);
#if defined(_WIN32)
  RUN_TEST(test_win32_abi_wrappers);
#endif
}
