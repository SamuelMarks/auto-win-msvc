#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-ucontext.h"

TEST test_ucontext(void) {
    ucontext_t u;
    if (getcontext(&u) == -1) SKIP();
    PASS();
}

TEST test_getcontext(void) {
    SKIP(); /* Generated stub for getcontext */
}

TEST test_setcontext(void) {
    SKIP(); /* Generated stub for setcontext */
}

TEST test_makecontext(void) {
    SKIP(); /* Generated stub for makecontext */
}

TEST test_swapcontext(void) {
    SKIP(); /* Generated stub for swapcontext */
}

SUITE(suite_posix_ucontext_core) {
    RUN_TEST(test_ucontext);
    RUN_TEST(test_getcontext);
    RUN_TEST(test_setcontext);
    RUN_TEST(test_makecontext);
    RUN_TEST(test_swapcontext);
}
