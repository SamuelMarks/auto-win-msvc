#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-spawn.h"
#include <stddef.h>
/* clang-format on */

TEST test_posix_spawnattr_init(void) {
  SKIP(); /* Generated stub for posix_spawnattr_init */
}

TEST test_posix_spawnattr_destroy(void) {
  SKIP(); /* Generated stub for posix_spawnattr_destroy */
}

TEST test_posix_spawnattr_getflags(void) {
  SKIP(); /* Generated stub for posix_spawnattr_getflags */
}

TEST test_posix_spawnattr_setflags(void) {
  SKIP(); /* Generated stub for posix_spawnattr_setflags */
}

TEST test_posix_spawnattr_getpgroup(void) {
  SKIP(); /* Generated stub for posix_spawnattr_getpgroup */
}

TEST test_posix_spawnattr_setpgroup(void) {
  SKIP(); /* Generated stub for posix_spawnattr_setpgroup */
}

TEST test_posix_spawnattr_getschedparam(void) {
  SKIP(); /* Generated stub for posix_spawnattr_getschedparam */
}

TEST test_posix_spawnattr_setschedparam(void) {
  SKIP(); /* Generated stub for posix_spawnattr_setschedparam */
}

TEST test_posix_spawnattr_getschedpolicy(void) {
  SKIP(); /* Generated stub for posix_spawnattr_getschedpolicy */
}

TEST test_posix_spawnattr_setschedpolicy(void) {
  SKIP(); /* Generated stub for posix_spawnattr_setschedpolicy */
}

TEST test_posix_spawnattr_getsigdefault(void) {
  SKIP(); /* Generated stub for posix_spawnattr_getsigdefault */
}

TEST test_posix_spawnattr_setsigdefault(void) {
  SKIP(); /* Generated stub for posix_spawnattr_setsigdefault */
}

TEST test_posix_spawnattr_getsigmask(void) {
  SKIP(); /* Generated stub for posix_spawnattr_getsigmask */
}

TEST test_posix_spawnattr_setsigmask(void) {
  SKIP(); /* Generated stub for posix_spawnattr_setsigmask */
}

SUITE(suite_posix_spawn_spawnattr) {
  RUN_TEST(test_posix_spawnattr_init);
  RUN_TEST(test_posix_spawnattr_destroy);
  RUN_TEST(test_posix_spawnattr_getflags);
  RUN_TEST(test_posix_spawnattr_setflags);
  RUN_TEST(test_posix_spawnattr_getpgroup);
  RUN_TEST(test_posix_spawnattr_setpgroup);
  RUN_TEST(test_posix_spawnattr_getschedparam);
  RUN_TEST(test_posix_spawnattr_setschedparam);
  RUN_TEST(test_posix_spawnattr_getschedpolicy);
  RUN_TEST(test_posix_spawnattr_setschedpolicy);
  RUN_TEST(test_posix_spawnattr_getsigdefault);
  RUN_TEST(test_posix_spawnattr_setsigdefault);
  RUN_TEST(test_posix_spawnattr_getsigmask);
  RUN_TEST(test_posix_spawnattr_setsigmask);
}
