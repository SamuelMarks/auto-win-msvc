#ifdef _MSC_VER
#endif /* _MSC_VER */
#if !defined(_WIN32) && !defined(_MSC_VER) && !defined(__APPLE__)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#endif
/* clang-format off */
#include "greatest.h"
#include "posix-stat.h"
#include "sys/sysmacros.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <direct.h>
#include <io.h>
#if defined(_MSC_VER)
#include <share.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) void *__stdcall GetModuleHandleA(const char *lpModuleName);
__declspec(dllimport) void *__stdcall GetProcAddress(void *hModule,
                                                     const char *lpProcName);
#ifdef __cplusplus
}
#endif
#ifndef open
#define open _open
#endif
#ifndef close
#define close _close
#endif
#ifndef rmdir
#define rmdir _rmdir
#endif
#else
#include <unistd.h>
#if !defined(__USE_XOPEN)
extern char *realpath(const char *path, char *resolved_path);
#endif
#endif

#ifndef O_RDWR
#ifdef _O_RDWR
#define O_RDWR _O_RDWR
#else
#define O_RDWR 2
#endif
#endif

#ifndef O_CREAT
#ifdef _O_CREAT
#define O_CREAT _O_CREAT
#else
#define O_CREAT 0x0200
#endif
#endif

#ifndef O_RDONLY
#ifdef _O_RDONLY
#define O_RDONLY _O_RDONLY
#else
#define O_RDONLY 0
#endif
#endif
/* clang-format on */

TEST test_posix_stat_get_info(void) {
  int info;
  auto_win_msvc_error_t rc;

  info = 0;
  rc = posix_stat_get_info(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_stat_get_info(&info);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_stat_get_info failed with %d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_stat_error_codes(void) {
  ASSERT_EQ(0, (int)AUTO_WIN_MSVC_SUCCESS);
  ASSERT_EQ(1, (int)AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT);
  ASSERT_EQ(7, (int)AUTO_WIN_MSVC_ERROR_NOT_FOUND);
  ASSERT_EQ(5, (int)AUTO_WIN_MSVC_ERROR_IO);
  ASSERT_EQ(3, (int)AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED);
  PASS();
}

#if defined(_WIN32)
TEST test_posix_stat_internal_helpers(void) {
  ptrdiff_t h;
  auto_win_msvc_error_t rc;
  char buf[260];
  struct timespec ts;
  unsigned char ft_buf[16];
  int omit;

  h = 0;
  rc = posix_stat_safe_get_osfhandle(0, NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_safe_get_osfhandle(-1, &h);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  ASSERT_EQ(-1, (long)h);

  rc = posix_stat_safe_get_osfhandle(9999, &h);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  ASSERT_EQ(-1, (long)h);

  rc = posix_stat_resolve_at_path(AT_FDCWD, NULL, buf, sizeof(buf));
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_resolve_at_path(AT_FDCWD, "test", NULL, sizeof(buf));
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_resolve_at_path(AT_FDCWD, "test", buf, 0);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_resolve_at_path(-1, "test", buf, sizeof(buf));
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_resolve_at_path(9999, "test", buf, sizeof(buf));
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_resolve_at_path(AT_FDCWD, "test", buf, sizeof(buf));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_STR_EQ("test", buf);

  rc = posix_stat_resolve_at_path(AT_FDCWD, "C:\\test", buf, sizeof(buf));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  rc = posix_stat_fill_filetime(NULL, NULL, &omit);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_fill_filetime(NULL, ft_buf, NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_fill_filetime(NULL, ft_buf, &omit);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(0, omit);

  ts.tv_sec = 0;
  ts.tv_nsec = UTIME_OMIT;
  rc = posix_stat_fill_filetime(&ts, ft_buf, &omit);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, omit);

  ts.tv_nsec = UTIME_NOW;
  rc = posix_stat_fill_filetime(&ts, ft_buf, &omit);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(0, omit);

  ts.tv_sec = 1600000000;
  ts.tv_nsec = 500;
  rc = posix_stat_fill_filetime(&ts, ft_buf, &omit);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(0, omit);

  PASS();
}
#endif

TEST test_stat_macros(void) {
  mode_t mode;
  long lval;

  mode = S_IFDIR;
  ASSERT(S_ISDIR(mode));
  ASSERT(!S_ISREG(mode));

  mode = S_IFCHR;
  ASSERT(S_ISCHR(mode));
  ASSERT(!S_ISDIR(mode));

  mode = S_IFREG;
  ASSERT(S_ISREG(mode));
  ASSERT(!S_ISDIR(mode));

  mode = S_IFIFO;
  ASSERT(S_ISFIFO(mode));
  ASSERT(!S_ISREG(mode));

  mode = S_IFLNK;
  ASSERT(S_ISLNK(mode));
  ASSERT(!S_ISREG(mode));

  mode = S_IFSOCK;
  ASSERT(S_ISSOCK(mode));
  ASSERT(!S_ISREG(mode));

  mode = S_IFBLK;
  ASSERT(S_ISBLK(mode));
  ASSERT(!S_ISREG(mode));

  /* Check permission macros */
  mode = (mode_t)S_IRUSR;
  ASSERT(mode != 0);
  mode = (mode_t)S_IWUSR;
  ASSERT(mode != 0);
  mode = (mode_t)S_IXUSR;
  ASSERT(mode != 0);
  mode = (mode_t)S_IRWXU;
  ASSERT(mode != 0);
  mode = (mode_t)S_IRGRP;
  ASSERT(mode != 0);
  mode = (mode_t)S_IWGRP;
  ASSERT(mode != 0);
  mode = (mode_t)S_IXGRP;
  ASSERT(mode != 0);
  mode = (mode_t)S_IRWXG;
  ASSERT(mode != 0);
  mode = (mode_t)S_IROTH;
  ASSERT(mode != 0);
  mode = (mode_t)S_IWOTH;
  ASSERT(mode != 0);
  mode = (mode_t)S_IXOTH;
  ASSERT(mode != 0);
  mode = (mode_t)S_IRWXO;
  ASSERT(mode != 0);

  /* Check special value macros */
  lval = (long)UTIME_NOW;
  ASSERT(lval != 0);
  lval = (long)UTIME_OMIT;
  ASSERT(lval != 0);
  lval = (long)AT_FDCWD;
  ASSERT(lval != 0);
  lval = (long)AT_SYMLINK_NOFOLLOW;
  ASSERT(lval != 0);

  ASSERT_EQ(8u, (unsigned int)major(makedev(8, 3)));
  ASSERT_EQ(3u, (unsigned int)minor(makedev(8, 3)));

  PASS();
}

TEST test_stat_basic(void) {
  struct stat st;
  int res;

  memset(&st, 0, sizeof(st));
  res = stat(".", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));

  res = stat("nonexistent_stat_test_xyz_12345.tmp", &st);
  ASSERT_EQ(-1, res);

#if defined(_WIN32)
  res = stat(NULL, &st);
  ASSERT_EQ(-1, res);
  res = stat(".", NULL);
  ASSERT_EQ(-1, res);

  memset(&st, 0, sizeof(st));
  res = stat("/proc", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));

  memset(&st, 0, sizeof(st));
  res = stat("/proc/1", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));

  memset(&st, 0, sizeof(st));
  res = stat("/proc/1/stat", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));

  memset(&st, 0, sizeof(st));
  res = stat("version", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));

  memset(&st, 0, sizeof(st));
  res = lstat("/proc", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
#else
  res = posix_stat(NULL, &st);
  ASSERT_EQ(-1, res);
  res = posix_stat(".", NULL);
  ASSERT_EQ(-1, res);
  res = posix_stat(".", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
#if !defined(__linux__)
  res = posix_stat("//proc", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
  res = posix_stat("/proc", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
  res = posix_stat("/proc/", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
  res = posix_stat("/proc/self", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
  res = posix_stat("/proc/self/", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
  res = posix_stat("/proc/self/stat", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));
  res = posix_stat("/proc/cpuinfo", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));
  res = posix_stat("/proc/+bad", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));
  res = posix_stat("/proc/1", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
  res = posix_stat("/proc/1/stat", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));
#else
  (void)posix_stat("//nonexistent_xyz_999", &st);
#endif

  res = posix_lstat(NULL, &st);
  ASSERT_EQ(-1, res);
  res = posix_lstat(".", NULL);
  ASSERT_EQ(-1, res);
  res = posix_lstat(".", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
#if !defined(__linux__)
  res = posix_lstat("//proc", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));
#else
  (void)posix_lstat("//nonexistent_xyz_999", &st);
#endif
#endif

  PASS();
}

TEST test_lstat_basic(void) {
  struct stat st;
  int res;

  memset(&st, 0, sizeof(st));
  res = lstat(".", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));

  res = lstat("nonexistent_lstat_test_xyz_12345.tmp", &st);
  ASSERT_EQ(-1, res);

#if defined(_WIN32)
  res = lstat(NULL, &st);
  ASSERT_EQ(-1, res);

  res = lstat(".", NULL);
  ASSERT_EQ(-1, res);
#endif

  PASS();
}

TEST test_mkdir_rmdir(void) {
  struct stat st;
  int res;

  rmdir("test_stat_subdir");

  res = mkdir("test_stat_subdir", 0755);
  ASSERT_EQ(0, res);

  memset(&st, 0, sizeof(st));
  res = stat("test_stat_subdir", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISDIR(st.st_mode));

  /* Creating again should fail */
  res = mkdir("test_stat_subdir", 0755);
  ASSERT_EQ(-1, res);

  res = rmdir("test_stat_subdir");
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_fstat_and_fchmod(void) {
  struct stat st;
  FILE *fp;
  int fd;
  int res;

#if defined(_MSC_VER)
  if (fopen_s(&fp, "test_stat_file.txt", "w") != 0) {
    fp = NULL;
  }
#else
  fp = fopen("test_stat_file.txt", "w");
#endif
  ASSERT(fp != NULL);
  fputs("test content for posix-stat coverage\n", fp);
  fclose(fp);

#if defined(_MSC_VER)
  if (_sopen_s(&fd, "test_stat_file.txt", _O_RDWR, _SH_DENYNO,
               _S_IREAD | _S_IWRITE) != 0) {
    fd = -1;
  }
#else
  fd = open("test_stat_file.txt", O_RDWR, 0644);
#endif
  ASSERT(fd >= 0);

  memset(&st, 0, sizeof(st));
  res = fstat(fd, &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));
  ASSERT(st.st_size > 0);
#if defined(_WIN32)
  {
    struct posix_stat pst;
    memset(&pst, 0, sizeof(pst));
    res = posix_fstat(fd, &pst);
    ASSERT_EQ(0, res);
    ASSERT_EQ(4096, pst.st_blksize);
    ASSERT(pst.st_blocks > 0);
    ASSERT(pst.st_atim.tv_sec > 0);
    ASSERT(pst.st_mtim.tv_sec > 0);
    ASSERT(pst.st_ctim.tv_sec > 0);
    memset(&pst, 0, sizeof(pst));
    res = posix_stat("test_stat_file.txt", &pst);
    ASSERT_EQ(0, res);
    ASSERT_EQ(4096, pst.st_blksize);
    ASSERT(pst.st_blocks > 0);
    ASSERT(pst.st_atim.tv_sec > 0);
    ASSERT(pst.st_mtim.tv_sec > 0);
    ASSERT(pst.st_ctim.tv_sec > 0);
  }
#endif

  res = fchmod(fd, 0644);
  ASSERT_EQ(0, res);

  res = fchmod(fd, 0444);
  ASSERT_EQ(0, res);

  res = fchmod(fd, 0644);
  ASSERT_EQ(0, res);

  res = fchmod(-1, 0644);
  ASSERT_EQ(-1, res);

  close(fd);

  PASS();
}

TEST test_fchmodat(void) {
  int res;

  res = fchmodat(AT_FDCWD, "test_stat_file.txt", 0644, 0);
  ASSERT_EQ(0, res);

  res = fchmodat(AT_FDCWD, "test_stat_file.txt", 0644, AT_SYMLINK_NOFOLLOW);
  ASSERT_EQ(0, res);

  res = fchmodat(-1, "test_stat_file.txt", 0644, 0);
  ASSERT_EQ(-1, res);

  res = fchmodat(AT_FDCWD, "nonexistent_fchmodat_xyz.tmp", 0644, 0);
  ASSERT_EQ(-1, res);

#if defined(_WIN32)
  res = fchmodat(AT_FDCWD, NULL, 0644, 0);
  ASSERT_EQ(-1, res);

  res = fchmodat(9999, "test_stat_file.txt", 0644, 0);
  ASSERT_EQ(-1, res);
#endif

  PASS();
}

TEST test_fstatat(void) {
  struct stat st;
  int res;

  memset(&st, 0, sizeof(st));
  res = fstatat(AT_FDCWD, "test_stat_file.txt", &st, 0);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));

  res = fstatat(AT_FDCWD, "test_stat_file.txt", &st, AT_SYMLINK_NOFOLLOW);
  ASSERT_EQ(0, res);

  res = fstatat(AT_FDCWD, "nonexistent_fstatat_xyz.tmp", &st, 0);
  ASSERT_EQ(-1, res);

#if defined(_WIN32)
  res = fstatat(AT_FDCWD, NULL, &st, 0);
  ASSERT_EQ(-1, res);

  res = fstatat(9999, "test_stat_file.txt", &st, 0);
  ASSERT_EQ(-1, res);
#endif

  PASS();
}

TEST test_futimens(void) {
  struct timespec ts[2];
  int fd;
  int res;

#if defined(_MSC_VER)
  if (_sopen_s(&fd, "test_stat_file.txt", _O_RDWR, _SH_DENYNO,
               _S_IREAD | _S_IWRITE) != 0) {
    fd = -1;
  }
#else
  fd = open("test_stat_file.txt", O_RDWR, 0644);
#endif
  ASSERT(fd >= 0);

  res = futimens(fd, NULL);
  ASSERT_EQ(0, res);

  ts[0].tv_sec = 1600000000;
  ts[0].tv_nsec = 0;
  ts[1].tv_sec = 1600000000;
  ts[1].tv_nsec = 0;
  res = futimens(fd, ts);
  ASSERT_EQ(0, res);

  ts[0].tv_nsec = UTIME_OMIT;
  ts[1].tv_nsec = UTIME_NOW;
  res = futimens(fd, ts);
  ASSERT_EQ(0, res);

  ts[0].tv_nsec = UTIME_NOW;
  ts[1].tv_nsec = UTIME_OMIT;
  res = futimens(fd, ts);
  ASSERT_EQ(0, res);

  ts[0].tv_nsec = UTIME_OMIT;
  ts[1].tv_nsec = UTIME_OMIT;
  res = futimens(fd, ts);
  ASSERT_EQ(0, res);

  ts[0].tv_nsec = UTIME_NOW;
  ts[1].tv_nsec = UTIME_NOW;
  res = futimens(fd, ts);
  ASSERT_EQ(0, res);

  res = futimens(-1, NULL);
  ASSERT_EQ(-1, res);

  res = futimens(9999, NULL);
  ASSERT_EQ(-1, res);

  close(fd);

  PASS();
}

TEST test_utimensat(void) {
  struct timespec ts[2];
  int res;

  res = utimensat(AT_FDCWD, "test_stat_file.txt", NULL, 0);
  ASSERT_EQ(0, res);

  ts[0].tv_sec = 1600000000;
  ts[0].tv_nsec = 0;
  ts[1].tv_sec = 1600000000;
  ts[1].tv_nsec = 0;
  res = utimensat(AT_FDCWD, "test_stat_file.txt", ts, 0);
  ASSERT_EQ(0, res);

  ts[0].tv_nsec = UTIME_OMIT;
  ts[1].tv_nsec = UTIME_NOW;
  res = utimensat(AT_FDCWD, "test_stat_file.txt", ts, AT_SYMLINK_NOFOLLOW);
  ASSERT_EQ(0, res);

  ts[0].tv_nsec = UTIME_NOW;
  ts[1].tv_nsec = UTIME_OMIT;
  res = utimensat(AT_FDCWD, "test_stat_file.txt", ts, 0);
  ASSERT_EQ(0, res);

  ts[0].tv_nsec = UTIME_OMIT;
  ts[1].tv_nsec = UTIME_OMIT;
  res = utimensat(AT_FDCWD, "test_stat_file.txt", ts, 0);
  ASSERT_EQ(0, res);

  ts[0].tv_nsec = UTIME_NOW;
  ts[1].tv_nsec = UTIME_NOW;
  res = utimensat(AT_FDCWD, "test_stat_file.txt", ts, 0);
  ASSERT_EQ(0, res);

  res = utimensat(AT_FDCWD, "nonexistent_utimensat_xyz.tmp", NULL, 0);
  ASSERT_EQ(-1, res);

  res = utimensat(-1, "test_stat_file.txt", NULL, 0);
  ASSERT_EQ(-1, res);

#if defined(_WIN32)
  res = utimensat(AT_FDCWD, NULL, NULL, 0);
  ASSERT_EQ(-1, res);

  res = utimensat(9999, "test_stat_file.txt", NULL, 0);
  ASSERT_EQ(-1, res);
#endif

  PASS();
}

TEST test_mknod(void) {
  struct stat st;
  int res;

  remove("test_stat_mknod.tmp");

#if defined(_WIN32)
  res = mknod("test_stat_mknod.tmp", S_IFREG | 0644, 0);
  ASSERT_EQ(0, res);

  memset(&st, 0, sizeof(st));
  res = stat("test_stat_mknod.tmp", &st);
  ASSERT_EQ(0, res);
  ASSERT(S_ISREG(st.st_mode));

  /* Trying to create already existing file */
  res = mknod("test_stat_mknod.tmp", S_IFREG | 0644, 0);
  ASSERT_EQ(-1, res);

  remove("test_stat_mknod.tmp");

  /* Attempt create under nonexistent subdirectory */
  res = mknod("nonexistent_subdir_xyz/test_stat_mknod.tmp", S_IFREG | 0644, 0);
  ASSERT_EQ(-1, res);

  rmdir("test_stat_mknod_dir");
  res = mknod("test_stat_mknod_dir", S_IFDIR | 0755, 0);
  if (res == 0) {
    rmdir("test_stat_mknod_dir");
  }

  res = mknod("test_stat_mknod_chr", S_IFCHR | 0644, 0);
  ASSERT_EQ(-1, res);

  res = mknod("test_stat_mknod_blk", S_IFBLK | 0644, 0);
  ASSERT_EQ(-1, res);

  res = mknod("test_stat_mknod_fifo", S_IFIFO | 0644, 0);
  ASSERT_EQ(-1, res);

  res = mknod("test_stat_mknod_sock", S_IFSOCK | 0644, 0);
  ASSERT_EQ(-1, res);

  res = mknod(NULL, S_IFREG | 0644, 0);
  ASSERT_EQ(-1, res);
#else
  /* On POSIX, mknod for non-FIFO by non-root returns -1 (EPERM/EINVAL) */
  res = mknod("test_stat_mknod.tmp", S_IFREG | 0644, 0);
  if (res == 0) {
    memset(&st, 0, sizeof(st));
    res = stat("test_stat_mknod.tmp", &st);
    ASSERT_EQ(0, res);
    ASSERT(S_ISREG(st.st_mode));
    remove("test_stat_mknod.tmp");
  } else {
    ASSERT(errno == EPERM || errno == EINVAL || errno == EACCES);
  }
#endif

  PASS();
}

TEST test_mknodat(void) {
  struct stat st;
  int res;

  remove("test_stat_mknodat.tmp");

  res = mknodat(AT_FDCWD, "test_stat_mknodat.tmp", S_IFREG | 0644, 0);
  if (res == 0) {
    memset(&st, 0, sizeof(st));
    res = stat("test_stat_mknodat.tmp", &st);
    ASSERT_EQ(0, res);
    remove("test_stat_mknodat.tmp");
  }

#if defined(_WIN32)
  res = mknodat(AT_FDCWD, NULL, S_IFREG | 0644, 0);
  ASSERT_EQ(-1, res);

  res = mknodat(9999, "test_stat_mknodat.tmp", S_IFREG | 0644, 0);
  ASSERT_EQ(-1, res);
#endif

  PASS();
}

TEST test_at_directory_fd(void) {
  struct stat st;
  int res;
#if defined(_WIN32)
  typedef void *HANDLE;
  typedef unsigned long DWORD;
  typedef HANDLE(__stdcall * CreateFileA_t)(const char *, DWORD, DWORD, void *,
                                            DWORD, DWORD, HANDLE);
  void *hK32;
  CreateFileA_t pCreateFileA;
  HANDLE hDir;
  int dfd;
#else
  int dfd;
#endif

  memset(&st, 0, sizeof(st));
#if defined(_WIN32)
  hK32 = GetModuleHandleA("kernel32.dll");
  if (hK32) {
    pCreateFileA = (CreateFileA_t)(size_t)GetProcAddress(hK32, "CreateFileA");
    if (pCreateFileA) {
      hDir = pCreateFileA(".", 0x80000000 /* GENERIC_READ */, 1 | 2 | 4, NULL,
                          3 /* OPEN_EXISTING */,
                          0x02000000 /* FILE_FLAG_BACKUP_SEMANTICS */, NULL);
      if (hDir != (HANDLE)(size_t)-1) {
        dfd = _open_osfhandle((intptr_t)hDir, 0);
        if (dfd >= 0) {
          res = fstatat(dfd, "test_stat_file.txt", &st, 0);
          ASSERT_EQ(0, res);
          ASSERT(S_ISREG(st.st_mode));

          res = fchmodat(dfd, "test_stat_file.txt", 0644, 0);
          ASSERT_EQ(0, res);

          res = utimensat(dfd, "test_stat_file.txt", NULL, 0);
          ASSERT_EQ(0, res);

          remove("test_stat_mknodat_dfd.tmp");
          res = mknodat(dfd, "test_stat_mknodat_dfd.tmp", S_IFREG | 0644, 0);
          if (res == 0) {
            remove("test_stat_mknodat_dfd.tmp");
          }

          close(dfd);
        }
      }
    }
  }
#else
  dfd = open(".", O_RDONLY);
  if (dfd >= 0) {
    res = fstatat(dfd, "test_stat_file.txt", &st, 0);
    ASSERT_EQ(0, res);
    close(dfd);
  }
#endif

  PASS();
}

TEST test_at_absolute_paths(void) {
  char fullpath[4096];
  struct stat st;
  int res;

  memset(&st, 0, sizeof(st));
#if defined(_WIN32) || defined(__WATCOMC__) || defined(__MSDOS__) ||           \
    defined(__DOS__)
  if (_fullpath(fullpath, "test_stat_file.txt", sizeof(fullpath)) != NULL) {
    res = fstatat(AT_FDCWD, fullpath, &st, 0);
    ASSERT_EQ(0, res);
    ASSERT(S_ISREG(st.st_mode));

    res = fchmodat(AT_FDCWD, fullpath, 0644, 0);
    ASSERT_EQ(0, res);

    res = utimensat(AT_FDCWD, fullpath, NULL, 0);
    ASSERT_EQ(0, res);
  }
#else
  if (realpath("test_stat_file.txt", fullpath) != NULL) {
    res = fstatat(AT_FDCWD, fullpath, &st, 0);
    ASSERT_EQ(0, res);
    ASSERT(S_ISREG(st.st_mode));

    res = fchmodat(AT_FDCWD, fullpath, 0644, 0);
    ASSERT_EQ(0, res);

    res = utimensat(AT_FDCWD, fullpath, NULL, 0);
    ASSERT_EQ(0, res);
  }
#endif

  PASS();
}

TEST test_reparse_point_symlink(void) {
#if defined(_WIN32)
  typedef int BOOL;
  typedef BOOL(__stdcall * CreateSymbolicLinkA_t)(const char *, const char *,
                                                  unsigned long);
  void *hK32;
  CreateSymbolicLinkA_t pCreateSymbolicLinkA;
  struct stat st;
  int res;

  hK32 = GetModuleHandleA("kernel32.dll");
  if (hK32) {
    pCreateSymbolicLinkA = (CreateSymbolicLinkA_t)(size_t)GetProcAddress(
        hK32, "CreateSymbolicLinkA");
    if (pCreateSymbolicLinkA) {
      remove("test_stat_symlink.tmp");
      if (pCreateSymbolicLinkA(
              "test_stat_symlink.tmp", "test_stat_file.txt",
              0x2 /* SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE */) ||
          pCreateSymbolicLinkA("test_stat_symlink.tmp", "test_stat_file.txt",
                               0)) {
        memset(&st, 0, sizeof(st));
        res = lstat("test_stat_symlink.tmp", &st);
        ASSERT_EQ(0, res);
        ASSERT(S_ISLNK(st.st_mode));

        memset(&st, 0, sizeof(st));
        res = fstatat(AT_FDCWD, "test_stat_symlink.tmp", &st,
                      AT_SYMLINK_NOFOLLOW);
        ASSERT_EQ(0, res);
        ASSERT(S_ISLNK(st.st_mode));

        res = utimensat(AT_FDCWD, "test_stat_symlink.tmp", NULL,
                        AT_SYMLINK_NOFOLLOW);
        ASSERT_EQ(0, res);

        remove("test_stat_symlink.tmp");
      }
    }
  }
#endif

  PASS();
}

TEST test_umask(void) {
  mode_t m1;
  mode_t m2;
#if defined(_WIN32)
  void *hUcrt;
  typedef int(__cdecl * pfn_umask)(int);
  pfn_umask p_umask;

  hUcrt = GetModuleHandleA("ucrtbase.dll");
  if (!hUcrt) {
    hUcrt = GetModuleHandleA("msvcrt.dll");
  }
  if (hUcrt) {
    p_umask = (pfn_umask)(size_t)GetProcAddress(hUcrt, "_umask");
    if (p_umask) {
      m1 = (mode_t)p_umask(0200);
      m2 = (mode_t)p_umask((int)m1);
      ASSERT_EQ(0200, m2);
    }
  }
#elif defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
  m1 = umask(0200);
  m2 = umask(m1);
  ASSERT_EQ(0200, m2);
#else
  m1 = umask(0022);
  m2 = umask(m1);
  ASSERT_EQ(0022, m2);
#endif

  PASS();
}

TEST test_chmod(void) {
  int res;

  res = chmod("test_stat_file.txt", 0644);
  ASSERT_EQ(0, res);

  res = chmod("nonexistent_chmod_xyz.tmp", 0644);
  ASSERT_EQ(-1, res);

  remove("test_stat_file.txt");

  PASS();
}

#if defined(_WIN32)
typedef struct mock_stat_ft {
  unsigned long dwLowDateTime;
  unsigned long dwHighDateTime;
} mock_stat_ft_t;

typedef struct mock_stat_fad {
  unsigned long dwFileAttributes;
  mock_stat_ft_t ftCreationTime;
  mock_stat_ft_t ftLastAccessTime;
  mock_stat_ft_t ftLastWriteTime;
  unsigned long nFileSizeHigh;
  unsigned long nFileSizeLow;
} mock_stat_fad_t;

typedef struct mock_stat_bhfi {
  unsigned long dwFileAttributes;
  mock_stat_ft_t ftCreationTime;
  mock_stat_ft_t ftLastAccessTime;
  mock_stat_ft_t ftLastWriteTime;
  unsigned long dwVolumeSerialNumber;
  unsigned long nFileSizeHigh;
  unsigned long nFileSizeLow;
  unsigned long nNumberOfLinks;
  unsigned long nFileIndexHigh;
  unsigned long nFileIndexLow;
} mock_stat_bhfi_t;

static const char *g_mock_final_path = "C:\\dir";
static unsigned long g_mock_final_ret = 6;

static unsigned long __stdcall mock_final_path_fn(void *hFile, char *buf,
                                                  unsigned long sz,
                                                  unsigned long flags) {
  (void)hFile;
  (void)flags;
  if (g_mock_final_ret > 0 && g_mock_final_ret < sz && g_mock_final_path) {
    size_t l = strlen(g_mock_final_path);
    memcpy(buf, g_mock_final_path, l + 1);
  }
  return g_mock_final_ret;
}

static void *mock_get_mod_null(const char *name) {
  (void)name;
  return NULL;
}

static void *mock_get_proc_null(void *hMod, const char *name) {
  (void)hMod;
  (void)name;
  return NULL;
}

static void *mock_get_proc_final(void *hMod, const char *name) {
  (void)hMod;
  (void)name;
  return (void *)(size_t)mock_final_path_fn;
}

static unsigned long mock_get_attrs_fail(const char *path) {
  (void)path;
  return (unsigned long)-1;
}

static int mock_set_attrs_fail(const char *path, unsigned long attrs) {
  (void)path;
  (void)attrs;
  return 0;
}

static int mock_get_attrs_ex_reparse(const char *path, void *info) {
  mock_stat_fad_t *d = (mock_stat_fad_t *)info;
  (void)path;
  memset(d, 0, sizeof(*d));
  d->dwFileAttributes = 1024UL; /* FILE_ATTRIBUTE_REPARSE_POINT */
  d->ftCreationTime.dwHighDateTime = 30000000UL;
  d->ftLastAccessTime.dwHighDateTime = 30000000UL;
  d->ftLastWriteTime.dwHighDateTime = 30000000UL;
  return 1;
}

static int g_mock_bhfi_mode = 0;
static int mock_get_info_fn(void *hFile, void *info) {
  mock_stat_bhfi_t *b = (mock_stat_bhfi_t *)info;
  (void)hFile;
  if (g_mock_bhfi_mode == 0) {
    return 0;
  }
  memset(b, 0, sizeof(*b));
  if (g_mock_bhfi_mode == 1) {
    /* vol == 0, idx == 0, links == 0 */
    b->dwVolumeSerialNumber = 0;
    b->nFileIndexLow = 0;
    b->nFileIndexHigh = 0;
    b->nNumberOfLinks = 0;
  } else {
    /* vol != 0, idx != 0 but (idx & 0xFFFF) == 0 -> st_ino = 1, links >= 32767
     */
    b->dwVolumeSerialNumber = 0x1234UL;
    b->nFileIndexLow = 0x10000UL;
    b->nFileIndexHigh = 0;
    b->nNumberOfLinks = 40000UL;
  }
  return 1;
}

static int mock_set_time_fail(void *h, const void *c, const void *a,
                              const void *m) {
  (void)h;
  (void)c;
  (void)a;
  (void)m;
  return 0;
}

static void *mock_create_file_fail(const char *p, unsigned long acc,
                                   unsigned long sh, unsigned long disp,
                                   unsigned long fl) {
  (void)p;
  (void)acc;
  (void)sh;
  (void)disp;
  (void)fl;
  return (void *)(size_t)-1;
}
#endif

TEST test_posix_stat_extra_coverage(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  ASSERT_EQ(-1, chflags(NULL, 0));
  ASSERT_EQ(0, chflags(".", 0));
  ASSERT_EQ(-1, lchflags(NULL, 0));
  ASSERT_EQ(0, lchflags(".", 0));
  ASSERT_EQ(-1, fchflags(-1, 0));
  ASSERT_EQ(0, fchflags(0, 0));
#endif

#if defined(_WIN32)
  {
    ptrdiff_t h = 0;
    char buf[260];
    struct posix_stat pst;
    struct timespec bad_ts[2];
    FILE *fp;
    int fd;
    size_t i;
    const char *vpaths[] = {
        "/dev/null",  "\\dev\\null", "\\proc",          "\\proc\\",
        "/proc/",     "\\proc\\1",   "\\proc\\1\\",     "\\proc\\1\\stat",
        "/proc/self", "/proc/self/", "/proc/self/stat", "/proc/cpu",
        "/proc/+bad", "version",     "uptime",          "meminfo",
        "loadavg",    "stat",        "mounts",          "filesystems",
        "swaps",      "cpuinfo"};

    /* Safe get osfhandle on closed valid-range fd (0 <= fd < 2048) */
    ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
              posix_stat_safe_get_osfhandle(999, &h));
    ASSERT_EQ(-1, (long)h);

    /* Create temporary file & open fd for tests */
#if defined(_MSC_VER)
    if (fopen_s(&fp, "test_stat_extra.txt", "w") != 0) {
      fp = NULL;
    }
#else
    fp = fopen("test_stat_extra.txt", "w");
#endif
    ASSERT(fp != NULL);
    fputs("extra\n", fp);
    fclose(fp);
#if defined(_MSC_VER)
    if (_sopen_s(&fd, "test_stat_extra.txt", _O_RDWR, _SH_DENYNO,
                 _S_IREAD | _S_IWRITE) != 0) {
      fd = -1;
    }
#else
    fd = open("test_stat_extra.txt", O_RDWR, 0644);
#endif
    ASSERT(fd >= 0);

    /* IS_ABSOLUTE_PATH branches with non-AT_FDCWD dirfd */
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "\\abs", buf, sizeof(buf)));
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "/abs", buf, sizeof(buf)));
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "C:\\abs", buf, sizeof(buf)));
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "C:/abs", buf, sizeof(buf)));
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "", buf, sizeof(buf)));
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "C:rel", buf, sizeof(buf)));
    ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
              posix_stat_resolve_at_path(999, "rel", buf, sizeof(buf)));

    /* resolve_at_path Win32 hook branches */
    posix_stat_set_win32_hooks(mock_get_mod_null, NULL, NULL, NULL, NULL, NULL,
                               NULL, NULL);
    ASSERT_EQ(AUTO_WIN_MSVC_ERROR_NOT_FOUND,
              posix_stat_resolve_at_path(fd, "rel", buf, sizeof(buf)));

    posix_stat_set_win32_hooks(NULL, mock_get_proc_null, NULL, NULL, NULL, NULL,
                               NULL, NULL);
    ASSERT_EQ(AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED,
              posix_stat_resolve_at_path(fd, "rel", buf, sizeof(buf)));

    posix_stat_set_win32_hooks(NULL, mock_get_proc_final, NULL, NULL, NULL,
                               NULL, NULL, NULL);
    g_mock_final_ret = 0;
    ASSERT_EQ(AUTO_WIN_MSVC_ERROR_NOT_FOUND,
              posix_stat_resolve_at_path(fd, "rel", buf, sizeof(buf)));
    g_mock_final_ret = 300;
    ASSERT_EQ(AUTO_WIN_MSVC_ERROR_NOT_FOUND,
              posix_stat_resolve_at_path(fd, "rel", buf, sizeof(buf)));

    /* Strip \\?\ resulting in len == 0 */
    g_mock_final_path = "\\\\?\\";
    g_mock_final_ret = 4;
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "rel", buf, sizeof(buf)));

    /* Ending with '\\' and '/' and non-slash with exact out_size == len + 1 */
    g_mock_final_path = "C:\\dir\\";
    g_mock_final_ret = 7;
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "rel", buf, sizeof(buf)));

    g_mock_final_path = "C:/dir/";
    g_mock_final_ret = 7;
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "rel", buf, sizeof(buf)));

    g_mock_final_path = "C:\\dir";
    g_mock_final_ret = 6;
    ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS,
              posix_stat_resolve_at_path(fd, "r", buf, 7));

    /* fchmod Win32 failure branches */
    posix_stat_set_win32_hooks(mock_get_mod_null, NULL, NULL, NULL, NULL, NULL,
                               NULL, NULL);
    ASSERT_EQ(-1, fchmod(fd, 0644));

    posix_stat_set_win32_hooks(NULL, mock_get_proc_null, NULL, NULL, NULL, NULL,
                               NULL, NULL);
    ASSERT_EQ(-1, fchmod(fd, 0644));

    posix_stat_set_win32_hooks(NULL, mock_get_proc_final, NULL, NULL, NULL,
                               NULL, NULL, NULL);
    g_mock_final_ret = 0;
    ASSERT_EQ(-1, fchmod(fd, 0644));

    g_mock_final_path = "test_stat_extra.txt";
    g_mock_final_ret = (unsigned long)strlen(g_mock_final_path);
    posix_stat_set_win32_hooks(NULL, mock_get_proc_final, mock_get_attrs_fail,
                               NULL, NULL, NULL, NULL, NULL);
    ASSERT_EQ(-1, fchmod(fd, 0644));

    posix_stat_set_win32_hooks(NULL, mock_get_proc_final, NULL,
                               mock_set_attrs_fail, NULL, NULL, NULL, NULL);
    ASSERT_EQ(-1, fchmod(fd, 0644));
    posix_stat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    /* populate_posix_stat_extras & CreateFileA failure in posix_stat */
    posix_stat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, mock_get_info_fn,
                               NULL, NULL);
    g_mock_bhfi_mode = 0;
    ASSERT_EQ(0, posix_fstat(fd, &pst));
    g_mock_bhfi_mode = 1;
    ASSERT_EQ(0, posix_fstat(fd, &pst));
    g_mock_bhfi_mode = 2;
    ASSERT_EQ(0, posix_fstat(fd, &pst));
    ASSERT_EQ(1, (int)pst.st_ino);

    posix_stat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                               mock_create_file_fail);
    ASSERT_EQ(0, posix_stat("test_stat_extra.txt", &pst));
    posix_stat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    /* posix_fstat NULL & bad fd */
    ASSERT_EQ(-1, posix_fstat(fd, NULL));
    ASSERT_EQ(-1, posix_fstat(-1, &pst));

    /* All virtual /proc and /dev/null paths + leading // */
    ASSERT_EQ(0, posix_stat("//proc", &pst));
    ASSERT_EQ(0, posix_lstat("//proc", &pst));
    for (i = 0; i < sizeof(vpaths) / sizeof(vpaths[0]); i++) {
      ASSERT_EQ(0, posix_stat(vpaths[i], &pst));
    }

    /* Reparse point branch in posix_lstat via hook */
    posix_stat_set_win32_hooks(NULL, NULL, NULL, NULL,
                               mock_get_attrs_ex_reparse, NULL, NULL, NULL);
    ASSERT_EQ(0, posix_lstat("fake_symlink", &pst));
    ASSERT(S_ISLNK(pst.st_mode));
    posix_stat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    /* futimens & utimensat invalid tv_nsec and SetFileTime failure */
    bad_ts[0].tv_sec = 0;
    bad_ts[0].tv_nsec = -5;
    bad_ts[1].tv_sec = 0;
    bad_ts[1].tv_nsec = 0;
    ASSERT_EQ(-1, futimens(fd, bad_ts));
    ASSERT_EQ(-1, utimensat(AT_FDCWD, "test_stat_extra.txt", bad_ts, 0));

    bad_ts[0].tv_nsec = 0;
    bad_ts[1].tv_nsec = 1000000000L;
    ASSERT_EQ(-1, futimens(fd, bad_ts));
    ASSERT_EQ(-1, utimensat(AT_FDCWD, "test_stat_extra.txt", bad_ts, 0));

    posix_stat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL,
                               mock_set_time_fail, NULL);
    ASSERT_EQ(-1, futimens(fd, NULL));
    ASSERT_EQ(-1, utimensat(AT_FDCWD, "test_stat_extra.txt", NULL, 0));
    posix_stat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    /* mkdirat, mkfifo, mkfifoat */
    ASSERT_EQ(-1, mkdirat(AT_FDCWD, NULL, 0755));
    rmdir("test_mkdirat_dir");
    ASSERT_EQ(0, mkdirat(AT_FDCWD, "test_mkdirat_dir", 0755));
    ASSERT_EQ(0, rmdir("test_mkdirat_dir"));
    ASSERT_EQ(-1, mkfifo("test_fifo", 0644));
    ASSERT_EQ(-1, mkfifoat(AT_FDCWD, "test_fifo", 0644));

    close(fd);
    remove("test_stat_extra.txt");
  }
#endif
  PASS();
}

SUITE(suite_posix_stat_core) {
  RUN_TEST(test_posix_stat_extra_coverage);
  RUN_TEST(test_posix_stat_get_info);
  RUN_TEST(test_posix_stat_error_codes);
#if defined(_WIN32)
  RUN_TEST(test_posix_stat_internal_helpers);
#endif
  RUN_TEST(test_stat_macros);
  RUN_TEST(test_stat_basic);
  RUN_TEST(test_lstat_basic);
  RUN_TEST(test_mkdir_rmdir);
  RUN_TEST(test_fstat_and_fchmod);
  RUN_TEST(test_fchmodat);
  RUN_TEST(test_fstatat);
  RUN_TEST(test_futimens);
  RUN_TEST(test_utimensat);
  RUN_TEST(test_mknod);
  RUN_TEST(test_mknodat);
  RUN_TEST(test_at_directory_fd);
  RUN_TEST(test_at_absolute_paths);
  RUN_TEST(test_reparse_point_symlink);
  RUN_TEST(test_umask);
  RUN_TEST(test_chmod);
}
