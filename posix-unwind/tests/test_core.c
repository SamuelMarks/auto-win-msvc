#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-unwind.h"
/* clang-format on */

TEST test_unwind(void) { SKIP(); }

TEST test__Unwind_Reason_Code(void) {
  SKIP(); /* Generated stub for _Unwind_Reason_Code */
}

TEST test__Unwind_GetIP(void) { SKIP(); /* Generated stub for _Unwind_GetIP */ }

TEST test__Unwind_Backtrace(void) {
  SKIP(); /* Generated stub for _Unwind_Backtrace */
}

SUITE(suite_posix_unwind_core) {
  RUN_TEST(test_unwind);
  RUN_TEST(test__Unwind_Reason_Code);
  RUN_TEST(test__Unwind_GetIP);
  RUN_TEST(test__Unwind_Backtrace);
}
