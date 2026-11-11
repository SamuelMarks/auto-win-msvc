/* clang-format off */
#include "greatest.h"
#include "sys/cdefs.h"
#include <stddef.h>
#include <stdint.h>
/* clang-format on */

__FBSDID("$FreeBSD$");
__RCSID("$NetBSD$");
__SCCSID("@(#)test");
__COPYRIGHT("Copyright");
__IDSTRING(myid, "id");
__Static_assert(sizeof(int) >= 2, "int size");

struct cdefs_test_s {
  int a;
  int b;
};

TEST test_bsd_sys_cdefs_dummy(void) {
  struct cdefs_test_s s;
  const int *cp = &s.b;
  int *p = __DECONST(int *, cp);
  struct cdefs_test_s *cont = __containerof(p, struct cdefs_test_s, b);
  s.a = 1;
  s.b = 2;
  ASSERT_EQ(&s, cont);
  ASSERT_EQ(2, __bitcount16(0x0005U));
  ASSERT_EQ(3, __bitcount32(0x00000007UL));
  ASSERT_EQ(4, __bitcount64((((uint64_t)1UL << 32) | (uint64_t)7UL)));
  ASSERT(__predict_true(1));
  ASSERT(!__predict_false(0));
  ASSERT_EQ(0, sys_cdefs_dummy_for_coverage());
  PASS();
}

SUITE(suite_bsd_sys_cdefs_core) { RUN_TEST(test_bsd_sys_cdefs_dummy); }
