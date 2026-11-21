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
#include <stdlib.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
static void ioctl_null_invalid_param_handler(const wchar_t *expression,
                                             const wchar_t *function,
                                             const wchar_t *file,
                                             unsigned int line,
                                             uintptr_t pReserved) {
  if (expression || function || file || line || pReserved) {
  }
}
#endif
static ptrdiff_t safe_get_osfhandle(intptr_t fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
  if (fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(ioctl_null_invalid_param_handler);
    AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
    h = (ptrdiff_t)_get_osfhandle((int)fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle((int)fd);
#endif
  errno = saved_err;
  return h;
}
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
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
auto_win_msvc_error_t posix_sys_ioctl_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}
#endif

#if defined(_MSC_VER) || defined(_WIN32)

static BOOL default_ioctl_csbi(HANDLE h, CONSOLE_SCREEN_BUFFER_INFO *csbi) {
  return GetConsoleScreenBufferInfo(h, csbi);
}
static BOOL default_ioctl_peek_pipe(HANDLE h, DWORD *avail) {
  return PeekNamedPipe(h, NULL, 0, NULL, avail, NULL);
}
static BOOL default_ioctl_fsize(HANDLE h, LARGE_INTEGER *sz) {
  return GetFileSizeEx(h, sz);
}
static BOOL default_ioctl_fseek(HANDLE h, LARGE_INTEGER *pos) {
  return SetFilePointerEx(h, *pos, pos, FILE_CURRENT);
}
static BOOL default_ioctl_pipe_state(HANDLE h, DWORD *mode) {
  return SetNamedPipeHandleState(h, mode, NULL, NULL);
}

static BOOL (*g_ioctl_csbi)(HANDLE,
                            CONSOLE_SCREEN_BUFFER_INFO *) = default_ioctl_csbi;
static BOOL (*g_ioctl_peek_pipe)(HANDLE, DWORD *) = default_ioctl_peek_pipe;
static BOOL (*g_ioctl_fsize)(HANDLE, LARGE_INTEGER *) = default_ioctl_fsize;
static BOOL (*g_ioctl_fseek)(HANDLE, LARGE_INTEGER *) = default_ioctl_fseek;
static BOOL (*g_ioctl_pipe_state)(HANDLE, DWORD *) = default_ioctl_pipe_state;

/**
 * @brief Overrides Win32 handle hooks for posix-sys-ioctl unit testing.
 */
void posix_sys_ioctl_set_hooks(int (*csbi_fn)(void *, void *),
                               int (*peek_fn)(void *, unsigned long *),
                               int (*fsize_fn)(void *, void *),
                               int (*fseek_fn)(void *, void *),
                               int (*pipe_state_fn)(void *, unsigned long *));
void posix_sys_ioctl_set_hooks(int (*csbi_fn)(void *, void *),
                               int (*peek_fn)(void *, unsigned long *),
                               int (*fsize_fn)(void *, void *),
                               int (*fseek_fn)(void *, void *),
                               int (*pipe_state_fn)(void *, unsigned long *)) {
  g_ioctl_csbi = (BOOL (*)(HANDLE, CONSOLE_SCREEN_BUFFER_INFO *))csbi_fn;
  g_ioctl_peek_pipe = (BOOL (*)(HANDLE, DWORD *))peek_fn;
  g_ioctl_fsize = (BOOL (*)(HANDLE, LARGE_INTEGER *))fsize_fn;
  g_ioctl_fseek = (BOOL (*)(HANDLE, LARGE_INTEGER *))fseek_fn;
  g_ioctl_pipe_state = (BOOL (*)(HANDLE, DWORD *))pipe_state_fn;
}

/**
 * @brief Resets Win32 handle hooks to defaults.
 */
void posix_sys_ioctl_reset_hooks(void);
void posix_sys_ioctl_reset_hooks(void) {
  g_ioctl_csbi = default_ioctl_csbi;
  g_ioctl_peek_pipe = default_ioctl_peek_pipe;
  g_ioctl_fsize = default_ioctl_fsize;
  g_ioctl_fseek = default_ioctl_fseek;
  g_ioctl_pipe_state = default_ioctl_pipe_state;
}

static int handle_console_ioctl(HANDLE h, unsigned long request, void *argp) {
  if (request == TIOCGWINSZ) {
    struct winsize *ws;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    ws = (struct winsize *)argp;
    if (ws == NULL) {
      errno = EFAULT;
      return -1;
    }
    if (g_ioctl_csbi(h, &csbi)) {
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
  } else {
    /* TIOCSWINSZ */
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
      DWORD bytes_avail = 0;
      if (g_ioctl_peek_pipe(h, &bytes_avail)) {
        *bytes_available = bytes_avail;
        return 0;
      }
    } else if (file_type == FILE_TYPE_DISK) {
      LARGE_INTEGER file_size;
      LARGE_INTEGER file_pos;
      file_pos.QuadPart = 0;
      if (g_ioctl_fsize(h, &file_size) && g_ioctl_fseek(h, &file_pos)) {
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
  } else {
    /* FIONBIO */
    DWORD *non_blocking;
    DWORD file_type;

    non_blocking = (DWORD *)argp;
    file_type = GetFileType(h);
    if (non_blocking == NULL) {
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
      if (g_ioctl_pipe_state(h, &mode)) {
        return 0;
      }
      errno = EINVAL;
      return -1;
    }
    errno = ENOTTY;
    return -1;
  }
}

int posix_ioctl(intptr_t fd, unsigned long request, ...) {
  va_list args;
  void *argp;
  intptr_t handle;
  SOCKET s;
  int optval = 0;
  int optlen = sizeof(optval);

  va_start(args, request);
  argp = va_arg(args, void *);
  va_end(args);

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

  handle = safe_get_osfhandle(fd);
  s = (handle == -1) ? (SOCKET)fd : (SOCKET)handle;

  if (getsockopt(s, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen) == 0) {
    if (ioctlsocket(s, (long)request, (u_long *)argp) != SOCKET_ERROR) {
      return 0;
    }
    if (WSAGetLastError() == WSAEFAULT) {
      errno = EFAULT;
      return -1;
    }
    errno = EINVAL;
    return -1;
  }

  if (request == TIOCGWINSZ || request == TIOCSWINSZ) {
    return handle_console_ioctl((HANDLE)s, request, argp);
  }
  if (request == FIONREAD || request == FIONBIO) {
    return handle_file_ioctl((HANDLE)s, request, argp);
  }

  errno = ENOTTY;
  return -1;
}

#else

int posix_ioctl(intptr_t fd, unsigned long request, ...) {
  va_list args;
  void *argp;

  va_start(args, request);
  argp = va_arg(args, void *);
  va_end(args);

#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }

  if (argp == NULL && (request == (unsigned long)TIOCGWINSZ ||
                       request == (unsigned long)TIOCSWINSZ ||
                       request == (unsigned long)FIONREAD ||
                       request == (unsigned long)FIONBIO)) {
    errno = EFAULT;
    return -1;
  }
#endif

#if defined(__CYGWIN__) || (defined(__linux__) && !defined(__GLIBC__)) ||      \
    defined(__EMSCRIPTEN__)
  return ioctl((int)fd, (int)request, argp);
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
  return ioctl((int)fd, request, argp);
#else
  (void)fd;
  if (request == (unsigned long)TIOCGWINSZ ||
      request == (unsigned long)TIOCSWINSZ ||
      request == (unsigned long)TIOCNOTTY ||
      request == (unsigned long)FIONREAD || request == (unsigned long)FIONBIO) {
    return 0;
  }
  (void)argp;
  errno = EINVAL;
  return -1;
#endif
}

#endif

typedef int make_iso_compilers_happy_tu_posix_sys_ioctl;
