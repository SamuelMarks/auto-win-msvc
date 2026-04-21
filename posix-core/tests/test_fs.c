#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-core.h"
#include <process.h> /* For _getpid if needed */

TEST test_chown(void) {
    SKIP(); /* Generated stub for chown */
}

TEST test_faccessat(void) {
    SKIP(); /* Generated stub for faccessat */
}

TEST test_fchown(void) {
    SKIP(); /* Generated stub for fchown */
}

TEST test_fchownat(void) {
    SKIP(); /* Generated stub for fchownat */
}

TEST test_lchown(void) {
    SKIP(); /* Generated stub for lchown */
}

TEST test_link(void) {
    SKIP(); /* Generated stub for link */
}

TEST test_linkat(void) {
    SKIP(); /* Generated stub for linkat */
}

TEST test_symlink(void) {
    SKIP(); /* Generated stub for symlink */
}

TEST test_symlinkat(void) {
    SKIP(); /* Generated stub for symlinkat */
}

SUITE(suite_posix_core_fs) {
    RUN_TEST(test_chown);
    RUN_TEST(test_faccessat);
    RUN_TEST(test_fchown);
    RUN_TEST(test_fchownat);
    RUN_TEST(test_lchown);
    RUN_TEST(test_link);
    RUN_TEST(test_linkat);
    RUN_TEST(test_symlink);
    RUN_TEST(test_symlinkat);
}
