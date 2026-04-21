#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-mman.h"

TEST test_mman(void) {
    void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NEQ(MAP_FAILED, ptr);
    munmap(ptr, 4096);
    PASS();
}

SUITE(suite_posix_mman_mman) {
    RUN_TEST(test_mman);
}
