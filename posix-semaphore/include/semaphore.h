#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/* clang-format off */
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef sem_close
#define sem_close posix_sem_close
#endif
int posix_sem_close(void);

#ifndef sem_destroy
#define sem_destroy posix_sem_destroy
#endif
int posix_sem_destroy(void);

#ifndef sem_getvalue
#define sem_getvalue posix_sem_getvalue
#endif
int posix_sem_getvalue(void);

#ifndef sem_init
#define sem_init posix_sem_init
#endif
int posix_sem_init(void);

#ifndef sem_open
#define sem_open posix_sem_open
#endif
int posix_sem_open(void);

#ifndef sem_post
#define sem_post posix_sem_post
#endif
int posix_sem_post(void);

#ifndef sem_timedwait
#define sem_timedwait posix_sem_timedwait
#endif
int posix_sem_timedwait(void);

#ifndef sem_trywait
#define sem_trywait posix_sem_trywait
#endif
int posix_sem_trywait(void);

#ifndef sem_unlink
#define sem_unlink posix_sem_unlink
#endif
int posix_sem_unlink(void);

#ifndef sem_wait
#define sem_wait posix_sem_wait
#endif
int posix_sem_wait(void);

#define __dependencies__

#define __include_next__

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SEMAPHORE_H */
