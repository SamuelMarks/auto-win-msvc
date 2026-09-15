/* posix-regex.c - Strict C89 Implementation */

/* clang-format off */
#include "posix-regex.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

/**
 * @brief Initializes and validates the posix-regex module.
 * @param[out] out_status Pointer to an integer receiving initialized status.
 * @return POSIX_REGEX_SUCCESS on success, or POSIX_REGEX_ERROR_NULL_POINTER.
 */
enum posix_regex_error_code posix_regex_init(int *out_status) {
  if (out_status == NULL) {
    return POSIX_REGEX_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return POSIX_REGEX_SUCCESS;
}

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) ||       \
    defined(__MSDOS__) || defined(__WATCOMC__)

/** @brief Compile regular expression pattern. */
int regcomp(regex_t *preg, const char *pattern, int cflags) {
  (void)preg;
  (void)pattern;
  (void)cflags;
  return REG_ENOSYS;
}

/** @brief Match compiled regular expression against a string. */
int regexec(const regex_t *preg, const char *string, size_t nmatch,
            regmatch_t pmatch[], int eflags) {
  (void)preg;
  (void)string;
  (void)nmatch;
  (void)pmatch;
  (void)eflags;
  return REG_ENOSYS;
}

/** @brief Return error string for regular expression error code. */
size_t regerror(int errcode, const regex_t *preg, char *errbuf,
                size_t errbuf_size) {
  const char *msg = "Regex error";
  size_t msg_len;
  (void)errcode;
  (void)preg;
  msg_len = strlen(msg) + 1;
  if (errbuf != NULL && errbuf_size > 0) {
#if defined(_MSC_VER)
    strncpy_s(errbuf, errbuf_size, msg, _TRUNCATE);
#else
    strncpy(errbuf, msg, errbuf_size - 1);
    errbuf[errbuf_size - 1] = '\0';
#endif
  }
  return msg_len;
}

/** @brief Free resources associated with compiled regular expression. */
void regfree(regex_t *preg) { (void)preg; }

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_regex;
