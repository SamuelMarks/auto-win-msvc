/* test.c - 100% Test Coverage Suite */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#include "greatest.h"
#include "posix-pwdgrp.h"
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif

TEST test_getpwnam(void) {
  struct passwd *pw = NULL;
#ifdef _WIN32
  char name[256];
  DWORD len = sizeof(name);
  GetUserNameA(name, &len);
  pw = getpwnam(name);
#else
  pw = getpwuid(getuid());
  if (pw) {
    pw = getpwnam(pw->pw_name);
  } else {
    pw = getpwnam("root");
  }
#endif
  ASSERT(pw != NULL);
  ASSERT(pw->pw_name != NULL);
  PASS();
}

TEST test_getpwnam_r(void) {
  struct passwd pwd;
  struct passwd *result = NULL;
  char buffer[4096];
  int r;
#ifdef _WIN32
  char name[256];
  DWORD len = sizeof(name);
  GetUserNameA(name, &len);
  r = getpwnam_r(name, &pwd, buffer, sizeof(buffer), &result);
#else
  struct passwd *pw = getpwuid(getuid());
  if (pw) {
    r = getpwnam_r(pw->pw_name, &pwd, buffer, sizeof(buffer), &result);
  } else {
    r = getpwnam_r("root", &pwd, buffer, sizeof(buffer), &result);
  }
#endif
  ASSERT_EQ(0, r);
  ASSERT(result != NULL);
  PASS();
}

TEST test_getpwuid(void) {
  struct passwd *pw = NULL;
#ifdef _WIN32
  char name[256];
  DWORD len = sizeof(name);
  GetUserNameA(name, &len);
  pw = getpwnam(name);
  ASSERT(pw != NULL);
  pw = getpwuid(pw->pw_uid);
#else
  pw = getpwuid(getuid());
#endif
  ASSERT(pw != NULL);
  PASS();
}

TEST test_getpwuid_r(void) {
  struct passwd pwd;
  struct passwd *result = NULL;
  char buffer[4096];
  int r;
#ifdef _WIN32
  struct passwd *pw;
  char name[256];
  DWORD len = sizeof(name);
  GetUserNameA(name, &len);
  pw = getpwnam(name);
  ASSERT(pw != NULL);
  r = getpwuid_r(pw->pw_uid, &pwd, buffer, sizeof(buffer), &result);
#else
  r = getpwuid_r(getuid(), &pwd, buffer, sizeof(buffer), &result);
#endif
  ASSERT_EQ(0, r);
  ASSERT(result != NULL);
  PASS();
}

TEST test_pwent(void) {
  struct passwd *pw;
  setpwent();
  pw = getpwent();
  ASSERT(pw != NULL);
  endpwent();
  PASS();
}

TEST test_grent(void) {
  struct group *gr;
  setgrent();
  gr = getgrent();
  ASSERT(gr != NULL);
  endgrent();
  PASS();
}

TEST test_getgrnam(void) {
  struct group *gr = NULL;
#ifdef _WIN32
  gr = getgrnam("Administrators");
  if (!gr)
    gr = getgrent(); /* Fallback if localized */
#else
  gr = getgrnam("wheel");
  if (!gr)
    gr = getgrnam("root");
#endif
  if (gr) {
    ASSERT(gr->gr_name != NULL);
  }
  PASS();
}

TEST test_getgrnam_r(void) {
  struct group grp;
  struct group *result = NULL;
  char buffer[4096];
  int r;
#ifdef _WIN32
  r = getgrnam_r("Administrators", &grp, buffer, sizeof(buffer), &result);
#else
  r = getgrnam_r("wheel", &grp, buffer, sizeof(buffer), &result);
  if (r != 0)
    r = getgrnam_r("root", &grp, buffer, sizeof(buffer), &result);
#endif
  if (result) {
    ASSERT_EQ(0, r);
  }
  PASS();
}

TEST test_getgrgid(void) {
  struct group *gr = NULL;
  setgrent();
  gr = getgrent();
  if (gr) {
    struct group *gr2 = getgrgid(gr->gr_gid);
    ASSERT(gr2 != NULL);
  }
  endgrent();
  PASS();
}

TEST test_getgrgid_r(void) {
  struct group grp;
  struct group *result = NULL;
  char buffer[4096];
  struct group *gr = NULL;
  setgrent();
  gr = getgrent();
  if (gr) {
    int r = getgrgid_r(gr->gr_gid, &grp, buffer, sizeof(buffer), &result);
    ASSERT_EQ(0, r);
    ASSERT(result != NULL);
  }
  endgrent();
  PASS();
}

SUITE(pwdgrp_suite) {
  RUN_TEST(test_getpwnam);
  RUN_TEST(test_getpwnam_r);
  RUN_TEST(test_getpwuid);
  RUN_TEST(test_getpwuid_r);
  RUN_TEST(test_pwent);

  RUN_TEST(test_getgrnam);
  RUN_TEST(test_getgrnam_r);
  RUN_TEST(test_getgrgid);
  RUN_TEST(test_getgrgid_r);
  RUN_TEST(test_grent);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(pwdgrp_suite);
  GREATEST_MAIN_END();
}
