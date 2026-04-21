#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "alloca.h"

TEST test_alloca(void) {
    void *ptr = alloca(128);
    ASSERT_NEQ(NULL, ptr);
    ((char *)ptr)[0] = 'a';
    ((char *)ptr)[127] = 'z';
    ASSERT_EQ('a', ((char *)ptr)[0]);
    ASSERT_EQ('z', ((char *)ptr)[127]);
    PASS();
}

SUITE(suite_posix_alloca_core) {
    RUN_TEST(test_alloca);
}
