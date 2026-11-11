#ifndef SYS_SBUF_H
#define SYS_SBUF_H

/**
 * @file sbuf.h
 * @brief FreeBSD safe string buffer (<sys/sbuf.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stdarg.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define SBUF_FIXEDLEN 0x00000000
#define SBUF_AUTOEXTEND 0x00000001
#define SBUF_USRFLAGMSK 0x0000ffff
#define SBUF_DYNAMIC 0x00010000
#define SBUF_FINISHED 0x00020000
#define SBUF_DYNSTRUCT 0x00080000
#define SBUF_INCLUDENUL 0x00000002
#define SBUF_DRAINTOEOR 0x00000004

typedef int(sbuf_drain_func)(void *, const char *, int);

/** @brief Safe string composition buffer structure. */
struct sbuf {
  char *s_buf;
  sbuf_drain_func *s_drain_func;
  void *s_drain_arg;
  int s_error;
  ssize_t s_size;
  ssize_t s_len;
  int s_flags;
};

/** @brief Initialize or allocate a safe string buffer. */
struct sbuf *sbuf_new(struct sbuf *s, char *buf, int length, int flags);
/** @brief Allocate an auto-extending sbuf. */
#define sbuf_new_auto() sbuf_new(NULL, NULL, 0, SBUF_AUTOEXTEND)
/** @brief Clear contents of an sbuf. */
void sbuf_clear(struct sbuf *s);
/** @brief Truncate sbuf to specified position. */
int sbuf_setpos(struct sbuf *s, ssize_t newpos);
/** @brief Append binary data to an sbuf. */
int sbuf_bcat(struct sbuf *s, const void *ptr, size_t len);
/** @brief Copy binary data into an sbuf, replacing existing content. */
int sbuf_bcpy(struct sbuf *s, const void *ptr, size_t len);
/** @brief Append a NUL-terminated string to an sbuf. */
int sbuf_cat(struct sbuf *s, const char *str);
/** @brief Copy a NUL-terminated string into an sbuf. */
int sbuf_cpy(struct sbuf *s, const char *str);
/** @brief Append formatted output to an sbuf. */
int sbuf_printf(struct sbuf *s, const char *fmt, ...);
/** @brief Append va_list formatted output to an sbuf. */
int sbuf_vprintf(struct sbuf *s, const char *fmt, va_list ap);
/** @brief Append a single character to an sbuf. */
int sbuf_putc(struct sbuf *s, int c);
/** @brief Ensure sbuf ends with a newline. */
int sbuf_nl_terminate(struct sbuf *s);
/** @brief Remove trailing whitespace from an sbuf. */
int sbuf_trim(struct sbuf *s);
/** @brief Check whether an sbuf has encountered an error. */
int sbuf_error(const struct sbuf *s);
/** @brief Finalize an sbuf and NUL-terminate or drain it. */
int sbuf_finish(struct sbuf *s);
/** @brief Return pointer to the sbuf data buffer. */
char *sbuf_data(struct sbuf *s);
/** @brief Return current length of data in the sbuf. */
ssize_t sbuf_len(struct sbuf *s);
/** @brief Return non-zero if sbuf_finish() has been called. */
int sbuf_done(const struct sbuf *s);
/** @brief Destroy and free resources associated with an sbuf. */
void sbuf_delete(struct sbuf *s);
/** @brief Attach a custom drain callback to an sbuf. */
void sbuf_set_drain(struct sbuf *s, sbuf_drain_func *func, void *arg);
/** @brief Append formatted hex dump into an sbuf. */
void sbuf_hexdump(struct sbuf *s, const void *ptr, int length, const char *hdr,
                  int flags);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_SBUF_H */
