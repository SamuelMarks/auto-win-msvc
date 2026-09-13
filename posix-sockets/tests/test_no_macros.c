#ifdef _MSC_VER
#endif /* _MSC_VER */
#define AUTO_WIN_MSVC_NO_FUNCTION_MACROS 1
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
/* clang-format on */

struct test_conn_struct {
  int (*read)(int fd);
  int (*write)(int fd);
  int (*sendmsg)(int fd);
  int (*close)(int fd);
};

static int mock_read(int fd) { return fd; }

TEST test_struct_member_conflict_avoidance(void) {
  struct test_conn_struct conn;
  conn.read = mock_read;
  conn.write = mock_read;
  conn.sendmsg = mock_read;
  conn.close = mock_read;

  ASSERT_EQ(10, conn.read(10));
  ASSERT_EQ(20, conn.write(20));
  ASSERT_EQ(30, conn.sendmsg(30));
  ASSERT_EQ(40, conn.close(40));
  PASS();
}

SUITE(suite_posix_sockets_no_macros) {
  RUN_TEST(test_struct_member_conflict_avoidance);
}
