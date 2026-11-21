#ifndef POSIX_NL_TYPES_H

/**
 * @file nl_types.h
 * @brief POSIX NLS message catalog (<nl_types.h>) passthrough and polyfill.
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <nl_types.h>
#define AWM_HAS_HOST_NL_TYPES 1
#else
#include <stddef.h>
#endif
/* clang-format on */

#ifndef POSIX_NL_TYPES_H
#define POSIX_NL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(AWM_HAS_HOST_NL_TYPES)

#define NL_SETD 1
#define NL_CAT_LOCALE 1

#ifndef _NL_ITEM_DECLARED
#define _NL_ITEM_DECLARED
typedef int nl_item;
#endif

typedef void *nl_catd;

/** @brief Open a message catalog. */
nl_catd catopen(const char *name, int oflag);
/** @brief Read a program message from a message catalog (returns s fallback).
 */
char *catgets(nl_catd catd, int set_id, int msg_id, const char *s);
/** @brief Close a message catalog. */
int catclose(nl_catd catd);

#elif defined(__linux__) && !defined(__GLIBC__) &&                             \
    (defined(__GNUC__) || defined(__clang__))

/**
 * @brief Safe wrapper around musl catgets handling invalid descriptors per
 * POSIX.
 * @param catd Catalog descriptor.
 * @param set_id Message set identifier.
 * @param msg_id Message identifier.
 * @param s Fallback default string.
 * @return Catalog message or fallback string s.
 */
static __inline__ char *awm_musl_catgets(nl_catd catd, int set_id, int msg_id,
                                         const char *s) {
  if (catd == (nl_catd)-1 || catd == (nl_catd)0) {
    union {
      const char *in;
      char *out;
    } u;
    u.in = s;
    return u.out;
  }
  return (catgets)(catd, set_id, msg_id, s);
}
#undef catgets
#define catgets awm_musl_catgets

#endif /* !AWM_HAS_HOST_NL_TYPES */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NL_TYPES_H */
#endif /* !POSIX_NL_TYPES_H */
