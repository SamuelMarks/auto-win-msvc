#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-netinet-in.h"

TEST test_netinet(void) {
    SKIP();
}

SUITE(suite_posix_netinet_core) {
    RUN_TEST(test_netinet);
}
