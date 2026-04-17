/* test.c - 100% Test Coverage Stubs */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
/* clang-format off */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef _WIN32
#define _open open
#define _close close
#define _unlink unlink
#define _rmdir rmdir
#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_CREAT O_CREAT
#define _O_TRUNC O_TRUNC
#endif

#include "greatest.h"
#include "posix-stat.h"
/* clang-format on */

#ifdef _WIN32
#include <windows.h>
#endif

TEST test_fchmodat(void) {
  int fd;
  int res;
#ifdef _WIN32
  HANDLE hDir = CreateFileA(
      ".", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  int dirfd = _open_osfhandle((intptr_t)hDir, _O_RDONLY);
#else
  int dirfd = _open(".", _O_RDONLY);
#endif

  fd = _open("fchmodat_test.txt", _O_CREAT | _O_WRONLY, 0666);
  ASSERT(fd >= 0);
  _close(fd);

  res = fchmodat(AT_FDCWD, "fchmodat_test.txt", 0644, 0);
  ASSERT_EQ(0, res);

  res = fchmodat(dirfd, "fchmodat_test.txt", 0666, 0);
  ASSERT_EQ(0, res);

  _unlink("fchmodat_test.txt");
  _close(dirfd);

  PASS();
}

TEST test_fstatat(void) {
  struct stat st;
  int res;
  char abs_path[1024];
#ifdef _WIN32
  HANDLE hDir = CreateFileA(
      ".", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  int dirfd = _open_osfhandle((intptr_t)hDir, _O_RDONLY);
#else
  int dirfd = _open(".", _O_RDONLY);
#endif

  res = fstatat(AT_FDCWD, "CMakeLists.txt", &st, 0);
  ASSERT_EQ(0, res);

  res = fstatat(dirfd, "CMakeLists.txt", &st, 0);
  ASSERT_EQ(0, res);

  /* Absolute path test to cover IS_ABSOLUTE_PATH */
#if defined(_WIN32)
  if (_fullpath(abs_path, "CMakeLists.txt", sizeof(abs_path))) {
    res = fstatat(dirfd, abs_path, &st, 0);
    ASSERT_EQ(0, res);
  }
#else
  if (realpath("CMakeLists.txt", abs_path)) {
    res = fstatat(dirfd, abs_path, &st, 0);
    ASSERT_EQ(0, res);
  }
#endif

  _close(dirfd);

  PASS();
}

TEST test_mknodat(void) {
  int res;
#ifdef _WIN32
  HANDLE hDir = CreateFileA(
      ".", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  int dirfd = _open_osfhandle((intptr_t)hDir, _O_RDONLY);
#else
  int dirfd = _open(".", _O_RDONLY);
#endif

  res = mknodat(AT_FDCWD, "mknodat_test.txt", S_IFREG | 0666, 0);
  ASSERT_EQ(0, res);
  _unlink("mknodat_test.txt");

  res = mknodat(dirfd, "mknodat_test.txt", S_IFREG | 0666, 0);
  ASSERT_EQ(0, res);
  _unlink("mknodat_test.txt");

  _close(dirfd);

  PASS();
}

TEST test_utimensat(void) {
  int fd;
  int res;
  struct timespec times[2];
#ifdef _WIN32
  HANDLE hDir = CreateFileA(
      ".", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  int dirfd = _open_osfhandle((intptr_t)hDir, _O_RDONLY);
#else
  int dirfd = _open(".", _O_RDONLY);
#endif

  fd = _open("utimensat_test.txt", _O_CREAT | _O_WRONLY, 0666);
  ASSERT(fd >= 0);
  _close(fd);

  times[0].tv_sec = 0;
  times[0].tv_nsec = UTIME_NOW;
  times[1].tv_sec = 0;
  times[1].tv_nsec = UTIME_NOW;

  res = utimensat(AT_FDCWD, "utimensat_test.txt", times, 0);
  ASSERT_EQ(0, res);

  res = utimensat(dirfd, "utimensat_test.txt", times, 0);
  ASSERT_EQ(0, res);

  _unlink("utimensat_test.txt");
  _close(dirfd);

  PASS();
}

TEST test_futimens(void) {
  int fd;
  int res;
  struct timespec times[2];

  fd = _open("futimens_test.txt", _O_CREAT | _O_WRONLY, 0666);
  ASSERT(fd >= 0);

  times[0].tv_sec = 0;
  times[0].tv_nsec = UTIME_NOW;
  times[1].tv_sec = 0;
  times[1].tv_nsec = UTIME_NOW;

  res = futimens(fd, times);
  ASSERT_EQ(0, res);

  _close(fd);
  _unlink("futimens_test.txt");

  PASS();
}

SUITE(posix_stat_suite);

TEST test_stat(void) {
  struct stat st;
  int stat_res = stat("CMakeLists.txt", &st);
  ASSERT_EQ(0, stat_res);
  ASSERT(S_ISREG(st.st_mode));
  PASS();
}

TEST test_fstat(void) {
  struct stat st;
  int fd = _open("CMakeLists.txt", _O_RDONLY);
  int res;
  ASSERT(fd >= 0);
  res = fstat(fd, &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));
  _close(fd);
  PASS();
}

TEST test_mkdir_chmod_umask(void) {
  struct stat st;
  int res;
  mode_t old_mask = (mode_t)umask(0022);

  res = mkdir("test_dir", 0755);
  if (res == -1 && errno == EEXIST) {
    _rmdir("test_dir");
    res = mkdir("test_dir", 0755);
  }
  ASSERT_EQ(0, res);

  res = chmod("test_dir", 0777);
  ASSERT_EQ(0, res);

  ASSERT_EQ(0, stat("test_dir", &st));
  ASSERT(S_ISDIR(st.st_mode));

  _rmdir("test_dir");
  umask(old_mask);
  PASS();
}

TEST test_fchmod_fchmodat(void) {
  int fd = _open("test_fchmod.txt", _O_WRONLY | _O_CREAT | _O_TRUNC, 0644);
  int res;
  ASSERT(fd >= 0);
  res = fchmod(fd, 0444);
  if (res == -1) {
    /* Acceptable if EINVAL on old Windows */
    ASSERT(errno == EINVAL || errno == EACCES || errno == EBADF);
  } else {
    ASSERT_EQ(0, res);
  }
  _close(fd);

  res = fchmodat(AT_FDCWD, "test_fchmod.txt", 0644, 0);
  ASSERT_EQ(0, res);

  _unlink("test_fchmod.txt");
  PASS();
}

TEST test_fstatat_lstat(void) {
  struct stat st;
  int fd = _open("test_lstat.txt", _O_WRONLY | _O_CREAT | _O_TRUNC, 0644);
  int res;
  _close(fd);

  res = fstatat(AT_FDCWD, "test_lstat.txt", &st, 0);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));

  res = lstat("test_lstat.txt", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));

  _unlink("test_lstat.txt");
  PASS();
}

TEST test_futimens_utimensat(void) {
  int fd = _open("test_time.txt", _O_WRONLY | _O_CREAT | _O_TRUNC, 0644);
  struct timespec ts[2];
  int res;

  ASSERT(fd >= 0);

  ts[0].tv_sec = 1000000;
  ts[0].tv_nsec = 0;
  ts[1].tv_sec = 1000000;
  ts[1].tv_nsec = 0;

  res = futimens(fd, ts);
  ASSERT_EQ(0, res);
  _close(fd);

  res = utimensat(AT_FDCWD, "test_time.txt", ts, 0);
  ASSERT_EQ(0, res);

  /* Test UTIME_NOW and UTIME_OMIT */
  ts[0].tv_nsec = UTIME_NOW;
  ts[1].tv_nsec = UTIME_OMIT;
  res = utimensat(AT_FDCWD, "test_time.txt", ts, 0);
  ASSERT_EQ(0, res);

  /* Test NULL times */
  res = utimensat(AT_FDCWD, "test_time.txt", NULL, 0);
  ASSERT_EQ(0, res);

  _unlink("test_time.txt");
  PASS();
}

TEST test_mknod_mknodat(void) {
  struct stat st;
  int res;

  _unlink("test_mknod.txt");
  res = mknod("test_mknod.txt", S_IFREG | 0644, 0);
  ASSERT_EQ(0, res);
  ASSERT_EQ(0, stat("test_mknod.txt", &st));
  _unlink("test_mknod.txt");

  _unlink("test_mknodat.txt");
  res = mknodat(AT_FDCWD, "test_mknodat.txt", S_IFREG | 0644, 0);
  ASSERT_EQ(0, res);
  ASSERT_EQ(0, stat("test_mknodat.txt", &st));
  _unlink("test_mknodat.txt");
  PASS();
}

SUITE(posix_stat_suite) {
  RUN_TEST(test_fchmodat);
  RUN_TEST(test_fstatat);
  RUN_TEST(test_mknodat);
  RUN_TEST(test_utimensat);
  RUN_TEST(test_futimens);
  RUN_TEST(test_stat);
  RUN_TEST(test_fstat);
  RUN_TEST(test_mkdir_chmod_umask);
  RUN_TEST(test_fchmod_fchmodat);
  RUN_TEST(test_fstatat_lstat);
  RUN_TEST(test_futimens_utimensat);
  RUN_TEST(test_mknod_mknodat);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(posix_stat_suite);
  GREATEST_MAIN_END();
}
