#include "greatest.h"
#include "posix-netinet-in.h"
#include "posix-netinet-tcp.h"

#if defined(_MSC_VER) || defined(_WIN32)

TEST test_netinet_constants(void) {
    /* Test that common constants compile and exist */
    int proto_tcp = IPPROTO_TCP;
    int addr_any = INADDR_ANY;
    int nodelay = TCP_NODELAY;
    
    ASSERT_EQ_FMT(6, proto_tcp, "%d"); /* IPPROTO_TCP is 6 */
    ASSERT_EQ_FMT(0, addr_any, "%d"); /* INADDR_ANY is 0 */
    ASSERT_EQ_FMT(1, nodelay, "%d"); /* TCP_NODELAY is usually 1, but we just check compilation */
    PASS();
}

SUITE(netinet_suite) {
    RUN_TEST(test_netinet_constants);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(netinet_suite);
    GREATEST_MAIN_END();
}
#else
int main(void) { return 0; }
#endif
