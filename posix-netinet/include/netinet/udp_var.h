#ifndef NETINET_UDP_VAR_H
#define NETINET_UDP_VAR_H

/**
 * @file udp_var.h
 * @brief FreeBSD kernel UDP statistics (<netinet/udp_var.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <netinet/udp.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct udpstat {
  uint64_t udps_ipackets;
  uint64_t udps_hdrops;
  uint64_t udps_badsum;
  uint64_t udps_badlen;
  uint64_t udps_noport;
  uint64_t udps_noportbcast;
  uint64_t udps_fullsock;
  uint64_t udps_opackets;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_UDP_VAR_H */
