#ifdef _MSC_VER
#endif /* _MSC_VER */
#define AUTO_WIN_MSVC_NO_FUNCTION_MACROS 1
/* clang-format off */
#include "greatest.h"
#include "posix-sockets.h"
#include "netlink/netlink_snl_generic.h"
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

TEST test_netlink_snl_inlines(void) {
  struct snl_state ss;
  struct snl_writer nw;
  struct snl_errmsg_data err_data;

  ASSERT_FALSE(snl_init(NULL, NETLINK_GENERIC));
  ASSERT_FALSE(snl_init(&ss, NETLINK_GENERIC));
  ASSERT_EQ(-1, ss.fd);
  ASSERT_EQ(1U, ss.seq);

  snl_init_writer(&ss, NULL);
  snl_init_writer(&ss, &nw);
  ASSERT_EQ(&ss, nw.ss);
  ASSERT(nw.error);

  ASSERT(snl_create_msg_request(&nw, 1) == NULL);
  ASSERT_FALSE(snl_finalize_msg(&nw));
  ASSERT_FALSE(snl_send_message(&ss, NULL));
  ASSERT(snl_read_reply_multi(&ss, 1U, &err_data) == NULL);
  ASSERT_FALSE(snl_parse_nlmsg(&ss, NULL, NULL, NULL));
  ASSERT_EQ(0U, (unsigned int)snl_get_genl_family(&ss, "nlctrl"));

  snl_clear_lb(&ss);
  snl_free(&ss);
  PASS();
}

SUITE(suite_posix_sockets_no_macros) {
  RUN_TEST(test_struct_member_conflict_avoidance);
  RUN_TEST(test_netlink_snl_inlines);
}
