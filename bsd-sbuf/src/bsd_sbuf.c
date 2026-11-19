#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_sbuf.c
 * @brief Implementation of FreeBSD <sys/sbuf.h> safe string buffers.
 */

/* clang-format off */
#include <sys/sbuf.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

static void *default_sbuf_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

static void *default_sbuf_malloc(size_t size) { return malloc(size); }

static void *default_sbuf_realloc(void *ptr, size_t size) {
  return realloc(ptr, size);
}

#if defined(__GNUC__) || defined(__clang__)
/** @brief Compiler printf-format checking attribute macro. */
#define BSD_SBUF_PRINTF_ATTR(fmt_idx, arg_idx)                                 \
  __attribute__((format(printf, fmt_idx, arg_idx)))
#else
/** @brief Compiler printf-format checking attribute macro. */
#define BSD_SBUF_PRINTF_ATTR(fmt_idx, arg_idx)
#endif

static int default_sbuf_vsnprintf(char *buf, size_t size, const char *fmt,
                                  va_list ap) BSD_SBUF_PRINTF_ATTR(3, 0);
static int default_sbuf_vsnprintf(char *buf, size_t size, const char *fmt,
                                  va_list ap) {
#if defined(_MSC_VER)
  int r;
  buf[0] = '\0';
  r = _vsnprintf_s(buf, size, _TRUNCATE, fmt, ap);
  if (r < 0 && buf[size - 1] == '\0' && strlen(buf) == size - 1) {
    return (int)size;
  }
  return r;
#else
  return vsnprintf(buf, size, fmt, ap);
#endif
}

/** @brief Overrideable calloc hook for OOM testing in bsd-sbuf. */
void *(*g_bsd_sbuf_calloc)(size_t, size_t) = default_sbuf_calloc;
/** @brief Overrideable malloc hook for OOM testing in bsd-sbuf. */
void *(*g_bsd_sbuf_malloc)(size_t) = default_sbuf_malloc;
/** @brief Overrideable realloc hook for OOM testing in bsd-sbuf. */
void *(*g_bsd_sbuf_realloc)(void *, size_t) = default_sbuf_realloc;
/** @brief Overrideable vsnprintf hook for format error testing in bsd-sbuf. */
int (*g_bsd_sbuf_vsnprintf)(char *, size_t, const char *,
                            va_list) = default_sbuf_vsnprintf;

/** @brief Resets all bsd-sbuf allocator and formatting hooks to defaults. */
void bsd_sbuf_reset_hooks(void);
void bsd_sbuf_reset_hooks(void) {
  g_bsd_sbuf_calloc = default_sbuf_calloc;
  g_bsd_sbuf_malloc = default_sbuf_malloc;
  g_bsd_sbuf_realloc = default_sbuf_realloc;
  g_bsd_sbuf_vsnprintf = default_sbuf_vsnprintf;
}

/**
 * @brief Ensure the sbuf has room for addlen additional bytes plus NUL.
 * @param[in,out] s Target string buffer.
 * @param[in] addlen Number of additional payload bytes required.
 * @return 0 on success, or -1 on error/overflow.
 */
static int sbuf_extend(struct sbuf *s, size_t addlen) {
  size_t newsize;
  char *newbuf;
  if (s->s_error != 0) {
    return -1;
  }
  if ((size_t)(s->s_len + (ssize_t)addlen + 1) <= (size_t)s->s_size) {
    return 0;
  }
  if (s->s_drain_func != NULL && s->s_len > 0) {
    int r = s->s_drain_func(s->s_drain_arg, s->s_buf, (int)s->s_len);
    if (r < 0) {
      s->s_error = ENOMEM;
      return -1;
    }
    s->s_len = 0;
    if ((size_t)(addlen + 1) <= (size_t)s->s_size) {
      return 0;
    }
  }
  if ((s->s_flags & SBUF_AUTOEXTEND) == 0) {
    s->s_error = ENOMEM;
    return -1;
  }
  newsize = (s->s_size > 0) ? (size_t)s->s_size : 64;
  while (newsize < (size_t)s->s_len + addlen + 1) {
    newsize *= 2;
  }
  newbuf = (char *)g_bsd_sbuf_realloc(s->s_buf, newsize);
  if (newbuf == NULL) {
    s->s_error = ENOMEM;
    return -1;
  }
  s->s_buf = newbuf;
  s->s_size = (ssize_t)newsize;
  return 0;
}

/**
 * @brief Initialize or allocate a safe string buffer.
 * @param[in,out] s Optional caller-provided sbuf struct, or NULL to allocate.
 * @param[in,out] buf Optional caller-provided backing buffer, or NULL to
 * allocate.
 * @param[in] length Size of backing buffer in bytes.
 * @param[in] flags Bitmask of SBUF_* flags.
 * @return Initialized sbuf pointer, or NULL on allocation failure.
 */
struct sbuf *sbuf_new(struct sbuf *s, char *buf, int length, int flags) {
  int dynstruct = 0;
  if (s == NULL) {
    s = (struct sbuf *)g_bsd_sbuf_calloc(1, sizeof(*s));
    if (s == NULL) {
      return NULL;
    }
    dynstruct = 1;
  } else {
    memset(s, 0, sizeof(*s));
  }
  s->s_flags = flags & SBUF_USRFLAGMSK;
  if (dynstruct) {
    s->s_flags |= SBUF_DYNSTRUCT;
  }
  if (buf != NULL) {
    s->s_buf = buf;
    s->s_size = length;
  } else {
    int alloc_len = (length > 0) ? length : 64;
    s->s_buf = (char *)g_bsd_sbuf_malloc((size_t)alloc_len);
    if (s->s_buf == NULL) {
      if (dynstruct) {
        free(s);
      }
      return NULL;
    }
    s->s_size = alloc_len;
    s->s_flags |= SBUF_DYNAMIC;
  }
  if (s->s_size > 0) {
    s->s_buf[0] = '\0';
  }
  return s;
}

/**
 * @brief Clear contents and error state of an sbuf.
 * @param[in,out] s Target string buffer (may be NULL).
 */
void sbuf_clear(struct sbuf *s) {
  if (s != NULL) {
    s->s_len = 0;
    s->s_error = 0;
    s->s_flags &= ~SBUF_FINISHED;
    if (s->s_buf != NULL && s->s_size > 0) {
      s->s_buf[0] = '\0';
    }
  }
}

/**
 * @brief Truncate sbuf to a specified position.
 * @param[in,out] s Target string buffer.
 * @param[in] newpos New length position (must satisfy 0 <= newpos <= s_len).
 * @return 0 on success, or -1 if invalid.
 */
int sbuf_setpos(struct sbuf *s, ssize_t newpos) {
  if (s == NULL || newpos < 0 || newpos > s->s_len) {
    return -1;
  }
  s->s_len = newpos;
  s->s_buf[s->s_len] = '\0';
  return 0;
}

/**
 * @brief Append binary data to an sbuf.
 * @param[in,out] s Target string buffer.
 * @param[in] ptr Source byte buffer.
 * @param[in] len Number of bytes to append.
 * @return 0 on success, or -1 on error.
 */
int sbuf_bcat(struct sbuf *s, const void *ptr, size_t len) {
  if (s == NULL || ptr == NULL) {
    return -1;
  }
  if (sbuf_extend(s, len) < 0) {
    return -1;
  }
  memcpy(s->s_buf + s->s_len, ptr, len);
  s->s_len += (ssize_t)len;
  s->s_buf[s->s_len] = '\0';
  return 0;
}

/**
 * @brief Copy binary data into an sbuf, replacing existing content.
 * @param[in,out] s Target string buffer.
 * @param[in] ptr Source byte buffer.
 * @param[in] len Number of bytes to copy.
 * @return 0 on success, or -1 on error.
 */
int sbuf_bcpy(struct sbuf *s, const void *ptr, size_t len) {
  sbuf_clear(s);
  return sbuf_bcat(s, ptr, len);
}

/**
 * @brief Append a NUL-terminated string to an sbuf.
 * @param[in,out] s Target string buffer.
 * @param[in] str Source NUL-terminated string.
 * @return 0 on success, or -1 on error.
 */
int sbuf_cat(struct sbuf *s, const char *str) {
  if (str == NULL) {
    return -1;
  }
  return sbuf_bcat(s, str, strlen(str));
}

/**
 * @brief Copy a NUL-terminated string into an sbuf, replacing existing content.
 * @param[in,out] s Target string buffer.
 * @param[in] str Source NUL-terminated string.
 * @return 0 on success, or -1 on error.
 */
int sbuf_cpy(struct sbuf *s, const char *str) {
  sbuf_clear(s);
  return sbuf_cat(s, str);
}

/**
 * @brief Append va_list formatted output to an sbuf.
 * @param[in,out] s Target string buffer.
 * @param[in] fmt Format string.
 * @param[in] ap Argument list.
 * @return 0 on success, or -1 on error.
 */
int sbuf_vprintf(struct sbuf *s, const char *fmt, va_list ap) {
  char tmp[1024];
  int len;
  if (s == NULL || fmt == NULL) {
    return -1;
  }
  len = g_bsd_sbuf_vsnprintf(tmp, sizeof(tmp), fmt, ap);
  if (len < 0) {
    s->s_error = EINVAL;
    return -1;
  }
  if ((size_t)len >= sizeof(tmp)) {
    len = (int)(sizeof(tmp) - 1);
  }
  return sbuf_bcat(s, tmp, (size_t)len);
}

/**
 * @brief Append printf-formatted output to an sbuf.
 * @param[in,out] s Target string buffer.
 * @param[in] fmt Format string.
 * @return 0 on success, or -1 on error.
 */
int sbuf_printf(struct sbuf *s, const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = sbuf_vprintf(s, fmt, ap);
  va_end(ap);
  return r;
}

/**
 * @brief Append a single character to an sbuf.
 * @param[in,out] s Target string buffer.
 * @param[in] c Character value.
 * @return 0 on success, or -1 on error.
 */
int sbuf_putc(struct sbuf *s, int c) {
  char ch = (char)c;
  return sbuf_bcat(s, &ch, 1);
}

/**
 * @brief Ensure sbuf ends with a newline character.
 * @param[in,out] s Target string buffer.
 * @return 0 on success, or -1 on error.
 */
int sbuf_nl_terminate(struct sbuf *s) {
  if (s == NULL) {
    return -1;
  }
  if (s->s_len == 0 || s->s_buf[s->s_len - 1] != '\n') {
    return sbuf_putc(s, '\n');
  }
  return 0;
}

/**
 * @brief Remove trailing whitespace characters from an sbuf.
 * @param[in,out] s Target string buffer.
 * @return 0 on success, or -1 on NULL argument.
 */
int sbuf_trim(struct sbuf *s) {
  if (s == NULL) {
    return -1;
  }
  while (s->s_len > 0 && isspace((unsigned char)s->s_buf[s->s_len - 1])) {
    s->s_len--;
  }
  s->s_buf[s->s_len] = '\0';
  return 0;
}

/**
 * @brief Return current error code stored in an sbuf.
 * @param[in] s Target string buffer.
 * @return 0 if no error, or non-zero errno value.
 */
int sbuf_error(const struct sbuf *s) {
  return (s != NULL) ? s->s_error : EINVAL;
}

/**
 * @brief Finalize an sbuf, invoking any drain callback and NUL-terminating.
 * @param[in,out] s Target string buffer.
 * @return 0 on success, or -1 on error (with errno set).
 */
int sbuf_finish(struct sbuf *s) {
  if (s == NULL) {
    return -1;
  }
  if (s->s_error != 0) {
    errno = s->s_error;
    return -1;
  }
  if (s->s_drain_func != NULL && s->s_len > 0) {
    int r = s->s_drain_func(s->s_drain_arg, s->s_buf, (int)s->s_len);
    if (r < 0) {
      s->s_error = ENOMEM;
      errno = s->s_error;
      return -1;
    }
    s->s_len = 0;
  }
  s->s_buf[s->s_len] = '\0';
  s->s_flags |= SBUF_FINISHED;
  return 0;
}

/**
 * @brief Return pointer to the underlying sbuf data buffer.
 * @param[in] s Target string buffer.
 * @return Data pointer, or NULL if s is NULL.
 */
char *sbuf_data(struct sbuf *s) { return (s != NULL) ? s->s_buf : NULL; }

/**
 * @brief Return current byte length of data in the sbuf.
 * @param[in] s Target string buffer.
 * @return Byte length, or -1 if s is NULL.
 */
ssize_t sbuf_len(struct sbuf *s) { return (s != NULL) ? s->s_len : -1; }

/**
 * @brief Check whether sbuf_finish() has been completed on an sbuf.
 * @param[in] s Target string buffer.
 * @return Non-zero if finished, 0 otherwise.
 */
int sbuf_done(const struct sbuf *s) {
  return (s != NULL) ? ((s->s_flags & SBUF_FINISHED) != 0) : 0;
}

/**
 * @brief Destroy an sbuf and free dynamically allocated resources.
 * @param[in,out] s Target string buffer (may be NULL).
 */
void sbuf_delete(struct sbuf *s) {
  if (s == NULL) {
    return;
  }
  if ((s->s_flags & SBUF_DYNAMIC) && s->s_buf != NULL) {
    free(s->s_buf);
  }
  if (s->s_flags & SBUF_DYNSTRUCT) {
    free(s);
  }
}

/**
 * @brief Attach or remove a drain callback on an sbuf.
 * @param[in,out] s Target string buffer.
 * @param[in] func Drain callback, or NULL.
 * @param[in] arg Opaque argument passed to func.
 */
void sbuf_set_drain(struct sbuf *s, sbuf_drain_func *func, void *arg) {
  if (s != NULL) {
    s->s_drain_func = func;
    s->s_drain_arg = arg;
  }
}

/**
 * @brief Append a formatted hexadecimal dump to an sbuf.
 * @param[in,out] s Target string buffer.
 * @param[in] ptr Source binary data.
 * @param[in] length Number of bytes to dump.
 * @param[in] hdr Optional prefix header string.
 * @param[in] flags Formatting flags (reserved).
 */
void sbuf_hexdump(struct sbuf *s, const void *ptr, int length, const char *hdr,
                  int flags) {
  const unsigned char *cp = (const unsigned char *)ptr;
  int i;
  (void)flags;
  if (s == NULL || ptr == NULL) {
    return;
  }
  if (hdr != NULL) {
    sbuf_cat(s, hdr);
  }
  for (i = 0; i < length; i++) {
    sbuf_printf(s, "%02x ", cp[i]);
  }
  sbuf_putc(s, '\n');
}

/** @brief Dummy typedef preventing empty translation unit warnings. */
typedef int make_iso_compilers_happy_tu_bsd_sbuf;

#endif
/** @brief Dummy typedef preventing empty mega translation unit warnings. */
typedef int make_iso_compilers_happy_mega_bsd_sbuf;
