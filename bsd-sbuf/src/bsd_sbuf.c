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
  newbuf = (char *)realloc(s->s_buf, newsize);
  if (newbuf == NULL) {
    s->s_error = ENOMEM;
    return -1;
  }
  s->s_buf = newbuf;
  s->s_size = (ssize_t)newsize;
  return 0;
}

struct sbuf *sbuf_new(struct sbuf *s, char *buf, int length, int flags) {
  int dynstruct = 0;
  if (s == NULL) {
    s = (struct sbuf *)calloc(1, sizeof(*s));
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
    s->s_buf = (char *)malloc((size_t)alloc_len);
    if (s->s_buf == NULL) {
      if (dynstruct) {
        free(s);
      }
      return NULL;
    }
    s->s_size = alloc_len;
    s->s_flags |= SBUF_DYNAMIC;
  }
  s->s_buf[0] = '\0';
  return s;
}

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

int sbuf_setpos(struct sbuf *s, ssize_t newpos) {
  if (s == NULL || newpos < 0 || newpos > s->s_len) {
    return -1;
  }
  s->s_len = newpos;
  s->s_buf[s->s_len] = '\0';
  return 0;
}

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

int sbuf_bcpy(struct sbuf *s, const void *ptr, size_t len) {
  sbuf_clear(s);
  return sbuf_bcat(s, ptr, len);
}

int sbuf_cat(struct sbuf *s, const char *str) {
  if (str == NULL) {
    return -1;
  }
  return sbuf_bcat(s, str, strlen(str));
}

int sbuf_cpy(struct sbuf *s, const char *str) {
  sbuf_clear(s);
  return sbuf_cat(s, str);
}

int sbuf_vprintf(struct sbuf *s, const char *fmt, va_list ap) {
  char tmp[1024];
  int len;
  if (s == NULL || fmt == NULL) {
    return -1;
  }
#if defined(_MSC_VER)
  len = _vsnprintf_s(tmp, sizeof(tmp), _TRUNCATE, fmt, ap);
#else
  len = vsnprintf(tmp, sizeof(tmp), fmt, ap);
#endif
  if (len < 0) {
    s->s_error = EINVAL;
    return -1;
  }
  if ((size_t)len >= sizeof(tmp)) {
    len = (int)(sizeof(tmp) - 1);
  }
  return sbuf_bcat(s, tmp, (size_t)len);
}

int sbuf_printf(struct sbuf *s, const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = sbuf_vprintf(s, fmt, ap);
  va_end(ap);
  return r;
}

int sbuf_putc(struct sbuf *s, int c) {
  char ch = (char)c;
  return sbuf_bcat(s, &ch, 1);
}

int sbuf_nl_terminate(struct sbuf *s) {
  if (s == NULL) {
    return -1;
  }
  if (s->s_len == 0 || s->s_buf[s->s_len - 1] != '\n') {
    return sbuf_putc(s, '\n');
  }
  return 0;
}

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

int sbuf_error(const struct sbuf *s) {
  return (s != NULL) ? s->s_error : EINVAL;
}

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

char *sbuf_data(struct sbuf *s) { return (s != NULL) ? s->s_buf : NULL; }

ssize_t sbuf_len(struct sbuf *s) { return (s != NULL) ? s->s_len : -1; }

int sbuf_done(const struct sbuf *s) {
  return (s != NULL) ? ((s->s_flags & SBUF_FINISHED) != 0) : 0;
}

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

void sbuf_set_drain(struct sbuf *s, sbuf_drain_func *func, void *arg) {
  if (s != NULL) {
    s->s_drain_func = func;
    s->s_drain_arg = arg;
  }
}

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

typedef int make_iso_compilers_happy_tu_bsd_sbuf;

#endif
typedef int make_iso_compilers_happy_mega_bsd_sbuf;
