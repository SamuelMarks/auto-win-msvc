/* clang-format off */
#include "linux-sys-statfs.h"
#include <stddef.h>

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#if defined(_WIN32)
#include <errno.h>
#include <stdlib.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
static void statfs_null_invalid_param_handler(const wchar_t *expression,
                                              const wchar_t *function,
                                              const wchar_t *file,
                                              unsigned int line,
                                              uintptr_t pReserved) {
  if (expression || function || file || line || pReserved) {
  }
}
#endif
static ptrdiff_t safe_get_osfhandle(intptr_t fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
  if (fd < 0 || fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(statfs_null_invalid_param_handler);
#if defined(_DEBUG) && !defined(_DLL)
    _CrtSetReportMode(2, 0);
    _CrtSetReportMode(1, 0);
    _CrtSetReportMode(0, 0);
#endif
    h = (ptrdiff_t)_get_osfhandle((int)fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle((int)fd);
#endif
  errno = saved_err;
  return h;
}
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <errno.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
/* clang-format on */

/** @brief statfs function. */
int statfs(const char *path, struct statfs *buf) {
  ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;

  if (!buf || !path) {
    errno = EFAULT;
    return -1;
  }

  if (GetDiskFreeSpaceExA(path, &freeBytesAvailable, &totalNumberOfBytes,
                          &totalNumberOfFreeBytes)) {
    buf->f_type = 0; /* Unknown / Windows */
    buf->f_bsize = 4096;
    buf->f_blocks = (long)(totalNumberOfBytes.QuadPart / 4096);
    buf->f_bfree = (long)(totalNumberOfFreeBytes.QuadPart / 4096);
    buf->f_bavail = (long)(freeBytesAvailable.QuadPart / 4096);
    buf->f_files = 0;
    buf->f_ffree = 0;
    buf->f_namelen = 255;
    return 0;
  }

  errno = ENOENT;
  return -1;
}

#ifndef FILE_NAME_NORMALIZED
#define FILE_NAME_NORMALIZED 0x0
#endif

typedef DWORD(WINAPI *GetFinalPathNameByHandleA_t)(HANDLE, LPSTR, DWORD, DWORD);

/** @brief fstatfs function. */
int fstatfs(int fd, struct statfs *buf) {
  HANDLE hFile;
  HMODULE hKernel32;
  GetFinalPathNameByHandleA_t pGetFinalPathNameByHandleA;
  char path[MAX_PATH];
  DWORD res;

  if (!buf) {
    errno = EFAULT;
    return -1;
  }

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

  hFile = (HANDLE)(size_t)safe_get_osfhandle(fd);
  if (hFile == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }

  hKernel32 = GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    errno = ENOSYS;
    return -1;
  }

  pGetFinalPathNameByHandleA =
      (GetFinalPathNameByHandleA_t)(size_t)GetProcAddress(
          hKernel32, "GetFinalPathNameByHandleA");

  if (!pGetFinalPathNameByHandleA) {
    errno = ENOSYS;
    return -1;
  }

  res = pGetFinalPathNameByHandleA(hFile, path, MAX_PATH, FILE_NAME_NORMALIZED);
  if (res == 0 || res >= MAX_PATH) {
    errno = ENOENT;
    return -1;
  }

  return statfs(path, buf);
}

/** @brief POSIX statvfs function. */
int statvfs(const char *path, struct statvfs *buf) {
  ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;

  if (!buf || !path) {
    errno = EFAULT;
    return -1;
  }

  if (GetDiskFreeSpaceExA(path, &freeBytesAvailable, &totalNumberOfBytes,
                          &totalNumberOfFreeBytes)) {
    buf->f_bsize = 4096;
    buf->f_frsize = 4096;
    buf->f_blocks = (linux_fsblkcnt_t)(totalNumberOfBytes.QuadPart / 4096);
    buf->f_bfree = (linux_fsblkcnt_t)(totalNumberOfFreeBytes.QuadPart / 4096);
    buf->f_bavail = (linux_fsblkcnt_t)(freeBytesAvailable.QuadPart / 4096);
    buf->f_files = 0;
    buf->f_ffree = 0;
    buf->f_favail = 0;
    buf->f_fsid = 0;
    buf->f_flag = 0;
    buf->f_namemax = 255;
    return 0;
  }

  errno = ENOENT;
  return -1;
}

/** @brief POSIX fstatvfs function. */
int fstatvfs(int fd, struct statvfs *buf) {
  HANDLE hFile;
  HMODULE hKernel32;
  GetFinalPathNameByHandleA_t pGetFinalPathNameByHandleA;
  char path[MAX_PATH];
  DWORD res;

  if (!buf) {
    errno = EFAULT;
    return -1;
  }

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

  hFile = (HANDLE)(size_t)safe_get_osfhandle(fd);
  if (hFile == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }

  hKernel32 = GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    errno = ENOSYS;
    return -1;
  }

  pGetFinalPathNameByHandleA =
      (GetFinalPathNameByHandleA_t)(size_t)GetProcAddress(
          hKernel32, "GetFinalPathNameByHandleA");

  if (!pGetFinalPathNameByHandleA) {
    errno = ENOSYS;
    return -1;
  }

  res = pGetFinalPathNameByHandleA(hFile, path, MAX_PATH, FILE_NAME_NORMALIZED);
  if (res == 0 || res >= MAX_PATH) {
    errno = ENOENT;
    return -1;
  }

  return statvfs(path, buf);
}

#endif /* _MSC_VER || _WIN32 */

/**
 * @brief Initializes and validates the linux-sys-statfs module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
auto_win_msvc_error_t linux_sys_statfs_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

#endif
typedef int make_iso_compilers_happy_tu_linux_sys_statfs;
