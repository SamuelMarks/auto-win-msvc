#include "greatest.h"
#include "posix-arpa-inet.h"

#if defined(_MSC_VER) || defined(_WIN32)

TEST test_posix_inet_aton(void) {
  struct in_addr addr;
  int res = posix_inet_aton("127.0.0.1", &addr);
  ASSERT_EQ_FMT(1, res, "%d");
  /* 127 = 0x7F, 0 = 0x00, 0 = 0x00, 1 = 0x01. Network byte order. */
  ASSERT_EQ_FMT(16777343, (int)addr.s_addr, "%d"); /* 0x0100007F */
  PASS();
}

SUITE(arpa_inet_suite) { RUN_TEST(test_posix_inet_aton); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  WSADATA wsaData;
  GREATEST_MAIN_BEGIN();
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  RUN_SUITE(arpa_inet_suite);
  WSACleanup();
  GREATEST_MAIN_END();
}
#else
int main(void) { return 0; }
#endif
