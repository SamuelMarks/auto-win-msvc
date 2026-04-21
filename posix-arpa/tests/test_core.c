#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-arpa-inet.h"

TEST test_arpa(void) {
  ASSERT_NEQ((unsigned long)-1, posix_inet_addr("127.0.0.1"));
  PASS();
}

TEST test_posix_inet_aton(void) {
  SKIP(); /* Generated stub for posix_inet_aton */
}

SUITE(suite_posix_arpa_core) {
  RUN_TEST(test_arpa);
  RUN_TEST(test_posix_inet_aton);
}
