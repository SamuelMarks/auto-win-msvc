/* posix-stat.c - Strict C89 Implementation */

/* clang-format off */
#include "posix-stat.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__GNUC__) || defined(__clang__)
__extension__ typedef unsigned long long stat_u64;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 stat_u64;
#define U64_C(x) x##UI64
#else
typedef unsigned long long stat_u64;
#endif

#ifdef _WIN32
#include <direct.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#include <stdlib.h>
static void null_invalid_param_handler(const wchar_t *expression,
                                       const wchar_t *function,
                                       const wchar_t *file, unsigned int line,
                                       uintptr_t pReserved) {
  if (expression || function || file || line || pReserved) {
    /* parameters checked */
  }
}
#endif

#define WINAPI __stdcall
typedef void *HANDLE;
typedef void *HMODULE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef int BOOL;
typedef char *LPSTR;

#define MAX_PATH 260
#define INVALID_HANDLE_VALUE ((HANDLE)(size_t)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
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

typedef struct _BY_HANDLE_FILE_INFORMATION {
  DWORD dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD dwVolumeSerialNumber;
  DWORD nFileSizeHigh;
  DWORD nFileSizeLow;
  DWORD nNumberOfLinks;
  DWORD nFileIndexHigh;
  DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION;

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
__declspec(dllimport) BOOL WINAPI GetFileInformationByHandle(
    HANDLE hFile, BY_HANDLE_FILE_INFORMATION *lpFileInformation);
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

#define IS_ABSOLUTE_PATH(p)                                                    \
  ((p)[0] == '\\' || (p)[0] == '/' ||                                          \
   (((p)[0] != '\0') && (p)[1] == ':' && ((p)[2] == '\\' || (p)[2] == '/')))

#endif /* _WIN32 */
/* clang-format on */

/**
 * @brief Retrieves information on posix-stat module availability and status.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
auto_win_msvc_error_t posix_stat_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}
#endif

#ifdef _WIN32

/**
 * @brief Safely retrieves the OS file handle for a given CRT file descriptor.
 * @param[in] fd File descriptor.
 * @param[out] out_handle Pointer to ptrdiff_t receiving the handle or -1.
 * @return AUTO_WIN_MSVC_SUCCESS on success,
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT or AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT
 * on failure.
 */
auto_win_msvc_error_t posix_stat_safe_get_osfhandle(int fd,
                                                    ptrdiff_t *out_handle) {
  ptrdiff_t h;
  if (out_handle == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  if (fd < 0 || fd >= 2048) {
    *out_handle = -1;
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(null_invalid_param_handler);
#if defined(_DEBUG) && !defined(_DLL)
    _CrtSetReportMode(_CRT_ASSERT, 0);
    _CrtSetReportMode(_CRT_ERROR, 0);
    _CrtSetReportMode(_CRT_WARN, 0);
#endif
    h = (ptrdiff_t)_get_osfhandle(fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle(fd);
#endif
  *out_handle = h;
  if (h == -1) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  return AUTO_WIN_MSVC_SUCCESS;
}

/**
 * @brief Resolves a pathname relative to a directory file descriptor on
 * Windows.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Relative or absolute path.
 * @param[out] out_path Output buffer receiving resolved path.
 * @param[in] out_size Size of output buffer in bytes.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code on failure.
 */
auto_win_msvc_error_t posix_stat_resolve_at_path(int dirfd,
                                                 const char *pathname,
                                                 char *out_path,
                                                 size_t out_size) {
  HANDLE hFile;
  HMODULE hKernel32;
  typedef DWORD(WINAPI * GetFinalPathNameByHandleA_t)(HANDLE, LPSTR, DWORD,
                                                      DWORD);
  GetFinalPathNameByHandleA_t pGetFinalPathName;
  DWORD len;
  ptrdiff_t osfh;
  auto_win_msvc_error_t osf_rc;

  if (pathname == NULL || out_path == NULL || out_size == 0) {
    errno = EINVAL;
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }

  if (IS_ABSOLUTE_PATH(pathname) || dirfd == AT_FDCWD) {
#if defined(_MSC_VER)
    strncpy_s(out_path, out_size, pathname, _TRUNCATE);
#else
    strncpy(out_path, pathname, out_size - 1);
    out_path[out_size - 1] = '\0';
#endif
    return AUTO_WIN_MSVC_SUCCESS;
  }

  if (dirfd < 0) {
    errno = EBADF;
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }

  osf_rc = posix_stat_safe_get_osfhandle(dirfd, &osfh);
  if (osf_rc != AUTO_WIN_MSVC_SUCCESS ||
      (HANDLE)(size_t)osfh == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  hFile = (HANDLE)(size_t)osfh;

  hKernel32 = GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    errno = EINVAL;
    return AUTO_WIN_MSVC_ERROR_NOT_FOUND;
  }

  pGetFinalPathName = (GetFinalPathNameByHandleA_t)(size_t)GetProcAddress(
      hKernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathName) {
    errno = ENOSYS;
    return AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED;
  }

  len = pGetFinalPathName(hFile, out_path, (DWORD)out_size, 0);
  if (len == 0 || len >= out_size) {
    errno = EACCES;
    return AUTO_WIN_MSVC_ERROR_NOT_FOUND;
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

  return AUTO_WIN_MSVC_SUCCESS;
}

/**
 * @brief Converts timespec structure to Win32 FILETIME.
 * @param[in] ts Pointer to timespec structure or NULL for current time.
 * @param[out] out_filetime Pointer to FILETIME buffer (as void *) receiving
 * converted timestamp.
 * @param[out] out_omit Pointer to integer receiving 1 if time should be
 * omitted, 0 otherwise.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code on failure.
 */
auto_win_msvc_error_t posix_stat_fill_filetime(const struct timespec *ts,
                                               void *out_filetime,
                                               int *out_omit) {
  FILETIME *ft;
  if (out_filetime == NULL || out_omit == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  ft = (FILETIME *)out_filetime;
  if (ts == NULL) {
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, ft);
    *out_omit = 0;
  } else if (ts->tv_nsec == UTIME_OMIT) {
    *out_omit = 1;
  } else if (ts->tv_nsec == UTIME_NOW) {
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, ft);
    *out_omit = 0;
  } else {
    stat_u64 t;
    t = ((stat_u64)ts->tv_sec * ((stat_u64)10000000UL)) +
        (((stat_u64)116444736UL) * 1000000000UL);
    ft->dwLowDateTime = (DWORD)(t & 0xFFFFFFFF);
    ft->dwHighDateTime = (DWORD)(t >> 32);
    *out_omit = 0;
  }
  return AUTO_WIN_MSVC_SUCCESS;
}

/**
 * @brief Changes permissions of a file descriptor.
 * @param[in] fd File descriptor.
 * @param[in] mode Target file mode.
 * @return 0 on success, -1 on failure with errno set.
 */
int fchmod(int fd, mode_t mode) {
  HANDLE hFile;
  HMODULE hKernel32;
  DWORD attrs;
  char path[MAX_PATH];
  typedef DWORD(WINAPI * GetFinalPathNameByHandleA_t)(HANDLE, LPSTR, DWORD,
                                                      DWORD);
  GetFinalPathNameByHandleA_t pGetFinalPathName;
  ptrdiff_t osfh;
  auto_win_msvc_error_t osf_rc;

  osf_rc = posix_stat_safe_get_osfhandle(fd, &osfh);
  if (osf_rc != AUTO_WIN_MSVC_SUCCESS ||
      (HANDLE)(size_t)osfh == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }
  hFile = (HANDLE)(size_t)osfh;

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

/**
 * @brief Changes permissions of a file relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to file.
 * @param[in] mode Target file mode.
 * @param[in] flags Bitwise flags controlling behavior.
 * @return 0 on success, -1 on failure with errno set.
 */
int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
  char fullpath[MAX_PATH];
  auto_win_msvc_error_t rc;

  rc = posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath));
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }
  if (flags & AT_SYMLINK_NOFOLLOW) {
    /* _chmod does not follow symlinks natively on older Windows */
  }
  return _chmod(fullpath, mode);
}

/**
 * @brief Populates extended POSIX.1-2008 fields on struct posix_stat from
 * standard fields and optional handle information.
 * @param[in,out] st Pointer to struct posix_stat.
 * @param[in] hFile Optional open Win32 file handle, or INVALID_HANDLE_VALUE.
 */
static void populate_posix_stat_extras(struct posix_stat *st, HANDLE hFile) {
  BY_HANDLE_FILE_INFORMATION bhfi;
  st->st_blksize = 4096;
  st->st_blocks = (blkcnt_t)((st->st_size + 511) / 512);
  st->st_atim.tv_sec = (time_t)st->st_atime;
  st->st_atim.tv_nsec = 0;
  st->st_mtim.tv_sec = (time_t)st->st_mtime;
  st->st_mtim.tv_nsec = 0;
  st->st_ctim.tv_sec = (time_t)st->st_ctime;
  st->st_ctim.tv_nsec = 0;
  if (hFile != INVALID_HANDLE_VALUE &&
      GetFileInformationByHandle(hFile, &bhfi)) {
    unsigned int idx = (unsigned int)(bhfi.nFileIndexLow ^ bhfi.nFileIndexHigh);
    if (bhfi.dwVolumeSerialNumber != 0) {
      st->st_dev = (_dev_t)(bhfi.dwVolumeSerialNumber & 0x7FFFU);
    }
    if (idx != 0) {
      st->st_ino = (_ino_t)(idx & 0xFFFFU);
      if (st->st_ino == 0) {
        st->st_ino = 1;
      }
    }
    if (bhfi.nNumberOfLinks > 0 && bhfi.nNumberOfLinks < 32767) {
      st->st_nlink = (short)bhfi.nNumberOfLinks;
    }
  }
}

/**
 * @brief Checks if pathname refers to a virtual /proc entry and populates
 * statbuf.
 * @param[in] pathname Path to check.
 * @param[out] statbuf Structure receiving synthetic status.
 * @return 1 if handled as a virtual /proc path, 0 otherwise.
 */
static int is_virtual_proc_stat(const char *pathname,
                                struct posix_stat *statbuf) {
  if (!pathname || !statbuf) {
    return 0;
  }
  if (strcmp(pathname, "/dev/null") == 0 ||
      strcmp(pathname, "\\dev\\null") == 0) {
    memset(statbuf, 0, sizeof(*statbuf));
    statbuf->st_mode =
        S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    statbuf->st_nlink = 1;
    statbuf->st_ino = (_ino_t)0xFFFE;
    statbuf->st_blksize = 4096;
    return 1;
  }
  if (strcmp(pathname, "/proc") == 0 || strcmp(pathname, "/proc/") == 0 ||
      strcmp(pathname, "\\proc") == 0 || strcmp(pathname, "\\proc\\") == 0 ||
      strncmp(pathname, "/proc/", 6) == 0 ||
      strncmp(pathname, "\\proc\\", 6) == 0 ||
      strcmp(pathname, "version") == 0 || strcmp(pathname, "uptime") == 0 ||
      strcmp(pathname, "meminfo") == 0 || strcmp(pathname, "loadavg") == 0 ||
      strcmp(pathname, "stat") == 0 || strcmp(pathname, "mounts") == 0 ||
      strcmp(pathname, "filesystems") == 0 || strcmp(pathname, "swaps") == 0 ||
      strcmp(pathname, "cpuinfo") == 0) {
    const char *sub = pathname;
    int is_dir = 0;
    if (strncmp(pathname, "/proc/", 6) == 0 ||
        strncmp(pathname, "\\proc\\", 6) == 0) {
      sub = pathname + 6;
      if (sub[0] == '\0') {
        is_dir = 1;
      }
    } else if (strcmp(pathname, "/proc") == 0 ||
               strcmp(pathname, "\\proc") == 0) {
      is_dir = 1;
    }
    if (!is_dir &&
        ((sub[0] >= '0' && sub[0] <= '9') || strncmp(sub, "self", 4) == 0)) {
      const char *slash = strchr(sub, '/');
      if (!slash) {
        slash = strchr(sub, '\\');
      }
      if (!slash || slash[1] == '\0') {
        is_dir = 1;
      }
    }
    memset(statbuf, 0, sizeof(*statbuf));
    statbuf->st_mode =
        (is_dir ? S_IFDIR : S_IFREG) | S_IRUSR | S_IXUSR | S_IRGRP | S_IROTH;
    statbuf->st_nlink = 1;
    statbuf->st_blksize = 4096;
    return 1;
  }
  return 0;
}

/**
 * @brief Obtains file status, supporting virtual /proc paths and Windows drive
 * paths.
 * @param[in] pathname Path to file or directory.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_stat(const char *pathname, void *statbuf) {
  struct posix_stat *st = (struct posix_stat *)statbuf;
  struct _stat64 raw_st;
  int rc;
  HANDLE hFile;
  if (pathname == NULL || st == NULL) {
    errno = EINVAL;
    return -1;
  }
  while (pathname[0] == '/' && pathname[1] == '/') {
    pathname++;
  }
  if (is_virtual_proc_stat(pathname, st)) {
    return 0;
  }
  memset(&raw_st, 0, sizeof(raw_st));
  rc = _stat64(pathname, &raw_st);
  if (rc != 0) {
    return rc;
  }
  memset(st, 0, sizeof(*st));
  memcpy(st, &raw_st, sizeof(raw_st));
  hFile = CreateFileA(pathname, 0,
                      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  populate_posix_stat_extras(st, hFile);
  if (hFile != INVALID_HANDLE_VALUE) {
    CloseHandle(hFile);
  }
  return 0;
}

/**
 * @brief Obtains file status by file descriptor.
 * @param[in] fd File descriptor.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_fstat(int fd, void *statbuf) {
  struct posix_stat *st = (struct posix_stat *)statbuf;
  struct _stat64 raw_st;
  int rc;
  ptrdiff_t osfh = -1;
  HANDLE hFile = INVALID_HANDLE_VALUE;
  if (st == NULL) {
    errno = EINVAL;
    return -1;
  }
  memset(&raw_st, 0, sizeof(raw_st));
  rc = _fstat64(fd, &raw_st);
  if (rc != 0) {
    return rc;
  }
  memset(st, 0, sizeof(*st));
  memcpy(st, &raw_st, sizeof(raw_st));
  if (posix_stat_safe_get_osfhandle(fd, &osfh) == AUTO_WIN_MSVC_SUCCESS &&
      osfh != -1) {
    hFile = (HANDLE)(size_t)osfh;
  }
  populate_posix_stat_extras(st, hFile);
  return 0;
}

/**
 * @brief Obtains file status relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to file.
 * @param[out] statbuf Structure receiving file status.
 * @param[in] flags Flags (e.g. AT_SYMLINK_NOFOLLOW).
 * @return 0 on success, -1 on failure with errno set.
 */
int fstatat(int dirfd, const char *pathname, void *statbuf, int flags) {
  char fullpath[MAX_PATH];
  auto_win_msvc_error_t rc;

  rc = posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath));
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }
  if (flags & AT_SYMLINK_NOFOLLOW) {
    return posix_lstat(fullpath, statbuf);
  }
  return posix_stat(fullpath, statbuf);
}

/**
 * @brief Sets file access and modification times by file descriptor.
 * @param[in] fd File descriptor.
 * @param[in] times Array of two timespec structures.
 * @return 0 on success, -1 on failure with errno set.
 */
int futimens(int fd, const struct timespec times[2]) {
  HANDLE hFile;
  FILETIME atime, mtime;
  FILETIME *pAtime;
  FILETIME *pMtime;
  int omit_a;
  int omit_m;
  ptrdiff_t osfh;
  auto_win_msvc_error_t rc;

  pAtime = NULL;
  pMtime = NULL;
  omit_a = 0;
  omit_m = 0;

  rc = posix_stat_safe_get_osfhandle(fd, &osfh);
  if (rc != AUTO_WIN_MSVC_SUCCESS ||
      (HANDLE)(size_t)osfh == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }
  hFile = (HANDLE)(size_t)osfh;

  if (times != NULL) {
    rc = posix_stat_fill_filetime(&times[0], &atime, &omit_a);
    if (rc != AUTO_WIN_MSVC_SUCCESS) {
      errno = auto_win_msvc_error_to_errno(rc);
      return -1;
    }
    if (!omit_a) {
      pAtime = &atime;
    }
    rc = posix_stat_fill_filetime(&times[1], &mtime, &omit_m);
    if (rc != AUTO_WIN_MSVC_SUCCESS) {
      errno = auto_win_msvc_error_to_errno(rc);
      return -1;
    }
    if (!omit_m) {
      pMtime = &mtime;
    }
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

/**
 * @brief Obtains file status without following symbolic links.
 * @param[in] pathname Path to file.
 * @param[out] statbuf_ptr Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_lstat(const char *pathname, void *statbuf_ptr) {
  struct posix_stat *statbuf = (struct posix_stat *)statbuf_ptr;
  WIN32_FILE_ATTRIBUTE_DATA info;
  stat_u64 t;

  if (pathname == NULL || statbuf == NULL) {
    errno = EINVAL;
    return -1;
  }
  while (pathname[0] == '/' && pathname[1] == '/') {
    pathname++;
  }

  if (is_virtual_proc_stat(pathname, statbuf)) {
    return 0;
  }

  if (!GetFileAttributesExA(pathname, GetFileExInfoStandard, &info)) {
    errno = ENOENT;
    return -1;
  }
  if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
    memset(statbuf, 0, sizeof(*statbuf));
    statbuf->st_mode = S_IFLNK | S_IRWXU | S_IRWXG | S_IRWXO;
    statbuf->st_size = 0;

    t = ((stat_u64)info.ftLastAccessTime.dwHighDateTime << 32) |
        info.ftLastAccessTime.dwLowDateTime;
    statbuf->st_atime =
        (time_t)((t - (((stat_u64)116444736UL) * 1000000000UL)) /
                 ((stat_u64)10000000UL));

    t = ((stat_u64)info.ftLastWriteTime.dwHighDateTime << 32) |
        info.ftLastWriteTime.dwLowDateTime;
    statbuf->st_mtime =
        (time_t)((t - (((stat_u64)116444736UL) * 1000000000UL)) /
                 ((stat_u64)10000000UL));

    t = ((stat_u64)info.ftCreationTime.dwHighDateTime << 32) |
        info.ftCreationTime.dwLowDateTime;
    statbuf->st_ctime =
        (time_t)((t - (((stat_u64)116444736UL) * 1000000000UL)) /
                 ((stat_u64)10000000UL));

    statbuf->st_dev = 0;
    statbuf->st_ino = 0;
    statbuf->st_nlink = 1;
    statbuf->st_uid = 0;
    statbuf->st_gid = 0;
    statbuf->st_rdev = 0;
    populate_posix_stat_extras(statbuf, INVALID_HANDLE_VALUE);
    return 0;
  }
  return posix_stat(pathname, statbuf);
}

/**
 * @brief Creates a filesystem node.
 * @param[in] pathname Path to create.
 * @param[in] mode Mode specifying type and permissions.
 * @param[in] dev Device identifier.
 * @return 0 on success, -1 on failure with errno set.
 */
int mknod(const char *pathname, mode_t mode, unsigned int dev) {
  HANDLE hFile;
  if (dev) {
    /* parameters checked */
  }

  if (pathname == NULL) {
    errno = EINVAL;
    return -1;
  }
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

/**
 * @brief Creates a filesystem node relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to create.
 * @param[in] mode Mode specifying type and permissions.
 * @param[in] dev Device identifier.
 * @return 0 on success, -1 on failure with errno set.
 */
int mknodat(int dirfd, const char *pathname, mode_t mode, unsigned int dev) {
  char fullpath[MAX_PATH];
  auto_win_msvc_error_t rc;

  rc = posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath));
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }
  return mknod(fullpath, mode, dev);
}

/**
 * @brief Sets file timestamps relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to file.
 * @param[in] times Array of two timespec structures.
 * @param[in] flags Flags (e.g. AT_SYMLINK_NOFOLLOW).
 * @return 0 on success, -1 on failure with errno set.
 */
int utimensat(int dirfd, const char *pathname, const struct timespec times[2],
              int flags) {
  HANDLE hFile;
  DWORD attrs;
  FILETIME atime, mtime;
  FILETIME *pAtime;
  FILETIME *pMtime;
  int omit_a;
  int omit_m;
  char fullpath[MAX_PATH];
  auto_win_msvc_error_t rc;

  attrs = FILE_FLAG_BACKUP_SEMANTICS;
  pAtime = NULL;
  pMtime = NULL;
  omit_a = 0;
  omit_m = 0;

  rc = posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath));
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
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
    rc = posix_stat_fill_filetime(&times[0], &atime, &omit_a);
    if (rc != AUTO_WIN_MSVC_SUCCESS) {
      CloseHandle(hFile);
      errno = auto_win_msvc_error_to_errno(rc);
      return -1;
    }
    if (!omit_a) {
      pAtime = &atime;
    }
    rc = posix_stat_fill_filetime(&times[1], &mtime, &omit_m);
    if (rc != AUTO_WIN_MSVC_SUCCESS) {
      CloseHandle(hFile);
      errno = auto_win_msvc_error_to_errno(rc);
      return -1;
    }
    if (!omit_m) {
      pMtime = &mtime;
    }
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

#else /* !_WIN32 */

#undef stat
#undef lstat

/**
 * @brief Checks if pathname refers to a virtual /proc entry on non-Linux POSIX
 * systems and populates statbuf.
 * @param[in] pathname Path to check.
 * @param[out] statbuf Structure receiving synthetic status.
 * @return 1 if handled as a virtual /proc path, 0 otherwise.
 */
static int is_virtual_proc_stat_posix(const char *pathname,
                                      struct stat *statbuf) {
#if defined(__linux__)
  (void)pathname;
  (void)statbuf;
  return 0;
#elif defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
  if (strncmp(pathname, "/proc/", 6) == 0) {
    const char *sub = pathname + 6;
    const char *slash = strchr(sub, '/');
    int is_dir = (slash == NULL);
    memset(statbuf, 0, sizeof(*statbuf));
    statbuf->st_mode = (mode_t)((is_dir ? S_IFDIR : S_IFREG) | S_IRUSR |
                                S_IXUSR | S_IRGRP | S_IROTH);
    statbuf->st_nlink = 1;
    statbuf->st_uid = getuid();
    statbuf->st_gid = getgid();
#if !defined(__STRICT_ANSI__) && defined(_DARWIN_C_SOURCE)
    statbuf->st_blksize = 4096;
#endif
    return 1;
  }
  return 0;
#else
  if (!pathname || !statbuf) {
    return 0;
  }
  if (strcmp(pathname, "/proc") == 0 || strcmp(pathname, "/proc/") == 0 ||
      strncmp(pathname, "/proc/", 6) == 0) {
    const char *sub = pathname;
    int is_dir = 0;
    if (strncmp(pathname, "/proc/", 6) == 0) {
      sub = pathname + 6;
      if (sub[0] == '\0') {
        is_dir = 1;
      }
    } else {
      is_dir = 1;
    }
    if (!is_dir &&
        ((sub[0] >= '0' && sub[0] <= '9') || strncmp(sub, "self", 4) == 0)) {
      const char *slash = strchr(sub, '/');
      if (!slash || slash[1] == '\0') {
        is_dir = 1;
      }
    }
    memset(statbuf, 0, sizeof(*statbuf));
    statbuf->st_mode = (mode_t)((is_dir ? S_IFDIR : S_IFREG) | S_IRUSR |
                                S_IXUSR | S_IRGRP | S_IROTH);
    statbuf->st_nlink = 1;
    statbuf->st_uid = getuid();
    statbuf->st_gid = getgid();
#if !defined(__STRICT_ANSI__) && defined(_DARWIN_C_SOURCE)
    statbuf->st_blksize = 4096;
#endif
    return 1;
  }
  return 0;
#endif
}

/**
 * @brief Obtains file status, supporting virtual /proc paths on non-Linux
 * POSIX systems.
 * @param[in] pathname Path to file or directory.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_stat(const char *pathname, void *statbuf) {
  struct stat *st = (struct stat *)statbuf;
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (pathname == NULL || st == NULL) {
    errno = EINVAL;
    return -1;
  }
  while (pathname[0] == '/' && pathname[1] == '/') {
    pathname++;
  }
#endif
  if (is_virtual_proc_stat_posix(pathname, st)) {
    return 0;
  }
  return stat(pathname, st);
}

/**
 * @brief Obtains file status without following symbolic links, supporting
 * virtual /proc paths on non-Linux POSIX systems.
 * @param[in] pathname Path to file or directory.
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int posix_lstat(const char *pathname, void *statbuf) {
  struct stat *st = (struct stat *)statbuf;
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (pathname == NULL || st == NULL) {
    errno = EINVAL;
    return -1;
  }
  while (pathname[0] == '/' && pathname[1] == '/') {
    pathname++;
  }
  if (is_virtual_proc_stat_posix(pathname, st)) {
    return 0;
  }
#endif
  return lstat(pathname, st);
}

#endif /* _WIN32 */

/* Prevent empty translation unit on ISO C compilers */
typedef int make_iso_compilers_happy_tu_posix_stat;
