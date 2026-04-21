#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-sys-select.h"
#include <stddef.h>

TEST test_sys_select(void) {
    if (select(0, NULL, NULL, NULL, NULL) == -1) SKIP();
    PASS();
}

SUITE(suite_posix_sys_select_core) {
    RUN_TEST(test_sys_select);
}
