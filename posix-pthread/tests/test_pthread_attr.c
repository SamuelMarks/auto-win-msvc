#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_pthread_attr_destroy(void) {
    SKIP(); /* Generated stub for pthread_attr_destroy */
}

TEST test_pthread_attr_getdetachstate(void) {
    SKIP(); /* Generated stub for pthread_attr_getdetachstate */
}

TEST test_pthread_attr_getguardsize(void) {
    SKIP(); /* Generated stub for pthread_attr_getguardsize */
}

TEST test_pthread_attr_getinheritsched(void) {
    SKIP(); /* Generated stub for pthread_attr_getinheritsched */
}

TEST test_pthread_attr_getschedparam(void) {
    SKIP(); /* Generated stub for pthread_attr_getschedparam */
}

TEST test_pthread_attr_getschedpolicy(void) {
    SKIP(); /* Generated stub for pthread_attr_getschedpolicy */
}

TEST test_pthread_attr_getscope(void) {
    SKIP(); /* Generated stub for pthread_attr_getscope */
}

TEST test_pthread_attr_getstack(void) {
    SKIP(); /* Generated stub for pthread_attr_getstack */
}

TEST test_pthread_attr_getstacksize(void) {
    SKIP(); /* Generated stub for pthread_attr_getstacksize */
}

TEST test_pthread_attr_init(void) {
    SKIP(); /* Generated stub for pthread_attr_init */
}

TEST test_pthread_attr_setdetachstate(void) {
    SKIP(); /* Generated stub for pthread_attr_setdetachstate */
}

TEST test_pthread_attr_setguardsize(void) {
    SKIP(); /* Generated stub for pthread_attr_setguardsize */
}

TEST test_pthread_attr_setinheritsched(void) {
    SKIP(); /* Generated stub for pthread_attr_setinheritsched */
}

TEST test_pthread_attr_setschedparam(void) {
    SKIP(); /* Generated stub for pthread_attr_setschedparam */
}

TEST test_pthread_attr_setschedpolicy(void) {
    SKIP(); /* Generated stub for pthread_attr_setschedpolicy */
}

TEST test_pthread_attr_setscope(void) {
    SKIP(); /* Generated stub for pthread_attr_setscope */
}

TEST test_pthread_attr_setstack(void) {
    SKIP(); /* Generated stub for pthread_attr_setstack */
}

TEST test_pthread_attr_setstacksize(void) {
    SKIP(); /* Generated stub for pthread_attr_setstacksize */
}

SUITE(suite_posix_pthread_pthread_attr) {
    RUN_TEST(test_pthread_attr_destroy);
    RUN_TEST(test_pthread_attr_getdetachstate);
    RUN_TEST(test_pthread_attr_getguardsize);
    RUN_TEST(test_pthread_attr_getinheritsched);
    RUN_TEST(test_pthread_attr_getschedparam);
    RUN_TEST(test_pthread_attr_getschedpolicy);
    RUN_TEST(test_pthread_attr_getscope);
    RUN_TEST(test_pthread_attr_getstack);
    RUN_TEST(test_pthread_attr_getstacksize);
    RUN_TEST(test_pthread_attr_init);
    RUN_TEST(test_pthread_attr_setdetachstate);
    RUN_TEST(test_pthread_attr_setguardsize);
    RUN_TEST(test_pthread_attr_setinheritsched);
    RUN_TEST(test_pthread_attr_setschedparam);
    RUN_TEST(test_pthread_attr_setschedpolicy);
    RUN_TEST(test_pthread_attr_setscope);
    RUN_TEST(test_pthread_attr_setstack);
    RUN_TEST(test_pthread_attr_setstacksize);
}
