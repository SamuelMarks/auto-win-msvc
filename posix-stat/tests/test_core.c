#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-stat.h"
#include <errno.h>
#include <fcntl.h>
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
  enum posix_stat_error_code rc;

  info = 0;
  rc = posix_stat_get_info(NULL);
  if (rc != POSIX_STAT_ERROR_NULL_POINTER) {
    printf("Expected POSIX_STAT_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_stat_get_info(&info);
  if (rc != POSIX_STAT_SUCCESS) {
    printf("posix_stat_get_info failed with %d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_stat_error_codes(void) {
  ASSERT_EQ(0, (int)POSIX_STAT_SUCCESS);
  ASSERT_EQ(1, (int)POSIX_STAT_ERROR_NULL_POINTER);
  ASSERT_EQ(2, (int)POSIX_STAT_ERROR_INVALID_ARGUMENT);
  ASSERT_EQ(3, (int)POSIX_STAT_ERROR_PATH_RESOLUTION);
  ASSERT_EQ(4, (int)POSIX_STAT_ERROR_IO);
  ASSERT_EQ(5, (int)POSIX_STAT_ERROR_NOT_SUPPORTED);
  PASS();
}

#if defined(_WIN32)
TEST test_posix_stat_internal_helpers(void) {
  ptrdiff_t h;
  enum posix_stat_error_code rc;
  char buf[260];
  struct timespec ts;
  unsigned char ft_buf[16];
  int omit;

  h = 0;
  rc = posix_stat_safe_get_osfhandle(0, NULL);
  ASSERT_EQ(POSIX_STAT_ERROR_NULL_POINTER, rc);

  rc = posix_stat_safe_get_osfhandle(-1, &h);
  ASSERT_EQ(POSIX_STAT_ERROR_INVALID_ARGUMENT, rc);
  ASSERT_EQ(-1, (long)h);

  rc = posix_stat_safe_get_osfhandle(9999, &h);
  ASSERT_EQ(POSIX_STAT_ERROR_INVALID_ARGUMENT, rc);
  ASSERT_EQ(-1, (long)h);

  rc = posix_stat_resolve_at_path(AT_FDCWD, NULL, buf, sizeof(buf));
  ASSERT_EQ(POSIX_STAT_ERROR_NULL_POINTER, rc);

  rc = posix_stat_resolve_at_path(AT_FDCWD, "test", NULL, sizeof(buf));
  ASSERT_EQ(POSIX_STAT_ERROR_NULL_POINTER, rc);

  rc = posix_stat_resolve_at_path(AT_FDCWD, "test", buf, 0);
  ASSERT_EQ(POSIX_STAT_ERROR_NULL_POINTER, rc);

  rc = posix_stat_resolve_at_path(-1, "test", buf, sizeof(buf));
  ASSERT_EQ(POSIX_STAT_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_resolve_at_path(9999, "test", buf, sizeof(buf));
  ASSERT_EQ(POSIX_STAT_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_stat_resolve_at_path(AT_FDCWD, "test", buf, sizeof(buf));
  ASSERT_EQ(POSIX_STAT_SUCCESS, rc);
  ASSERT_STR_EQ("test", buf);

  rc = posix_stat_resolve_at_path(AT_FDCWD, "C:\\test", buf, sizeof(buf));
  ASSERT_EQ(POSIX_STAT_SUCCESS, rc);

  rc = posix_stat_fill_filetime(NULL, NULL, &omit);
  ASSERT_EQ(POSIX_STAT_ERROR_NULL_POINTER, rc);

  rc = posix_stat_fill_filetime(NULL, ft_buf, NULL);
  ASSERT_EQ(POSIX_STAT_ERROR_NULL_POINTER, rc);

  rc = posix_stat_fill_filetime(NULL, ft_buf, &omit);
  ASSERT_EQ(POSIX_STAT_SUCCESS, rc);
  ASSERT_EQ(0, omit);

  ts.tv_sec = 0;
  ts.tv_nsec = UTIME_OMIT;
  rc = posix_stat_fill_filetime(&ts, ft_buf, &omit);
  ASSERT_EQ(POSIX_STAT_SUCCESS, rc);
  ASSERT_EQ(1, omit);

  ts.tv_nsec = UTIME_NOW;
  rc = posix_stat_fill_filetime(&ts, ft_buf, &omit);
  ASSERT_EQ(POSIX_STAT_SUCCESS, rc);
  ASSERT_EQ(0, omit);

  ts.tv_sec = 1600000000;
  ts.tv_nsec = 500;
  rc = posix_stat_fill_filetime(&ts, ft_buf, &omit);
  ASSERT_EQ(POSIX_STAT_SUCCESS, rc);
  ASSERT_EQ(0, omit);

  PASS();
}
#endif

TEST test_stat_macros(void) {
  mode_t mode;

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
  ASSERT(S_IRUSR != 0);
  ASSERT(S_IWUSR != 0);
  ASSERT(S_IXUSR != 0);
  ASSERT(S_IRWXU != 0);
  ASSERT(S_IRGRP != 0);
  ASSERT(S_IWGRP != 0);
  ASSERT(S_IXGRP != 0);
  ASSERT(S_IRWXG != 0);
  ASSERT(S_IROTH != 0);
  ASSERT(S_IWOTH != 0);
  ASSERT(S_IXOTH != 0);
  ASSERT(S_IRWXO != 0);

  /* Check special value macros */
  ASSERT(UTIME_NOW != 0);
  ASSERT(UTIME_OMIT != 0);
  ASSERT(AT_FDCWD != 0);
  ASSERT(AT_SYMLINK_NOFOLLOW != 0);

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
  char fullpath[512];
  struct stat st;
  int res;

  memset(&st, 0, sizeof(st));
#if defined(_WIN32)
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
      m1 = (mode_t)p_umask(0022);
      m2 = (mode_t)p_umask((int)m1);
      ASSERT_EQ(0022, m2);
    }
  }
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

SUITE(suite_posix_stat_core) {
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
