/* netinet/icmp6.h - Strict C89 Header */
#ifndef POSIX_NETINET_ICMP6_H
#define POSIX_NETINET_ICMP6_H

/**
 * @file icmp6.h
 * @brief POSIX/BSD netinet/icmp6.h implementation for MSVC and Windows.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <netinet/icmp6.h>
#else
#include <netinet/icmp6.h>
#endif
#else
#include <netinet/in.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ICMPv6 packet header structure.
 */
struct icmp6_hdr {
  uint8_t icmp6_type;   /**< Type field */
  uint8_t icmp6_code;   /**< Code field */
  uint16_t icmp6_cksum; /**< Checksum field */
  union {
    uint32_t icmp6_un_data32[1]; /**< Type-specific 32-bit field */
    uint16_t icmp6_un_data16[2]; /**< Type-specific 16-bit field */
    uint8_t icmp6_un_data8[4];   /**< Type-specific 8-bit field */
  } icmp6_dataun;
};

#ifndef icmp6_data32
#define icmp6_data32 icmp6_dataun.icmp6_un_data32
#define icmp6_data16 icmp6_dataun.icmp6_un_data16
#define icmp6_data8 icmp6_dataun.icmp6_un_data8
#define icmp6_pptr icmp6_data32[0]
#define icmp6_mtu icmp6_data32[0]
#define icmp6_id icmp6_data16[0]
#define icmp6_seq icmp6_data16[1]
#define icmp6_maxdelay icmp6_data16[0]
#endif

#ifndef ICMP6_DST_UNREACH
#define ICMP6_DST_UNREACH 1
#define ICMP6_PACKET_TOO_BIG 2
#define ICMP6_TIME_EXCEEDED 3
#define ICMP6_PARAM_PROB 4
#define ICMP6_ECHO_REQUEST 128
#define ICMP6_ECHO_REPLY 129
#define ICMP6_MEMBERSHIP_QUERY 130
#define ICMP6_MEMBERSHIP_REPORT 131
#define ICMP6_MEMBERSHIP_REDUCTION 132
#define MLD_LISTENER_QUERY 130
#define MLD_LISTENER_REPORT 131
#define MLD_LISTENER_REDUCTION 132
#define ND_ROUTER_SOLICIT 133
#define ND_ROUTER_ADVERT 134
#define ND_NEIGHBOR_SOLICIT 135
#define ND_NEIGHBOR_ADVERT 136
#define ND_REDIRECT 137
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WIN32 */

#endif /* POSIX_NETINET_ICMP6_H */
