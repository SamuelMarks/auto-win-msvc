#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "netdb.h"
#include "posix-netdb.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_netdb_get_info(void) {
  enum posix_netdb_error_code rc;
  int info;

  info = 0;
  rc = posix_netdb_get_info(NULL);
  if (rc != POSIX_NETDB_ERROR_NULL_POINTER) {
    printf("Expected POSIX_NETDB_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_netdb_get_info(&info);
  if (rc != POSIX_NETDB_SUCCESS) {
    printf("posix_netdb_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_netdb_addrinfo(void) {
  struct addrinfo *ai;
  int res;
#if defined(_WIN32) || defined(_MSC_VER)
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

  ai = NULL;
  res = posix_getaddrinfo("localhost", NULL, NULL, NULL);
  ASSERT_EQ(-1, res);

  posix_freeaddrinfo(NULL);

  res = posix_getaddrinfo("127.0.0.1", NULL, NULL, &ai);
  ASSERT_EQ(0, res);
  ASSERT(ai != NULL);

  posix_freeaddrinfo(ai);

  /* Macro test */
  ai = NULL;
  res = getaddrinfo("127.0.0.1", NULL, NULL, &ai);
  ASSERT_EQ(0, res);
  ASSERT(ai != NULL);
  freeaddrinfo(ai);

#if defined(_WIN32) || defined(_MSC_VER)
  WSACleanup();
#endif
  PASS();
}

TEST test_posix_netdb_gai_strerror(void) {
  const char *err;

  ASSERT_EQ(11, EAI_SYSTEM);

  err = posix_gai_strerror(EAI_SYSTEM);
  ASSERT_STR_EQ("System error", err);

  err = posix_gai_strerror(0);
  ASSERT(err != NULL);

  err = gai_strerror(EAI_SYSTEM);
  ASSERT_STR_EQ("System error", err);

  PASS();
}

SUITE(suite_posix_netdb_core) {
  RUN_TEST(test_posix_netdb_get_info);
  RUN_TEST(test_posix_netdb_addrinfo);
  RUN_TEST(test_posix_netdb_gai_strerror);
}
