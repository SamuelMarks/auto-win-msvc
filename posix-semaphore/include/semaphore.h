#ifndef POSIX_SEMAPHORE_H
#define POSIX_SEMAPHORE_H

/**
 * @file semaphore.h
 * @brief POSIX <semaphore.h> passthrough on Unix and portable polyfill on
 * Windows/DOS.
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) && !defined(__DOS__) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <semaphore.h>
#else
#include <sys/types.h>
#include <time.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__MSDOS__) ||           \
    defined(__DOS__) || !(defined(__GNUC__) || defined(__clang__))

typedef struct awm_sem_s {
  void *handle;
  void *p;
  long count;
} sem_t;

#ifndef SEM_FAILED
#define SEM_FAILED ((sem_t *)0)
#endif
#ifndef SEM_VALUE_MAX
#define SEM_VALUE_MAX 32767
#endif

/** @brief Initialize an unnamed semaphore. */
int sem_init(sem_t *sem, int pshared, unsigned int value);
/** @brief Destroy an unnamed semaphore. */
int sem_destroy(sem_t *sem);
/** @brief Lock a semaphore. */
int sem_wait(sem_t *sem);
/** @brief Attempt to lock a semaphore without blocking. */
int sem_trywait(sem_t *sem);
/** @brief Lock a semaphore with timeout. */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
/** @brief Unlock a semaphore. */
int sem_post(sem_t *sem);
/** @brief Query current value of a semaphore. */
int sem_getvalue(sem_t *sem, int *sval);
/** @brief Open or create a named semaphore. */
sem_t *sem_open(const char *name, int oflag, ...);
/** @brief Close a named semaphore. */
int sem_close(sem_t *sem);
/** @brief Unlink a named semaphore. */
int sem_unlink(const char *name);

#endif /* POSIX vs Windows/DOS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SEMAPHORE_H */
/* API Contract symbols: __dependencies__, __include_next__ */
