/* test_pthread.c - Strict C89 Integration & Atfork Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

static int g_shared_counter = 0;
static pthread_mutex_t g_test_mutex;

static void *worker_thread(void *arg) {
  int inc = *(int *)arg;
  pthread_mutex_lock(&g_test_mutex);
  g_shared_counter += inc;
  pthread_mutex_unlock(&g_test_mutex);
  return NULL;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
static int g_atfork_order[8];
static int g_atfork_idx = 0;
static int g_parent_runs = 0;
static int g_child_runs = 0;

static void prep1(void) { g_atfork_order[g_atfork_idx++] = 1; }
static void prep2(void) { g_atfork_order[g_atfork_idx++] = 2; }
static void par1(void) { g_parent_runs += 1; }
static void par2(void) { g_parent_runs += 10; }
static void chd1(void) { g_child_runs += 1; }
static void chd2(void) { g_child_runs += 10; }

static void *oom_malloc_always_null(size_t sz) {
  (void)sz;
  return NULL;
}

static void *mock_null_module(const char *name) {
  (void)name;
  return NULL;
}

static posix_pthread_farproc_t mock_null_proc(void *mod, const char *name) {
  (void)mod;
  (void)name;
  return NULL;
}

static enum greatest_test_res exercise_all_dyn_fallbacks(void) {
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
  pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
  struct timespec ts;

  ts.tv_sec = 0;
  ts.tv_nsec = 1000000L;

  ASSERT_EQ(0, pthread_mutex_init(&mtx, NULL));
  ASSERT_EQ(0, pthread_mutex_lock(&mtx));
  ASSERT_EQ(EBUSY, pthread_mutex_trylock(&mtx));
  ASSERT_EQ(0, pthread_mutex_unlock(&mtx));

  ASSERT_EQ(0, pthread_rwlock_init(&rw, NULL));
  ASSERT_EQ(0, pthread_rwlock_rdlock(&rw));
  ASSERT_EQ(EBUSY, pthread_rwlock_tryrdlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_wrlock(&rw));
  ASSERT_EQ(0, pthread_rwlock_unlock(&rw));

  ASSERT_EQ(0, pthread_cond_init(&cv, NULL));
  ASSERT_EQ(0, pthread_cond_signal(&cv));
  ASSERT_EQ(0, pthread_cond_broadcast(&cv));
  ASSERT_EQ(ETIMEDOUT, pthread_cond_timedwait(&cv, &mtx, &ts));
  ASSERT_EQ(EINVAL, pthread_cond_wait(&cv, &mtx));

  ASSERT_EQ(EINVAL, pthread_setname_np(pthread_self(), "fallback"));
  PASS();
}
#endif

TEST test_pthread(void) {
  pthread_t t1;
  pthread_t t2;
  int arg1 = 10;
  int arg2 = 25;

  ASSERT_EQ(0, dummy_posix_pthread());
  g_shared_counter = 0;
  ASSERT_EQ(0, pthread_mutex_init(&g_test_mutex, NULL));

  ASSERT_EQ(0, pthread_create(&t1, NULL, worker_thread, &arg1));
  ASSERT_EQ(0, pthread_create(&t2, NULL, worker_thread, &arg2));

  ASSERT_EQ(0, pthread_join(t1, NULL));
  ASSERT_EQ(0, pthread_join(t2, NULL));

  ASSERT_EQ(35, g_shared_counter);
  ASSERT_EQ(0, pthread_mutex_destroy(&g_test_mutex));

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct posix_pthread_win32_hooks hooks;
    posix_pthread_get_default_win32_hooks(NULL);
    posix_pthread_get_default_win32_hooks(&hooks);
    posix_pthread_set_win32_hooks(&hooks);

    hooks.get_module_handle_a = mock_null_module;
    posix_pthread_set_win32_hooks(&hooks);
    CHECK_CALL(exercise_all_dyn_fallbacks());

    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.get_proc_address = mock_null_proc;
    posix_pthread_set_win32_hooks(&hooks);
    CHECK_CALL(exercise_all_dyn_fallbacks());

    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test_posix_pthread_atfork(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  struct posix_pthread_win32_hooks hooks;

  /* Call before any handlers are registered to cover empty list */
  posix_pthread_atfork_prepare();
  posix_pthread_atfork_parent();
  posix_pthread_atfork_child();

  /* OOM path */
  posix_pthread_get_default_win32_hooks(&hooks);
  hooks.malloc_fn = oom_malloc_always_null;
  posix_pthread_set_win32_hooks(&hooks);
  ASSERT_EQ(ENOMEM, pthread_atfork(prep1, par1, chd1));
  posix_pthread_set_win32_hooks(NULL);

  /* Register multiple handlers including NULL callbacks */
  g_atfork_idx = 0;
  g_parent_runs = 0;
  g_child_runs = 0;
  ASSERT_EQ(0, pthread_atfork(prep1, par1, chd1));
  ASSERT_EQ(0, pthread_atfork(NULL, NULL, NULL));
  ASSERT_EQ(0, pthread_atfork(prep2, par2, chd2));

  posix_pthread_atfork_prepare();
  /* Prepare runs LIFO: prep2 then prep1 */
  ASSERT_EQ(2, g_atfork_idx);
  ASSERT_EQ(2, g_atfork_order[0]);
  ASSERT_EQ(1, g_atfork_order[1]);

  posix_pthread_atfork_parent();
  ASSERT_EQ(11, g_parent_runs);

  posix_pthread_atfork_child();
  ASSERT_EQ(11, g_child_runs);
#else
  ASSERT_EQ(0, pthread_atfork(NULL, NULL, NULL));
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread) {
  RUN_TEST(test_pthread);
  RUN_TEST(test_posix_pthread_atfork);
}
