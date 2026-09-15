#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-arpa-inet.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_arpa_null_arguments(void) {
  enum posix_arpa_inet_error_code rc;
  struct in_addr addr;

  rc = posix_arpa_inet_parse_ipv4(NULL, &addr);
  if (rc != POSIX_ARPA_INET_ERROR_NULL_POINTER) {
    printf("Expected POSIX_ARPA_INET_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_arpa_inet_parse_ipv4("127.0.0.1", NULL);
  if (rc != POSIX_ARPA_INET_ERROR_NULL_POINTER) {
    printf("Expected POSIX_ARPA_INET_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  PASS();
}

TEST test_posix_arpa_invalid_formats(void) {
  enum posix_arpa_inet_error_code rc;
  struct in_addr addr;

  /* Empty string */
  rc = posix_arpa_inet_parse_ipv4("", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* Hex prefix without digits */
  rc = posix_arpa_inet_parse_ipv4("0x", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* Trailing dot */
  rc = posix_arpa_inet_parse_ipv4("127.0.0.", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* Too many parts */
  rc = posix_arpa_inet_parse_ipv4("1.2.3.4.5", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* Trailing characters */
  rc = posix_arpa_inet_parse_ipv4("127.0.0.1 extra", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* Invalid octal digit */
  rc = posix_arpa_inet_parse_ipv4("08.0.0.1", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* Hex digits out of range */
  rc = posix_arpa_inet_parse_ipv4("0x7g", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\\n",
           (int)rc);
    FAIL();
  }

  rc = posix_arpa_inet_parse_ipv4("0X7G", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\\n",
           (int)rc);
    FAIL();
  }

  /* 4 parts overflow */
  rc = posix_arpa_inet_parse_ipv4("256.0.0.1", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }
  rc = posix_arpa_inet_parse_ipv4("1.256.0.1", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }
  rc = posix_arpa_inet_parse_ipv4("1.0.256.1", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }
  rc = posix_arpa_inet_parse_ipv4("1.0.0.256", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* 3 parts overflow */
  rc = posix_arpa_inet_parse_ipv4("256.0.1", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }
  rc = posix_arpa_inet_parse_ipv4("1.256.1", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }
  rc = posix_arpa_inet_parse_ipv4("1.0.65536", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* 1 part overflow */
  rc = posix_arpa_inet_parse_ipv4("4294967296", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  /* 2 parts overflow */
  rc = posix_arpa_inet_parse_ipv4("256.1", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }
  rc = posix_arpa_inet_parse_ipv4("1.16777216", &addr);
  if (rc != POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT) {
    printf("Expected POSIX_ARPA_INET_ERROR_INVALID_ARGUMENT, got %d\n",
           (int)rc);
    FAIL();
  }

  PASS();
}

TEST test_posix_arpa_valid_formats(void) {
  enum posix_arpa_inet_error_code rc;
  struct in_addr addr;
  struct in_addr ref_addr;

  /* 4 parts standard */
  rc = posix_arpa_inet_parse_ipv4("127.0.0.1", &ref_addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\n", (int)rc);
    FAIL();
  }

  /* 4 parts hex lowercase */
  rc = posix_arpa_inet_parse_ipv4("0x7f.0x0.0x0.0x1", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(ref_addr.s_addr, addr.s_addr);

  /* Hex string containing all lowercase hex letters */
  rc = posix_arpa_inet_parse_ipv4("0x0a.0x0b.0x0c.0x0d", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\\n", (int)rc);
    FAIL();
  }

  /* Hex string containing all uppercase hex letters */
  rc = posix_arpa_inet_parse_ipv4("0X0A.0X0B.0X0C.0X0D", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\\n", (int)rc);
    FAIL();
  }

  /* 4 parts hex uppercase */
  rc = posix_arpa_inet_parse_ipv4("0X7F.0X0.0X0.0X1", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(ref_addr.s_addr, addr.s_addr);

  /* 4 parts octal */
  rc = posix_arpa_inet_parse_ipv4("0177.0.0.01", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(ref_addr.s_addr, addr.s_addr);

  /* 1 part decimal */
  rc = posix_arpa_inet_parse_ipv4("2130706433", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(ref_addr.s_addr, addr.s_addr);

  /* 1 part hex */
  rc = posix_arpa_inet_parse_ipv4("0x7f000001", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(ref_addr.s_addr, addr.s_addr);

  /* 3 parts */
  rc = posix_arpa_inet_parse_ipv4("127.0.1", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(ref_addr.s_addr, addr.s_addr);

  /* 2 parts */
  rc = posix_arpa_inet_parse_ipv4("127.1", &addr);
  if (rc != POSIX_ARPA_INET_SUCCESS) {
    printf("posix_arpa_inet_parse_ipv4 failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(ref_addr.s_addr, addr.s_addr);

  PASS();
}

TEST test_posix_inet_aton(void) {
  struct in_addr addr;
  int ret;

  ret = posix_inet_aton("127.0.0.1", &addr);
  ASSERT_EQ(1, ret);

  ret = posix_inet_aton(NULL, &addr);
  ASSERT_EQ(0, ret);

  ret = posix_inet_aton("invalid.ip", &addr);
  ASSERT_EQ(0, ret);

  PASS();
}

TEST test_posix_inet_addr(void) {
  unsigned long addr;

  addr = posix_inet_addr("127.0.0.1");
  ASSERT_NEQ((unsigned long)INADDR_NONE, addr);

  addr = posix_inet_addr("invalid.ip");
  ASSERT_EQ((unsigned long)INADDR_NONE, addr);

  PASS();
}

SUITE(suite_posix_arpa_core) {
  RUN_TEST(test_posix_arpa_null_arguments);
  RUN_TEST(test_posix_arpa_invalid_formats);
  RUN_TEST(test_posix_arpa_valid_formats);
  RUN_TEST(test_posix_inet_aton);
  RUN_TEST(test_posix_inet_addr);
}
