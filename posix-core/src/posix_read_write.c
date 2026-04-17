#if defined(_WIN32) && !defined(__CYGWIN__)
#include <errno.h>
#include <io.h>
#include <winsock2.h>

#undef _read
#undef _write

int posix_read(int fd, void *buf, unsigned int count) {
  int ret = recv((SOCKET)(unsigned int)fd, buf, count, 0);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
      return _read(fd, buf, count);
    }
    errno = err;
    return -1;
  }
  return ret;
}

int posix_write(int fd, const void *buf, unsigned int count) {
  int ret = send((SOCKET)(unsigned int)fd, buf, count, 0);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
      return _write(fd, buf, count);
    }
    errno = err;
    return -1;
  }
  return ret;
}

#undef close

int posix_close(int fd) {
  SOCKET s;
  int ret;
  if (fd >= 100 && fd < 1124) {
    /* wepoll handle leak workaround? No, this is handled separately if needed.
     */
  }
  s = (SOCKET)(unsigned int)fd;
  ret = closesocket(s);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
      return _close(fd);
    }
    errno = err;
    return -1;
  }
  return ret;
}
#endif

typedef int make_iso_compilers_happy_posix_read_write;
