#ifndef NETINET_IP6_H
#define NETINET_IP6_H

/**
 * @file ip6.h
 * @brief BSD IPv6 packet header definitions (<netinet/ip6.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <netinet/in.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define IPV6_VERSION 0x60
#define IPV6_VERSION_MASK 0xf0

struct ip6_hdr {
  union {
    struct ip6_hdrctl {
      uint32_t ip6_un1_flow;
      uint16_t ip6_un1_plen;
      uint8_t ip6_un1_nxt;
      uint8_t ip6_un1_hlim;
    } ip6_un1;
    uint8_t ip6_un2_vfc;
  } ip6_ctlun;
  struct in6_addr ip6_src;
  struct in6_addr ip6_dst;
};

#define ip6_vfc ip6_ctlun.ip6_un2_vfc
#define ip6_flow ip6_ctlun.ip6_un1.ip6_un1_flow
#define ip6_plen ip6_ctlun.ip6_un1.ip6_un1_plen
#define ip6_nxt ip6_ctlun.ip6_un1.ip6_un1_nxt
#define ip6_hlim ip6_ctlun.ip6_un1.ip6_un1_hlim
#define ip6_hops ip6_ctlun.ip6_un1.ip6_un1_hlim

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_IP6_H */
