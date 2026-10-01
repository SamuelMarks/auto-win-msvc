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
/* clang-format on */

#pragma intrinsic(_BitScanForward)

#if defined(_WIN64)
#pragma intrinsic(_BitScanForward64)
#endif

/** @brief Finds the first (least significant) bit set in an integer. */
int ffs(int i) {
  unsigned long index;
  if (i == 0) {
    return 0;
  }
  if (_BitScanForward(&index, (unsigned long)i)) {
    return (int)index + 1;
  }
  return 0;
}

/** @brief Finds the first (least significant) bit set in a long integer. */
int ffsl(long i) {
  unsigned long index;
  if (i == 0) {
    return 0;
  }
  if (_BitScanForward(&index, (unsigned long)i)) {
    return (int)index + 1;
  }
  return 0;
}

/** @brief Finds the first (least significant) bit set in a long long integer.
 */
int ffsll(posix_strings_llong i) {
  unsigned long index;
  if (i == 0) {
    return 0;
  }
#if defined(_WIN64)
  if (_BitScanForward64(&index, (unsigned __int64)i)) {
    return (int)index + 1;
  }
#else
  if ((unsigned long)i != 0) {
    if (_BitScanForward(&index, (unsigned long)i)) {
      return (int)index + 1;
    }
  } else {
    if (_BitScanForward(&index, (unsigned long)(i >> 32))) {
      return (int)index + 33;
    }
  }
#endif
  return 0;
}

#endif /* _MSC_VER */

/**
 * @brief Retrieves information on posix-strings polyfill availability.
 */
enum posix_strings_error_code posix_strings_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_STRINGS_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_STRINGS_SUCCESS;
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

/** @brief Copies n bytes from src to dest, returning pointer to byte following
 * last written byte. */
void *posix_mempcpy(void *dest, const void *src, size_t n) {
  return (void *)((char *)memcpy(dest, src, n) + n);
}

/** @brief Finds last occurrence of c in initial n bytes of s. */
void *posix_memrchr(const void *s, int c, size_t n) {
  const unsigned char *p;
  unsigned char uc;
  p = (const unsigned char *)s;
  uc = (unsigned char)c;
  while (n > 0) {
    n--;
    if (p[n] == uc) {
      return (void *)((size_t)s + n);
    }
  }
  return NULL;
}

/** @brief Returns description string for signal number. */
char *posix_strsignal(int sig) {
  static char unknown_buf[32];
  switch (sig) {
  case 1:
    return "Hangup";
  case 2:
    return "Interrupt";
  case 3:
    return "Quit";
  case 4:
    return "Illegal instruction";
  case 6:
    return "Aborted";
  case 8:
    return "Floating point exception";
  case 9:
    return "Killed";
  case 11:
    return "Segmentation fault";
  case 13:
    return "Broken pipe";
  case 14:
    return "Alarm clock";
  case 15:
    return "Terminated";
  case 17:
    return "Child exited";
  case 18:
    return "Continued";
  case 19:
    return "Stopped (signal)";
  case 20:
    return "Stopped";
  case 21:
    return "Stopped (tty input)";
  case 22:
    return "Stopped (tty output)";
  case 28:
    return "Window changed";
  default:
    break;
  }
#if defined(_MSC_VER)
  sprintf_s(unknown_buf, sizeof(unknown_buf), "Unknown signal %d", sig);
#else
  sprintf(unknown_buf, "Unknown signal %d", sig);
#endif
  return unknown_buf;
}

typedef int make_iso_compilers_happy_tu_posix_strings;
