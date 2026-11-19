/* test_pthread_key.c - Strict C89 Thread-Specific Key Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#if defined(_WIN32) && !defined(__CYGWIN__)
static unsigned long mock_key_tls_exhausted(void) { return 0xFFFFFFFFUL; }
#endif

TEST test_pthread_key_create(void) {
  pthread_key_t key;
  ASSERT_EQ(0, pthread_key_create(&key, NULL));
  ASSERT_EQ(0, pthread_key_delete(key));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(EINVAL, pthread_key_create(NULL, NULL));
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.tls_alloc = mock_key_tls_exhausted;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(EAGAIN, pthread_key_create(&key, NULL));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test_pthread_key_delete(void) {
  pthread_key_t key;
  ASSERT_EQ(0, pthread_key_create(&key, NULL));
  ASSERT_EQ(0, pthread_key_delete(key));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(EINVAL, pthread_key_delete(0xFFFFFFFFUL));
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread_key) {
  RUN_TEST(test_pthread_key_create);
  RUN_TEST(test_pthread_key_delete);
}
