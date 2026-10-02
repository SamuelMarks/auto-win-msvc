/* clang-format off */
#ifndef SYS_TIMERFD_H_
#define SYS_TIMERFD_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#include <sys/types.h>
#include <time.h>
struct itimerspec;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief These system calls create and operate on a timer that delivers  timer expiration notifications via a file descriptor. They  provide an alternative to the use of setitimer(2) or  timer_create(2), with the advantage that the file descriptor may  be monitored by select(2), poll(2), and epoll(7).
 */
int timerfd_create(int clockid, int flags);

/**
 * @brief These system calls create and operate on a timer that delivers  timer expiration notifications via a file descriptor. They  provide an alternative to the use of setitimer(2) or  timer_create(2), with the advantage that the file descriptor may  be monitored by select(2), poll(2), and epoll(7).
 */
int timerfd_gettime(int fd, struct itimerspec *curr_value);

/**
 * @brief These system calls create and operate on a timer that delivers  timer expiration notifications via a file descriptor. They  provide an alternative to the use of setitimer(2) or  timer_create(2), with the advantage that the file descriptor may  be monitored by select(2), poll(2), and epoll(7).
 */
int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);

#ifdef __cplusplus
}
#endif

#endif /* SYS_TIMERFD_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
