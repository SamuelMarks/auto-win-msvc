#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-arpa-inet.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_arpa_init(void) {
  auto_win_msvc_error_t rc;

  rc = posix_arpa_inet_parse_ipv4(NULL, NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  PASS();
}

TEST test_inet_parse_ipv4(void) {
  struct in_addr addr;
  auto_win_msvc_error_t rc;

  rc = posix_arpa_inet_parse_ipv4("127.0.0.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  rc = posix_arpa_inet_parse_ipv4("0x7f.0.0.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  rc = posix_arpa_inet_parse_ipv4("0177.0.0.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  rc = posix_arpa_inet_parse_ipv4("0xffffffff", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  rc = posix_arpa_inet_parse_ipv4("0X7F.0X00.0X00.0X01", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  rc = posix_arpa_inet_parse_ipv4("127.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  rc = posix_arpa_inet_parse_ipv4("127.0.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  /* Overflows and invalid */
  rc = posix_arpa_inet_parse_ipv4("256.0.0.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  /* test 2 part branch failure limit */
  rc = posix_arpa_inet_parse_ipv4("256.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("127.0x1000000", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("127.0.0.0x100", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("127.0.0.256", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("127.0x10000.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("127.0.0.1.5", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("127.0.0.1.5.", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("foo.bar", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("127.0.0.1foo", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_arpa_inet_parse_ipv4("9999999999999999999999999999999", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  PASS();
}

TEST test_inet_parse_ipv4_more(void) {
  struct in_addr addr;
  auto_win_msvc_error_t rc;

  /* null check */
  rc = posix_arpa_inet_parse_ipv4(NULL, &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  rc = posix_arpa_inet_parse_ipv4("127.0.0.1", NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  /* hex parsing with non-hex alpha */
  rc = posix_arpa_inet_parse_ipv4("0x1z", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  rc = posix_arpa_inet_parse_ipv4("0x1Z", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  /* digit_val >= base */
  rc = posix_arpa_inet_parse_ipv4("019", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  /* 2 parts (a.b) limits */
  rc = posix_arpa_inet_parse_ipv4("256.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  rc = posix_arpa_inet_parse_ipv4("127.16777216", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  /* 3 parts (a.b.c) limits */
  rc = posix_arpa_inet_parse_ipv4("256.0.0", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  rc = posix_arpa_inet_parse_ipv4("127.256.0", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  rc = posix_arpa_inet_parse_ipv4("127.0.65536", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  /* 4 parts (a.b.c.d) limits */
  rc = posix_arpa_inet_parse_ipv4("127.256.0.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);
  rc = posix_arpa_inet_parse_ipv4("127.0.256.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  /* overflow value check */
  rc = posix_arpa_inet_parse_ipv4("4294967296", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  /* 1 part success */
  rc = posix_arpa_inet_parse_ipv4("2130706433", &addr); /* 127.0.0.1 */
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  /* 2 parts success */
  rc = posix_arpa_inet_parse_ipv4("127.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  /* 3 parts success */
  rc = posix_arpa_inet_parse_ipv4("127.0.1", &addr);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  PASS();
}

TEST test_inet_addr(void) {
  ASSERT_EQ(0x0100007f, posix_inet_addr("127.0.0.1"));
  ASSERT_EQ(INADDR_NONE, posix_inet_addr("256.256.256.256"));
  PASS();
}

TEST test_inet_aton(void) {
  struct in_addr addr;
  ASSERT_EQ(1, posix_inet_aton("127.0.0.1", &addr));
  ASSERT_EQ(0x0100007f, addr.s_addr);

  ASSERT_EQ(0, posix_inet_aton("invalid", &addr));
  PASS();
}

SUITE(suite_posix_arpa_core) {
  RUN_TEST(test_posix_arpa_init);
  RUN_TEST(test_inet_parse_ipv4);
  RUN_TEST(test_inet_parse_ipv4_more);
  RUN_TEST(test_inet_addr);
  RUN_TEST(test_inet_aton);
}
