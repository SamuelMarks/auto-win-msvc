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
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
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

#define IS_ABSOLUTE_PATH(p)                                                    \
  ((p)[0] == '\\' || (p)[0] == '/' ||                                          \
   (((p)[0] != '\0') && (p)[1] == ':' && ((p)[2] == '\\' || (p)[2] == '/')))

#endif /* _WIN32 */
/* clang-format on */

/**
 * @brief Retrieves information on posix-stat module availability and status.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_STAT_SUCCESS on success, or POSIX_STAT_ERROR_NULL_POINTER on
 * NULL pointer.
 */
enum posix_stat_error_code posix_stat_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_STAT_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_STAT_SUCCESS;
}

#ifdef _WIN32

/**
 * @brief Safely retrieves the OS file handle for a given CRT file descriptor.
 * @param[in] fd File descriptor.
 * @param[out] out_handle Pointer to ptrdiff_t receiving the handle or -1.
 * @return POSIX_STAT_SUCCESS on success, POSIX_STAT_ERROR_NULL_POINTER or
 * POSIX_STAT_ERROR_INVALID_ARGUMENT on failure.
 */
enum posix_stat_error_code
posix_stat_safe_get_osfhandle(int fd, ptrdiff_t *out_handle) {
  ptrdiff_t h;
  if (out_handle == NULL) {
    return POSIX_STAT_ERROR_NULL_POINTER;
  }
  if (fd < 0 || fd >= 2048) {
    *out_handle = -1;
    return POSIX_STAT_ERROR_INVALID_ARGUMENT;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(null_invalid_param_handler);
    h = (ptrdiff_t)_get_osfhandle(fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle(fd);
#endif
  *out_handle = h;
  if (h == -1) {
    return POSIX_STAT_ERROR_INVALID_ARGUMENT;
  }
  return POSIX_STAT_SUCCESS;
}

/**
 * @brief Resolves a pathname relative to a directory file descriptor on
 * Windows.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Relative or absolute path.
 * @param[out] out_path Output buffer receiving resolved path.
 * @param[in] out_size Size of output buffer in bytes.
 * @return POSIX_STAT_SUCCESS on success, or error code on failure.
 */
enum posix_stat_error_code posix_stat_resolve_at_path(int dirfd,
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
  enum posix_stat_error_code osf_rc;

  if (pathname == NULL || out_path == NULL || out_size == 0) {
    errno = EINVAL;
    return POSIX_STAT_ERROR_NULL_POINTER;
  }

  if (IS_ABSOLUTE_PATH(pathname) || dirfd == AT_FDCWD) {
#if defined(_MSC_VER)
    strncpy_s(out_path, out_size, pathname, _TRUNCATE);
#else
    strncpy(out_path, pathname, out_size - 1);
    out_path[out_size - 1] = '\0';
#endif
    return POSIX_STAT_SUCCESS;
  }

  if (dirfd < 0) {
    errno = EBADF;
    return POSIX_STAT_ERROR_INVALID_ARGUMENT;
  }

  osf_rc = posix_stat_safe_get_osfhandle(dirfd, &osfh);
  if (osf_rc != POSIX_STAT_SUCCESS ||
      (HANDLE)(size_t)osfh == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return POSIX_STAT_ERROR_INVALID_ARGUMENT;
  }
  hFile = (HANDLE)(size_t)osfh;

  hKernel32 = GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    errno = EINVAL;
    return POSIX_STAT_ERROR_PATH_RESOLUTION;
  }

  pGetFinalPathName = (GetFinalPathNameByHandleA_t)(size_t)GetProcAddress(
      hKernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathName) {
    errno = ENOSYS;
    return POSIX_STAT_ERROR_NOT_SUPPORTED;
  }

  len = pGetFinalPathName(hFile, out_path, (DWORD)out_size, 0);
  if (len == 0 || len >= out_size) {
    errno = EACCES;
    return POSIX_STAT_ERROR_PATH_RESOLUTION;
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

  return POSIX_STAT_SUCCESS;
}

/**
 * @brief Converts timespec structure to Win32 FILETIME.
 * @param[in] ts Pointer to timespec structure or NULL for current time.
 * @param[out] out_filetime Pointer to FILETIME buffer (as void *) receiving
 * converted timestamp.
 * @param[out] out_omit Pointer to integer receiving 1 if time should be
 * omitted, 0 otherwise.
 * @return POSIX_STAT_SUCCESS on success, or error code on failure.
 */
enum posix_stat_error_code posix_stat_fill_filetime(const struct timespec *ts,
                                                    void *out_filetime,
                                                    int *out_omit) {
  FILETIME *ft;
  if (out_filetime == NULL || out_omit == NULL) {
    return POSIX_STAT_ERROR_NULL_POINTER;
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
  return POSIX_STAT_SUCCESS;
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
  enum posix_stat_error_code osf_rc;

  osf_rc = posix_stat_safe_get_osfhandle(fd, &osfh);
  if (osf_rc != POSIX_STAT_SUCCESS ||
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
  enum posix_stat_error_code rc;

  rc = posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath));
  if (rc != POSIX_STAT_SUCCESS) {
    return -1;
  }
  if (flags & AT_SYMLINK_NOFOLLOW) {
    /* _chmod does not follow symlinks natively on older Windows */
  }
  return _chmod(fullpath, mode);
}

/**
 * @brief Obtains file status relative to a directory file descriptor.
 * @param[in] dirfd Directory file descriptor or AT_FDCWD.
 * @param[in] pathname Path to file.
 * @param[out] statbuf Structure receiving file status.
 * @param[in] flags Flags (e.g. AT_SYMLINK_NOFOLLOW).
 * @return 0 on success, -1 on failure with errno set.
 */
int fstatat(int dirfd, const char *pathname, struct _stat64 *statbuf,
            int flags) {
  char fullpath[MAX_PATH];
  enum posix_stat_error_code rc;

  rc = posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath));
  if (rc != POSIX_STAT_SUCCESS) {
    return -1;
  }
  if (flags & AT_SYMLINK_NOFOLLOW) {
    return lstat(fullpath, statbuf);
  }
  return _stat64(fullpath, statbuf);
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
  enum posix_stat_error_code rc;

  pAtime = NULL;
  pMtime = NULL;
  omit_a = 0;
  omit_m = 0;

  rc = posix_stat_safe_get_osfhandle(fd, &osfh);
  if (rc != POSIX_STAT_SUCCESS ||
      (HANDLE)(size_t)osfh == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }
  hFile = (HANDLE)(size_t)osfh;

  if (times != NULL) {
    rc = posix_stat_fill_filetime(&times[0], &atime, &omit_a);
    if (rc != POSIX_STAT_SUCCESS) {
      return -1;
    }
    if (!omit_a) {
      pAtime = &atime;
    }
    rc = posix_stat_fill_filetime(&times[1], &mtime, &omit_m);
    if (rc != POSIX_STAT_SUCCESS) {
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
 * @param[out] statbuf Structure receiving file status.
 * @return 0 on success, -1 on failure with errno set.
 */
int lstat(const char *pathname, struct _stat64 *statbuf) {
  WIN32_FILE_ATTRIBUTE_DATA info;
  stat_u64 t;

  if (pathname == NULL || statbuf == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (!GetFileAttributesExA(pathname, GetFileExInfoStandard, &info)) {
    errno = ENOENT;
    return -1;
  }
  if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
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
    return 0;
  }
  return _stat64(pathname, statbuf);
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
  (void)dev;

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
  enum posix_stat_error_code rc;

  rc = posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath));
  if (rc != POSIX_STAT_SUCCESS) {
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
  enum posix_stat_error_code rc;

  attrs = FILE_FLAG_BACKUP_SEMANTICS;
  pAtime = NULL;
  pMtime = NULL;
  omit_a = 0;
  omit_m = 0;

  rc = posix_stat_resolve_at_path(dirfd, pathname, fullpath, sizeof(fullpath));
  if (rc != POSIX_STAT_SUCCESS) {
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
    if (rc != POSIX_STAT_SUCCESS) {
      CloseHandle(hFile);
      return -1;
    }
    if (!omit_a) {
      pAtime = &atime;
    }
    rc = posix_stat_fill_filetime(&times[1], &mtime, &omit_m);
    if (rc != POSIX_STAT_SUCCESS) {
      CloseHandle(hFile);
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

#endif /* _WIN32 */

/* Prevent empty translation unit on ISO C compilers */
typedef int make_iso_compilers_happy_tu_posix_stat;
