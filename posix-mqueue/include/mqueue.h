#ifndef POSIX_MQUEUE_H
#define POSIX_MQUEUE_H

/**
 * @file mqueue.h
 * @brief POSIX <mqueue.h> passthrough on Linux/FreeBSD and portable polyfill on
 * macOS/Windows/DOS.
 */

/* clang-format off */
#if (defined(__linux__) || defined(__FreeBSD__)) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <mqueue.h>
#else
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if !((defined(__linux__) || defined(__FreeBSD__)) &&                          \
      (defined(__GNUC__) || defined(__clang__)))

typedef int mqd_t;

#ifndef MQ_PRIO_MAX
#define MQ_PRIO_MAX 32
#endif

struct sigevent;

/** @brief POSIX message queue attributes structure. */
struct mq_attr {
  long mq_flags;
  long mq_maxmsg;
  long mq_msgsize;
  long mq_curmsgs;
};

/** @brief Open or create a POSIX message queue. */
mqd_t mq_open(const char *name, int oflag, ...);
/** @brief Close a POSIX message queue descriptor. */
int mq_close(mqd_t mqdes);
/** @brief Remove a POSIX message queue name. */
int mq_unlink(const char *name);
/** @brief Retrieve message queue attributes. */
int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat);
/** @brief Set message queue attributes. */
int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat,
               struct mq_attr *omqstat);
/** @brief Send a message to a message queue. */
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
            unsigned int msg_prio);
/** @brief Send a message to a message queue with timeout. */
int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
                 unsigned int msg_prio, const struct timespec *abs_timeout);
/** @brief Receive a message from a message queue. */
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
                   unsigned int *msg_prio);
/** @brief Receive a message from a message queue with timeout. */
ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
                        unsigned int *msg_prio,
                        const struct timespec *abs_timeout);
/** @brief Register asynchronous notification on message arrival. */
int mq_notify(mqd_t mqdes, const struct sigevent *notification);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_MQUEUE_H */
/* API Contract symbols: __dependencies__, __include_next__ */
