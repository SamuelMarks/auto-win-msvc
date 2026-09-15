#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-spawn.h"
#include <stddef.h>
/* clang-format on */

TEST test_posix_spawnattr_init(void) {
  posix_spawnattr_t attr;
  ASSERT(posix_spawnattr_init(NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_init(&attr));
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_destroy(void) {
  posix_spawnattr_t attr;
  ASSERT(posix_spawnattr_destroy(NULL) != 0);
  posix_spawnattr_init(&attr);
  ASSERT_EQ(0, posix_spawnattr_destroy(&attr));
  PASS();
}

TEST test_posix_spawnattr_getflags(void) {
  posix_spawnattr_t attr;
  short flags = 0;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_getflags(NULL, &flags) != 0);
  ASSERT(posix_spawnattr_getflags(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_getflags(&attr, &flags));
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_setflags(void) {
  posix_spawnattr_t attr;
  short flags = 0;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_setflags(NULL, POSIX_SPAWN_RESETIDS) != 0);
  ASSERT_EQ(0, posix_spawnattr_setflags(&attr, POSIX_SPAWN_RESETIDS));
  ASSERT_EQ(0, posix_spawnattr_getflags(&attr, &flags));
  ASSERT_EQ(POSIX_SPAWN_RESETIDS, flags);
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_getpgroup(void) {
  posix_spawnattr_t attr;
  pid_t pgroup = 0;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_getpgroup(NULL, &pgroup) != 0);
  ASSERT(posix_spawnattr_getpgroup(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_getpgroup(&attr, &pgroup));
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_setpgroup(void) {
  posix_spawnattr_t attr;
  pid_t pgroup = 0;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_setpgroup(NULL, 1234) != 0);
  ASSERT_EQ(0, posix_spawnattr_setpgroup(&attr, 1234));
  ASSERT_EQ(0, posix_spawnattr_getpgroup(&attr, &pgroup));
  ASSERT_EQ(1234, (int)pgroup);
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_getschedparam(void) {
  posix_spawnattr_t attr;
  struct sched_param sp;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_getschedparam(NULL, &sp) != 0);
  ASSERT(posix_spawnattr_getschedparam(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_getschedparam(&attr, &sp));
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_setschedparam(void) {
  posix_spawnattr_t attr;
  struct sched_param sp;
  struct sched_param out_sp;
  sp.sched_priority = 5;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_setschedparam(NULL, &sp) != 0);
  ASSERT(posix_spawnattr_setschedparam(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_setschedparam(&attr, &sp));
  ASSERT_EQ(0, posix_spawnattr_getschedparam(&attr, &out_sp));
  ASSERT_EQ(5, out_sp.sched_priority);
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_getschedpolicy(void) {
  posix_spawnattr_t attr;
  int pol = 0;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_getschedpolicy(NULL, &pol) != 0);
  ASSERT(posix_spawnattr_getschedpolicy(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_getschedpolicy(&attr, &pol));
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_setschedpolicy(void) {
  posix_spawnattr_t attr;
  int pol = 0;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_setschedpolicy(NULL, 1) != 0);
  ASSERT_EQ(0, posix_spawnattr_setschedpolicy(&attr, 1));
  ASSERT_EQ(0, posix_spawnattr_getschedpolicy(&attr, &pol));
  ASSERT_EQ(1, pol);
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_getsigdefault(void) {
  posix_spawnattr_t attr;
  sigset_t sig;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_getsigdefault(NULL, &sig) != 0);
  ASSERT(posix_spawnattr_getsigdefault(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_getsigdefault(&attr, &sig));
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_setsigdefault(void) {
  posix_spawnattr_t attr;
  sigset_t sig;
  sigset_t out_sig;
  memset(&sig, 0, sizeof(sig));
  memset(&out_sig, 0, sizeof(out_sig));
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_setsigdefault(NULL, &sig) != 0);
  ASSERT(posix_spawnattr_setsigdefault(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_setsigdefault(&attr, &sig));
  ASSERT_EQ(0, posix_spawnattr_getsigdefault(&attr, &out_sig));
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_getsigmask(void) {
  posix_spawnattr_t attr;
  sigset_t sig;
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_getsigmask(NULL, &sig) != 0);
  ASSERT(posix_spawnattr_getsigmask(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_getsigmask(&attr, &sig));
  posix_spawnattr_destroy(&attr);
  PASS();
}

TEST test_posix_spawnattr_setsigmask(void) {
  posix_spawnattr_t attr;
  sigset_t sig;
  sigset_t out_sig;
  memset(&sig, 0, sizeof(sig));
  memset(&out_sig, 0, sizeof(out_sig));
  posix_spawnattr_init(&attr);
  ASSERT(posix_spawnattr_setsigmask(NULL, &sig) != 0);
  ASSERT(posix_spawnattr_setsigmask(&attr, NULL) != 0);
  ASSERT_EQ(0, posix_spawnattr_setsigmask(&attr, &sig));
  ASSERT_EQ(0, posix_spawnattr_getsigmask(&attr, &out_sig));
  posix_spawnattr_destroy(&attr);
  PASS();
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
