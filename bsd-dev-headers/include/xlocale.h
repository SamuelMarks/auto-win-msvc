#ifndef BSD_XLOCALE_H
#define BSD_XLOCALE_H

/**
 * @file xlocale.h
 * @brief Extended locale (<xlocale.h>) compatibility header.
 */

/* clang-format off */
#if defined(__APPLE__) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <xlocale.h>
#else
#include <ctype.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__APPLE__) && (defined(__GNUC__) || defined(__clang__)))
#if defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__) ||               \
    defined(__WATCOMC__)
/** @brief Opaque extended locale handle. */
typedef void *locale_t;
#ifndef LC_ALL_MASK
/** @brief All locale categories bitmask. */
#define LC_ALL_MASK 0x7f
/** @brief Collation locale category bitmask. */
#define LC_COLLATE_MASK 0x01
/** @brief Character classification locale category bitmask. */
#define LC_CTYPE_MASK 0x02
/** @brief Monetary formatting locale category bitmask. */
#define LC_MONETARY_MASK 0x04
/** @brief Numeric formatting locale category bitmask. */
#define LC_NUMERIC_MASK 0x08
/** @brief Time formatting locale category bitmask. */
#define LC_TIME_MASK 0x10
/** @brief Messages locale category bitmask. */
#define LC_MESSAGES_MASK 0x20
#endif
#ifndef LC_GLOBAL_LOCALE
/** @brief Sentinel handle representing the process-wide global locale. */
#define LC_GLOBAL_LOCALE ((locale_t)(size_t)-1)
#endif

/** @brief Create or modify a locale object. */
static __inline locale_t newlocale(int mask, const char *loc, locale_t base) {
  (void)mask;
  (void)loc;
  return base ? base : (locale_t)(size_t)1;
}

/** @brief Duplicate an existing locale object. */
static __inline locale_t duplocale(locale_t loc) {
  return loc ? loc : (locale_t)(size_t)1;
}

/** @brief Set or query the current thread's locale. */
static __inline locale_t uselocale(locale_t loc) {
  return loc ? loc : (locale_t)(size_t)1;
}

/** @brief Free resources associated with a locale object. */
static __inline void freelocale(locale_t loc) { (void)loc; }

/** @brief Locale-aware isalnum. */
#define isalnum_l(c, l) isalnum((unsigned char)(c))
/** @brief Locale-aware isalpha. */
#define isalpha_l(c, l) isalpha((unsigned char)(c))
/** @brief Locale-aware isdigit. */
#define isdigit_l(c, l) isdigit((unsigned char)(c))
/** @brief Locale-aware isxdigit. */
#define isxdigit_l(c, l) isxdigit((unsigned char)(c))
/** @brief Locale-aware isspace. */
#define isspace_l(c, l) isspace((unsigned char)(c))
/** @brief Locale-aware isupper. */
#define isupper_l(c, l) isupper((unsigned char)(c))
/** @brief Locale-aware islower. */
#define islower_l(c, l) islower((unsigned char)(c))
/** @brief Locale-aware tolower. */
#define tolower_l(c, l) tolower((unsigned char)(c))
/** @brief Locale-aware toupper. */
#define toupper_l(c, l) toupper((unsigned char)(c))
/** @brief Locale-aware strtol. */
#define strtol_l(s, e, b, l) strtol((s), (e), (b))
/** @brief Locale-aware strtoul. */
#define strtoul_l(s, e, b, l) strtoul((s), (e), (b))
#if defined(_MSC_VER)
/** @brief Locale-aware strtoll. */
#define strtoll_l(s, e, b, l) _strtoi64((s), (e), (b))
/** @brief Locale-aware strtoull. */
#define strtoull_l(s, e, b, l) _strtoui64((s), (e), (b))
#else
/** @brief Locale-aware strtoll. */
#define strtoll_l(s, e, b, l) strtoll((s), (e), (b))
/** @brief Locale-aware strtoull. */
#define strtoull_l(s, e, b, l) strtoull((s), (e), (b))
#endif
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_XLOCALE_H */
