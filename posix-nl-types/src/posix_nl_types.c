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

nl_catd catopen(const char *name, int oflag) {
  (void)name;
  (void)oflag;
  return (nl_catd)0;
}

char *catgets(nl_catd catd, int set_id, int msg_id, const char *s) {
  (void)catd;
  (void)set_id;
  (void)msg_id;
  return (char *)s;
}

int catclose(nl_catd catd) {
  (void)catd;
  return 0;
}

#endif

int posix_nl_types_dummy_for_coverage(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_nl_types;

#endif
typedef int make_iso_compilers_happy_mega_posix_nl_types;
