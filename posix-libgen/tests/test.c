/* test.c - 100% Test Coverage Stubs */
#undef _GNU_SOURCE
#include "greatest.h"
#include "posix-libgen.h"
#include <stdio.h>
#include <string.h>

TEST test_basename_null_empty(void) {
  char empty[] = "";
  ASSERT_STR_EQ(".", basename(NULL));
  ASSERT_STR_EQ(".", basename(empty));
  PASS();
}

TEST test_basename_all_slashes(void) {
  char slash1[] = "/";
  char slash2[] = "//";
  char bslash1[] = "\\";
  ASSERT_STR_EQ("/", basename(slash1));
  ASSERT_STR_EQ("/", basename(slash2));
  ASSERT_STR_EQ("/", basename(bslash1));
  PASS();
}

TEST test_basename_standard(void) {
  char p1[] = "usr";
  char p2[] = "usr/lib";
  char p3[] = "/usr/lib";
  char p4[] = "usr/lib/";

  ASSERT_STR_EQ("usr", basename(p1));
  ASSERT_STR_EQ("lib", basename(p2));
  ASSERT_STR_EQ("lib", basename(p3));
  ASSERT_STR_EQ("lib", basename(p4));
  PASS();
}

TEST test_basename_windows(void) {
  char p1[] = "C:\\usr\\lib";
  char p2[] = "C:\\usr\\lib\\";
  char p3[] = "C:";
  char p4[] = "C:\\";

  ASSERT_STR_EQ("lib", basename(p1));
  ASSERT_STR_EQ("lib", basename(p2));
  ASSERT_STR_EQ(".", basename(p3));
  ASSERT_STR_EQ(".", basename(p4));
  PASS();
}

TEST test_dirname_null_empty(void) {
  char empty[] = "";
  ASSERT_STR_EQ(".", dirname(NULL));
  ASSERT_STR_EQ(".", dirname(empty));
  PASS();
}

TEST test_dirname_all_slashes(void) {
  char slash1[] = "/";
  char slash2[] = "//";
  char bslash1[] = "\\";
  ASSERT_STR_EQ("/", dirname(slash1));
  ASSERT_STR_EQ("/", dirname(slash2));
  ASSERT_STR_EQ("/", dirname(bslash1));
  PASS();
}

TEST test_dirname_standard(void) {
  char p1[] = "usr";
  char p2[] = "usr/lib";
  char p3[] = "/usr/lib";
  char p4[] = "usr/lib/";
  char p5[] = "/";
  char p6[] = "/usr";

  ASSERT_STR_EQ(".", dirname(p1));
  ASSERT_STR_EQ("usr", dirname(p2));
  ASSERT_STR_EQ("/usr", dirname(p3));
  ASSERT_STR_EQ("usr", dirname(p4));
  ASSERT_STR_EQ("/", dirname(p5));
  ASSERT_STR_EQ("/", dirname(p6));
  PASS();
}

TEST test_dirname_windows(void) {
  char p1[] = "C:\\usr\\lib";
  char p2[] = "C:\\usr\\lib\\";
  char p3[] = "C:";
  char p4[] = "C:\\";
  char p5[] = "C:\\usr";

  ASSERT_STR_EQ("C:\\usr", dirname(p1));
  ASSERT_STR_EQ("C:\\usr", dirname(p2));
  ASSERT_STR_EQ("C:", dirname(p3));
  ASSERT_STR_EQ("C:", dirname(p4));
  ASSERT_STR_EQ("C:\\", dirname(p5));
  PASS();
}

SUITE(basename_suite) {
  RUN_TEST(test_basename_null_empty);
  RUN_TEST(test_basename_all_slashes);
  RUN_TEST(test_basename_standard);
  RUN_TEST(test_basename_windows);
}

SUITE(dirname_suite) {
  RUN_TEST(test_dirname_null_empty);
  RUN_TEST(test_dirname_all_slashes);
  RUN_TEST(test_dirname_standard);
  RUN_TEST(test_dirname_windows);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(basename_suite);
  RUN_SUITE(dirname_suite);
  GREATEST_MAIN_END();
}
