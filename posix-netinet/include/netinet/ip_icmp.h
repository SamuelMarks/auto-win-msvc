/* netinet/ip_icmp.h - Strict C89 Header */
#ifndef POSIX_NETINET_IP_ICMP_H
#define POSIX_NETINET_IP_ICMP_H

/**
 * @file ip_icmp.h
 * @brief POSIX/BSD netinet/ip_icmp.h implementation for MSVC and Windows.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <netinet/ip_icmp.h>
#else
#include <netinet/ip_icmp.h>
#endif
#else
#include <netinet/in.h>
#include <netinet/ip.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ICMP_MINLEN
/** @brief Minimum ICMP packet length. */
#define ICMP_MINLEN 8
#endif

#ifndef ICMP_ECHOREPLY
#define ICMP_ECHOREPLY 0
#define ICMP_DEST_UNREACH 3
#define ICMP_SOURCE_QUENCH 4
#define ICMP_REDIRECT 5
#define ICMP_ECHO 8
#define ICMP_TIME_EXCEEDED 11
#define ICMP_PARAMETERPROB 12
#define ICMP_TIMESTAMP 13
#define ICMP_TIMESTAMPREPLY 14
#define ICMP_INFO_REQUEST 15
#define ICMP_INFO_REPLY 16
#define ICMP_ADDRESS 17
#define ICMP_ADDRESSREPLY 18
#endif

/**
 * @brief BSD/Linux compatible ICMP packet structure.
 */
struct icmp {
  uint8_t icmp_type;   /**< Type of message */
  uint8_t icmp_code;   /**< Type sub code */
  uint16_t icmp_cksum; /**< Ones complement checksum */
  union {
    uint8_t ih_pptr;          /**< ICMP_PARAMPROB */
    struct in_addr ih_gwaddr; /**< Gateway address */
    struct {
      uint16_t icd_id;  /**< Identifier */
      uint16_t icd_seq; /**< Sequence number */
    } ih_idseq;
    uint32_t ih_void;
  } icmp_hun;
  union {
    struct {
      uint32_t its_otime;
      uint32_t its_rtime;
      uint32_t its_ttime;
    } id_ts;
    uint32_t id_mask;
    uint8_t id_data[1];
  } icmp_dun;
};

#ifndef icmp_pptr
#define icmp_pptr icmp_hun.ih_pptr
#define icmp_gwaddr icmp_hun.ih_gwaddr
#define icmp_id icmp_hun.ih_idseq.icd_id
#define icmp_seq icmp_hun.ih_idseq.icd_seq
#define icmp_void icmp_hun.ih_void
#define icmp_otime icmp_dun.id_ts.its_otime
#define icmp_rtime icmp_dun.id_ts.its_rtime
#define icmp_ttime icmp_dun.id_ts.its_ttime
#define icmp_mask icmp_dun.id_mask
#define icmp_data icmp_dun.id_data
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WIN32 */

#endif /* POSIX_NETINET_IP_ICMP_H */
