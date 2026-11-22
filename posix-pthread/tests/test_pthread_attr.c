/* test_pthread_attr.c - Strict C89 Thread Attribute Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#if defined(_WIN32) && !defined(__CYGWIN__)
static void *attr_oom_malloc(size_t sz) {
  (void)sz;
  return NULL;
}
#endif

TEST test_pthread_attr_init(void) {
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(EINVAL, pthread_attr_init(NULL));
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.malloc_fn = attr_oom_malloc;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(ENOMEM, pthread_attr_init(&attr));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test_pthread_attr_destroy(void) {
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_destroy(NULL));
  ASSERT_EQ(EINVAL, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_getdetachstate(void) {
  pthread_attr_t attr;
  int state = -1;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_getdetachstate(&attr, &state));
  ASSERT_EQ(PTHREAD_CREATE_JOINABLE, state);
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_getdetachstate(NULL, &state));
  ASSERT_EQ(EINVAL, pthread_attr_getdetachstate(&attr, &state));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_getdetachstate(&attr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_setdetachstate(void) {
  pthread_attr_t attr;
  int state = -1;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
  ASSERT_EQ(0, pthread_attr_getdetachstate(&attr, &state));
  ASSERT_EQ(PTHREAD_CREATE_DETACHED, state);
  ASSERT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
  ASSERT_EQ(EINVAL, pthread_attr_setdetachstate(&attr, 999));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_setdetachstate(NULL, PTHREAD_CREATE_JOINABLE));
  ASSERT_EQ(EINVAL,
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
#endif
  PASS();
}

TEST test_pthread_attr_getguardsize(void) {
  pthread_attr_t attr;
  size_t gs = 0;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_getguardsize(&attr, &gs));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_getguardsize(NULL, &gs));
  ASSERT_EQ(EINVAL, pthread_attr_getguardsize(&attr, &gs));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_getguardsize(&attr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_setguardsize(void) {
  pthread_attr_t attr;
  size_t gs = 0;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setguardsize(&attr, 8192));
  ASSERT_EQ(0, pthread_attr_getguardsize(&attr, &gs));
  ASSERT(gs >= 8192);
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_setguardsize(NULL, 4096));
  ASSERT_EQ(EINVAL, pthread_attr_setguardsize(&attr, 4096));
#endif
  PASS();
}

TEST test_pthread_attr_getinheritsched(void) {
  pthread_attr_t attr;
  int inh = -1;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_getinheritsched(&attr, &inh));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_getinheritsched(NULL, &inh));
  ASSERT_EQ(EINVAL, pthread_attr_getinheritsched(&attr, &inh));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_getinheritsched(&attr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_setinheritsched(void) {
  pthread_attr_t attr;
#if defined(_WIN32) && !defined(__CYGWIN__)
  int inh = -1;
  ASSERT_EQ(EINVAL, pthread_attr_setinheritsched(NULL, 0));
  attr.ptr = NULL;
  ASSERT_EQ(EINVAL, pthread_attr_setinheritsched(&attr, 0));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setinheritsched(&attr, 1));
  ASSERT_EQ(0, pthread_attr_getinheritsched(&attr, &inh));
  ASSERT_EQ(1, inh);
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#else
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_getschedparam(void) {
  pthread_attr_t attr;
  struct sched_param sp;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_getschedparam(&attr, &sp));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_getschedparam(NULL, &sp));
  ASSERT_EQ(EINVAL, pthread_attr_getschedparam(&attr, &sp));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_getschedparam(&attr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_setschedparam(void) {
  pthread_attr_t attr;
  struct sched_param sp;
  struct sched_param out;
  int rc;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_getschedparam(&attr, &sp));
  rc = pthread_attr_setschedparam(&attr, &sp);
#if defined(__CYGWIN__)
  ASSERT(rc == 0 || rc == ENOTSUP || rc == EINVAL);
  if (rc == 0) {
    ASSERT_EQ(0, pthread_attr_getschedparam(&attr, &out));
    ASSERT_EQ(sp.sched_priority, out.sched_priority);
  }
#else
  ASSERT_EQ(0, rc);
  ASSERT_EQ(0, pthread_attr_getschedparam(&attr, &out));
  ASSERT_EQ(sp.sched_priority, out.sched_priority);
#endif
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_setschedparam(NULL, &sp));
  ASSERT_EQ(EINVAL, pthread_attr_setschedparam(&attr, &sp));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_setschedparam(&attr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_getschedpolicy(void) {
  pthread_attr_t attr;
  int pol = -1;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_getschedpolicy(&attr, &pol));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_getschedpolicy(NULL, &pol));
  ASSERT_EQ(EINVAL, pthread_attr_getschedpolicy(&attr, &pol));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_getschedpolicy(&attr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_setschedpolicy(void) {
  pthread_attr_t attr;
  int pol = -1;
  int rc;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  rc = pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
#if defined(__CYGWIN__)
  ASSERT(rc == 0 || rc == ENOTSUP);
#else
  ASSERT_EQ(0, rc);
#endif
  ASSERT_EQ(0, pthread_attr_getschedpolicy(&attr, &pol));
  ASSERT_EQ(SCHED_OTHER, pol);
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_setschedpolicy(NULL, SCHED_OTHER));
  ASSERT_EQ(EINVAL, pthread_attr_setschedpolicy(&attr, SCHED_OTHER));
#endif
  PASS();
}

TEST test_pthread_attr_getscope(void) {
  pthread_attr_t attr;
  int scope = -1;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_getscope(&attr, &scope));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_getscope(NULL, &scope));
  ASSERT_EQ(EINVAL, pthread_attr_getscope(&attr, &scope));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_getscope(&attr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_setscope(void) {
  pthread_attr_t attr;
#if defined(_WIN32) && !defined(__CYGWIN__)
  int scope = -1;
  ASSERT_EQ(EINVAL, pthread_attr_setscope(NULL, 0));
  attr.ptr = NULL;
  ASSERT_EQ(EINVAL, pthread_attr_setscope(&attr, 0));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setscope(&attr, 0));
  ASSERT_EQ(0, pthread_attr_setscope(&attr, 1));
  ASSERT_EQ(0, pthread_attr_getscope(&attr, &scope));
  ASSERT_EQ(1, scope);
  ASSERT_EQ(EINVAL, pthread_attr_setscope(&attr, 99));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#else
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_getstack(void) {
  pthread_attr_t attr;
  void *addr = NULL;
  size_t sz = 0;
  int rc;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  rc = pthread_attr_getstack(&attr, &addr, &sz);
#if defined(__linux__) && !defined(__GLIBC__)
  ASSERT(rc == 0 || rc == EINVAL);
#else
  ASSERT_EQ(0, rc);
#endif
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_getstack(NULL, &addr, &sz));
  ASSERT_EQ(EINVAL, pthread_attr_getstack(&attr, &addr, &sz));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_getstack(&attr, NULL, &sz));
  ASSERT_EQ(EINVAL, pthread_attr_getstack(&attr, &addr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_setstack(void) {
  pthread_attr_t attr;
#if defined(_WIN32) && !defined(__CYGWIN__)
  static unsigned char dummy_stack[65536];
  void *addr = NULL;
  size_t sz = 0;
  ASSERT_EQ(EINVAL,
            pthread_attr_setstack(NULL, dummy_stack, sizeof(dummy_stack)));
  attr.ptr = NULL;
  ASSERT_EQ(EINVAL,
            pthread_attr_setstack(&attr, dummy_stack, sizeof(dummy_stack)));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstack(&attr, dummy_stack, sizeof(dummy_stack)));
  ASSERT_EQ(0, pthread_attr_getstack(&attr, &addr, &sz));
  ASSERT_EQ((void *)dummy_stack, addr);
  ASSERT_EQ(sizeof(dummy_stack), sz);
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#else
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_getstacksize(void) {
  pthread_attr_t attr;
  size_t sz = 0;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_getstacksize(&attr, &sz));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_getstacksize(NULL, &sz));
  ASSERT_EQ(EINVAL, pthread_attr_getstacksize(&attr, &sz));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(EINVAL, pthread_attr_getstacksize(&attr, NULL));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#endif
  PASS();
}

TEST test_pthread_attr_setstacksize(void) {
  pthread_attr_t attr;
  size_t sz = 0;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstacksize(&attr, 1024 * 1024));
  ASSERT_EQ(0, pthread_attr_getstacksize(&attr, &sz));
  ASSERT_EQ((size_t)(1024 * 1024), sz);
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_attr_setstacksize(NULL, 65536));
  ASSERT_EQ(EINVAL, pthread_attr_setstacksize(&attr, 65536));
#endif
  PASS();
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
