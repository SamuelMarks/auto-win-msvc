/* clang-format off */
#include "greatest.h"
#include "netdb.h"
#include "posix-netdb.h"
#include "resolv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_WIN32)
extern void *(*g_posix_netdb_calloc)(size_t, size_t);
extern void posix_netdb_reset_calloc_hook(void);

static int g_mock_netdb_oom = 0;

static void *mock_netdb_calloc(size_t nmemb, size_t size) {
  if (g_mock_netdb_oom) {
    return NULL;
  }
  return calloc(nmemb, size);
}
#endif

TEST test_posix_netdb_get_info(void) {
  int info = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_netdb_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_netdb_get_info(&info));
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_netdb_addrinfo(void) {
  struct addrinfo *ai = NULL;
  struct addrinfo hints;
  int res;
#if defined(_WIN32) || defined(_MSC_VER)
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

  ASSERT_EQ(-1, posix_getaddrinfo("localhost", NULL, NULL, NULL));
  posix_freeaddrinfo(NULL);

  /* Valid numeric IPv4 with NULL servname and NULL hints */
  ai = NULL;
  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", NULL, NULL, &ai));
  ASSERT(ai != NULL);
  posix_freeaddrinfo(ai);

  /* Valid numeric IPv4 with numeric servname & explicit hints.ai_socktype */
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  ai = NULL;
  ASSERT_EQ(0, posix_getaddrinfo("127.0.0.1", "8080", &hints, &ai));
  ASSERT(ai != NULL);
  posix_freeaddrinfo(ai);

  /* Valid numeric IPv4 with hints.ai_socktype == 0 and non-digit servnames */
  memset(&hints, 0, sizeof(hints));
  ai = NULL;
  if (posix_getaddrinfo("127.0.0.1", "http", &hints, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("127.0.0.1", "", &hints, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }

  /* Exercise non-IPv4 and malformed IPv4 inputs + non-custom freeaddrinfo */
  ai = NULL;
  if (posix_getaddrinfo(NULL, "80", &hints, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("/invalid", NULL, NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("localhost", "80", NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("999.1.1.1", NULL, NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("1..1.1", NULL, NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("1.2.3.4.5", NULL, NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("127.0.0.1a", NULL, NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("127.0.0./", NULL, NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("1.2.3.", NULL, NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  ai = NULL;
  if (posix_getaddrinfo("127.1", NULL, NULL, &ai) == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }

#if defined(_WIN32)
  /* OOM in custom IPv4 calloc falls back to system getaddrinfo */
  g_posix_netdb_calloc = mock_netdb_calloc;
  g_mock_netdb_oom = 1;
  ai = NULL;
  res = posix_getaddrinfo("127.0.0.1", NULL, NULL, &ai);
  if (res == 0 && ai != NULL) {
    posix_freeaddrinfo(ai);
  }
  g_mock_netdb_oom = 0;
  posix_netdb_reset_calloc_hook();
#else
  (void)res;
#endif

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
