#ifndef NET_ETHERNET_H
#define NET_ETHERNET_H

/**
 * @file ethernet.h
 * @brief BSD Ethernet frame and address definitions (<net/ethernet.h>).
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

#define ETHER_ADDR_LEN 6
#define ETHER_TYPE_LEN 2
#define ETHER_CRC_LEN 4
#define ETHER_HDR_LEN (ETHER_ADDR_LEN * 2 + ETHER_TYPE_LEN)
#define ETHER_MIN_LEN 64
#define ETHER_MAX_LEN 1518
#define ETHERMTU (ETHER_MAX_LEN - ETHER_HDR_LEN - ETHER_CRC_LEN)

struct ether_addr {
  uint8_t octet[ETHER_ADDR_LEN];
};

struct ether_header {
  uint8_t ether_dhost[ETHER_ADDR_LEN];
  uint8_t ether_shost[ETHER_ADDR_LEN];
  uint16_t ether_type;
};

#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_REVARP 0x8035
#define ETHERTYPE_VLAN 0x8100
#define ETHERTYPE_IPV6 0x86dd
#define ETHERTYPE_LOOPBACK 0x9000

char *ether_ntoa(const struct ether_addr *addr);
struct ether_addr *ether_aton(const char *astr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NET_ETHERNET_H */
