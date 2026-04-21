#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_pthread_key_create(void) {
    SKIP(); /* Generated stub for pthread_key_create */
}

TEST test_pthread_key_delete(void) {
    SKIP(); /* Generated stub for pthread_key_delete */
}

SUITE(suite_posix_pthread_pthread_key) {
    RUN_TEST(test_pthread_key_create);
    RUN_TEST(test_pthread_key_delete);
}
