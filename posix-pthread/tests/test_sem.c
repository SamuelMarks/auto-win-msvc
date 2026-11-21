/* test_sem.c - Strict C89 Semaphore Polyfill & Mock Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
/* clang-format on */

#if defined(_WIN32) && !defined(__CYGWIN__)
static void *sem_oom_malloc(size_t sz) {
  (void)sz;
  return NULL;
}

static void *mock_create_sem_null(long init_c, long max_c, const char *name) {
  (void)init_c;
  (void)max_c;
  (void)name;
  return NULL;
}

static int mock_release_sem_fail(void *sem, long count, long *prev) {
  (void)sem;
  (void)count;
  (void)prev;
  return 0;
}

static unsigned long mock_wait_failed_code(void *h, unsigned long ms) {
  (void)h;
  (void)ms;
  return 0xFFFFFFFFUL;
}
#endif

TEST test_sem_init(void) {
#if !defined(__APPLE__)
  sem_t s;
  ASSERT_EQ(0, sem_init(&s, 0, 2));
  ASSERT_EQ(0, sem_destroy(&s));
#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(-1, sem_init(NULL, 0, 1));
    ASSERT_EQ(-1, sem_init(&s, 0, 2147483648U));

    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.create_semaphore_a = mock_create_sem_null;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(-1, sem_init(&s, 0, 1));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif
#endif
  PASS();
}

TEST test_sem_destroy(void) {
#if !defined(__APPLE__)
  sem_t s;
  ASSERT_EQ(0, sem_init(&s, 0, 1));
  ASSERT_EQ(0, sem_destroy(&s));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, sem_destroy(NULL));
  ASSERT_EQ(-1, sem_destroy(&s));
#endif
#endif
  PASS();
}

TEST test_sem_getvalue(void) {
#if !defined(__APPLE__)
  sem_t s;
  int val = -1;
  ASSERT_EQ(0, sem_init(&s, 0, 2));
  ASSERT_EQ(0, sem_getvalue(&s, &val));
  ASSERT_EQ(2, val);
  ASSERT_EQ(0, sem_wait(&s));
  ASSERT_EQ(0, sem_wait(&s));
  ASSERT_EQ(0, sem_getvalue(&s, &val));
  ASSERT_EQ(0, val);

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(-1, sem_getvalue(NULL, &val));
    ASSERT_EQ(-1, sem_getvalue(&s, NULL));

    /* Cover ReleaseSemaphore failure fallback (*sval = 1) */
    ASSERT_EQ(0, sem_post(&s));
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.release_semaphore = mock_release_sem_fail;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(0, sem_getvalue(&s, &val));
    ASSERT_EQ(1, val);
    posix_pthread_set_win32_hooks(NULL);
  }
#endif

  ASSERT_EQ(0, sem_destroy(&s));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, sem_getvalue(&s, &val));
#endif
#endif
  PASS();
}

TEST test_sem_open(void) {
  sem_t *s1;
  sem_t *s2 = NULL;

  (void)sem_unlink("/awm_test_sem");
  s1 = sem_open("/awm_test_sem", O_CREAT, 0600, 1U);
  ASSERT(s1 != SEM_FAILED);
  ASSERT_EQ(0, sem_close(s1));
  (void)sem_unlink("/awm_test_sem");

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT(sem_open(NULL, 0) == SEM_FAILED);
    ASSERT(sem_open("", 0) == SEM_FAILED);
    ASSERT(sem_open("/", 0) == SEM_FAILED);

    /* Non-existent semaphore without O_CREAT -> ENOENT */
    ASSERT(sem_open("awm_nonexistent_sem_xyz", 0) == SEM_FAILED);
    ASSERT_EQ(ENOENT, errno);

    /* Interior slash replacement & O_CREAT | O_EXCL collision */
    s1 = sem_open("/awm_dir/sub_sem", O_CREAT | O_EXCL, 0600, 1U);
    ASSERT(s1 != SEM_FAILED);
    s2 = sem_open("/awm_dir/sub_sem", O_CREAT | O_EXCL, 0600, 1U);
    ASSERT(s2 == SEM_FAILED);
    ASSERT_EQ(EEXIST, errno);

    /* Open existing without O_CREAT */
    s2 = sem_open("awm_dir/sub_sem", 0);
    ASSERT(s2 != SEM_FAILED);
    ASSERT_EQ(0, sem_close(s2));

    /* OOM while opening existing semaphore */
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.malloc_fn = sem_oom_malloc;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT(sem_open("awm_dir/sub_sem", 0) == SEM_FAILED);
    ASSERT_EQ(ENOMEM, errno);

    /* CreateSemaphoreA failure & value overflow */
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.create_semaphore_a = mock_create_sem_null;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT(sem_open("/awm_fail_sem", O_CREAT, 0600, 1U) == SEM_FAILED);
    ASSERT(sem_open("/awm_big_sem", O_CREAT, 0600, 2147483648U) == SEM_FAILED);
    posix_pthread_set_win32_hooks(NULL);

    ASSERT_EQ(0, sem_close(s1));
  }
#else
  (void)s2;
#endif
  PASS();
}

TEST test_sem_close(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  sem_t *empty_sem;
  ASSERT_EQ(-1, sem_close(NULL));
  empty_sem = (sem_t *)malloc(sizeof(sem_t));
  ASSERT(empty_sem != NULL);
  empty_sem->p = NULL;
  ASSERT_EQ(0, sem_close(empty_sem));
#endif
  PASS();
}

TEST test_sem_post(void) {
#if !defined(__APPLE__)
  sem_t s;
  ASSERT_EQ(0, sem_init(&s, 0, 0));
  ASSERT_EQ(0, sem_post(&s));
  ASSERT_EQ(0, sem_wait(&s));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(-1, sem_post(NULL));
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.release_semaphore = mock_release_sem_fail;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(-1, sem_post(&s));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif

  ASSERT_EQ(0, sem_destroy(&s));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, sem_post(&s));
#endif
#endif
  PASS();
}

TEST test_sem_wait(void) {
#if !defined(__APPLE__)
  sem_t s;
  ASSERT_EQ(0, sem_init(&s, 0, 1));
  ASSERT_EQ(0, sem_wait(&s));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(-1, sem_wait(NULL));
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.wait_for_single_object = mock_wait_failed_code;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(-1, sem_wait(&s));
    posix_pthread_set_win32_hooks(NULL);
  }
#endif

  ASSERT_EQ(0, sem_destroy(&s));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, sem_wait(&s));
#endif
#endif
  PASS();
}

TEST test_sem_trywait(void) {
#if !defined(__APPLE__)
  sem_t s;
  ASSERT_EQ(0, sem_init(&s, 0, 1));
  ASSERT_EQ(0, sem_trywait(&s));
  ASSERT_EQ(-1, sem_trywait(&s));
  ASSERT_EQ(EAGAIN, errno);

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(-1, sem_trywait(NULL));
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.wait_for_single_object = mock_wait_failed_code;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(-1, sem_trywait(&s));
    ASSERT_EQ(EINVAL, errno);
    posix_pthread_set_win32_hooks(NULL);
  }
#endif

  ASSERT_EQ(0, sem_destroy(&s));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, sem_trywait(&s));
#endif
#endif
  PASS();
}

TEST test_sem_timedwait(void) {
#if !defined(__APPLE__)
  sem_t s;
  struct timespec ts;

  ASSERT_EQ(0, sem_init(&s, 0, 1));
  /* Future deadline while posted succeeds immediately */
  ts.tv_sec = time(NULL) + 10;
  ts.tv_nsec = 500000L;
  ASSERT_EQ(0, sem_timedwait(&s, &ts));

  /* Past deadline while empty times out immediately */
  ts.tv_sec = 0;
  ts.tv_nsec = 0;
  ASSERT_EQ(-1, sem_timedwait(&s, &ts));
  ASSERT_EQ(ETIMEDOUT, errno);

  /* Invalid tv_nsec */
  ts.tv_sec = time(NULL) + 5;
  ts.tv_nsec = -1;
  ASSERT_EQ(-1, sem_timedwait(&s, &ts));
  ts.tv_nsec = 1000000000L;
  ASSERT_EQ(-1, sem_timedwait(&s, &ts));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    ASSERT_EQ(-1, sem_timedwait(NULL, &ts));

    /* NULL abs_timeout while posted */
    ASSERT_EQ(0, sem_post(&s));
    ASSERT_EQ(0, sem_timedwait(&s, NULL));

    /* WAIT_FAILED path */
    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.wait_for_single_object = mock_wait_failed_code;
    posix_pthread_set_win32_hooks(&hooks);
    ASSERT_EQ(-1, sem_timedwait(&s, NULL));
    ASSERT_EQ(EINVAL, errno);
    posix_pthread_set_win32_hooks(NULL);
  }
#endif

  ASSERT_EQ(0, sem_destroy(&s));
#if defined(_WIN32) && !defined(__CYGWIN__)
  ts.tv_sec = 0;
  ts.tv_nsec = 0;
  ASSERT_EQ(-1, sem_timedwait(&s, &ts));
#endif
#endif
  PASS();
}

TEST test_sem_unlink(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  ASSERT_EQ(-1, sem_unlink(NULL));
  ASSERT_EQ(-1, sem_unlink(""));
  ASSERT_EQ(0, sem_unlink("/awm_any"));
#else
  (void)sem_unlink("/awm_nonexistent_unlink");
#endif
  PASS();
}

SUITE(suite_posix_pthread_sem) {
  RUN_TEST(test_sem_close);
  RUN_TEST(test_sem_destroy);
  RUN_TEST(test_sem_getvalue);
  RUN_TEST(test_sem_init);
  RUN_TEST(test_sem_open);
  RUN_TEST(test_sem_post);
  RUN_TEST(test_sem_timedwait);
  RUN_TEST(test_sem_trywait);
  RUN_TEST(test_sem_unlink);
  RUN_TEST(test_sem_wait);
}
