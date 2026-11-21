/* posix-termios.c - Strict C89 Implementation */
/**
 * @file posix-termios.c
 * @brief Implementation of POSIX termios API for native MSVC.
 */
/* clang-format off */
#include "posix-termios.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <stdlib.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#endif
#endif
/* clang-format on */

#ifdef _WIN32

#if defined(_MSC_VER) && _MSC_VER >= 1400
static void termios_null_invalid_param_handler(const wchar_t *expression,
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
  if (fd < 0 || fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(termios_null_invalid_param_handler);
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

typedef void *HANDLE;
typedef unsigned long DWORD;
typedef int BOOL;

/** @brief INVALID_HANDLE_VALUE macro. */
#define INVALID_HANDLE_VALUE ((HANDLE)(size_t)-1)
/** @brief STD_INPUT_HANDLE macro. */
#define STD_INPUT_HANDLE ((DWORD) - 10)
/** @brief STD_OUTPUT_HANDLE macro. */
#define STD_OUTPUT_HANDLE ((DWORD) - 11)
/** @brief STD_ERROR_HANDLE macro. */
#define STD_ERROR_HANDLE ((DWORD) - 12)

#define ENABLE_PROCESSED_INPUT 0x0001
#define ENABLE_LINE_INPUT 0x0002
#define ENABLE_ECHO_INPUT 0x0004
#define ENABLE_PROCESSED_OUTPUT 0x0001

#define FILE_TYPE_CHAR 0x0002
#define SETXOFF 1
#define SETXON 2

__declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
__declspec(dllimport) BOOL __stdcall FlushFileBuffers(HANDLE hFile);
__declspec(dllimport) BOOL __stdcall
FlushConsoleInputBuffer(HANDLE hConsoleInput);
__declspec(dllimport) BOOL __stdcall GetConsoleMode(HANDLE hConsoleHandle,
                                                    DWORD *lpMode);
__declspec(dllimport) BOOL __stdcall SetConsoleMode(HANDLE hConsoleHandle,
                                                    DWORD dwMode);
__declspec(dllimport) DWORD __stdcall GetFileType(HANDLE hFile);
__declspec(dllimport) BOOL __stdcall EscapeCommFunction(HANDLE hFile,
                                                        DWORD dwFunc);
__declspec(dllimport) BOOL __stdcall SetCommBreak(HANDLE hFile);
__declspec(dllimport) BOOL __stdcall ClearCommBreak(HANDLE hFile);
__declspec(dllimport) void __stdcall Sleep(DWORD dwMilliseconds);

static ptrdiff_t default_termios_osfhandle(intptr_t fd) {
  return safe_get_osfhandle(fd);
}
static HANDLE default_termios_std_handle(DWORD nStdHandle) {
  return GetStdHandle(nStdHandle);
}
static BOOL default_termios_flush_file(HANDLE hFile) {
  return FlushFileBuffers(hFile);
}
static BOOL default_termios_get_mode(HANDLE hConsoleHandle, DWORD *lpMode) {
  return GetConsoleMode(hConsoleHandle, lpMode);
}
static BOOL default_termios_set_mode(HANDLE hConsoleHandle, DWORD dwMode) {
  return SetConsoleMode(hConsoleHandle, dwMode);
}
static DWORD default_termios_file_type(HANDLE hFile) {
  return GetFileType(hFile);
}
static BOOL default_termios_set_break(HANDLE hFile) {
  return SetCommBreak(hFile);
}

static ptrdiff_t (*g_termios_osfhandle)(intptr_t) = default_termios_osfhandle;
static HANDLE (*g_termios_std_handle)(DWORD) = default_termios_std_handle;
static BOOL (*g_termios_flush_file)(HANDLE) = default_termios_flush_file;
static BOOL (*g_termios_get_mode)(HANDLE, DWORD *) = default_termios_get_mode;
static BOOL (*g_termios_set_mode)(HANDLE, DWORD) = default_termios_set_mode;
static DWORD (*g_termios_file_type)(HANDLE) = default_termios_file_type;
static BOOL (*g_termios_set_break)(HANDLE) = default_termios_set_break;

/**
 * @brief Overrides Win32 hooks used by posix-termios for testing.
 */
void posix_termios_set_win32_hooks(ptrdiff_t (*osfhandle_fn)(intptr_t),
                                   HANDLE (*std_handle_fn)(DWORD),
                                   BOOL (*flush_file_fn)(HANDLE),
                                   BOOL (*get_mode_fn)(HANDLE, DWORD *),
                                   BOOL (*set_mode_fn)(HANDLE, DWORD),
                                   DWORD (*file_type_fn)(HANDLE),
                                   BOOL (*set_break_fn)(HANDLE));
void posix_termios_set_win32_hooks(ptrdiff_t (*osfhandle_fn)(intptr_t),
                                   HANDLE (*std_handle_fn)(DWORD),
                                   BOOL (*flush_file_fn)(HANDLE),
                                   BOOL (*get_mode_fn)(HANDLE, DWORD *),
                                   BOOL (*set_mode_fn)(HANDLE, DWORD),
                                   DWORD (*file_type_fn)(HANDLE),
                                   BOOL (*set_break_fn)(HANDLE)) {
  g_termios_osfhandle = osfhandle_fn;
  g_termios_std_handle = std_handle_fn;
  g_termios_flush_file = flush_file_fn;
  g_termios_get_mode = get_mode_fn;
  g_termios_set_mode = set_mode_fn;
  g_termios_file_type = file_type_fn;
  g_termios_set_break = set_break_fn;
}

/**
 * @brief Resets Win32 hooks used by posix-termios to defaults.
 */
void posix_termios_reset_win32_hooks(void);
void posix_termios_reset_win32_hooks(void) {
  g_termios_osfhandle = default_termios_osfhandle;
  g_termios_std_handle = default_termios_std_handle;
  g_termios_flush_file = default_termios_flush_file;
  g_termios_get_mode = default_termios_get_mode;
  g_termios_set_mode = default_termios_set_mode;
  g_termios_file_type = default_termios_file_type;
  g_termios_set_break = default_termios_set_break;
}

/**
 * @brief Internal helper to get HANDLE from file descriptor.
 */
static int get_handle_from_fd_helper(int fd, HANDLE *out_handle) {
  HANDLE h = (HANDLE)(size_t)g_termios_osfhandle((intptr_t)fd);

  if (h == INVALID_HANDLE_VALUE) {
    if (fd == 0) {
      h = g_termios_std_handle(STD_INPUT_HANDLE);
    } else if (fd == 1) {
      h = g_termios_std_handle(STD_OUTPUT_HANDLE);
    } else if (fd == 2) {
      h = g_termios_std_handle(STD_ERROR_HANDLE);
    }
  }

  *out_handle = h;
  return (h != NULL && h != INVALID_HANDLE_VALUE) ? 0 : 1;
}
#endif

/** @brief cfgetispeed function. */
#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
speed_t cfgetispeed(const struct termios *termios_p) {
  if (!termios_p)
    return 0;
  return termios_p->c_ispeed;
}

/** @brief cfgetospeed function. */
speed_t cfgetospeed(const struct termios *termios_p) {
  if (!termios_p)
    return 0;
  return termios_p->c_ospeed;
}

/** @brief cfsetispeed function. */
int cfsetispeed(struct termios *termios_p, speed_t speed) {
  if (!termios_p)
    return -1;
  termios_p->c_ispeed = speed;
  return 0;
}

/** @brief cfsetospeed function. */
int cfsetospeed(struct termios *termios_p, speed_t speed) {
  if (!termios_p)
    return -1;
  termios_p->c_ospeed = speed;
  return 0;
}

/** @brief tcdrain function. */
int tcdrain(int fd) {
#ifdef _WIN32
  HANDLE h;
  if (get_handle_from_fd_helper(fd, &h) == 0) {
    if (g_termios_flush_file(h)) {
      return 0;
    }
  }
  return -1;
#else
  if (!fd) {
    errno = EINVAL;
    return -1;
  }
  return -1;
#endif
}

/** @brief tcflow function. */
int tcflow(int fd, int action) {
#ifdef _WIN32
  HANDLE h;
  if (get_handle_from_fd_helper(fd, &h) == 0) {
    if (g_termios_file_type(h) != FILE_TYPE_CHAR) {
      errno = ENOTTY;
      return -1;
    }
    switch (action) {
    case TCOOFF:
      EscapeCommFunction(h, SETXOFF);
      return 0;
    case TCOON:
      EscapeCommFunction(h, SETXON);
      return 0;
    case TCIOFF:
    case TCION:
      return 0;
    default:
      errno = EINVAL;
      return -1;
    }
  }
  errno = EBADF;
  return -1;
#else
  (void)fd;
  (void)action;
  errno = ENOSYS;
  return -1;
#endif
}

/** @brief tcflush function. */
int tcflush(int fd, int queue_selector) {
#ifdef _WIN32
  HANDLE h;
  if (get_handle_from_fd_helper(fd, &h) == 0) {
    if (queue_selector == TCIFLUSH || queue_selector == TCIOFLUSH) {
      FlushConsoleInputBuffer(h);
    }
    return 0;
  }
  return -1;
#else
  (void)fd;
  (void)queue_selector;
  return -1;
#endif
}

/** @brief tcgetattr function. */
#endif
int tcgetattr(int fd, struct termios *termios_p) {
#ifdef _WIN32
  HANDLE h;
  DWORD mode;

  if (!termios_p) {
    return -1;
  }

  termios_p->c_iflag = 0;
  termios_p->c_oflag = 0;
  termios_p->c_cflag = 0;
  termios_p->c_lflag = 0;
  termios_p->c_ispeed = B9600;
  termios_p->c_ospeed = B9600;

  if (get_handle_from_fd_helper(fd, &h) == 0 && g_termios_get_mode(h, &mode)) {
    if (fd == 0) {
      if (mode & ENABLE_ECHO_INPUT)
        termios_p->c_lflag |= ECHO;
      if (mode & ENABLE_LINE_INPUT)
        termios_p->c_lflag |= ICANON;
      if (mode & ENABLE_PROCESSED_INPUT) {
        termios_p->c_lflag |= ISIG;
        termios_p->c_iflag |= IGNBRK;
      }
    } else {
      if (mode & ENABLE_PROCESSED_OUTPUT)
        termios_p->c_oflag |= OPOST;
    }
    return 0;
  }
  return -1;
#else
  (void)fd;
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (termios_p) {
#endif
    termios_p->c_iflag = 0;
    termios_p->c_oflag = 0;
    termios_p->c_cflag = 0;
    termios_p->c_lflag = 0;
    termios_p->c_ispeed = B0;
    termios_p->c_ospeed = B0;
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  }
#endif
  return -1;
#endif
}

#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/** @brief tcgetsid function. */
pid_t tcgetsid(int fd) {
  (void)fd;
  return -1;
}

/** @brief tcsendbreak function. */
int tcsendbreak(int fd, int duration) {
#ifdef _WIN32
  HANDLE h;
  if (get_handle_from_fd_helper(fd, &h) == 0) {
    if (g_termios_file_type(h) != FILE_TYPE_CHAR) {
      errno = ENOTTY;
      return -1;
    }
    if (g_termios_set_break(h)) {
      Sleep(duration > 0 ? (unsigned long)duration * 250 : 250);
      ClearCommBreak(h);
    }
    return 0;
  }
  errno = EBADF;
  return -1;
#else
  (void)fd;
  (void)duration;
  errno = ENOSYS;
  return -1;
#endif
}

/** @brief tcsetattr function. */
#endif
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p) {
#ifdef _WIN32
  HANDLE h;
  DWORD mode;

  (void)optional_actions;

  if (!termios_p) {
    return -1;
  }

  if (get_handle_from_fd_helper(fd, &h) == 0 && g_termios_get_mode(h, &mode)) {
    if (fd == 0) {
      if (termios_p->c_lflag & ECHO)
        mode |= (DWORD)ENABLE_ECHO_INPUT;
      else
        mode &= ~(DWORD)ENABLE_ECHO_INPUT;

      if (termios_p->c_lflag & ICANON)
        mode |= (DWORD)ENABLE_LINE_INPUT;
      else
        mode &= ~(DWORD)ENABLE_LINE_INPUT;

      if (termios_p->c_lflag & ISIG)
        mode |= (DWORD)ENABLE_PROCESSED_INPUT;
      else
        mode &= ~(DWORD)ENABLE_PROCESSED_INPUT;
    } else {
      if (termios_p->c_oflag & OPOST)
        mode |= (DWORD)ENABLE_PROCESSED_OUTPUT;
      else
        mode &= ~(DWORD)ENABLE_PROCESSED_OUTPUT;
    }
    if (g_termios_set_mode(h, mode)) {
      return 0;
    }
  }
  return -1;
#else
  (void)fd;
  (void)optional_actions;
  (void)termios_p;
  return -1;
#endif
}

/**
 * @brief Set terminal attributes to raw mode.
 */
#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
void cfmakeraw(struct termios *termios_p) {
  if (!termios_p) {
    return;
  }
  termios_p->c_iflag &= ~(tcflag_t)(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR |
                                    IGNCR | ICRNL | IXON);
  termios_p->c_oflag &= ~(tcflag_t)OPOST;
  termios_p->c_lflag &= ~(tcflag_t)(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  termios_p->c_cflag &= ~(tcflag_t)(CSIZE | PARENB);
  termios_p->c_cflag |= CS8;
  termios_p->c_cc[VMIN] = 1;
  termios_p->c_cc[VTIME] = 0;
}

/**
 * @brief Retrieves information on posix-termios module availability.
 */
auto_win_msvc_error_t posix_termios_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_termios(void);
int dummy_posix_termios(void) { return 0; }

#endif
typedef int make_iso_compilers_happy_tu_posix_termios;
