#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file posix-sys-uio.c
 * @brief Strict C89 implementation of POSIX sys/uio.h scatter/gather I/O.
 */

/* clang-format off */
#include "posix-sys-uio.h"
#include <errno.h>

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK 140
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <stdlib.h>
#include <winsock2.h>
#elif defined(__MSDOS__) || defined(__DOS__) || defined(__WATCOMC__)
#include <io.h>
#include <stddef.h>
#else
#include <stddef.h>
#include <unistd.h>
#endif
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

typedef int(WSAAPI *uio_wsasend_fn)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD,
                                    LPWSAOVERLAPPED,
                                    LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int(WSAAPI *uio_wsarecv_fn)(SOCKET, LPWSABUF, DWORD, LPDWORD, LPDWORD,
                                    LPWSAOVERLAPPED,
                                    LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int(WSAAPI *uio_wsagetlasterror_fn)(void);
typedef int (*uio_write_fn)(int, const void *, unsigned int);
typedef int (*uio_read_fn)(int, void *, unsigned int);
typedef __int64 (*uio_lseeki64_fn)(int, __int64, int);

static void *default_uio_malloc(size_t sz) { return malloc(sz); }
static int WSAAPI default_uio_wsasend(SOCKET s, LPWSABUF bufs, DWORD cnt,
                                      LPDWORD sent, DWORD flags,
                                      LPWSAOVERLAPPED ov,
                                      LPWSAOVERLAPPED_COMPLETION_ROUTINE cr) {
  return WSASend(s, bufs, cnt, sent, flags, ov, cr);
}
static int WSAAPI default_uio_wsarecv(SOCKET s, LPWSABUF bufs, DWORD cnt,
                                      LPDWORD recvd, LPDWORD flags,
                                      LPWSAOVERLAPPED ov,
                                      LPWSAOVERLAPPED_COMPLETION_ROUTINE cr) {
  return WSARecv(s, bufs, cnt, recvd, flags, ov, cr);
}
static int WSAAPI default_uio_wsagetlasterror(void) {
  return WSAGetLastError();
}
static int default_uio_write(int fd, const void *buf, unsigned int cnt) {
  return _write(fd, buf, cnt);
}
static int default_uio_read(int fd, void *buf, unsigned int cnt) {
  return _read(fd, buf, cnt);
}
static __int64 default_uio_lseeki64(int fd, __int64 off, int whence) {
  return _lseeki64(fd, off, whence);
}

static void *(*g_uio_malloc)(size_t) = default_uio_malloc;
static uio_wsasend_fn g_uio_wsasend = default_uio_wsasend;
static uio_wsarecv_fn g_uio_wsarecv = default_uio_wsarecv;
static uio_wsagetlasterror_fn g_uio_wsagetlasterror =
    default_uio_wsagetlasterror;
static uio_write_fn g_uio_write = default_uio_write;
static uio_read_fn g_uio_read = default_uio_read;
static uio_lseeki64_fn g_uio_lseeki64 = default_uio_lseeki64;

/**
 * @brief Overrides the allocator hook used by posix-sys-uio for OOM testing.
 * @param[in] malloc_fn Replacement malloc function, or NULL to restore default.
 */
void posix_sys_uio_set_alloc_hook(void *(*malloc_fn)(size_t));
void posix_sys_uio_set_alloc_hook(void *(*malloc_fn)(size_t)) {
  g_uio_malloc = (malloc_fn != NULL) ? malloc_fn : default_uio_malloc;
}

/**
 * @brief Overrides Win32 Winsock and CRT I/O hooks for deterministic testing.
 * @param[in] send_fn Replacement WSASend hook, or NULL for default.
 * @param[in] recv_fn Replacement WSARecv hook, or NULL for default.
 * @param[in] err_fn Replacement WSAGetLastError hook, or NULL for default.
 * @param[in] write_fn Replacement _write hook, or NULL for default.
 * @param[in] read_fn Replacement _read hook, or NULL for default.
 * @param[in] seek_fn Replacement _lseeki64 hook, or NULL for default.
 */
void posix_sys_uio_set_win32_hooks(uio_wsasend_fn send_fn,
                                   uio_wsarecv_fn recv_fn,
                                   uio_wsagetlasterror_fn err_fn,
                                   uio_write_fn write_fn, uio_read_fn read_fn,
                                   uio_lseeki64_fn seek_fn);
void posix_sys_uio_set_win32_hooks(uio_wsasend_fn send_fn,
                                   uio_wsarecv_fn recv_fn,
                                   uio_wsagetlasterror_fn err_fn,
                                   uio_write_fn write_fn, uio_read_fn read_fn,
                                   uio_lseeki64_fn seek_fn) {
  g_uio_wsasend = (send_fn != NULL) ? send_fn : default_uio_wsasend;
  g_uio_wsarecv = (recv_fn != NULL) ? recv_fn : default_uio_wsarecv;
  g_uio_wsagetlasterror =
      (err_fn != NULL) ? err_fn : default_uio_wsagetlasterror;
  g_uio_write = (write_fn != NULL) ? write_fn : default_uio_write;
  g_uio_read = (read_fn != NULL) ? read_fn : default_uio_read;
  g_uio_lseeki64 = (seek_fn != NULL) ? seek_fn : default_uio_lseeki64;
}

/**
 * @brief Resets all posix-sys-uio allocator and Win32/CRT hooks to defaults.
 */
void posix_sys_uio_reset_hooks(void);
void posix_sys_uio_reset_hooks(void) {
  g_uio_malloc = default_uio_malloc;
  g_uio_wsasend = default_uio_wsasend;
  g_uio_wsarecv = default_uio_wsarecv;
  g_uio_wsagetlasterror = default_uio_wsagetlasterror;
  g_uio_write = default_uio_write;
  g_uio_read = default_uio_read;
  g_uio_lseeki64 = default_uio_lseeki64;
}

#ifndef safe_get_osfhandle
#if defined(_MSC_VER) && _MSC_VER >= 1400
static void uio_null_invalid_param_handler(const wchar_t *expression,
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
static ptrdiff_t safe_get_osfhandle(int fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
  if (fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(uio_null_invalid_param_handler);
    AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
    h = (ptrdiff_t)_get_osfhandle(fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle(fd);
#endif
  errno = saved_err;
  return h;
}
#endif

static int uio_wsa_err_to_errno(int wsaErr) {
  if (wsaErr == WSAEWOULDBLOCK || wsaErr == WSA_IO_PENDING) {
    return EWOULDBLOCK;
  }
  if (wsaErr == WSAECONNRESET) {
    return 104;
  }
  if (wsaErr == WSAEINPROGRESS) {
    return 115;
  }
  if (wsaErr == WSAEALREADY) {
    return 114;
  }
  if (wsaErr == WSAEMSGSIZE) {
    return 90;
  }
  if (wsaErr == WSAEADDRINUSE) {
    return 98;
  }
  if (wsaErr == WSAEADDRNOTAVAIL) {
    return 99;
  }
  if (wsaErr == WSAECONNABORTED) {
    return 103;
  }
  if (wsaErr == WSAECONNREFUSED) {
    return 111;
  }
  return wsaErr;
}

/**
 * @brief Writes data from multiple buffers to a socket or file descriptor.
 * @param[in] fd Socket or file descriptor.
 * @param[in] iov Array of iovec buffers.
 * @param[in] iovcnt Count of buffers in iov.
 * @return Total bytes written, or -1 on error.
 */
long posix_writev(int fd, const struct iovec *iov, int iovcnt) {
  DWORD bytesSent = 0;
  int ret;
  WSABUF *bufs;
  size_t alloc_cnt;
  int i;

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

  if (iovcnt <= 0 || iovcnt > 1024 || iov == NULL) {
    errno = EINVAL;
    return -1;
  }

  alloc_cnt = (size_t)iovcnt;
  bufs = (WSABUF *)g_uio_malloc(alloc_cnt * sizeof(WSABUF));
  if (!bufs) {
    errno = ENOMEM;
    return -1;
  }

  for (i = 0; i < iovcnt; i++) {
    bufs[i].buf = (char *)iov[i].iov_base;
    bufs[i].len = (ULONG)iov[i].iov_len;
  }

  ret = g_uio_wsasend((SOCKET)(size_t)fd, bufs, (DWORD)iovcnt, &bytesSent, 0,
                      NULL, NULL);
  if (ret == SOCKET_ERROR && g_uio_wsagetlasterror() == WSAENOTSOCK) {
    ret = g_uio_wsasend((SOCKET)safe_get_osfhandle(fd), bufs, (DWORD)iovcnt,
                        &bytesSent, 0, NULL, NULL);
  }
  free(bufs);

  if (ret == SOCKET_ERROR) {
    int wsaErr = g_uio_wsagetlasterror();
    if (wsaErr == WSAENOTSOCK || wsaErr == WSANOTINITIALISED) {
      long total = 0;
      for (i = 0; i < iovcnt; i++) {
        int written =
            g_uio_write(fd, iov[i].iov_base, (unsigned int)iov[i].iov_len);
        if (written < 0) {
          if (total > 0) {
            return total;
          }
          return -1;
        }
        total += written;
        if ((size_t)written < iov[i].iov_len) {
          break;
        }
      }
      return total;
    }

    errno = uio_wsa_err_to_errno(wsaErr);
    return -1;
  }

  return (long)bytesSent;
}

/**
 * @brief Reads data into multiple buffers from a socket or file descriptor.
 * @param[in] fd Socket or file descriptor.
 * @param[in] iov Array of iovec buffers.
 * @param[in] iovcnt Count of buffers in iov.
 * @return Total bytes read, or -1 on error.
 */
long posix_readv(int fd, const struct iovec *iov, int iovcnt) {
  DWORD bytesRecv = 0;
  DWORD flags = 0;
  int ret;
  WSABUF *bufs;
  size_t alloc_cnt;
  int i;

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

  if (iovcnt <= 0 || iovcnt > 1024 || iov == NULL) {
    errno = EINVAL;
    return -1;
  }

  alloc_cnt = (size_t)iovcnt;
  bufs = (WSABUF *)g_uio_malloc(alloc_cnt * sizeof(WSABUF));
  if (!bufs) {
    errno = ENOMEM;
    return -1;
  }

  for (i = 0; i < iovcnt; i++) {
    bufs[i].buf = (char *)iov[i].iov_base;
    bufs[i].len = (ULONG)iov[i].iov_len;
  }

  ret = g_uio_wsarecv((SOCKET)(size_t)fd, bufs, (DWORD)iovcnt, &bytesRecv,
                      &flags, NULL, NULL);
  if (ret == SOCKET_ERROR && g_uio_wsagetlasterror() == WSAENOTSOCK) {
    ret = g_uio_wsarecv((SOCKET)safe_get_osfhandle(fd), bufs, (DWORD)iovcnt,
                        &bytesRecv, &flags, NULL, NULL);
  }
  free(bufs);

  if (ret == SOCKET_ERROR) {
    int wsaErr = g_uio_wsagetlasterror();
    if (wsaErr == WSAENOTSOCK || wsaErr == WSANOTINITIALISED) {
      long total = 0;
      for (i = 0; i < iovcnt; i++) {
        int bytes_read =
            g_uio_read(fd, iov[i].iov_base, (unsigned int)iov[i].iov_len);
        if (bytes_read < 0) {
          if (total > 0) {
            return total;
          }
          return -1;
        }
        total += bytes_read;
        if ((size_t)bytes_read < iov[i].iov_len) {
          break;
        }
      }
      return total;
    }

    errno = uio_wsa_err_to_errno(wsaErr);
    return -1;
  }

  return (long)bytesRecv;
}

/**
 * @brief Reads data into multiple buffers at a given file offset.
 * @param[in] fd File descriptor.
 * @param[in] iov Array of iovec buffers.
 * @param[in] iovcnt Count of buffers in iov.
 * @param[in] offset Target byte offset.
 * @return Total bytes read, or -1 on error.
 */
long posix_preadv(int fd, const struct iovec *iov, int iovcnt,
                  posix_uio_off_t offset) {
  __int64 old_pos;
  long res;

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

  old_pos = g_uio_lseeki64(fd, 0, 1);
  if (old_pos == -1) {
    return posix_readv(fd, iov, iovcnt);
  }
  if (g_uio_lseeki64(fd, offset, 0) == -1) {
    return -1;
  }
  res = posix_readv(fd, iov, iovcnt);
  g_uio_lseeki64(fd, old_pos, 0);
  return res;
}

/**
 * @brief Writes data from multiple buffers at a given file offset.
 * @param[in] fd File descriptor.
 * @param[in] iov Array of iovec buffers.
 * @param[in] iovcnt Count of buffers in iov.
 * @param[in] offset Target byte offset.
 * @return Total bytes written, or -1 on error.
 */
long posix_pwritev(int fd, const struct iovec *iov, int iovcnt,
                   posix_uio_off_t offset) {
  __int64 old_pos;
  long res;

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

  old_pos = g_uio_lseeki64(fd, 0, 1);
  if (old_pos == -1) {
    return posix_writev(fd, iov, iovcnt);
  }
  if (g_uio_lseeki64(fd, offset, 0) == -1) {
    return -1;
  }
  res = posix_writev(fd, iov, iovcnt);
  g_uio_lseeki64(fd, old_pos, 0);
  return res;
}

#elif defined(__MSDOS__) || defined(__DOS__) || defined(__WATCOMC__)

long posix_readv(int fd, const struct iovec *iov, int iovcnt) {
  long total = 0;
  int i;
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  if (iovcnt < 0 || iovcnt > 1024 || (iovcnt > 0 && iov == NULL)) {
    errno = EINVAL;
    return -1;
  }
  for (i = 0; i < iovcnt; ++i) {
    if (iov[i].iov_len == 0) {
      continue;
    }
    if (iov[i].iov_base == NULL) {
      errno = EINVAL;
      return (total > 0) ? total : -1;
    }
    {
      int r = read(fd, iov[i].iov_base, (unsigned int)iov[i].iov_len);
      if (r < 0) {
        return (total > 0) ? total : -1;
      }
      total += (long)r;
      if ((size_t)r < iov[i].iov_len) {
        break;
      }
    }
  }
  return total;
}

long posix_writev(int fd, const struct iovec *iov, int iovcnt) {
  long total = 0;
  int i;
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  if (iovcnt < 0 || iovcnt > 1024 || (iovcnt > 0 && iov == NULL)) {
    errno = EINVAL;
    return -1;
  }
  for (i = 0; i < iovcnt; ++i) {
    if (iov[i].iov_len == 0) {
      continue;
    }
    if (iov[i].iov_base == NULL) {
      errno = EINVAL;
      return (total > 0) ? total : -1;
    }
    {
      int w = write(fd, iov[i].iov_base, (unsigned int)iov[i].iov_len);
      if (w < 0) {
        return (total > 0) ? total : -1;
      }
      total += (long)w;
      if ((size_t)w < iov[i].iov_len) {
        break;
      }
    }
  }
  return total;
}

long posix_preadv(int fd, const struct iovec *iov, int iovcnt,
                  posix_uio_off_t offset) {
  long old_pos;
  long res;
  if (offset < 0) {
    errno = EINVAL;
    return -1;
  }
  old_pos = lseek(fd, 0L, 1);
  if (old_pos < 0L) {
    return -1;
  }
  if (lseek(fd, (long)offset, 0) < 0L) {
    return -1;
  }
  res = posix_readv(fd, iov, iovcnt);
  (void)lseek(fd, old_pos, 0);
  return res;
}

long posix_pwritev(int fd, const struct iovec *iov, int iovcnt,
                   posix_uio_off_t offset) {
  long old_pos;
  long res;
  if (offset < 0) {
    errno = EINVAL;
    return -1;
  }
  old_pos = lseek(fd, 0L, 1);
  if (old_pos < 0L) {
    return -1;
  }
  if (lseek(fd, (long)offset, 0) < 0L) {
    return -1;
  }
  res = posix_writev(fd, iov, iovcnt);
  (void)lseek(fd, old_pos, 0);
  return res;
}

#else

long posix_readv(int fd, const struct iovec *iov, int iovcnt) {
  if (iovcnt < 0 || iovcnt > 1024 || iov == NULL) {
    errno = EINVAL;
    return -1;
  }
  return (long)readv(fd, iov, iovcnt);
}

long posix_writev(int fd, const struct iovec *iov, int iovcnt) {
  if (iovcnt < 0 || iovcnt > 1024 || iov == NULL) {
    errno = EINVAL;
    return -1;
  }
  return (long)writev(fd, iov, iovcnt);
}

long posix_preadv(int fd, const struct iovec *iov, int iovcnt,
                  posix_uio_off_t offset) {
  off_t old_pos = lseek(fd, 0, SEEK_CUR);
  long res;
  if (old_pos == (off_t)-1) {
    return (long)readv(fd, iov, iovcnt);
  }
  if (lseek(fd, (off_t)offset, SEEK_SET) == (off_t)-1) {
    return -1;
  }
  res = (long)readv(fd, iov, iovcnt);
  lseek(fd, old_pos, SEEK_SET);
  return res;
}

long posix_pwritev(int fd, const struct iovec *iov, int iovcnt,
                   posix_uio_off_t offset) {
  off_t old_pos = lseek(fd, 0, SEEK_CUR);
  long res;
  if (old_pos == (off_t)-1) {
    return (long)writev(fd, iov, iovcnt);
  }
  if (lseek(fd, (off_t)offset, SEEK_SET) == (off_t)-1) {
    return -1;
  }
  res = (long)writev(fd, iov, iovcnt);
  lseek(fd, old_pos, SEEK_SET);
  return res;
}

#endif

/**
 * @brief Retrieves information on posix-sys-uio module availability.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_sys_uio_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

/**
 * @brief Dummy symbol ensuring non-empty translation unit across linkers.
 * @return Always returns 0.
 */
int dummy_posix_sys_uio(void);
int dummy_posix_sys_uio(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sys_uio;

#endif
typedef int make_iso_compilers_happy_mega_batch3_21;
