#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/* posix-regex.c - Strict C89 Implementation */

/* clang-format off */
#include "posix-regex.h"
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if (defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)) &&     \
    !defined(__I86__) && !defined(__WATCOMC__) && !defined(__MSDOS__) && !defined(__DOS__)
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
auto_win_msvc_error_t posix_regex_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

#if defined(__I86__) || defined(__WATCOMC__) || defined(__MSDOS__) ||          \
    defined(__DOS__)

typedef struct {
  char *pattern;
  int cflags;
} dos16_regex_t;

static int dos16_char_eq(char a, char b, int icase) {
  if (icase) {
    return tolower((unsigned char)a) == tolower((unsigned char)b);
  }
  return a == b;
}

static int dos16_in_class(const char *cls, size_t cls_len, char ch, int icase) {
  size_t i = 0;
  int neg = 0;
  int matched = 0;

  if (cls_len > 0 && cls[0] == '^') {
    neg = 1;
    i = 1;
  }
  while (i < cls_len) {
    if (i + 2 < cls_len && cls[i + 1] == '-') {
      unsigned char lo = (unsigned char)cls[i];
      unsigned char hi = (unsigned char)cls[i + 2];
      unsigned char c = (unsigned char)ch;
      if (icase) {
        lo = (unsigned char)tolower(lo);
        hi = (unsigned char)tolower(hi);
        c = (unsigned char)tolower(c);
      }
      if (c >= lo && c <= hi) {
        matched = 1;
      }
      i += 3;
    } else {
      if (dos16_char_eq(cls[i], ch, icase)) {
        matched = 1;
      }
      i++;
    }
  }
  return neg ? !matched : matched;
}

static const char *dos16_match_here(const char *pat, const char *str,
                                    int cflags);

static int dos16_atom_matches(const char *pat, size_t atom_len, char ch,
                              int cflags) {
  if (ch == '\0') {
    return 0;
  }
  if (atom_len == 1 && pat[0] == '.') {
    return (cflags & REG_NEWLINE) ? (ch != '\n') : 1;
  }
  if (atom_len >= 2 && pat[0] == '\\') {
    return dos16_char_eq(pat[1], ch, cflags & REG_ICASE);
  }
  if (atom_len >= 2 && pat[0] == '[' && pat[atom_len - 1] == ']') {
    return dos16_in_class(pat + 1, atom_len - 2, ch, cflags & REG_ICASE);
  }
  return dos16_char_eq(pat[0], ch, cflags & REG_ICASE);
}

static const char *dos16_match_here(const char *pat, const char *str,
                                    int cflags) {
  size_t atom_len = 1;
  char quant = '\0';
  const char *rest;

  if (pat[0] == '\0') {
    return str;
  }
  if (pat[0] == '$' && pat[1] == '\0') {
    return (*str == '\0') ? str : NULL;
  }
  if (pat[0] == '\\' && pat[1] != '\0') {
    atom_len = 2;
  } else if (pat[0] == '[') {
    const char *close_b = strchr(pat + 1, ']');
    if (close_b == NULL) {
      return NULL;
    }
    atom_len = (size_t)(close_b - pat + 1);
  }

  if (pat[atom_len] == '*' || pat[atom_len] == '+' || pat[atom_len] == '?') {
    quant = pat[atom_len];
    rest = pat + atom_len + 1;
  } else {
    rest = pat + atom_len;
  }

  if (quant == '*') {
    const char *s = str;
    while (dos16_atom_matches(pat, atom_len, *s, cflags)) {
      s++;
    }
    while (s >= str) {
      const char *m = dos16_match_here(rest, s, cflags);
      if (m != NULL) {
        return m;
      }
      if (s == str) {
        break;
      }
      s--;
    }
    return NULL;
  }
  if (quant == '+') {
    const char *s = str;
    if (!dos16_atom_matches(pat, atom_len, *s, cflags)) {
      return NULL;
    }
    while (dos16_atom_matches(pat, atom_len, *s, cflags)) {
      s++;
    }
    while (s > str) {
      const char *m = dos16_match_here(rest, s, cflags);
      if (m != NULL) {
        return m;
      }
      s--;
    }
    return NULL;
  }
  if (quant == '?') {
    if (dos16_atom_matches(pat, atom_len, *str, cflags)) {
      const char *m = dos16_match_here(rest, str + 1, cflags);
      if (m != NULL) {
        return m;
      }
    }
    return dos16_match_here(rest, str, cflags);
  }

  if (dos16_atom_matches(pat, atom_len, *str, cflags)) {
    return dos16_match_here(rest, str + 1, cflags);
  }
  return NULL;
}

/** @brief Compile regular expression pattern. */
int regcomp(regex_t *preg, const char *pattern, int cflags) {
  dos16_regex_t *re;
  size_t len;
  const char *p;
  int open_brackets = 0;
  int open_parens = 0;

  if (preg == NULL || pattern == NULL) {
    return REG_INVARG;
  }
  for (p = pattern; *p != '\0'; p++) {
    if (*p == '\\') {
      if (p[1] == '\0') {
        return REG_EESCAPE;
      }
      p++;
    } else if (*p == '[') {
      open_brackets++;
    } else if (*p == ']') {
      if (open_brackets > 0) {
        open_brackets--;
      }
    } else if (*p == '(') {
      open_parens++;
    } else if (*p == ')') {
      if (open_parens == 0) {
        return REG_EPAREN;
      }
      open_parens--;
    }
  }
  if (open_brackets != 0 || open_parens != 0) {
    return REG_BADPAT;
  }

  re = (dos16_regex_t *)malloc(sizeof(dos16_regex_t));
  if (re == NULL) {
    return REG_ESPACE;
  }
  len = strlen(pattern);
  re->pattern = (char *)malloc(len + 1);
  if (re->pattern == NULL) {
    free(re);
    return REG_ESPACE;
  }
  memcpy(re->pattern, pattern, len + 1);
  re->cflags = cflags;
  preg->re_magic = 0xD016;
  preg->re_nsub = 0;
  preg->re_endp = pattern + len;
  preg->re_guts = (void *)re;
  return 0;
}

/** @brief Match compiled regular expression against a string. */
int regexec(const regex_t *preg, const char *string, size_t nmatch,
            regmatch_t pmatch[], int eflags) {
  const dos16_regex_t *re;
  const char *pat;
  const char *s;
  const char *end_ptr = NULL;
  const char *start_ptr = NULL;
  size_t i;

  (void)eflags;
  if (preg == NULL || string == NULL || preg->re_guts == NULL) {
    return REG_INVARG;
  }
  re = (const dos16_regex_t *)preg->re_guts;
  pat = re->pattern;
  if (pat[0] == '^') {
    end_ptr = dos16_match_here(pat + 1, string, re->cflags);
    if (end_ptr != NULL) {
      start_ptr = string;
    }
  } else {
    s = string;
    do {
      end_ptr = dos16_match_here(pat, s, re->cflags);
      if (end_ptr != NULL) {
        start_ptr = s;
        break;
      }
    } while (*s++ != '\0');
  }

  if (end_ptr == NULL) {
    return REG_NOMATCH;
  }
  if (pmatch != NULL && nmatch > 0) {
    pmatch[0].rm_so = (regoff_t)(start_ptr - string);
    pmatch[0].rm_eo = (regoff_t)(end_ptr - string);
    for (i = 1; i < nmatch; i++) {
      pmatch[i].rm_so = -1;
      pmatch[i].rm_eo = -1;
    }
  }
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
    strncpy(errbuf, msg, errbuf_size - 1);
    errbuf[errbuf_size - 1] = '\0';
  }
  return msg_len;
}

/** @brief Free resources associated with compiled regular expression. */
void regfree(regex_t *preg) {
  if (preg != NULL && preg->re_guts != NULL) {
    dos16_regex_t *re = (dos16_regex_t *)preg->re_guts;
    if (re->pattern != NULL) {
      free(re->pattern);
    }
    free(re);
    preg->re_guts = NULL;
  }
}

#elif defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) ||     \
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
  if (preg != NULL) {
    /* preg provided */
  }

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

#endif
typedef int make_iso_compilers_happy_mega_batch3_12;
