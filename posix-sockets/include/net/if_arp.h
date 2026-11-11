#ifndef NET_IF_ARP_H
#define NET_IF_ARP_H

/**
 * @file if_arp.h
 * @brief BSD Address Resolution Protocol (<net/if_arp.h>) definitions.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/socket.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define ARPHRD_ETHER 1
#define ARPHRD_IEEE802 6
#define ARPHRD_FRELAY 15
#define ARPHRD_IEEE1394 24

#define ARPOP_REQUEST 1
#define ARPOP_REPLY 2
#define ARPOP_REVREQUEST 3
#define ARPOP_REVREPLY 4

struct arphdr {
  unsigned short ar_hrd;
  unsigned short ar_pro;
  unsigned char ar_hln;
  unsigned char ar_pln;
  unsigned short ar_op;
};

struct arpreq {
  struct sockaddr arp_pa;
  struct sockaddr arp_ha;
  int arp_flags;
};

#define ATF_INUSE 0x01
#define ATF_COM 0x02
#define ATF_PERM 0x04
#define ATF_PUBL 0x08
#define ATF_USETRAILERS 0x10

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NET_IF_ARP_H */
