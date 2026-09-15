/* clang-format off */
#include "linux-sys-statfs.h"
#include <stddef.h>

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
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

#if defined(_MSC_VER)

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

#endif /* _MSC_VER */

/**
 * @brief Initializes and validates the linux-sys-statfs module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_SYS_STATFS_SUCCESS on success, or an error code on failure.
 */
enum linux_sys_statfs_error_code linux_sys_statfs_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_SYS_STATFS_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return LINUX_SYS_STATFS_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_sys_statfs;
