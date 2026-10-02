/* clang-format off */
#ifndef SYS_EVENTFD_H_
#define SYS_EVENTFD_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief eventfd() creates an "eventfd object" that can be used as an event  wait/notify mechanism by user-space applications, and by the  kernel to notify user-space applications of events. The object  contains an unsigned 64-bit integer (uint64_t) counter that is  maintained by the kernel. This counter is initialized with the  value specified in the argument initval.
 */
int eventfd(unsigned int initval, int flags);

#ifdef __cplusplus
}
#endif

#endif /* SYS_EVENTFD_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
