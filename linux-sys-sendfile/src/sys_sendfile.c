/* clang-format off */
#include "sys/sendfile.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#if defined(_MSC_VER) || defined(_WIN32)
#include <io.h>
#endif
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count) {
  char buf[8192];
  size_t total;
  off_t orig_pos;
  int restore_pos;

  total = 0;
  orig_pos = 0;
  restore_pos = 0;

  if (out_fd < 0 || in_fd < 0) {
    errno = EBADF;
    return -1;
  }
  if (count == 0) {
    return 0;
  }
  if (offset != NULL) {
    if (*offset < 0) {
      errno = EINVAL;
      return -1;
    }
    orig_pos = (off_t)_lseek(in_fd, 0, SEEK_CUR);
    if (_lseek(in_fd, (long)*offset, SEEK_SET) == -1) {
      return -1;
    }
    restore_pos = 1;
  }

  while (total < count) {
    size_t to_read;
    int nread;
    int nwritten;

    to_read = count - total;
    if (to_read > sizeof(buf)) {
      to_read = sizeof(buf);
    }
    nread = _read(in_fd, buf, (unsigned int)to_read);
    if (nread <= 0) {
      if (nread < 0 && total == 0) {
        if (restore_pos) {
          _lseek(in_fd, (long)orig_pos, SEEK_SET);
        }
        return -1;
      }
      break;
    }
    nwritten = _write(out_fd, buf, (unsigned int)nread);
    if (nwritten <= 0) {
      if (total == 0) {
        if (restore_pos) {
          _lseek(in_fd, (long)orig_pos, SEEK_SET);
        }
        return -1;
      }
      break;
    }
    total += (size_t)nwritten;
    if (nwritten < nread) {
      break;
    }
  }

  if (offset != NULL) {
    *offset += (off_t)total;
    _lseek(in_fd, (long)orig_pos, SEEK_SET);
  }

  return (ssize_t)total;
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_sendfile_c;
