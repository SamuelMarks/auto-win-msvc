/* clang-format off */
#include <posix-regex.h>
#include <stdlib.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) ||       \
    defined(__MSDOS__) || defined(__WATCOMC__)

/** \brief regcomp function. */
int regcomp(regex_t *preg, const char *pattern, int cflags) {
  preg = preg;
  pattern = pattern;
  cflags = cflags;
  return REG_ENOSYS;
}

/** \brief regexec function. */
int regexec(const regex_t *preg, const char *string, size_t nmatch,
            regmatch_t pmatch[], int eflags) {
  preg = preg;
  string = string;
  nmatch = nmatch;
  pmatch = pmatch;
  eflags = eflags;
  return REG_ENOSYS;
}

/** \brief regerror function. */
size_t regerror(int errcode, const regex_t *preg, char *errbuf,
                size_t errbuf_size) {
  errcode = errcode;
  preg = preg;
  errbuf = errbuf;
  return errbuf_size;
}

/** \brief regfree function. */
void regfree(regex_t *preg) { preg = preg; }

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
/* Dummy function to prevent empty translation unit */
int dummy_posix_regex(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_regex;
