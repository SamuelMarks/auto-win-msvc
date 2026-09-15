#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "poll.h"
#include "posix-poll.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_poll_get_info(void) {
  enum posix_poll_error_code rc;
  int info;

  info = 0;
  rc = posix_poll_get_info(NULL);
  if (rc != POSIX_POLL_ERROR_NULL_POINTER) {
    printf("Expected POSIX_POLL_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_poll_get_info(&info);
  if (rc != POSIX_POLL_SUCCESS) {
    printf("posix_poll_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_posix_poll_args(void) {
  struct pollfd pfd;
  int res;
#if defined(_WIN32) || defined(_MSC_VER)
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

  /* NULL fds with nfds > 0 returns -1 */
  res = posix_poll(NULL, 1, 0);
  ASSERT_EQ(-1, res);

  /* NULL fds with nfds == 0 returns 0 */
  res = posix_poll(NULL, 0, 0);
  ASSERT_EQ(0, res);

  /* fds with invalid fd (-1) */
#if defined(_WIN32) || defined(_MSC_VER)
  pfd.fd = (SOCKET)INVALID_SOCKET;
#else
  pfd.fd = -1;
#endif
  pfd.events = POLLIN | POLLOUT | POLLPRI;
  pfd.revents = 0;
  res = posix_poll(&pfd, 1, 10);
  ASSERT(res >= 0);

  /* Test macro */
  res = poll(NULL, 0, 0);
  ASSERT_EQ(0, res);

#if defined(_WIN32) || defined(_MSC_VER)
  WSACleanup();
#endif
  PASS();
}

TEST test_posix_poll_constants(void) {
  ASSERT_NEQ(0, POLLIN);
  ASSERT_NEQ(0, POLLPRI);
  ASSERT_NEQ(0, POLLOUT);
  ASSERT_NEQ(0, POLLERR);
  ASSERT_NEQ(0, POLLHUP);
  ASSERT_NEQ(0, POLLNVAL);

#if !defined(_WIN32) && !defined(_MSC_VER)
  ASSERT_EQ(0x01, POLLIN);
  ASSERT_EQ(0x02, POLLPRI);
  ASSERT_EQ(0x04, POLLOUT);
  ASSERT_EQ(0x08, POLLERR);
  ASSERT_EQ(0x10, POLLHUP);
  ASSERT_EQ(0x20, POLLNVAL);
#endif

  PASS();
}

SUITE(suite_posix_poll_core) {
  RUN_TEST(test_posix_poll_get_info);
  RUN_TEST(test_posix_poll_args);
  RUN_TEST(test_posix_poll_constants);
}
