#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_sched_get_priority_max(void) {
    SKIP(); /* Generated stub for sched_get_priority_max */
}

TEST test_sched_get_priority_min(void) {
    SKIP(); /* Generated stub for sched_get_priority_min */
}

TEST test_sched_getparam(void) {
    SKIP(); /* Generated stub for sched_getparam */
}

TEST test_sched_getscheduler(void) {
    SKIP(); /* Generated stub for sched_getscheduler */
}

TEST test_sched_rr_get_interval(void) {
    SKIP(); /* Generated stub for sched_rr_get_interval */
}

TEST test_sched_setparam(void) {
    SKIP(); /* Generated stub for sched_setparam */
}

TEST test_sched_setscheduler(void) {
    SKIP(); /* Generated stub for sched_setscheduler */
}

TEST test_sched_yield(void) {
    SKIP(); /* Generated stub for sched_yield */
}

SUITE(suite_posix_pthread_sched) {
    RUN_TEST(test_sched_get_priority_max);
    RUN_TEST(test_sched_get_priority_min);
    RUN_TEST(test_sched_getparam);
    RUN_TEST(test_sched_getscheduler);
    RUN_TEST(test_sched_rr_get_interval);
    RUN_TEST(test_sched_setparam);
    RUN_TEST(test_sched_setscheduler);
    RUN_TEST(test_sched_yield);
}
