#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"

SUITE_EXTERN(suite_posix_pthread_pthread);
SUITE_EXTERN(suite_posix_pthread_pthread_core);
SUITE_EXTERN(suite_posix_pthread_pthread_attr);
SUITE_EXTERN(suite_posix_pthread_pthread_barrier);
SUITE_EXTERN(suite_posix_pthread_pthread_barrierattr);
SUITE_EXTERN(suite_posix_pthread_);
SUITE_EXTERN(suite_posix_pthread_pthread_cond);
SUITE_EXTERN(suite_posix_pthread_pthread_condattr);
SUITE_EXTERN(suite_posix_pthread_pthread_key);
SUITE_EXTERN(suite_posix_pthread_pthread_mutex);
SUITE_EXTERN(suite_posix_pthread_pthread_mutexattr);
SUITE_EXTERN(suite_posix_pthread_pthread_rwlock);
SUITE_EXTERN(suite_posix_pthread_pthread_rwlockattr);
SUITE_EXTERN(suite_posix_pthread_pthread_spin);
SUITE_EXTERN(suite_posix_pthread_sched);
SUITE_EXTERN(suite_posix_pthread_sem);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(suite_posix_pthread_pthread);
    RUN_SUITE(suite_posix_pthread_pthread_core);
    RUN_SUITE(suite_posix_pthread_pthread_attr);
    RUN_SUITE(suite_posix_pthread_pthread_barrier);
    RUN_SUITE(suite_posix_pthread_pthread_barrierattr);
    RUN_SUITE(suite_posix_pthread_);
    RUN_SUITE(suite_posix_pthread_pthread_cond);
    RUN_SUITE(suite_posix_pthread_pthread_condattr);
    RUN_SUITE(suite_posix_pthread_pthread_key);
    RUN_SUITE(suite_posix_pthread_pthread_mutex);
    RUN_SUITE(suite_posix_pthread_pthread_mutexattr);
    RUN_SUITE(suite_posix_pthread_pthread_rwlock);
    RUN_SUITE(suite_posix_pthread_pthread_rwlockattr);
    RUN_SUITE(suite_posix_pthread_pthread_spin);
    RUN_SUITE(suite_posix_pthread_sched);
    RUN_SUITE(suite_posix_pthread_sem);
    GREATEST_MAIN_END();
    return 0;
}
