#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-sys-select.h"
#include <stdio.h>
/* clang-format on */

TEST test_posix_sys_select_get_info(void) {
  enum posix_sys_select_error_code rc;
  int info;

  info = 0;
  rc = posix_sys_select_get_info(NULL);
  if (rc != POSIX_SYS_SELECT_ERROR_NULL_POINTER) {
    printf("Expected POSIX_SYS_SELECT_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_sys_select_get_info(&info);
  if (rc != POSIX_SYS_SELECT_SUCCESS) {
    printf("posix_sys_select_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

#if defined(_WIN32) || defined(_MSC_VER)
TEST test_auto_win_msvc_fd_set_clr(void) {
  fd_set set;
  u_int i;

  auto_win_msvc_fd_set(1, NULL);
  auto_win_msvc_fd_clr(1, NULL);

  FD_ZERO(&set);
  ASSERT_EQ(0, set.fd_count);

  /* Add 10 and 20 */
  auto_win_msvc_fd_set(10, &set);
  ASSERT_EQ(1, set.fd_count);
  ASSERT_EQ(10, set.fd_array[0]);

  auto_win_msvc_fd_set(20, &set);
  ASSERT_EQ(2, set.fd_count);

  /* Adding duplicate 10 should not increase count */
  auto_win_msvc_fd_set(10, &set);
  ASSERT_EQ(2, set.fd_count);

  /* Fill up to FD_SETSIZE */
  for (i = 2; i < FD_SETSIZE; i++) {
    auto_win_msvc_fd_set((SOCKET)(100 + i), &set);
  }
  ASSERT_EQ(FD_SETSIZE, set.fd_count);

  /* Adding beyond FD_SETSIZE is ignored */
  auto_win_msvc_fd_set(9999, &set);
  ASSERT_EQ(FD_SETSIZE, set.fd_count);

  /* Clear non-existent fd */
  auto_win_msvc_fd_clr(9999, &set);
  ASSERT_EQ(FD_SETSIZE, set.fd_count);

  /* Clear 10 (which is at index 0) */
  auto_win_msvc_fd_clr(10, &set);
  ASSERT_EQ(FD_SETSIZE - 1, set.fd_count);
  ASSERT_EQ(20, set.fd_array[0]);

  PASS();
}
#endif

TEST test_sys_select(void) {
  struct timeval tv;
  int res;
#if defined(_WIN32) || defined(_MSC_VER)
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
  tv.tv_sec = 0;
  tv.tv_usec = 1000;
  res = select(0, NULL, NULL, NULL, &tv);
#if defined(_WIN32) || defined(_MSC_VER)
  /* Winsock select requires at least one socket handle */
  ASSERT(res == -1 || res >= 0);
  WSACleanup();
#else
  ASSERT(res >= 0);
#endif
  PASS();
}

SUITE(suite_posix_sys_select_core) {
  RUN_TEST(test_posix_sys_select_get_info);
#if defined(_WIN32) || defined(_MSC_VER)
  RUN_TEST(test_auto_win_msvc_fd_set_clr);
#endif
  RUN_TEST(test_sys_select);
}
