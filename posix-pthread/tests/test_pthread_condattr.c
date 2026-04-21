#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_pthread_condattr_destroy(void) {
    SKIP(); /* Generated stub for pthread_condattr_destroy */
}

TEST test_pthread_condattr_getclock(void) {
    SKIP(); /* Generated stub for pthread_condattr_getclock */
}

TEST test_pthread_condattr_getpshared(void) {
    SKIP(); /* Generated stub for pthread_condattr_getpshared */
}

TEST test_pthread_condattr_init(void) {
    SKIP(); /* Generated stub for pthread_condattr_init */
}

TEST test_pthread_condattr_setclock(void) {
    SKIP(); /* Generated stub for pthread_condattr_setclock */
}

TEST test_pthread_condattr_setpshared(void) {
    SKIP(); /* Generated stub for pthread_condattr_setpshared */
}

SUITE(suite_posix_pthread_pthread_condattr) {
    RUN_TEST(test_pthread_condattr_destroy);
    RUN_TEST(test_pthread_condattr_getclock);
    RUN_TEST(test_pthread_condattr_getpshared);
    RUN_TEST(test_pthread_condattr_init);
    RUN_TEST(test_pthread_condattr_setclock);
    RUN_TEST(test_pthread_condattr_setpshared);
}
