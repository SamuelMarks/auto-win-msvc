#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
/**
 * @file uname_main.c
 * @brief Command-line utility emulating uname.
 */

/* clang-format off */
#include "posix-utsname.h"
#include <stdio.h>
/* clang-format on */

/**
 * @brief Entry point for uname utility.
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments.
 * @return 0 on success, non-zero on failure.
 */
int main(int argc, char **argv) {
  struct utsname u;
  (void)argc;
  (void)argv;

  if (uname(&u) == 0) {
    printf("%s\n", u.sysname);
  } else {
    printf("Windows\n");
  }
  return 0;
}

#endif
typedef int make_iso_compilers_happy_uname_main;
