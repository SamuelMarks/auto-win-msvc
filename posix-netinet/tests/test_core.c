#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "netinet/in.h"
#include "netinet/ip.h"
#include "netinet/tcp.h"
#include "posix-netinet-in.h"
#include "posix-netinet-tcp.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_netinet_get_info(void) {
  enum posix_netinet_error_code rc;
  int info;

  info = 0;
  rc = posix_netinet_get_info(NULL);
  if (rc != POSIX_NETINET_ERROR_NULL_POINTER) {
    printf("Expected POSIX_NETINET_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_netinet_get_info(&info);
  if (rc != POSIX_NETINET_SUCCESS) {
    printf("posix_netinet_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_netinet_is_ipv4_loopback(void) {
  enum posix_netinet_error_code rc;
  int is_loopback;

  is_loopback = -1;
  rc = posix_netinet_is_ipv4_loopback(0x7F000001UL, NULL);
  if (rc != POSIX_NETINET_ERROR_NULL_POINTER) {
    printf("Expected POSIX_NETINET_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_netinet_is_ipv4_loopback(0x7F000001UL, &is_loopback);
  if (rc != POSIX_NETINET_SUCCESS) {
    printf("posix_netinet_is_ipv4_loopback failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, is_loopback);

  rc = posix_netinet_is_ipv4_loopback(0xC0A80101UL, &is_loopback);
  if (rc != POSIX_NETINET_SUCCESS) {
    printf("posix_netinet_is_ipv4_loopback failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(0, is_loopback);

  PASS();
}

TEST test_posix_netinet_constants(void) {
  ASSERT_EQ(0, IPPROTO_IP);
  ASSERT_EQ(6, IPPROTO_TCP);
  ASSERT_EQ(17, IPPROTO_UDP);
  ASSERT_EQ(0UL, INADDR_ANY);
  ASSERT_EQ(0x7F000001UL, INADDR_LOOPBACK);
  ASSERT_EQ(1, TCP_NODELAY);
  ASSERT_EQ(4, IPVERSION);
  ASSERT_EQ(0x10, IPTOS_LOWDELAY);
  PASS();
}

SUITE(suite_posix_netinet_core) {
  RUN_TEST(test_posix_netinet_get_info);
  RUN_TEST(test_posix_netinet_is_ipv4_loopback);
  RUN_TEST(test_posix_netinet_constants);
}
