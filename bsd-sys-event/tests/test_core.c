#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

TEST test_stub(void) { SKIP(); }

TEST test_kqueue(void) { SKIP(); /* Generated stub for kqueue */ }

TEST test_kevent(void) { SKIP(); /* Generated stub for kevent */ }

SUITE(suite_bsd_sys_event_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_kqueue);
  RUN_TEST(test_kevent);
}
