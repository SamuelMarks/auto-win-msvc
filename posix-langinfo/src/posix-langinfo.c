/* posix-langinfo.c - Strict C89 Implementation */
/* clang-format off */
#include "langinfo.h"
/* clang-format on */

#if defined(POSIX_LANGINFO_MSVC) || defined(_WIN32)

char *posix_langinfo(nl_item item) {
  if (item == CODESET) {
    /* Valkey and Redis expect "UTF-8" typically on POSIX when CODESET is
     * requested */
    return "UTF-8";
  }
  return "";
}

#endif
