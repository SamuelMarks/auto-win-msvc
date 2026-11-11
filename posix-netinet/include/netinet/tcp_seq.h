#ifndef NETINET_TCP_SEQ_H
#define NETINET_TCP_SEQ_H

/**
 * @file tcp_seq.h
 * @brief BSD TCP sequence number macros (<netinet/tcp_seq.h>).
 */

/* clang-format off */
#include <sys/types.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t tcp_seq;

#define SEQ_LT(a, b) ((int)((a) - (b)) < 0)
#define SEQ_LEQ(a, b) ((int)((a) - (b)) <= 0)
#define SEQ_GT(a, b) ((int)((a) - (b)) > 0)
#define SEQ_GEQ(a, b) ((int)((a) - (b)) >= 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_TCP_SEQ_H */
