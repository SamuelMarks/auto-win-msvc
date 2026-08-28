/* posix-stat.c - Strict C89 Implementation */

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wlong-long"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

/* clang-format off */
#include "posix-stat.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle(fd))
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif

#ifdef _WIN32
#include <direct.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
/* clang-format on */
#endif

#define WINAPI __stdcall
typedef void *HANDLE;
typedef void *HMODULE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef int BOOL;
typedef char *LPSTR;

#define MAX_PATH 260
/** \brief INVALID_HANDLE_VALUE macro. */
#define INVALID_HANDLE_VALUE ((HANDLE)(size_t)-1)
/** \brief INVALID_FILE_ATTRIBUTES macro. */
#define INVALID_FILE_ATTRIBUTES ((DWORD) - 1)
#define FILE_ATTRIBUTE_READONLY 1
#define FILE_ATTRIBUTE_NORMAL 128
#define FILE_ATTRIBUTE_REPARSE_POINT 1024
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000
#define GENERIC_WRITE 0x40000000
#define FILE_WRITE_ATTRIBUTES 0x0100
#define CREATE_NEW 1
#define OPEN_EXISTING 3
#define ERROR_FILE_EXISTS 80
#define FILE_SHARE_READ 1
#define FILE_SHARE_WRITE 2
#define FILE_SHARE_DELETE 4

typedef struct _FILETIME {
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
} FILETIME;

typedef struct _SYSTEMTIME {
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME;

typedef enum _GET_FILEEX_INFO_LEVELS {
  GetFileExInfoStandard,
  GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
  DWORD dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD nFileSizeHigh;
  DWORD nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA;

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) HMODULE WINAPI GetModuleHandleA(const char *lpModuleName);
__declspec(dllimport) void *WINAPI GetProcAddress(HMODULE hModule,
                                                  const char *lpProcName);
__declspec(dllimport) DWORD WINAPI GetFileAttributesA(const char *lpFileName);
__declspec(dllimport) BOOL WINAPI SetFileAttributesA(const char *lpFileName,
                                                     DWORD dwFileAttributes);
__declspec(dllimport) BOOL WINAPI GetFileAttributesExA(
    const char *lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
    void *lpFileInformation);
__declspec(dllimport) void WINAPI GetSystemTime(SYSTEMTIME *lpSystemTime);
__declspec(dllimport) BOOL WINAPI
SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, FILETIME *lpFileTime);
__declspec(dllimport) BOOL WINAPI SetFileTime(HANDLE hFile,
                                              const FILETIME *lpCreationTime,
                                              const FILETIME *lpLastAccessTime,
                                              const FILETIME *lpLastWriteTime);
__declspec(dllimport) HANDLE WINAPI
CreateFileA(const char *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
            void *lpSecurityAttributes, DWORD dwCreationDisposition,
            DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE hObject);
__declspec(dllimport) DWORD WINAPI GetLastError(void);

#ifdef __cplusplus
}
#endif
#endif /* _WIN32 */

#ifdef _WIN32
#define IS_ABSOLUTE_PATH(p)                                                    \
  ((p)[0] == '\\' || (p)[0] == '/' ||                                          \
   (((p)[0] != '\0') && (p)[1] == ':' && ((p)[2] == '\\' || (p)[2] == '/')))

static int posix_stat_resolve_at_path(int dirfd, const char *pathname,
                                      char *out_path, size_t out_size) {
  HANDLE hFile;
  HMODULE hKernel32;
  typedef DWORD(WINAPI * GetFinalPathNameByHandleA_t)(HANDLE, LPSTR, DWORD,
                                                      DWORD);
  GetFinalPathNameByHandleA_t pGetFinalPathName;
  DWORD len;

  if (IS_ABSOLUTE_PATH(pathname) || dirfd == AT_FDCWD || dirfd == -1) {
#if defined(_MSC_VER)
    strncpy_s(out_path, out_size, pathname, _TRUNCATE);
#else
    strncpy(out_path, pathname, out_size - 1);
    out_path[out_size - 1] = '\0';
#endif
    return 0;
  }

  hFile = (HANDLE)(size_t)safe_get_osfhandle(dirfd);
  if (hFile == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }

  hKernel32 = GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    errno = EINVAL;
    return -1;
  }

  pGetFinalPathName = (GetFinalPathNameByHandleA_t)(size_t)GetProcAddress(
      hKernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathName) {
    errno = ENOSYS;
    return -1;
  }

  len = pGetFinalPathName(hFile, out_path, (DWORD)out_size, 0);
  if (len == 0 || len >= out_size) {
    errno = EACCES;
    return -1;
  }

  /* Strip \\?\ prefix if present */
  if (strncmp(out_path, "\\\\?\\", 4) == 0) {
    memmove(out_path, out_path + 4, len - 3);
    len -= 4;
  }

  if (len > 0 && out_path[len - 1] != '\\' && out_path[len - 1] != '/') {
    if (len + 1 < out_size) {
      out_path[len] = '\\';
      out_path[len + 1] = '\0';
      len++;
    }
  }

#if defined(_MSC_VER)
  strncat_s(out_path, out_size, pathname, _TRUNCATE);
#else
  strncat(out_path, pathname, out_size - len - 1);
#endif

  return 0;
}

/** \brief fchmod function. */
int fchmod(int fd, mode_t mode) {
  HANDLE hFile;
  HMODULE hKernel32;
  DWORD attrs;
  char path[MAX_PATH];
  typedef DWORD(WINAPI * GetFinalPathNameByHandleA_t)(HANDLE, LPSTR, DWORD,
                                                      DWORD);
  GetFinalPathNameByHandleA_t pGetFinalPathName;

  hFile = (HANDLE)(size_t)safe_get_osfhandle(fd);
  if (hFile == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }

  hKernel32 = GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    errno = EINVAL;
    return -1;
  }

  pGetFinalPathName = (GetFinalPathNameByHandleA_t)(size_t)GetProcAddress(
      hKernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathName) {
    errno = EINVAL;
    return -1;
  }

  if (pGetFinalPathName(hFile, path, MAX_PATH, 0) == 0) {
    errno = EACCES;
    return -1;
  }

  attrs = GetFileAttributesA(path);
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    errno = ENOENT;
    return -1;
  }

  if (!(mode & _S_IWRITE)) {
    attrs |= FILE_ATTRIBUTE_READONLY;
  } else {
    attrs &= ~FILE_ATTRIBUTE_READONLY;
  }

  if (!SetFileAttributesA(path, attrs)) {
    errno = EACCES;
    return -1;
  }

  return 0;
}

/** \brief fchmodat function. */
int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
  char fullpath[MAX_PATH];
  if (posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath)) !=
      0) {
    return -1;
  }
  if (flags & AT_SYMLINK_NOFOLLOW) {
    /* _chmod doesn't natively follow symlinks in older Windows,
       but we do the best we can here. */
  }
  return _chmod(fullpath, mode);
}

/** \brief fstatat function. */
int fstatat(int dirfd, const char *pathname, struct _stat64 *statbuf,
            int flags) {
  char fullpath[MAX_PATH];
  if (posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath)) !=
      0) {
    return -1;
  }
  if (flags & AT_SYMLINK_NOFOLLOW) {
    return lstat(fullpath, statbuf);
  }
  return _stat64(fullpath, statbuf);
}

/** \brief fill_filetime function. */
static void fill_filetime(const struct timespec *ts, FILETIME *ft, int *omit) {
  if (ts == NULL) {
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, ft);
    *omit = 0;
  } else if (ts->tv_nsec == UTIME_OMIT) {
    *omit = 1;
  } else if (ts->tv_nsec == UTIME_NOW) {
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, ft);
    *omit = 0;
  } else {
    unsigned long long t =
        ((unsigned long long)ts->tv_sec * 10000000ULL) + 116444736000000000ULL;
    ft->dwLowDateTime = (DWORD)(t & 0xFFFFFFFF);
    ft->dwHighDateTime = (DWORD)(t >> 32);
    *omit = 0;
  }
}

/** \brief futimens function. */
int futimens(int fd, const struct timespec times[2]) {
  HANDLE hFile = (HANDLE)(size_t)safe_get_osfhandle(fd);
  FILETIME atime, mtime;
  FILETIME *pAtime = NULL, *pMtime = NULL;
  int omit_a = 0, omit_m = 0;

  if (hFile == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }

  if (times != NULL) {
    fill_filetime(&times[0], &atime, &omit_a);
    if (!omit_a)
      pAtime = &atime;
    fill_filetime(&times[1], &mtime, &omit_m);
    if (!omit_m)
      pMtime = &mtime;
  } else {
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &atime);
    pAtime = &atime;
    pMtime = &atime;
  }

  if (!SetFileTime(hFile, NULL, pAtime, pMtime)) {
    errno = EACCES;
    return -1;
  }
  return 0;
}

/** \brief lstat function. */
int lstat(const char *pathname, struct _stat64 *statbuf) {
  WIN32_FILE_ATTRIBUTE_DATA info;
  unsigned long long t;
  if (!GetFileAttributesExA(pathname, GetFileExInfoStandard, &info)) {
    errno = ENOENT;
    return -1;
  }
  if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
    statbuf->st_mode = S_IFLNK | S_IRWXU | S_IRWXG | S_IRWXO;
    statbuf->st_size = 0;

    t = ((unsigned long long)info.ftLastAccessTime.dwHighDateTime << 32) |
        info.ftLastAccessTime.dwLowDateTime;
    statbuf->st_atime = (time_t)((t - 116444736000000000ULL) / 10000000ULL);

    t = ((unsigned long long)info.ftLastWriteTime.dwHighDateTime << 32) |
        info.ftLastWriteTime.dwLowDateTime;
    statbuf->st_mtime = (time_t)((t - 116444736000000000ULL) / 10000000ULL);

    t = ((unsigned long long)info.ftCreationTime.dwHighDateTime << 32) |
        info.ftCreationTime.dwLowDateTime;
    statbuf->st_ctime = (time_t)((t - 116444736000000000ULL) / 10000000ULL);

    statbuf->st_dev = 0;
    statbuf->st_ino = 0;
    statbuf->st_nlink = 1;
    statbuf->st_uid = 0;
    statbuf->st_gid = 0;
    statbuf->st_rdev = 0;
    return 0;
  }
  return _stat64(pathname, statbuf);
}

/** \brief mknod function. */
int mknod(const char *pathname, mode_t mode, unsigned int dev) {
  HANDLE hFile;
  (void)dev;
  if (S_ISDIR(mode)) {
    return _mkdir(pathname);
  }
  if (S_ISCHR(mode) || S_ISBLK(mode) || S_ISFIFO(mode) || S_ISSOCK(mode)) {
    errno = EINVAL;
    return -1;
  }
  hFile = CreateFileA(pathname, GENERIC_WRITE, 0, NULL, CREATE_NEW,
                      FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    if (GetLastError() == ERROR_FILE_EXISTS) {
      errno = EEXIST;
    } else {
      errno = EINVAL;
    }
    return -1;
  }
  CloseHandle(hFile);
  return 0;
}

/** \brief mknodat function. */
int mknodat(int dirfd, const char *pathname, mode_t mode, unsigned int dev) {
  char fullpath[MAX_PATH];
  if (posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath)) !=
      0) {
    return -1;
  }
  return mknod(fullpath, mode, dev);
}

/** \brief utimensat function. */
int utimensat(int dirfd, const char *pathname, const struct timespec times[2],
              int flags) {
  HANDLE hFile;
  DWORD attrs = FILE_FLAG_BACKUP_SEMANTICS;
  FILETIME atime, mtime;
  FILETIME *pAtime = NULL, *pMtime = NULL;
  int omit_a = 0, omit_m = 0;
  char fullpath[MAX_PATH];

  if (posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath)) !=
      0) {
    return -1;
  }

  if (flags & AT_SYMLINK_NOFOLLOW) {
    attrs |= 0x00200000; /* FILE_FLAG_OPEN_REPARSE_POINT */
  }

  hFile = CreateFileA(fullpath, FILE_WRITE_ATTRIBUTES,
                      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                      NULL, OPEN_EXISTING, attrs, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    errno = ENOENT;
    return -1;
  }

  if (times != NULL) {
    fill_filetime(&times[0], &atime, &omit_a);
    if (!omit_a)
      pAtime = &atime;
    fill_filetime(&times[1], &mtime, &omit_m);
    if (!omit_m)
      pMtime = &mtime;
  } else {
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &atime);
    pAtime = &atime;
    pMtime = &atime;
  }

  if (!SetFileTime(hFile, NULL, pAtime, pMtime)) {
    CloseHandle(hFile);
    errno = EACCES;
    return -1;
  }

  CloseHandle(hFile);
  return 0;
}
#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_stat;
