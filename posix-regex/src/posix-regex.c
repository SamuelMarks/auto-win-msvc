/* posix-regex.c - Strict C89 Implementation */

/* clang-format off */
#include "posix-regex.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) ||       \
    defined(__MSDOS__) || defined(__WATCOMC__)
#ifndef PCRE2_STATIC
#define PCRE2_STATIC 1
#endif
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#endif
/* clang-format on */

/**
 * @brief Initializes and validates the posix-regex module.
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
  int errorcode;
  PCRE2_SIZE erroroffset;
  uint32_t options;
  pcre2_code *re;
  uint32_t capture_count;

  options = 0;
  capture_count = 0;

  if (preg == NULL || pattern == NULL) {
    return REG_INVARG;
  }

  if (cflags & REG_ICASE) {
    options |= PCRE2_CASELESS;
  }
  if (cflags & REG_NEWLINE) {
    options |= PCRE2_MULTILINE | PCRE2_DOTALL;
  }

  re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, options,
                     &errorcode, &erroroffset, NULL);
  if (re == NULL) {
    if (errorcode == PCRE2_ERROR_NOMEMORY) {
      return REG_ESPACE;
    }
    return REG_BADPAT;
  }

  preg->re_guts = (void *)re;
  if (pcre2_pattern_info(re, PCRE2_INFO_CAPTURECOUNT, &capture_count) != 0) {
    pcre2_code_free(re);
    preg->re_guts = NULL;
    return REG_BADPAT;
  }
  preg->re_nsub = (size_t)capture_count;
  return 0;
}

/** @brief Match compiled regular expression against a string. */
int regexec(const regex_t *preg, const char *string, size_t nmatch,
            regmatch_t pmatch[], int eflags) {
  pcre2_code *re;
  pcre2_match_data *match_data;
  int rc;
  size_t i;
  PCRE2_SIZE *ovector;
  uint32_t options;

  options = 0;

  if (preg == NULL || string == NULL) {
    return REG_INVARG;
  }

  re = (pcre2_code *)preg->re_guts;
  if (re == NULL) {
    return REG_INVARG;
  }

  if (eflags & REG_NOTBOL) {
    options |= PCRE2_NOTBOL;
  }
  if (eflags & REG_NOTEOL) {
    options |= PCRE2_NOTEOL;
  }

  match_data = pcre2_match_data_create_from_pattern(re, NULL);
  if (match_data == NULL) {
    return REG_ESPACE;
  }

  rc = pcre2_match(re, (PCRE2_SPTR)string, PCRE2_ZERO_TERMINATED, 0, options,
                   match_data, NULL);

  if (rc < 0) {
    pcre2_match_data_free(match_data);
    if (rc == PCRE2_ERROR_NOMEMORY) {
      return REG_ESPACE;
    }
    return REG_NOMATCH;
  }

  ovector = pcre2_get_ovector_pointer(match_data);
  for (i = 0; i < nmatch; i++) {
    if ((int)i < rc) {
      pmatch[i].rm_so = (regoff_t)ovector[2 * i];
      pmatch[i].rm_eo = (regoff_t)ovector[2 * i + 1];
    } else {
      pmatch[i].rm_so = -1;
      pmatch[i].rm_eo = -1;
    }
  }

  pcre2_match_data_free(match_data);
  return 0;
}

/** @brief Return error string for regular expression error code. */
size_t regerror(int errcode, const regex_t *preg, char *errbuf,
                size_t errbuf_size) {
  const char *msg = "Regex error";
  size_t msg_len;
  (void)preg;

  if (errcode == REG_NOMATCH)
    msg = "No match";
  else if (errcode == REG_BADPAT)
    msg = "Invalid regular expression";
  else if (errcode == REG_INVARG)
    msg = "Invalid argument";
  else if (errcode == REG_ESPACE)
    msg = "Out of memory";

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
void regfree(regex_t *preg) {
  if (preg != NULL && preg->re_guts != NULL) {
    pcre2_code_free((pcre2_code *)preg->re_guts);
    preg->re_guts = NULL;
  }
}

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_regex;
