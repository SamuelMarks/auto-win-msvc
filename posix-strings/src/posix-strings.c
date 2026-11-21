/**
 * @file posix-strings.c
 * @brief Implementation of posix-strings polyfills.
 */

/* clang-format off */
#include "posix-strings.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
/* clang-format on */

#if defined(_MSC_VER)

#pragma intrinsic(_BitScanForward)

#if defined(_WIN64)
#pragma intrinsic(_BitScanForward64)
#endif

/** @brief Finds the first (least significant) bit set in an integer. */
int ffs(int i) {
  unsigned long index;
  if (_BitScanForward(&index, (unsigned long)i)) {
    return (int)index + 1;
  }
  return 0;
}

/** @brief Finds the first (least significant) bit set in a long integer. */
int ffsl(long i) {
  unsigned long index;
  if (_BitScanForward(&index, (unsigned long)i)) {
    return (int)index + 1;
  }
  return 0;
}

/** @brief Finds the first (least significant) bit set in a long long integer.
 */
int ffsll(posix_strings_llong i) {
  unsigned long index;
#if defined(_WIN64)
  if (_BitScanForward64(&index, (unsigned __int64)i)) {
    return (int)index + 1;
  }
#else
  if (_BitScanForward(&index, (unsigned long)i)) {
    return (int)index + 1;
  }
  if (_BitScanForward(&index, (unsigned long)((unsigned __int64)i >> 32))) {
    return (int)index + 33;
  }
#endif
  return 0;
}

#elif defined(_WIN32) && (defined(__GNUC__) || defined(__clang__))

/** @brief Finds the first (least significant) bit set in an integer. */
int ffs(int i) { return __builtin_ffs(i); }

/** @brief Finds the first (least significant) bit set in a long integer. */
int ffsl(long i) { return __builtin_ffsl(i); }

/** @brief Finds the first (least significant) bit set in a long long integer.
 */
int ffsll(posix_strings_llong i) { return __builtin_ffsll(i); }

#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)

/** @brief Finds the first (least significant) bit set in a long integer. */
int ffsl(long i) {
  unsigned long u = (unsigned long)i;
  int bit = 1;
  if (u == 0) {
    return 0;
  }
  while ((u & 1UL) == 0) {
    u >>= 1;
    bit++;
  }
  return bit;
}

/** @brief Finds the first (least significant) bit set in a long long integer.
 */
int ffsll(posix_strings_llong i) {
  unsigned __int64 u = (unsigned __int64)i;
  int bit = 1;
  if (u == 0) {
    return 0;
  }
  while ((u & 1U) == 0) {
    u >>= 1;
    bit++;
  }
  return bit;
}

#endif /* _MSC_VER */

#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @brief Retrieves information on posix-strings polyfill availability.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT if out_available is NULL.
 */
auto_win_msvc_error_t posix_strings_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

/** @brief Copies src to dest, returning pointer to '\0'. */
char *posix_stpcpy(char *dest, const char *src) {
  while ((*dest = *src) != '\0') {
    dest++;
    src++;
  }
  return dest;
}

/** @brief Copies at most n bytes of src to dest, returning pointer to '\0' or
 * dest + n. */
char *posix_stpncpy(char *dest, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    dest[i] = src[i];
    if (src[i] == '\0') {
      char *ret = &dest[i];
      for (i = i + 1; i < n; i++) {
        dest[i] = '\0';
      }
      return ret;
    }
  }
  return &dest[n];
}

/** @brief Locates first occurrence of c in s, or pointer to terminating '\0'.
 */
char *posix_strchrnul(const char *s, int c) {
  char ch;
  size_t offset;
  union {
    const char *in;
    char *out;
  } u;
  ch = (char)c;
  offset = 0;
  while (s[offset] != '\0' && s[offset] != ch) {
    offset++;
  }
  u.in = s + offset;
  return u.out;
}
#endif

/** @brief Copies n bytes from src to dest, returning pointer to byte following
 * last written byte. */
void *posix_mempcpy(void *dest, const void *src, size_t n) {
  return (void *)((char *)memcpy(dest, src, n) + n);
}

/** @brief Finds last occurrence of c in initial n bytes of s. */
void *posix_memrchr(const void *s, int c, size_t n) {
  const unsigned char *p;
  unsigned char uc;
  union {
    const unsigned char *in;
    void *out;
  } u;
  p = (const unsigned char *)s;
  uc = (unsigned char)c;
  while (n > 0) {
    n--;
    if (p[n] == uc) {
      u.in = p + n;
      return u.out;
    }
  }
  return NULL;
}

/** @brief Returns description string for signal number. */
char *posix_strsignal(int sig) {
  static char unknown_buf[32];
  const char *msg = NULL;
#if defined(AUTO_WIN_MSVC_MEGA_LIBRARY) && !defined(_WIN32)
  msg = (sig == 15) ? "Terminated" : "Segmentation fault";
#else
  switch (sig) {
  case 1:
    msg = "Hangup";
    break;
  case 2:
    msg = "Interrupt";
    break;
  case 3:
    msg = "Quit";
    break;
  case 4:
    msg = "Illegal instruction";
    break;
  case 6:
    msg = "Aborted";
    break;
  case 8:
    msg = "Floating point exception";
    break;
  case 9:
    msg = "Killed";
    break;
  case 11:
    msg = "Segmentation fault";
    break;
  case 13:
    msg = "Broken pipe";
    break;
  case 14:
    msg = "Alarm clock";
    break;
  case 15:
    msg = "Terminated";
    break;
  case 17:
    msg = "Child exited";
    break;
  case 18:
    msg = "Continued";
    break;
  case 19:
    msg = "Stopped (signal)";
    break;
  case 20:
    msg = "Stopped";
    break;
  case 21:
    msg = "Stopped (tty input)";
    break;
  case 22:
    msg = "Stopped (tty output)";
    break;
  case 28:
    msg = "Window changed";
    break;
  default:
    break;
  }
  if (msg == NULL) {
#if defined(_MSC_VER)
    (void)sprintf_s(unknown_buf, sizeof(unknown_buf), "Unknown signal %d", sig);
#else
    (void)sprintf(unknown_buf, "Unknown signal %d", sig);
#endif
    return unknown_buf;
  }
#endif
  {
    size_t len = strlen(msg);
    memcpy(unknown_buf, msg, len + 1);
  }
  return unknown_buf;
}

#if defined(_WIN32)
#undef mempcpy
/** @brief ABI wrapper for mempcpy on Windows. */
void *mempcpy(void *dest, const void *src, size_t n) {
  return posix_mempcpy(dest, src, n);
}

#undef strchrnul
/** @brief ABI wrapper for strchrnul on Windows. */
char *strchrnul(const char *s, int c);
char *strchrnul(const char *s, int c) { return posix_strchrnul(s, c); }

#undef stpcpy
/** @brief ABI wrapper for stpcpy on Windows. */
char *stpcpy(char *dest, const char *src);
char *stpcpy(char *dest, const char *src) { return posix_stpcpy(dest, src); }

#undef stpncpy
/** @brief ABI wrapper for stpncpy on Windows. */
char *stpncpy(char *dest, const char *src, size_t n);
char *stpncpy(char *dest, const char *src, size_t n) {
  return posix_stpncpy(dest, src, n);
}

#undef memrchr
/** @brief ABI wrapper for memrchr on Windows. */
void *memrchr(const void *s, int c, size_t n);
void *memrchr(const void *s, int c, size_t n) { return posix_memrchr(s, c, n); }

#undef strsignal
/** @brief ABI wrapper for strsignal on Windows. */
char *strsignal(int sig);
char *strsignal(int sig) { return posix_strsignal(sig); }

#undef strtok_r
/** @brief ABI wrapper for strtok_r on Windows. */
char *strtok_r(char *str, const char *delim, char **saveptr) {
  return strtok_s(str, delim, saveptr);
}
#endif

typedef int make_iso_compilers_happy_tu_posix_strings;
