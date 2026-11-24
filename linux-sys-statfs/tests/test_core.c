/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include "greatest.h"
#include "linux-sys-statfs.h"
#include "mntent.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(__CYGWIN__)
#include <sys/statvfs.h>
#endif
#if defined(__APPLE__)
#include <sys/param.h>
#include <sys/mount.h>
#endif
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
static intptr_t g_mock_osfhandle_ret = 10;
static int g_mock_diskfree_ok = 1;
static int g_mock_k32_ok = 1;
static int g_mock_finalpath_present = 1;
static DWORD g_mock_finalpath_res = 3;

static intptr_t mock_statfs_get_osfhandle(int fd) {
  (void)fd;
  return g_mock_osfhandle_ret;
}

static BOOL WINAPI mock_statfs_GetDiskFreeSpaceExA(LPCSTR path,
                                                   PULARGE_INTEGER pFreeAvail,
                                                   PULARGE_INTEGER pTotal,
                                                   PULARGE_INTEGER pTotalFree) {
  (void)path;
  if (!g_mock_diskfree_ok) {
    return FALSE;
  }
  pFreeAvail->QuadPart = 4096 * 100;
  pTotal->QuadPart = 4096 * 200;
  pTotalFree->QuadPart = 4096 * 150;
  return TRUE;
}

static HMODULE WINAPI mock_statfs_GetModuleHandleA(LPCSTR name) {
  (void)name;
  return g_mock_k32_ok ? (HMODULE)1 : (HMODULE)0;
}

static DWORD WINAPI mock_GetFinalPathNameByHandleA_fn(HANDLE hFile, LPSTR buf,
                                                      DWORD cch, DWORD flags) {
  (void)hFile;
  (void)flags;
  if (g_mock_finalpath_res > 0 && g_mock_finalpath_res < cch) {
    memcpy(buf, "C:\\", 4);
  }
  return g_mock_finalpath_res;
}

static FARPROC WINAPI mock_statfs_GetProcAddress(HMODULE hMod, LPCSTR proc) {
  (void)hMod;
  (void)proc;
  return g_mock_finalpath_present
             ? (FARPROC)(size_t)mock_GetFinalPathNameByHandleA_fn
             : (FARPROC)0;
}

#define STATFS_MOCK_get_osfhandle mock_statfs_get_osfhandle
#define STATFS_MOCK_GetDiskFreeSpaceExA mock_statfs_GetDiskFreeSpaceExA
#define STATFS_MOCK_GetModuleHandleA mock_statfs_GetModuleHandleA
#define STATFS_MOCK_GetProcAddress mock_statfs_GetProcAddress

int mocked_win_statfs(const char *path, struct statfs *buf);
int mocked_win_fstatfs(int fd, struct statfs *buf);
int mocked_win_statvfs(const char *path, struct statvfs *buf);
int mocked_win_fstatvfs(int fd, struct statvfs *buf);
auto_win_msvc_error_t mocked_linux_sys_statfs_init(int *out_status);
#define statfs(path, buf) mocked_win_statfs(path, buf)
#define fstatfs(fd, buf) mocked_win_fstatfs(fd, buf)
#define statvfs(path, buf) mocked_win_statvfs(path, buf)
#define fstatvfs(fd, buf) mocked_win_fstatvfs(fd, buf)
#define linux_sys_statfs_init mocked_linux_sys_statfs_init

#include "../src/linux-sys-statfs.c"

#undef statfs
#undef fstatfs
#undef statvfs
#undef fstatvfs
#undef linux_sys_statfs_init
#endif

TEST test_linux_sys_statfs_init(void) {
  int status = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, linux_sys_statfs_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, linux_sys_statfs_init(&status));
  ASSERT_EQ(1, status);

#if defined(_MSC_VER) || defined(_WIN32)
  status = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            mocked_linux_sys_statfs_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_linux_sys_statfs_init(&status));
  ASSERT_EQ(1, status);
#if defined(_MSC_VER) && _MSC_VER >= 1400
  statfs_null_invalid_param_handler(NULL, NULL, NULL, 0, 0);
#endif
#endif

  PASS();
}

TEST test_statfs_fstatfs(void) {
  struct statfs buf;
  FILE *tmp = NULL;

  (void)statfs("/", &buf);
  (void)fstatfs(0, &buf);

#if defined(_MSC_VER) || defined(_WIN32)
  /* Real linked calls including error paths and real file fd */
  ASSERT_EQ(-1, statfs(NULL, &buf));
  ASSERT_EQ(-1, statfs(".", NULL));
  ASSERT_EQ(0, statfs(".", &buf));
  ASSERT_EQ(-1, statfs("Z:\\nonexistent_statfs_drive_999\\sub", &buf));
  ASSERT_EQ(-1, fstatfs(0, NULL));
  ASSERT_EQ(-1, fstatfs(-1, &buf));
  ASSERT_EQ(-1, fstatfs(3000, &buf));
  ASSERT_EQ(-1, fstatfs(999, &buf));
#if defined(_MSC_VER)
  if (fopen_s(&tmp, "test_statfs_tmp.txt", "wb+") != 0) {
    tmp = NULL;
  }
#else
  tmp = fopen("test_statfs_tmp.txt", "wb+");
#endif
  if (tmp != NULL) {
    (void)fstatfs(_fileno(tmp), &buf);
    fclose(tmp);
    remove("test_statfs_tmp.txt");
  }

  /* Mocked OS branch coverage for statfs and fstatfs */
  ASSERT_EQ(-1, mocked_win_statfs(".", NULL));
  ASSERT_EQ(-1, mocked_win_statfs(NULL, &buf));

  g_mock_diskfree_ok = 1;
  ASSERT_EQ(0, mocked_win_statfs("C:\\", &buf));
  ASSERT_EQ(200, buf.f_blocks);

  g_mock_diskfree_ok = 0;
  ASSERT_EQ(-1, mocked_win_statfs("C:\\", &buf));
  g_mock_diskfree_ok = 1;

  ASSERT_EQ(-1, mocked_win_fstatfs(0, NULL));
  ASSERT_EQ(-1, mocked_win_fstatfs(-1, &buf));
  ASSERT_EQ(-1, mocked_win_fstatfs(2048, &buf));

  g_mock_osfhandle_ret = (intptr_t)-1;
  ASSERT_EQ(-1, mocked_win_fstatfs(3, &buf));
  g_mock_osfhandle_ret = 10;

  g_mock_k32_ok = 0;
  ASSERT_EQ(-1, mocked_win_fstatfs(3, &buf));
  g_mock_k32_ok = 1;

  g_mock_finalpath_present = 0;
  ASSERT_EQ(-1, mocked_win_fstatfs(3, &buf));
  g_mock_finalpath_present = 1;

  g_mock_finalpath_res = 0;
  ASSERT_EQ(-1, mocked_win_fstatfs(3, &buf));

  g_mock_finalpath_res = MAX_PATH;
  ASSERT_EQ(-1, mocked_win_fstatfs(3, &buf));

  g_mock_finalpath_res = 3;
  ASSERT_EQ(0, mocked_win_fstatfs(3, &buf));
#else
  (void)tmp;
#endif
  PASS();
}

TEST test_statvfs_fstatvfs(void) {
  struct statvfs buf;
  FILE *tmp = NULL;

  (void)statvfs("/", &buf);
  (void)fstatvfs(0, &buf);

#if defined(_MSC_VER) || defined(_WIN32)
  ASSERT_EQ(-1, statvfs(NULL, &buf));
  ASSERT_EQ(-1, statvfs(".", NULL));
  ASSERT_EQ(0, statvfs(".", &buf));
  ASSERT_EQ(-1, statvfs("Z:\\nonexistent_statvfs_drive_999\\sub", &buf));
  ASSERT_EQ(-1, fstatvfs(0, NULL));
  ASSERT_EQ(-1, fstatvfs(-1, &buf));
  ASSERT_EQ(-1, fstatvfs(3000, &buf));
  ASSERT_EQ(-1, fstatvfs(999, &buf));
#if defined(_MSC_VER)
  if (fopen_s(&tmp, "test_statvfs_tmp.txt", "wb+") != 0) {
    tmp = NULL;
  }
#else
  tmp = fopen("test_statvfs_tmp.txt", "wb+");
#endif
  if (tmp != NULL) {
    (void)fstatvfs(_fileno(tmp), &buf);
    fclose(tmp);
    remove("test_statvfs_tmp.txt");
  }

  ASSERT_EQ(-1, mocked_win_statvfs(".", NULL));
  ASSERT_EQ(-1, mocked_win_statvfs(NULL, &buf));

  g_mock_diskfree_ok = 1;
  ASSERT_EQ(0, mocked_win_statvfs("C:\\", &buf));
  ASSERT_EQ((linux_fsblkcnt_t)200, buf.f_blocks);

  g_mock_diskfree_ok = 0;
  ASSERT_EQ(-1, mocked_win_statvfs("C:\\", &buf));
  g_mock_diskfree_ok = 1;

  ASSERT_EQ(-1, mocked_win_fstatvfs(0, NULL));
  ASSERT_EQ(-1, mocked_win_fstatvfs(-1, &buf));
  ASSERT_EQ(-1, mocked_win_fstatvfs(2048, &buf));

  g_mock_osfhandle_ret = (intptr_t)-1;
  ASSERT_EQ(-1, mocked_win_fstatvfs(3, &buf));
  g_mock_osfhandle_ret = 10;

  g_mock_k32_ok = 0;
  ASSERT_EQ(-1, mocked_win_fstatvfs(3, &buf));
  g_mock_k32_ok = 1;

  g_mock_finalpath_present = 0;
  ASSERT_EQ(-1, mocked_win_fstatvfs(3, &buf));
  g_mock_finalpath_present = 1;

  g_mock_finalpath_res = 0;
  ASSERT_EQ(-1, mocked_win_fstatvfs(3, &buf));

  g_mock_finalpath_res = MAX_PATH;
  ASSERT_EQ(-1, mocked_win_fstatvfs(3, &buf));

  g_mock_finalpath_res = 3;
  ASSERT_EQ(0, mocked_win_fstatvfs(3, &buf));
#else
  (void)tmp;
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

FILE *mock_setmntent(const char *filename, const char *type);
struct mntent *mock_getmntent(FILE *stream);
int mock_endmntent(FILE *streamp);
char *mock_hasmntopt(const struct mntent *mnt, const char *opt);
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
  char buf[64];

  memcpy(buf, "C:\\\\\\\0D:/\0E:\0F\0\\\\?\\Volume{0123456789}\\\0\0", 40);

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
  ASSERT_EQ(1, endmntent(NULL));
  ASSERT_EQ(1, endmntent(f));

  /* hasmntopt */
  ASSERT(hasmntopt(NULL, "rw") == NULL);
  ASSERT(hasmntopt(&s_mnt, NULL) == NULL);

  s_mnt.mnt_opts = NULL;
  ASSERT(hasmntopt(&s_mnt, "rw") == NULL);

  {
    char rw_opts[] = "rw";
    s_mnt.mnt_opts = rw_opts;
    opt = hasmntopt(&s_mnt, "rw");
    ASSERT(opt != NULL);
  }

  /* setmntent fail cases */
  mock_GetLogicalDriveStringsA_ret = 0;
  f = setmntent("file", "r");
  ASSERT(f == NULL);

  mock_GetLogicalDriveStringsA_ret = 1024;
  f = setmntent("file", "r");
  ASSERT(f == NULL);

  /* Success case including long > 16-char volume name truncation */
  mock_GetLogicalDriveStringsA_ret = 39;
  mock_GetLogicalDriveStringsA_size = 40;
  mock_GetLogicalDriveStringsA_buf = buf;
  f = setmntent("file", "r");
  ASSERT(f != NULL);

  /* 5 entries: C:\, D:/, E:, F, \?\Volume{0123456789}\ */
  m = getmntent(f);
  ASSERT(m != NULL);
  m = getmntent(f);
  ASSERT(m != NULL);
  m = getmntent(f);
  ASSERT(m != NULL);
  m = getmntent(f);
  ASSERT(m != NULL);
  m = getmntent(f);
  ASSERT(m != NULL);
  ASSERT_EQ(15, (int)strlen(m->mnt_dir));

  /* Exhausted */
  m = getmntent(f);
  ASSERT(m == NULL);

  /* getmntent when stream != NULL and s_drive_ptr points directly at '\0' */
  s_drive_ptr = "";
  m = getmntent(f);
  ASSERT(m == NULL);

  /* getmntent when stream != NULL and s_drive_ptr == NULL */
  s_drive_ptr = NULL;
  m = getmntent(f);
  ASSERT(m == NULL);

  /* endmntent when stream != NULL */
  ASSERT_EQ(1, endmntent(f));

  PASS();
}

#undef setmntent
#undef getmntent
#undef endmntent
#undef hasmntopt

TEST test_native_mntent(void) {
  FILE *f;
  struct mntent *m;
  struct mntent empty_mnt;

  memset(&empty_mnt, 0, sizeof(empty_mnt));

#if !defined(__linux__) && !defined(__GLIBC__) && !defined(__CYGWIN__)
  f = setmntent(NULL, "r");
  ASSERT(f == NULL);
  f = setmntent("/etc/mtab", NULL);
  ASSERT(f == NULL);
#endif

  f = setmntent("/etc/mtab", "r");
  if (f != NULL) {
    while ((m = getmntent(f)) != NULL) {
      ASSERT(m->mnt_dir != NULL);
      (void)hasmntopt(m, "rw");
    }
#if !defined(__linux__) && !defined(__GLIBC__) && !defined(__CYGWIN__)
    ASSERT(getmntent(NULL) == NULL);
#endif
#if !defined(__linux__) && !defined(__GLIBC__)
    ASSERT(hasmntopt(NULL, "rw") == NULL);
    ASSERT(hasmntopt(&empty_mnt, NULL) == NULL);
    ASSERT(hasmntopt(&empty_mnt, "rw") == NULL);
#endif
    ASSERT_EQ(1, endmntent(f));
  }
#if !defined(__linux__) && !defined(__GLIBC__) && !defined(__CYGWIN__)
  ASSERT_EQ(1, endmntent(NULL));
#endif
  PASS();
}

SUITE(suite_linux_sys_statfs_extra) {
  RUN_TEST(test_mntent_branches);
  RUN_TEST(test_native_mntent);
}
