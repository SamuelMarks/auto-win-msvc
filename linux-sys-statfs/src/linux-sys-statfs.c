/**
 * @file linux-sys-statfs.c
 * @brief Polyfill implementation for Linux <sys/statfs.h> and POSIX
 * <sys/statvfs.h>.
 */

/* clang-format off */
#include "linux-sys-statfs.h"
#include <stddef.h>
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
#include <errno.h>
#include <string.h>
#endif
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef STATFS_MOCK_get_osfhandle
#define STATFS_MOCK_get_osfhandle _get_osfhandle
#endif
#ifndef STATFS_MOCK_GetDiskFreeSpaceExA
#define STATFS_MOCK_GetDiskFreeSpaceExA GetDiskFreeSpaceExA
#endif
#ifndef STATFS_MOCK_GetModuleHandleA
#define STATFS_MOCK_GetModuleHandleA GetModuleHandleA
#endif
#ifndef STATFS_MOCK_GetProcAddress
#define STATFS_MOCK_GetProcAddress GetProcAddress
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400
static void statfs_null_invalid_param_handler(const wchar_t *expression,
                                              const wchar_t *function,
                                              const wchar_t *file,
                                              unsigned int line,
                                              uintptr_t pReserved) {
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
}
#endif

static ptrdiff_t statfs_safe_get_osfhandle(int fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
  if (fd < 0 || fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(statfs_null_invalid_param_handler);
    AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
    h = (ptrdiff_t)STATFS_MOCK_get_osfhandle(fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)STATFS_MOCK_get_osfhandle(fd);
#endif
  errno = saved_err;
  return h;
}

static int statfs_win_impl(const char *path, struct statfs *buf) {
  ULARGE_INTEGER freeBytesAvailable;
  ULARGE_INTEGER totalNumberOfBytes;
  ULARGE_INTEGER totalNumberOfFreeBytes;

  if (!buf || !path) {
    errno = EFAULT;
    return -1;
  }

  if (STATFS_MOCK_GetDiskFreeSpaceExA(path, &freeBytesAvailable,
                                      &totalNumberOfBytes,
                                      &totalNumberOfFreeBytes)) {
    memset(buf, 0, sizeof(*buf));
    buf->f_type = 0;
    buf->f_bsize = 4096;
    buf->f_iosize = 4096;
    buf->f_blocks = (long)(totalNumberOfBytes.QuadPart / 4096);
    buf->f_bfree = (long)(totalNumberOfFreeBytes.QuadPart / 4096);
    buf->f_bavail = (long)(freeBytesAvailable.QuadPart / 4096);
    buf->f_files = 0;
    buf->f_ffree = 0;
    buf->f_namelen = 255;
#if defined(_MSC_VER)
    strncpy_s(buf->f_fstypename, sizeof(buf->f_fstypename), "ntfs", _TRUNCATE);
    strncpy_s(buf->f_mntfromname, sizeof(buf->f_mntfromname), "C:", _TRUNCATE);
    strncpy_s(buf->f_mntonname, sizeof(buf->f_mntonname), "C:\\", _TRUNCATE);
#else
    strncpy(buf->f_fstypename, "ntfs", sizeof(buf->f_fstypename) - 1);
    strncpy(buf->f_mntfromname, "C:", sizeof(buf->f_mntfromname) - 1);
    strncpy(buf->f_mntonname, "C:\\", sizeof(buf->f_mntonname) - 1);
#endif
    return 0;
  }

  errno = ENOENT;
  return -1;
}

#ifndef FILE_NAME_NORMALIZED
#define FILE_NAME_NORMALIZED 0x0
#endif

typedef DWORD(WINAPI *GetFinalPathNameByHandleA_t)(HANDLE, LPSTR, DWORD, DWORD);

static int fstatfs_win_impl(int fd, struct statfs *buf) {
  HANDLE hFile;
  HMODULE hKernel32;
  GetFinalPathNameByHandleA_t pGetFinalPathNameByHandleA;
  char path[MAX_PATH];
  DWORD res;

  if (!buf) {
    errno = EFAULT;
    return -1;
  }

  hFile = (HANDLE)(size_t)statfs_safe_get_osfhandle(fd);
  if (hFile == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }

  hKernel32 = STATFS_MOCK_GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    errno = ENOSYS;
    return -1;
  }

  pGetFinalPathNameByHandleA =
      (GetFinalPathNameByHandleA_t)(size_t)STATFS_MOCK_GetProcAddress(
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

  return statfs_win_impl(path, buf);
}

static int statvfs_win_impl(const char *path, struct statvfs *buf) {
  ULARGE_INTEGER freeBytesAvailable;
  ULARGE_INTEGER totalNumberOfBytes;
  ULARGE_INTEGER totalNumberOfFreeBytes;

  if (!buf || !path) {
    errno = EFAULT;
    return -1;
  }

  if (STATFS_MOCK_GetDiskFreeSpaceExA(path, &freeBytesAvailable,
                                      &totalNumberOfBytes,
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

static int fstatvfs_win_impl(int fd, struct statvfs *buf) {
  HANDLE hFile;
  HMODULE hKernel32;
  GetFinalPathNameByHandleA_t pGetFinalPathNameByHandleA;
  char path[MAX_PATH];
  DWORD res;

  if (!buf) {
    errno = EFAULT;
    return -1;
  }

  hFile = (HANDLE)(size_t)statfs_safe_get_osfhandle(fd);
  if (hFile == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return -1;
  }

  hKernel32 = STATFS_MOCK_GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    errno = ENOSYS;
    return -1;
  }

  pGetFinalPathNameByHandleA =
      (GetFinalPathNameByHandleA_t)(size_t)STATFS_MOCK_GetProcAddress(
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

  return statvfs_win_impl(path, buf);
}

/**
 * @brief Gets file system statistics for a path.
 * @param[in] path File system path.
 * @param[out] buf Output buffer for statfs.
 * @return 0 on success, or -1 on failure with errno set.
 */
int statfs(const char *path, struct statfs *buf) {
  return statfs_win_impl(path, buf);
}

/**
 * @brief Gets file system statistics for an open file descriptor.
 * @param[in] fd Open file descriptor.
 * @param[out] buf Output buffer for statfs.
 * @return 0 on success, or -1 on failure with errno set.
 */
int fstatfs(int fd, struct statfs *buf) { return fstatfs_win_impl(fd, buf); }

/**
 * @brief Gets virtual file system statistics for a path (POSIX statvfs).
 * @param[in] path File system path.
 * @param[out] buf Output buffer for statvfs.
 * @return 0 on success, or -1 on failure with errno set.
 */
int statvfs(const char *path, struct statvfs *buf) {
  return statvfs_win_impl(path, buf);
}

/**
 * @brief Gets virtual file system statistics for an open file descriptor (POSIX
 * fstatvfs).
 * @param[in] fd Open file descriptor.
 * @param[out] buf Output buffer for statvfs.
 * @return 0 on success, or -1 on failure with errno set.
 */
int fstatvfs(int fd, struct statvfs *buf) { return fstatvfs_win_impl(fd, buf); }

#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)

/** @brief statfs function for DOS. */
int statfs(const char *path, struct statfs *buf) {
  if (!buf || !path) {
    errno = EFAULT;
    return -1;
  }
  memset(buf, 0, sizeof(*buf));
  buf->f_bsize = 512;
  buf->f_frsize = 512;
  buf->f_namelen = 12;
  return 0;
}

/** @brief fstatfs function for DOS. */
int fstatfs(int fd, struct statfs *buf) {
  if (!buf) {
    errno = EFAULT;
    return -1;
  }
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return statfs(".", buf);
}

/** @brief POSIX statvfs function for DOS. */
int statvfs(const char *path, struct statvfs *buf) {
  if (!buf || !path) {
    errno = EFAULT;
    return -1;
  }
  memset(buf, 0, sizeof(*buf));
  buf->f_bsize = 512;
  buf->f_frsize = 512;
  buf->f_namemax = 12;
  return 0;
}

/** @brief POSIX fstatvfs function for DOS. */
int fstatvfs(int fd, struct statvfs *buf) {
  if (!buf) {
    errno = EFAULT;
    return -1;
  }
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return statvfs(".", buf);
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
