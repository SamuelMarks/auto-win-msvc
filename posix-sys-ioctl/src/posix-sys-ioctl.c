/**
 * @file posix-sys-ioctl.c
 * @brief Implementation of posix-sys-ioctl compatibility functions.
 */

/* clang-format off */
#include "posix-sys-ioctl.h"
#include <errno.h>
#include <stdarg.h>

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle((int)(fd)))
#endif

#undef ioctlsocket
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/ioctl.h>
#endif
/* clang-format on */

#undef ioctl

/**
 * @brief Retrieves information on posix-sys-ioctl availability.
 */
enum posix_sys_ioctl_error_code posix_sys_ioctl_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_SYS_IOCTL_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_SYS_IOCTL_SUCCESS;
}

#if defined(_MSC_VER) || defined(_WIN32)

static int handle_console_ioctl(HANDLE h, unsigned long request, void *argp) {
  if (request == TIOCGWINSZ) {
    struct winsize *ws;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    ws = (struct winsize *)argp;
    if (ws == NULL) {
      errno = EFAULT;
      return -1;
    }
    if (GetConsoleScreenBufferInfo(h, &csbi)) {
      ws->ws_row =
          (unsigned short)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
      ws->ws_col =
          (unsigned short)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
      ws->ws_xpixel = 0;
      ws->ws_ypixel = 0;
      return 0;
    }
    errno = ENOTTY;
    return -1;
  } else if (request == TIOCSWINSZ) {
    struct winsize *ws;
    SMALL_RECT rect;
    COORD size;

    ws = (struct winsize *)argp;
    if (ws == NULL) {
      errno = EFAULT;
      return -1;
    }
    size.X = (short)ws->ws_col;
    size.Y = (short)ws->ws_row;
    rect.Top = 0;
    rect.Left = 0;
    rect.Bottom = (short)(size.Y - 1);
    rect.Right = (short)(size.X - 1);
    SetConsoleScreenBufferSize(h, size);
    SetConsoleWindowInfo(h, TRUE, &rect);
    return 0;
  }

  errno = EINVAL;
  return -1;
}

static int handle_file_ioctl(HANDLE h, unsigned long request, void *argp) {
  if (request == FIONREAD) {
    DWORD *bytes_available;
    DWORD file_type;

    bytes_available = (DWORD *)argp;
    file_type = GetFileType(h);
    if (bytes_available == NULL) {
      errno = EFAULT;
      return -1;
    }

    if (file_type == FILE_TYPE_PIPE) {
      DWORD bytes_avail;
      bytes_avail = 0;
      if (PeekNamedPipe(h, NULL, 0, NULL, &bytes_avail, NULL)) {
        *bytes_available = bytes_avail;
        return 0;
      }
    } else if (file_type == FILE_TYPE_DISK) {
      LARGE_INTEGER file_size;
      LARGE_INTEGER file_pos;
      file_pos.QuadPart = 0;
      if (GetFileSizeEx(h, &file_size) &&
          SetFilePointerEx(h, file_pos, &file_pos, FILE_CURRENT)) {
        if (file_size.QuadPart > file_pos.QuadPart) {
          *bytes_available = (DWORD)(file_size.QuadPart - file_pos.QuadPart);
        } else {
          *bytes_available = 0;
        }
        return 0;
      }
    }
    errno = ENOTTY;
    return -1;
  } else if (request == FIONBIO) {
    DWORD *non_blocking;
    DWORD file_type;

    non_blocking = (DWORD *)argp;
    file_type = GetFileType(h);
    if (non_blocking == NULL) {
      errno = EFAULT;
      return -1;
    }

    if (file_type == FILE_TYPE_PIPE) {
      DWORD mode;
      mode = PIPE_READMODE_BYTE;
      if (*non_blocking) {
        mode |= PIPE_NOWAIT;
      } else {
        mode |= PIPE_WAIT;
      }
      if (SetNamedPipeHandleState(h, &mode, NULL, NULL)) {
        return 0;
      }
      errno = EINVAL;
      return -1;
    }
    errno = ENOTTY;
    return -1;
  }

  errno = EINVAL;
  return -1;
}

int posix_ioctl(intptr_t fd, unsigned long request, ...) {
  int ret;
  va_list args;
  void *argp;
  HANDLE h;
  intptr_t handle;
  SOCKET s;

  va_start(args, request);
  argp = va_arg(args, void *);
  va_end(args);

  handle = safe_get_osfhandle(fd);
  s = (handle == -1) ? (SOCKET)fd : (SOCKET)handle;

  ret = ioctlsocket(s, (long)request, (u_long *)argp);
  if (ret != SOCKET_ERROR) {
    return 0;
  } else {
    int wsaErr;
    wsaErr = WSAGetLastError();
    if (wsaErr == WSAENOTSOCK || wsaErr == WSANOTINITIALISED) {
      h = (HANDLE)s;
      if (h == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
      }

      if (request == TIOCGWINSZ || request == TIOCSWINSZ) {
        return handle_console_ioctl(h, request, argp);
      } else if (request == FIONREAD || request == FIONBIO) {
        return handle_file_ioctl(h, request, argp);
      }

      errno = ENOTTY;
      return -1;
    } else if (wsaErr == WSAEFAULT) {
      errno = EFAULT;
      return -1;
    } else {
      errno = EINVAL;
      return -1;
    }
  }
}

#else

int posix_ioctl(intptr_t fd, unsigned long request, ...) {
  va_list args;
  void *argp;

  va_start(args, request);
  argp = va_arg(args, void *);
  va_end(args);

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) ||           \
    defined(__CYGWIN__)
  return ioctl((int)fd, request, argp);
#else
  (void)request;
  (void)argp;
  return 0;
#endif
}

#endif

typedef int make_iso_compilers_happy_tu_posix_sys_ioctl;
