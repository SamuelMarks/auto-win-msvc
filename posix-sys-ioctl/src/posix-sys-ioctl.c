/* clang-format off */
#include "posix-sys-ioctl.h"
#include <errno.h>
#include <stdarg.h>
#if defined(_MSC_VER) || defined(_WIN32)
#include <io.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

static int handle_console_ioctl(HANDLE h, unsigned long request, void *argp) {
  if (request == TIOCGWINSZ) {
    struct winsize *ws = (struct winsize *)argp;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!ws) {
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
    /* Not practically useful to set console size from arbitrary apps easily
       without full context, but we can stub it out so it doesn't fail hard, or
       we can try to set it. For now, setting it returns success to appease apps
       that expect it. */
    struct winsize *ws = (struct winsize *)argp;
    SMALL_RECT rect;
    COORD size;
    if (!ws) {
      errno = EFAULT;
      return -1;
    }
    size.X = (short)ws->ws_col;
    size.Y = (short)ws->ws_row;
    rect.Top = 0;
    rect.Left = 0;
    rect.Bottom = size.Y - 1;
    rect.Right = size.X - 1;
    SetConsoleScreenBufferSize(h, size);
    SetConsoleWindowInfo(h, TRUE, &rect);
    return 0;
  }

  errno = EINVAL;
  return -1;
}

static int handle_file_ioctl(HANDLE h, unsigned long request, void *argp) {
  if (request == FIONREAD) {
    DWORD *bytes_available = (DWORD *)argp;
    DWORD file_type = GetFileType(h);
    if (!bytes_available) {
      errno = EFAULT;
      return -1;
    }

    if (file_type == FILE_TYPE_PIPE) {
      DWORD bytes_avail = 0;
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
    DWORD *non_blocking = (DWORD *)argp;
    DWORD file_type = GetFileType(h);
    if (!non_blocking) {
      errno = EFAULT;
      return -1;
    }

    if (file_type == FILE_TYPE_PIPE) {
      DWORD mode = PIPE_READMODE_BYTE;
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
    /* Windows doesn't really have O_NONBLOCK for disk files. */
    errno = ENOTTY;
    return -1;
  }

  errno = EINVAL;
  return -1;
}

/** \brief posix_ioctl function. */
int posix_ioctl(int fd, unsigned long request, ...) {
  int ret;
  va_list args;
  void *argp;
  HANDLE h;

  va_start(args, request);
  argp = va_arg(args, void *);
  va_end(args);

  /* Try as a socket first */
  ret = ioctlsocket((SOCKET)_get_osfhandle(fd), (long)request, (u_long *)argp);
  if (ret != SOCKET_ERROR) {
    return 0;
  } else {
    int wsaErr = WSAGetLastError();
    if (wsaErr == WSAENOTSOCK || wsaErr == WSANOTINITIALISED) {
      /* Not a socket. Try as a file/console handle. */
      h = (HANDLE)_get_osfhandle(fd);
      if (h == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
      }

      if (request == TIOCGWINSZ || request == TIOCSWINSZ) {
        return handle_console_ioctl(h, request, argp);
      } else if (request == FIONREAD || request == FIONBIO) {
        return handle_file_ioctl(h, request, argp);
      }

      /* Unrecognized or unsupported ioctl */
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

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_sys_ioctl(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sys_ioctl;
