/* clang-format off */
#include "posix-sys-uio.h"

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <errno.h>
#ifndef EWOULDBLOCK
#define EWOULDBLOCK 140
#endif
#ifndef ECONNRESET
#define ECONNRESET 108
#endif
#ifndef EINPROGRESS
#define EINPROGRESS 112
#endif
#ifndef EALREADY
#define EALREADY 103
#endif
#ifndef ENOTSOCK
#define ENOTSOCK 128
#endif
#ifndef EMSGSIZE
#define EMSGSIZE 122
#endif
#ifndef EADDRINUSE
#define EADDRINUSE 100
#endif
#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL 101
#endif
#ifndef ECONNABORTED
#define ECONNABORTED 106
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED 107
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#ifndef safe_get_osfhandle
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle(fd))
#endif
#include <stdlib.h>
#include <winsock2.h>

/** \brief posix_writev function. */
long posix_writev(int fd, const struct iovec *iov, int iovcnt) {
  DWORD bytesSent = 0;
  int ret;
  WSABUF *bufs;
  int i;

  if (iovcnt < 0 || iovcnt > 1024) {
    errno = EINVAL;
    return -1;
  }

  bufs = (WSABUF *)malloc(iovcnt * sizeof(WSABUF));
  if (!bufs) {
    errno = ENOMEM;
    return -1;
  }

  for (i = 0; i < iovcnt; i++) {
    bufs[i].buf = (char *)iov[i].iov_base;
    bufs[i].len = (ULONG)iov[i].iov_len;
  }

  ret = WSASend((SOCKET)fd, bufs, (DWORD)iovcnt, &bytesSent, 0, NULL, NULL);
  if (ret == SOCKET_ERROR && WSAGetLastError() == WSAENOTSOCK) {
    ret = WSASend((SOCKET)safe_get_osfhandle(fd), bufs, (DWORD)iovcnt, &bytesSent, 0, NULL, NULL);
  }
  free(bufs);

  if (ret == SOCKET_ERROR) {
    int wsaErr = WSAGetLastError();
    if (wsaErr == WSAENOTSOCK || wsaErr == WSANOTINITIALISED) {
      long total = 0;
      for (i = 0; i < iovcnt; i++) {
        int written = _write(fd, iov[i].iov_base, (unsigned int)iov[i].iov_len);
        if (written < 0) {
          if (total > 0)
            return total;
          return -1;
        }
        total += written;
        if ((size_t)written < iov[i].iov_len) {
          break;
        }
      }
      return total;
    }

    if (wsaErr == WSAEWOULDBLOCK)
      errno = EWOULDBLOCK;
    else if (wsaErr == WSA_IO_PENDING)
      errno = EWOULDBLOCK;
    else if (wsaErr == WSAECONNRESET)
      errno = 104; /* ECONNRESET */
    else if (wsaErr == WSAEINPROGRESS)
      errno = 115; /* EINPROGRESS */
    else if (wsaErr == WSAEALREADY)
      errno = 114; /* EALREADY */
    else if (wsaErr == WSAENOTSOCK)
      errno = 108; /* ENOTSOCK */
    else if (wsaErr == WSAEMSGSIZE)
      errno = 90; /* EMSGSIZE */
    else if (wsaErr == WSAEADDRINUSE)
      errno = 98; /* EADDRINUSE */
    else if (wsaErr == WSAEADDRNOTAVAIL)
      errno = 99; /* EADDRNOTAVAIL */
    else if (wsaErr == WSAECONNABORTED)
      errno = 103; /* ECONNABORTED */
    else if (wsaErr == WSAECONNREFUSED)
      errno = 111; /* ECONNREFUSED */
    else
      errno = wsaErr;
    return -1;
  }

  return bytesSent;
}

/** \brief posix_readv function. */
long posix_readv(int fd, const struct iovec *iov, int iovcnt) {
  DWORD bytesRecv = 0;
  DWORD flags = 0;
  int ret;
  WSABUF *bufs;
  int i;

  if (iovcnt < 0 || iovcnt > 1024) {
    errno = EINVAL;
    return -1;
  }

  bufs = (WSABUF *)malloc(iovcnt * sizeof(WSABUF));
  if (!bufs) {
    errno = ENOMEM;
    return -1;
  }

  for (i = 0; i < iovcnt; i++) {
    bufs[i].buf = (char *)iov[i].iov_base;
    bufs[i].len = (ULONG)iov[i].iov_len;
  }

  ret =
      WSARecv((SOCKET)fd, bufs, (DWORD)iovcnt, &bytesRecv, &flags, NULL, NULL);
  if (ret == SOCKET_ERROR && WSAGetLastError() == WSAENOTSOCK) {
    ret = WSARecv((SOCKET)safe_get_osfhandle(fd), bufs, (DWORD)iovcnt, &bytesRecv, &flags, NULL, NULL);
  }
  free(bufs);

  if (ret == SOCKET_ERROR) {
    int wsaErr = WSAGetLastError();
    if (wsaErr == WSAENOTSOCK || wsaErr == WSANOTINITIALISED) {
      long total = 0;
      for (i = 0; i < iovcnt; i++) {
        int bytes_read =
            _read(fd, iov[i].iov_base, (unsigned int)iov[i].iov_len);
        if (bytes_read < 0) {
          if (total > 0)
            return total;
          return -1;
        }
        total += bytes_read;
        if ((size_t)bytes_read < iov[i].iov_len) {
          break;
        }
      }
      return total;
    }

    if (wsaErr == WSAEWOULDBLOCK)
      errno = EWOULDBLOCK;
    else if (wsaErr == WSA_IO_PENDING)
      errno = EWOULDBLOCK;
    else if (wsaErr == WSAECONNRESET)
      errno = 104; /* ECONNRESET */
    else if (wsaErr == WSAEINPROGRESS)
      errno = 115; /* EINPROGRESS */
    else if (wsaErr == WSAEALREADY)
      errno = 114; /* EALREADY */
    else if (wsaErr == WSAENOTSOCK)
      errno = 108; /* ENOTSOCK */
    else if (wsaErr == WSAEMSGSIZE)
      errno = 90; /* EMSGSIZE */
    else if (wsaErr == WSAEADDRINUSE)
      errno = 98; /* EADDRINUSE */
    else if (wsaErr == WSAEADDRNOTAVAIL)
      errno = 99; /* EADDRNOTAVAIL */
    else if (wsaErr == WSAECONNABORTED)
      errno = 103; /* ECONNABORTED */
    else if (wsaErr == WSAECONNREFUSED)
      errno = 111; /* ECONNREFUSED */
    else
      errno = wsaErr;
    return -1;
  }

  return bytesRecv;
}

/** \brief posix_preadv function. */
long posix_preadv(int fd, const struct iovec *iov, int iovcnt,
                  posix_uio_off_t offset) {
  __int64 old_pos = _lseeki64(fd, 0, 1 /* SEEK_CUR */);
  long res;
  if (old_pos == -1 && errno != 0) {
    return posix_readv(fd, iov, iovcnt);
  }
  if (_lseeki64(fd, offset, 0 /* SEEK_SET */) == -1) {
    return -1;
  }
  res = posix_readv(fd, iov, iovcnt);
  _lseeki64(fd, old_pos, 0 /* SEEK_SET */);
  return res;
}

/** \brief posix_pwritev function. */
long posix_pwritev(int fd, const struct iovec *iov, int iovcnt,
                   posix_uio_off_t offset) {
  __int64 old_pos = _lseeki64(fd, 0, 1 /* SEEK_CUR */);
  long res;
  if (old_pos == -1 && errno != 0) {
    return posix_writev(fd, iov, iovcnt);
  }
  if (_lseeki64(fd, offset, 0 /* SEEK_SET */) == -1) {
    return -1;
  }
  res = posix_writev(fd, iov, iovcnt);
  _lseeki64(fd, old_pos, 0 /* SEEK_SET */);
  return res;
}

#elif defined(__MSDOS__) || defined(__WATCOMC__)

#include <errno.h>
/* clang-format on */
#ifndef EWOULDBLOCK
#define EWOULDBLOCK 140
#endif
#ifndef ECONNRESET
#define ECONNRESET 108
#endif
#ifndef EINPROGRESS
#define EINPROGRESS 112
#endif
#ifndef EALREADY
#define EALREADY 103
#endif
#ifndef ENOTSOCK
#define ENOTSOCK 128
#endif
#ifndef EMSGSIZE
#define EMSGSIZE 122
#endif
#ifndef EADDRINUSE
#define EADDRINUSE 100
#endif
#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL 101
#endif
#ifndef ECONNABORTED
#define ECONNABORTED 106
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED 107
#endif

long posix_readv(int fd, const struct iovec *iov, int iovcnt) {
  if (fd || iov || iovcnt) {
  }
  errno = EINVAL;
  return -1;
}

long posix_writev(int fd, const struct iovec *iov, int iovcnt) {
  if (fd || iov || iovcnt) {
  }
  errno = EINVAL;
  return -1;
}

long posix_preadv(int fd, const struct iovec *iov, int iovcnt,
                  posix_uio_off_t offset) {
  if (fd || iov || iovcnt || offset) {
  }
  errno = EINVAL;
  return -1;
}

long posix_pwritev(int fd, const struct iovec *iov, int iovcnt,
                   posix_uio_off_t offset) {
  if (fd || iov || iovcnt || offset) {
  }
  errno = EINVAL;
  return -1;
}

#else

/* clang-format off */
#include <unistd.h>
/* clang-format on */

long posix_readv(int fd, const struct iovec *iov, int iovcnt) {
  return (long)readv(fd, iov, iovcnt);
}

long posix_writev(int fd, const struct iovec *iov, int iovcnt) {
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
 */
enum posix_sys_uio_error_code posix_sys_uio_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_SYS_UIO_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_SYS_UIO_SUCCESS;
}

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_sys_uio(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sys_uio;
