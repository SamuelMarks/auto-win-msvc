#include "greatest.h"
#include "posix-sys-ioctl.h"

#if defined(_MSC_VER) || defined(_WIN32)

TEST test_sys_ioctl(void) {
  /* Test with an invalid socket */
  int val = 1;
  int res = posix_ioctl(999999, FIONBIO, &val);
  ASSERT_EQ_FMT(-1, res, "%d");
  /* It should fail with ENOTTY on invalid fd, but winsock might just return
   * WSAENOTSOCK */
  PASS();
}

SUITE(sys_ioctl_suite) { RUN_TEST(test_sys_ioctl); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  WSADATA wsaData;
  GREATEST_MAIN_BEGIN();
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  RUN_SUITE(sys_ioctl_suite);
  WSACleanup();
  GREATEST_MAIN_END();
}
#else
int main(void) { return 0; }
#endif
