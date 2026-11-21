#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file posix_nl_types.c
 * @brief Implementation of POSIX <nl_types.h> message catalog functions.
 */

/* clang-format off */
#include <nl_types.h>
/* clang-format on */

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__)

/**
 * @brief Opens a message catalog (stub returning 0 handle on Windows/DOS).
 * @param name Catalog path or name.
 * @param oflag Catalog flags.
 * @return Catalog descriptor.
 */
nl_catd catopen(const char *name, int oflag) {
  (void)name;
  (void)oflag;
  return (nl_catd)0;
}

/**
 * @brief Reads a message from a catalog, returning default fallback string.
 * @param catd Catalog descriptor.
 * @param set_id Message set identifier.
 * @param msg_id Message identifier.
 * @param s Fallback default string.
 * @return Pointer to fallback string s.
 */
char *catgets(nl_catd catd, int set_id, int msg_id, const char *s) {
  char *res = NULL;
  (void)catd;
  (void)set_id;
  (void)msg_id;
  if (s != NULL) {
    union {
      const char *in;
      char *out;
    } u;
    u.in = s;
    res = u.out;
  }
  return res;
}

/**
 * @brief Closes a message catalog descriptor.
 * @param catd Catalog descriptor.
 * @return Always 0.
 */
int catclose(nl_catd catd) {
  (void)catd;
  return 0;
}

#endif

/**
 * @brief Compilation unit anchor symbol for posix-nl-types coverage.
 * @return Always 0.
 */
int posix_nl_types_dummy_for_coverage(void);
int posix_nl_types_dummy_for_coverage(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_nl_types;

#endif
typedef int make_iso_compilers_happy_mega_posix_nl_types;
