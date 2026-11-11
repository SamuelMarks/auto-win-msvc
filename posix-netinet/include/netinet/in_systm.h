#ifndef NETINET_IN_SYSTM_H
#define NETINET_IN_SYSTM_H

/**
 * @file in_systm.h
 * @brief BSD network system types (<netinet/in_systm.h>).
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

typedef uint16_t n_short;
typedef uint32_t n_long;
typedef uint32_t n_time;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_IN_SYSTM_H */
