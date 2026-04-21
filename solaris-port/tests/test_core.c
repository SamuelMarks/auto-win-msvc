#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"

TEST test_stub(void) { SKIP(); }

TEST test_port_create(void) { SKIP(); /* Generated stub for port_create */ }

TEST test_port_associate(void) {
  SKIP(); /* Generated stub for port_associate */
}

TEST test_port_getn(void) { SKIP(); /* Generated stub for port_getn */ }

SUITE(suite_solaris_port_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_port_create);
  RUN_TEST(test_port_associate);
  RUN_TEST(test_port_getn);
}
