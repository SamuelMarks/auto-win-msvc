#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-statfs.h"
#include "mntent.h"
#include <errno.h>
#include <stdio.h>
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(__CYGWIN__)
#include <sys/statvfs.h>
#endif
#if defined(__APPLE__)
#include <sys/param.h>
#include <sys/mount.h>
#endif
/* clang-format on */

TEST test_linux_sys_statfs_init(void) {
  int rc = 0;
  int status = 0;

  status = 0;
  rc = linux_sys_statfs_init(NULL);

  rc = linux_sys_statfs_init(&status);
  (void)rc;
  (void)status;

  PASS();
}

TEST test_statfs_fstatfs(void) {
#if defined(_WIN32)
  struct statfs buf;
  int rc = statfs("/", &buf);
  (void)rc;

  rc = fstatfs(0, &buf);

#else
  struct statfs buf;
  int rc = statfs("/", &buf);
  (void)rc;

  rc = fstatfs(0, &buf);

#endif
  PASS();
}

TEST test_statvfs_fstatvfs(void) {
#if defined(_WIN32)
  struct statvfs buf;
  int rc = statvfs("/", &buf);
  (void)rc;

  rc = fstatvfs(0, &buf);

#else
  struct statvfs buf;
  int rc = statvfs("/", &buf);
  (void)rc;

  rc = fstatvfs(0, &buf);

#endif
  PASS();
}
SUITE(suite_linux_sys_statfs_core) {
  RUN_TEST(test_linux_sys_statfs_init);
  RUN_TEST(test_statfs_fstatfs);
  RUN_TEST(test_statvfs_fstatvfs);
}

#ifndef _WIN32
#define DEFINED_WIN32_FOR_TEST
#define _WIN32 1
#endif

#define setmntent mock_setmntent
#define getmntent mock_getmntent
#define endmntent mock_endmntent
#define hasmntopt mock_hasmntopt
#define GetLogicalDriveStringsA mock_GetLogicalDriveStringsA_fn

#define MOCK_GETLOGICALDRIVESTRINGS 1
typedef unsigned long WIN_DWORD;

static WIN_DWORD mock_GetLogicalDriveStringsA_ret = 0;
static const char *mock_GetLogicalDriveStringsA_buf = NULL;
static WIN_DWORD mock_GetLogicalDriveStringsA_size = 0;

static WIN_DWORD mock_GetLogicalDriveStringsA_fn(WIN_DWORD nBufferLength,
                                                 char *lpBuffer) {
  (void)nBufferLength;
  if (mock_GetLogicalDriveStringsA_buf) {
    size_t len = mock_GetLogicalDriveStringsA_size;
    if (len > 0) {
      memcpy(lpBuffer, mock_GetLogicalDriveStringsA_buf, len);
    }
  }
  return mock_GetLogicalDriveStringsA_ret;
}

#include "../src/mntent.c"

#undef GetLogicalDriveStringsA

#ifdef DEFINED_WIN32_FOR_TEST
#undef _WIN32
#endif

TEST test_mntent_branches(void) {
  FILE *f;
  struct mntent *m;
  char *opt;
  char buf[32];
  (void)m;
  (void)opt;
  memcpy(buf, "C:\\\\\\\0D:/\0E:\0F\0\0", 17);

  /* NULLs */
  f = setmntent(NULL, "r");
  ASSERT(f == NULL);
  f = setmntent("file", NULL);
  ASSERT(f == NULL);

  /* getmntent with no stream */
  m = getmntent(NULL);
  ASSERT(m == NULL);

  f = setmntent("file", "r");
  s_drive_ptr = NULL;

  /* getmntent with NULL s_drive_ptr */
  m = getmntent(f);
  ASSERT(m == NULL);

  /* endmntent */
  endmntent(NULL);
  endmntent(f);

  /* hasmntopt */
  hasmntopt(NULL, "rw");
  hasmntopt(&s_mnt, NULL);

  s_mnt.mnt_opts = NULL;
  hasmntopt(&s_mnt, "rw");

  s_mnt.mnt_opts = "rw";
  opt = hasmntopt(&s_mnt, "rw");

  /* setmntent fail cases */
  mock_GetLogicalDriveStringsA_ret = 0;
  f = setmntent("file", "r");
  ASSERT(f == NULL);

  mock_GetLogicalDriveStringsA_ret = 1024; /* greater than buffer size */
  f = setmntent("file", "r");
  ASSERT(f == NULL);

  /* Success case */
  mock_GetLogicalDriveStringsA_ret = 17;
  mock_GetLogicalDriveStringsA_size = 17;
  mock_GetLogicalDriveStringsA_buf = buf;
  f = setmntent("file", "r");
  ASSERT(f != NULL);

  /* getmntent normal */
  m = getmntent(f);
  ASSERT(m != NULL);

  /* check strip slash */
  m = getmntent(f);
  ASSERT(m != NULL);

  m = getmntent(f);
  ASSERT(m != NULL);

  m = getmntent(f);
  ASSERT(m != NULL);

  /* no more */
  m = getmntent(f);
  ASSERT(m == NULL);

  /* getmntent when s_drive_ptr is exactly pointing at \0 */
  s_drive_ptr = "\0";
  m = getmntent(f);
  ASSERT(m == NULL);

  PASS();
}

#undef setmntent
#undef getmntent
#undef endmntent
#undef hasmntopt

TEST test_native_mntent(void) {
  FILE *f;
  struct mntent *m;

#if !defined(__linux__) && !defined(__GLIBC__)
  f = setmntent(NULL, "r");
  ASSERT(f == NULL);
  f = setmntent("/etc/mtab", NULL);
  ASSERT(f == NULL);
#endif

  f = setmntent("/etc/mtab", "r");
  if (f != NULL) {
    m = getmntent(f);
    if (m != NULL) {
      ASSERT(m->mnt_dir != NULL);
      (void)hasmntopt(m, "rw");
    }
#if !defined(__linux__) && !defined(__GLIBC__)
    ASSERT(getmntent(NULL) == NULL);
    ASSERT(hasmntopt(NULL, "rw") == NULL);
    ASSERT(hasmntopt(m, NULL) == NULL);
#endif
    ASSERT_EQ(1, endmntent(f));
  }
#if !defined(__linux__) && !defined(__GLIBC__)
  ASSERT_EQ(1, endmntent(NULL));
#endif
  PASS();
}

SUITE(suite_linux_sys_statfs_extra) {
  RUN_TEST(test_mntent_branches);
  RUN_TEST(test_native_mntent);
}
