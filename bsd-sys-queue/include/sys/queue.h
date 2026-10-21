/* clang-format off */
#ifndef SYS_QUEUE_H_
#define SYS_QUEUE_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#if (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#pragma GCC system_header
#include_next <sys/queue.h>
#elif !defined(_WIN32)
#include <sys/queue.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_QUEUE_H_ */
/* clang-format on */

/* API Contract symbols: __include_next__ */
