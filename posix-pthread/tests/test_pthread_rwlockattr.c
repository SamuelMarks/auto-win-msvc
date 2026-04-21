#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_pthread_rwlockattr_destroy(void) {
    SKIP(); /* Generated stub for pthread_rwlockattr_destroy */
}

TEST test_pthread_rwlockattr_getpshared(void) {
    SKIP(); /* Generated stub for pthread_rwlockattr_getpshared */
}

TEST test_pthread_rwlockattr_init(void) {
    SKIP(); /* Generated stub for pthread_rwlockattr_init */
}

TEST test_pthread_rwlockattr_setpshared(void) {
    SKIP(); /* Generated stub for pthread_rwlockattr_setpshared */
}

SUITE(suite_posix_pthread_pthread_rwlockattr) {
    RUN_TEST(test_pthread_rwlockattr_destroy);
    RUN_TEST(test_pthread_rwlockattr_getpshared);
    RUN_TEST(test_pthread_rwlockattr_init);
    RUN_TEST(test_pthread_rwlockattr_setpshared);
}
