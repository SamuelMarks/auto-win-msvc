/* clang-format off */
#include "greatest.h"
#include <rpc/rpc.h>
#include <rpcsvc/ypclnt.h>
/* clang-format on */

extern int bsd_rpc_dummy_for_coverage(void);

TEST test_bsd_rpc_basic(void) {
  ASSERT_EQ(0, bsd_rpc_dummy_for_coverage());
  PASS();
}

SUITE(suite_bsd_rpc_core) { RUN_TEST(test_bsd_rpc_basic); }
