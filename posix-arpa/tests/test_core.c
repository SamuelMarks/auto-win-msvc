#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-arpa-inet.h"
/* clang-format on */

TEST test_arpa(void) {
  ASSERT_NEQ((unsigned long)-1, (unsigned long)inet_addr("127.0.0.1"));
  PASS();
}

TEST test_posix_inet_aton(void) {
  SKIP(); /* Generated stub for posix_inet_aton */
}

SUITE(suite_posix_arpa_core) {
  RUN_TEST(test_arpa);
  RUN_TEST(test_posix_inet_aton);
}
