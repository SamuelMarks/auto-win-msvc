#ifndef POSIX_REGEX_H
#define POSIX_REGEX_H

/**
 * @file posix-regex.h
 * @brief POSIX regular expressions and error codes.
 */

/* clang-format off */
#include <stddef.h>
#if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(__MINGW64__) && \
    !defined(__MSDOS__) && !defined(__WATCOMC__)
#include <regex.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-regex operations.
 */
enum posix_regex_error_code {
  /** @brief Successful operation. */
  POSIX_REGEX_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  POSIX_REGEX_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed. */
  POSIX_REGEX_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Initializes and validates the posix-regex module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return POSIX_REGEX_SUCCESS on success, or an error code on failure.
 */
enum posix_regex_error_code posix_regex_init(int *out_status);

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) ||       \
    defined(__MSDOS__) || defined(__WATCOMC__)

/** @brief Regular expression compiled pattern structure. */
typedef struct {
  /** @brief Internal magic value. */
  int re_magic;
  /** @brief Number of subexpressions. */
  size_t re_nsub;
  /** @brief End of pattern pointer. */
  const char *re_endp;
  /** @brief Internal guts pointer. */
  void *re_guts;
} regex_t;

/** @brief Regular expression match offset type. */
typedef int regoff_t;

/** @brief Subexpression match structure. */
typedef struct {
  /** @brief Start offset of match. */
  regoff_t rm_so;
  /** @brief End offset of match. */
  regoff_t rm_eo;
} regmatch_t;

#define REG_BASIC 0000
#define REG_EXTENDED 0001
#define REG_ICASE 0002
#define REG_NOSUB 0004
#define REG_NEWLINE 0010
#define REG_NOSPEC 0020
#define REG_PEND 0040
#define REG_DUMP 0200

#define REG_NOTBOL 00001
#define REG_NOTEOL 00002
#define REG_STARTEND 00004
#define REG_TRACE 00400
#define REG_LARGE 01000
#define REG_BACKR 02000

#define REG_NOMATCH 1
#define REG_BADPAT 2
#define REG_ECOLLATE 3
#define REG_ECTYPE 4
#define REG_EESCAPE 5
#define REG_ESUBREG 6
#define REG_EBRACK 7
#define REG_EPAREN 8
#define REG_EBRACE 9
#define REG_BADBR 10
#define REG_ERANGE 11
#define REG_ESPACE 12
#define REG_BADRPT 13
#define REG_EMPTY 14
#define REG_ASSERT 15
#define REG_INVARG 16
#define REG_ILLSEQ 17
#define REG_ENOSYS 18

/** @brief Compile regular expression pattern. */
int regcomp(regex_t *preg, const char *pattern, int cflags);

/** @brief Match compiled regular expression against a string. */
int regexec(const regex_t *preg, const char *string, size_t nmatch,
            regmatch_t pmatch[], int eflags);

/** @brief Return error string for regular expression error code. */
size_t regerror(int errcode, const regex_t *preg, char *errbuf,
                size_t errbuf_size);

/** @brief Free resources associated with compiled regular expression. */
void regfree(regex_t *preg);

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_REGEX_H */
