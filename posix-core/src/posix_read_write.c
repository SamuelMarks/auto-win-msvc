#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
/* clang-format off */
#if defined(_WIN32) && !defined(__CYGWIN__)
#include "posix-core.h"
#include <errno.h>

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
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
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <stdlib.h>
#include <winsock2.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400
static void my_invalid_parameter_handler(const wchar_t *expression,
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

#undef _read
#undef _write
#define _CRT_SECURE_NO_WARNINGS
#include <fcntl.h>

#include <stdio.h>
#include <string.h>

/* Removed g_cloned_cwd */

FILE *posix_fopen(const char *pathname, const char *mode) {
  int flags = 0;
  DWORD dwAccess = 0, dwCreation = OPEN_EXISTING;
  char abs_path[1024];
  HANDLE h;
  int fd;
  FILE *f;

  if (strchr(mode, 'r')) {
    flags |= _O_RDONLY;
    dwAccess |= GENERIC_READ;
  }
  if (strchr(mode, 'w')) {
    flags |= _O_WRONLY | _O_CREAT | _O_TRUNC;
    dwAccess |= GENERIC_WRITE;
    dwCreation = CREATE_ALWAYS;
  }
  if (strchr(mode, 'a')) {
    flags |= _O_WRONLY | _O_CREAT | _O_APPEND;
    dwAccess |= GENERIC_WRITE;
    dwCreation = OPEN_ALWAYS;
  }
  if (strchr(mode, '+')) {
    flags &= ~(_O_RDONLY | _O_WRONLY);
    flags |= _O_RDWR;
    dwAccess |= GENERIC_READ | GENERIC_WRITE;
  }
  flags |= _O_BINARY;

  if (pathname[0] != '/' && pathname[0] != '\\' && pathname[1] != ':') {
    char cwd[1024];
    GetCurrentDirectoryA(1024, cwd);
#if defined(_MSC_VER) && _MSC_VER < 1900
    _snprintf(abs_path, 1024, "%s\\%s", cwd, pathname);
#else
    snprintf(abs_path, 1024, "%s\\%s", cwd, pathname);
#endif
  } else {
#if defined(_MSC_VER) && _MSC_VER < 1900
    _snprintf(abs_path, 1024, "%s", pathname);
#else
    snprintf(abs_path, 1024, "%s", pathname);
#endif
  }

  h = CreateFileA(abs_path, dwAccess,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
                  dwCreation, FILE_ATTRIBUTE_NORMAL, NULL);
  if (h == INVALID_HANDLE_VALUE) {
    errno = GetLastError();
    return NULL;
  }
  fd = _open_osfhandle((intptr_t)h, flags);
  if (fd == -1) {
    CloseHandle(h);
    return NULL;
  }
  f = _fdopen(fd, mode);
  if (f == NULL) {
    _close(fd);
    return NULL;
  }
  return f;
}

#include <sys/stat.h>
/* clang-format on */
int posix_open(const char *pathname, int flags, ...) {
  int mode = 0;
  DWORD dwDesiredAccess = 0;
  DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
  DWORD dwCreationDisposition = OPEN_EXISTING;
  DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
  HANDLE hFile;
  int fd_flags;
  int fd;
  (void)mode;

  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
  }

  if ((flags & 3) == O_RDONLY)
    dwDesiredAccess = GENERIC_READ;
  else if ((flags & 3) == O_WRONLY)
    dwDesiredAccess = GENERIC_WRITE;
  else if ((flags & 3) == O_RDWR)
    dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;

  if ((flags & O_CREAT) && (flags & O_EXCL)) {
    dwCreationDisposition = CREATE_NEW;
  } else if ((flags & O_CREAT) && (flags & O_TRUNC)) {
    dwCreationDisposition = CREATE_ALWAYS;
  } else if (flags & O_TRUNC) {
    dwCreationDisposition = TRUNCATE_EXISTING;
  } else if (flags & O_CREAT) {
    dwCreationDisposition = OPEN_ALWAYS;
  }

  hFile = CreateFileA(pathname, dwDesiredAccess, dwShareMode, NULL,
                      dwCreationDisposition, dwFlagsAndAttributes, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD err = GetLastError();
    if (err == ERROR_FILE_EXISTS || err == ERROR_ALREADY_EXISTS)
      errno = EEXIST;
    else if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
      errno = ENOENT;
    else if (err == ERROR_ACCESS_DENIED)
      errno = EACCES;
    else
      errno = EINVAL;
    return -1;
  }

  fd_flags = _O_BINARY;
  if (flags & O_APPEND)
    fd_flags |= _O_APPEND;
  if ((flags & 3) == O_RDONLY)
    fd_flags |= _O_RDONLY;
  else if ((flags & 3) == O_WRONLY)
    fd_flags |= _O_WRONLY;
  else if ((flags & 3) == O_RDWR)
    fd_flags |= _O_RDWR;

  fd = _open_osfhandle((intptr_t)hFile, fd_flags);
  if (fd == -1) {
    CloseHandle(hFile);
    errno = EMFILE;
    return -1;
  }
  return fd;
}

extern int is_socket(int fd);
ssize_t posix_read(int fd, void *buf, size_t count) {
  int ret;
  if (!is_socket(fd)) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(my_invalid_parameter_handler);
#endif
    intptr_t osfh = safe_get_osfhandle(fd);
    if (osfh != -1 && osfh != -2) {
      int rret = _read(fd, buf, (unsigned int)count);
#if defined(_MSC_VER) && _MSC_VER >= 1400
      _set_invalid_parameter_handler(old);
#endif
      if (rret == -1 && errno == EINVAL)
        errno = EBADF;
      return rret;
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _set_invalid_parameter_handler(old);
#endif
  }
  ret = recv((SOCKET)safe_get_osfhandle(fd), buf, (int)count, 0);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSANOTINITIALISED) {
      WSADATA wsaData;
      WSAStartup(MAKEWORD(2, 2), &wsaData);
      ret = recv((SOCKET)safe_get_osfhandle(fd), buf, (int)count, 0);
      if (ret != SOCKET_ERROR)
        return ret;
      err = WSAGetLastError();
    }
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
      DWORD bytes_read = 0;
      if (ReadFile((HANDLE)(intptr_t)fd, buf, (DWORD)count, &bytes_read,
                   NULL)) {
        return (int)bytes_read;
      }
      errno = EBADF;
      return -1;
    }
    if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT)
      errno = EWOULDBLOCK;
    else if (err == WSAECONNRESET)
      errno = ECONNRESET;
    else if (err == WSAEINPROGRESS)
      errno = EINPROGRESS;
    else if (err == WSAEALREADY)
      errno = EALREADY;
    else if (err == WSAENOTSOCK)
      errno = ENOTSOCK;
    else if (err == WSAEMSGSIZE)
      errno = EMSGSIZE;
    else if (err == WSAEADDRINUSE)
      errno = EADDRINUSE;
    else if (err == WSAEADDRNOTAVAIL)
      errno = EADDRNOTAVAIL;
    else if (err == WSAECONNABORTED)
      errno = ECONNABORTED;
    else if (err == WSAECONNREFUSED)
      errno = ECONNREFUSED;
    else
      errno = err;
    return -1;
  }
  return ret;
}

extern int is_socket(int fd);
ssize_t posix_write(int fd, const void *buf, size_t count) {
  int ret;
  if (!is_socket(fd)) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(my_invalid_parameter_handler);
#endif
    intptr_t osfh = safe_get_osfhandle(fd);
    if (osfh != -1 && osfh != -2) {
      int wret = _write(fd, buf, (unsigned int)count);
#if defined(_MSC_VER) && _MSC_VER >= 1400
      _set_invalid_parameter_handler(old);
#endif
      if (wret == -1 && errno == EINVAL)
        errno = EBADF;
      return wret;
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _set_invalid_parameter_handler(old);
#endif
  }
  ret = send((SOCKET)safe_get_osfhandle(fd), buf, (int)count, 0);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSANOTINITIALISED) {
      WSADATA wsaData;
      WSAStartup(MAKEWORD(2, 2), &wsaData);
      ret = send((SOCKET)safe_get_osfhandle(fd), buf, (int)count, 0);
      if (ret != SOCKET_ERROR)
        return ret;
      err = WSAGetLastError();
    }
    if (err != WSAEWOULDBLOCK) {
      /* printf("DEBUG posix_write send failed: fd=%d, err=%d\n", fd, err); */
    }
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
      DWORD written = 0;
      OVERLAPPED ov = {0};
      if (WriteFile((HANDLE)(intptr_t)fd, buf, (DWORD)count, &written, &ov)) {
        /* If WriteFile completes synchronously, we get the result. */
        return (int)written;
      } else {
        DWORD werr = GetLastError();
        if (werr == ERROR_IO_PENDING) {
          if (GetOverlappedResult((HANDLE)(intptr_t)fd, &ov, &written, TRUE)) {
            return (int)written;
          }
          werr = GetLastError();
        }
        /* printf("DEBUG posix_write WriteFile failed: fd=%d, err=%lu\n", fd,
         * werr); */
      }
      errno = EBADF;
      return -1;
    }
    if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT)
      errno = EWOULDBLOCK;
    else if (err == WSAECONNRESET)
      errno = ECONNRESET;
    else if (err == WSAEINPROGRESS)
      errno = EINPROGRESS;
    else if (err == WSAEALREADY)
      errno = EALREADY;
    else if (err == WSAENOTSOCK)
      errno = ENOTSOCK;
    else if (err == WSAEMSGSIZE)
      errno = EMSGSIZE;
    else if (err == WSAEADDRINUSE)
      errno = EADDRINUSE;
    else if (err == WSAEADDRNOTAVAIL)
      errno = EADDRNOTAVAIL;
    else if (err == WSAECONNABORTED)
      errno = ECONNABORTED;
    else if (err == WSAECONNREFUSED)
      errno = ECONNREFUSED;
    else
      errno = err;
    return -1;
  }
  return ret;
}

#undef close

#if defined(_MSC_VER)
#pragma comment(linker,                                                        \
                "/alternatename:posix_epoll_close=posix_epoll_close_stub")
int posix_epoll_close_stub(int fd) {
  (void)fd;
  return -1;
}
#elif defined(__GNUC__) || defined(__clang__)
int __attribute__((weak)) posix_epoll_close(int fd) {
  (void)fd;
  return -1;
}
#endif

extern void clear_nonblock(SOCKET s);
extern void clear_as_socket(int fd);
extern int posix_epoll_close(int);
int posix_close(int fd) {
  SOCKET s;
  int ret;
  if (fd >= 100 && fd < 1124) {
    return posix_epoll_close(fd);
  }
  s = (SOCKET)safe_get_osfhandle(fd);
  clear_nonblock(s);
  clear_as_socket(fd);
  ret = closesocket(s);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
      _invalid_parameter_handler old =
          _set_invalid_parameter_handler(my_invalid_parameter_handler);
#endif
      int cret = -1;
      if (fd >= 0)
        cret = _close(fd);
#if defined(_MSC_VER) && _MSC_VER >= 1400
      _set_invalid_parameter_handler(old);
#endif
      return cret;
    }
    errno = err;
    return -1;
  }
  return ret;
}

extern int is_socket(int);
extern void mark_as_socket(int);
extern void clear_as_socket(int);

int posix_dup2(int oldfd, int newfd) {
  int ret = _dup2(oldfd, newfd);
  if (ret != -1) {
    if (is_socket(oldfd))
      mark_as_socket(newfd);
    else
      clear_as_socket(newfd);
  }
  return ret;
}

#endif

typedef int make_iso_compilers_happy_posix_read_write;
