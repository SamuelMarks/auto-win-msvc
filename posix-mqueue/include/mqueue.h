#ifndef MQUEUE_H
#define MQUEUE_H

/* clang-format off */
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef mq_close
#define mq_close posix_mq_close
#endif
int posix_mq_close(void);

#ifndef mq_getattr
#define mq_getattr posix_mq_getattr
#endif
int posix_mq_getattr(void);

#ifndef mq_notify
#define mq_notify posix_mq_notify
#endif
int posix_mq_notify(void);

#ifndef mq_open
#define mq_open posix_mq_open
#endif
int posix_mq_open(void);

#ifndef mq_receive
#define mq_receive posix_mq_receive
#endif
int posix_mq_receive(void);

#ifndef mq_send
#define mq_send posix_mq_send
#endif
int posix_mq_send(void);

#ifndef mq_unlink
#define mq_unlink posix_mq_unlink
#endif
int posix_mq_unlink(void);

#define __dependencies__

#define __include_next__

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MQUEUE_H */
