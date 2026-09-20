/* clang-format off */
#include "bsd-sys-file.h"
#include <errno.h>
#include <stddef.h>
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <winsock2.h>
#else
#include <sys/file.h>
#endif
/* clang-format on */

/**
 * @brief Apply or remove an advisory lock on the open file.
 */
int posix_flock(int fd, int operation) {
  int op;
  op = operation & ~LOCK_NB;
  if (op != LOCK_SH && op != LOCK_EX && op != LOCK_UN) {
    errno = EINVAL;
    return -1;
  }
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
#if defined(_MSC_VER) || defined(_WIN32)
  {
    intptr_t osfh;
    osfh = _get_osfhandle(fd);
    if (osfh == -1) {
      errno = EBADF;
      return -1;
    }
    if (op == LOCK_UN) {
      if (!UnlockFile((HANDLE)osfh, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF)) {
        errno = EIO;
        return -1;
      }
      return 0;
    } else {
      DWORD flags;
      OVERLAPPED ov;
      flags = (op == LOCK_EX) ? LOCKFILE_EXCLUSIVE_LOCK : 0;
      if (operation & LOCK_NB) {
        flags |= LOCKFILE_FAIL_IMMEDIATELY;
      }
      memset(&ov, 0, sizeof(ov));
      if (!LockFileEx((HANDLE)osfh, flags, 0, 0xFFFFFFFF, 0xFFFFFFFF, &ov)) {
        if (GetLastError() == ERROR_LOCK_VIOLATION) {
          errno = EWOULDBLOCK;
        } else {
          errno = EIO;
        }
        return -1;
      }
      return 0;
    }
  }
#else
  return flock(fd, operation);
#endif
}

/**
 * @brief Initializes and validates the bsd-sys-file module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return BSD_SYS_FILE_SUCCESS on success, or an error code on failure.
 */
enum bsd_sys_file_error_code bsd_sys_file_init(int *out_status) {
  if (out_status == NULL) {
    return BSD_SYS_FILE_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return BSD_SYS_FILE_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_file;
