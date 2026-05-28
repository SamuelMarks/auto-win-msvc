#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-netdb.h"
#include <stddef.h>
/* clang-format on */

TEST test_netdb(void) {
  struct addrinfo *ai;
  if (getaddrinfo("localhost", NULL, NULL, &ai) == 0) {
    freeaddrinfo(ai);
    PASS();
  }
  SKIP();
}

TEST test_posix_getaddrinfo(void) {
  SKIP(); /* Generated stub for posix_getaddrinfo */
}

TEST test_posix_freeaddrinfo(void) {
  SKIP(); /* Generated stub for posix_freeaddrinfo */
}

TEST test_posix_gai_strerror(void) {
  SKIP(); /* Generated stub for posix_gai_strerror */
}

SUITE(suite_posix_netdb_core) {
  RUN_TEST(test_netdb);
  RUN_TEST(test_posix_getaddrinfo);
  RUN_TEST(test_posix_freeaddrinfo);
  RUN_TEST(test_posix_gai_strerror);
}
