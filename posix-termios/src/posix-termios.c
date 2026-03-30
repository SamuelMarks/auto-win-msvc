/* posix-termios.c - Strict C89 Implementation */
/**
 * @file posix-termios.c
 * @brief Implementation of POSIX termios API for native MSVC.
 */
#include "posix-termios.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#include <stdlib.h>

/* Helper definitions to avoid <windows.h> */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

typedef void *HANDLE;
typedef unsigned long DWORD;
typedef int BOOL;

/** \brief INVALID_HANDLE_VALUE macro. */
#define INVALID_HANDLE_VALUE ((HANDLE)(long)-1)
/** \brief STD_INPUT_HANDLE macro. */
#define STD_INPUT_HANDLE ((DWORD) - 10)
/** \brief STD_OUTPUT_HANDLE macro. */
#define STD_OUTPUT_HANDLE ((DWORD) - 11)
/** \brief STD_ERROR_HANDLE macro. */
#define STD_ERROR_HANDLE ((DWORD) - 12)

#define ENABLE_PROCESSED_INPUT 0x0001
#define ENABLE_LINE_INPUT 0x0002
#define ENABLE_ECHO_INPUT 0x0004
#define ENABLE_PROCESSED_OUTPUT 0x0001

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
__declspec(dllimport) BOOL __stdcall FlushFileBuffers(HANDLE hFile);
__declspec(dllimport)
BOOL __stdcall FlushConsoleInputBuffer(HANDLE hConsoleInput);
__declspec(dllimport) BOOL __stdcall GetConsoleMode(HANDLE hConsoleHandle,
                                                    DWORD *lpMode);
__declspec(dllimport) BOOL __stdcall SetConsoleMode(HANDLE hConsoleHandle,
                                                    DWORD dwMode);

#ifdef __cplusplus
}
#endif

#endif

#ifdef _MSC_VER
#if _MSC_VER >= 1400
#define USE_SAFE_CRT 1
#endif
#endif

#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#define NUM_FORMAT "%d"
#else
#define NUM_FORMAT "%d"
#endif

#ifdef _WIN32
/**
 * @brief Helper to format error. Returns int (exit code) as required.
 */
static int format_error_msg(char *buffer, size_t size, int errcode) {
  if (!buffer || size == 0)
    return 1;
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
  sprintf_s(buffer, size, "Error code: " NUM_FORMAT, errcode);
#else
  sprintf(buffer, "Error code: " NUM_FORMAT, errcode);
#endif
  return 0;
}
#endif

#ifdef _WIN32

#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
/** \brief null_invalid_parameter_handler function. */
static void __cdecl null_invalid_parameter_handler(
    const wchar_t *expression, const wchar_t *function, const wchar_t *file,
    unsigned int line,
    size_t pReserved /* Changed from uintptr_t to size_t to avoid stdint.h */
) {
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
}
#endif

/**
 * @brief Internal helper to get HANDLE from file descriptor. Returns int (0 for
 * success) and sets HANDLE.
 */
static int get_handle_from_fd_helper(int fd, HANDLE *out_handle) {
  HANDLE h = INVALID_HANDLE_VALUE;
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
  _invalid_parameter_handler old_handler;
#if defined(_MSC_VER) && _MSC_VER >= 1900
  old_handler = _set_thread_local_invalid_parameter_handler(
      (_invalid_parameter_handler)null_invalid_parameter_handler);
#else
  old_handler = _set_invalid_parameter_handler(
      (_invalid_parameter_handler)null_invalid_parameter_handler);
#endif
#endif

  if (fd >= 0) {
    h = (HANDLE)_get_osfhandle(fd);
  }

#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#if defined(_MSC_VER) && _MSC_VER >= 1900
  _set_thread_local_invalid_parameter_handler(old_handler);
#else
  _set_invalid_parameter_handler(old_handler);
#endif
#endif

  if (h == INVALID_HANDLE_VALUE) {
    if (fd == 0)
      h = GetStdHandle(STD_INPUT_HANDLE);
    else if (fd == 1)
      h = GetStdHandle(STD_OUTPUT_HANDLE);
    else if (fd == 2)
      h = GetStdHandle(STD_ERROR_HANDLE);
  }

  *out_handle = h;
  return (h != INVALID_HANDLE_VALUE) ? 0 : 1;
}
#endif

/** \brief cfgetispeed function. */
speed_t cfgetispeed(const struct termios *termios_p) {
  if (!termios_p)
    return 0;
  return termios_p->c_ispeed;
}

/** \brief cfgetospeed function. */
speed_t cfgetospeed(const struct termios *termios_p) {
  if (!termios_p)
    return 0;
  return termios_p->c_ospeed;
}

/** \brief cfsetispeed function. */
int cfsetispeed(struct termios *termios_p, speed_t speed) {
  if (!termios_p)
    return -1;
  termios_p->c_ispeed = speed;
  return 0;
}

/** \brief cfsetospeed function. */
int cfsetospeed(struct termios *termios_p, speed_t speed) {
  if (!termios_p)
    return -1;
  termios_p->c_ospeed = speed;
  return 0;
}

/** \brief tcdrain function. */
int tcdrain(int fd) {
#ifdef _WIN32
  HANDLE h;
  char errbuf[64];
  if (get_handle_from_fd_helper(fd, &h) == 0 && h &&
      h != INVALID_HANDLE_VALUE) {
    if (FlushFileBuffers(h)) {
      return 0;
    }
  }
  format_error_msg(errbuf, sizeof(errbuf), fd);
  return -1;
#else
  (void)fd;
  return -1;
#endif
}

/** \brief tcflow function. */
int tcflow(int fd, int action) {
  (void)fd;
  (void)action;
  return 0;
}

/** \brief tcflush function. */
int tcflush(int fd, int queue_selector) {
#ifdef _WIN32
  HANDLE h;
  if (get_handle_from_fd_helper(fd, &h) == 0 && h &&
      h != INVALID_HANDLE_VALUE) {
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

/** \brief tcgetattr function. */
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

  if (get_handle_from_fd_helper(fd, &h) == 0 && h &&
      h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
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
  if (termios_p) {
    termios_p->c_iflag = 0;
    termios_p->c_oflag = 0;
    termios_p->c_cflag = 0;
    termios_p->c_lflag = 0;
    termios_p->c_ispeed = B0;
    termios_p->c_ospeed = B0;
  }
  return -1;
#endif
}

/** \brief tcgetsid function. */
pid_t tcgetsid(int fd) {
  (void)fd;
  return -1;
}

/** \brief tcsendbreak function. */
int tcsendbreak(int fd, int duration) {
  (void)fd;
  (void)duration;
  return 0;
}

/** \brief tcsetattr function. */
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p) {
#ifdef _WIN32
  HANDLE h;
  DWORD mode;

  (void)optional_actions;

  if (!termios_p) {
    return -1;
  }

  if (get_handle_from_fd_helper(fd, &h) == 0 && h &&
      h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
    if (fd == 0) {
      if (termios_p->c_lflag & ECHO)
        mode |= ENABLE_ECHO_INPUT;
      else
        mode &= ~ENABLE_ECHO_INPUT;

      if (termios_p->c_lflag & ICANON)
        mode |= ENABLE_LINE_INPUT;
      else
        mode &= ~ENABLE_LINE_INPUT;

      if (termios_p->c_lflag & ISIG)
        mode |= ENABLE_PROCESSED_INPUT;
      else
        mode &= ~ENABLE_PROCESSED_INPUT;
    } else {
      if (termios_p->c_oflag & OPOST)
        mode |= ENABLE_PROCESSED_OUTPUT;
      else
        mode &= ~ENABLE_PROCESSED_OUTPUT;
    }
    if (SetConsoleMode(h, mode)) {
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

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_termios(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_termios;
