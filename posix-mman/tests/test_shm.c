#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-mman.h"

TEST test_shm_open(void) {
    SKIP(); /* Generated stub for shm_open */
}

TEST test_shm_unlink(void) {
    SKIP(); /* Generated stub for shm_unlink */
}

SUITE(suite_posix_mman_shm) {
    RUN_TEST(test_shm_open);
    RUN_TEST(test_shm_unlink);
}
