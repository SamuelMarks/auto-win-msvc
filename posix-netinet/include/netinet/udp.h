#ifndef NETINET_UDP_H
#define NETINET_UDP_H

/**
 * @file udp.h
 * @brief BSD UDP packet header (<netinet/udp.h>).
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

struct udphdr {
  uint16_t uh_sport;
  uint16_t uh_dport;
  uint16_t uh_ulen;
  uint16_t uh_sum;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_UDP_H */
