#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "netdb.h"
#include "posix-netdb.h"
#include "resolv.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_netdb_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_netdb_get_info(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_netdb_get_info(&info);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
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

#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(11, EAI_SYSTEM);

  err = posix_gai_strerror(EAI_SYSTEM);
  ASSERT_STR_EQ("System error", err);
#endif

  err = posix_gai_strerror(0);
  ASSERT(err != NULL);

  err = gai_strerror(EAI_SYSTEM);
  ASSERT_STR_EQ("System error", err);

  ASSERT_STR_EQ("Resolver Error 0 (no error)", posix_hstrerror(0));
  ASSERT_STR_EQ("Unknown host", posix_hstrerror(1));
  ASSERT_STR_EQ("Host name lookup failure", posix_hstrerror(2));
  ASSERT_STR_EQ("Unknown server error", posix_hstrerror(3));
  ASSERT_STR_EQ("No address associated with name", posix_hstrerror(4));
  ASSERT_STR_EQ("Unknown resolver error", posix_hstrerror(999));

  posix_herror(NULL);
  posix_herror("");
  posix_herror("test_herror");

#if defined(_WIN32)
  ASSERT_EQ(0, res_init());
  _res.options = 1UL;
  ASSERT_EQ(1UL, _res.options);
#endif

  PASS();
}

SUITE(suite_posix_netdb_core) {
  RUN_TEST(test_posix_netdb_get_info);
  RUN_TEST(test_posix_netdb_addrinfo);
  RUN_TEST(test_posix_netdb_gai_strerror);
}
