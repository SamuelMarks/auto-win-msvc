#include <posix-regex.h>

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)

/** \brief regcomp function. */
int regcomp(regex_t *preg, const char *pattern, int cflags) {
  (void)preg;
  (void)pattern;
  (void)cflags;
  return REG_NOSYS;
}

/** \brief regexec function. */
int regexec(const regex_t *preg, const char *string, size_t nmatch,
            regmatch_t pmatch[], int eflags) {
  (void)preg;
  (void)string;
  (void)nmatch;
  (void)pmatch;
  (void)eflags;
  return REG_NOSYS;
}

/** \brief regerror function. */
size_t regerror(int errcode, const regex_t *preg, char *errbuf,
                size_t errbuf_size) {
  (void)errcode;
  (void)preg;
  (void)errbuf;
  return errbuf_size;
}

/** \brief regfree function. */
void regfree(regex_t *preg) { (void)preg; }

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
/* Dummy function to prevent empty translation unit */
int dummy_posix_regex(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_regex;
