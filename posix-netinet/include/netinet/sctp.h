#ifndef NETINET_SCTP_H
#define NETINET_SCTP_H

/**
 * @file sctp.h
 * @brief FreeBSD SCTP protocol definitions (<netinet/sctp.h>).
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

#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP 132
#endif

typedef uint32_t sctp_assoc_t;

struct sctphdr {
  uint16_t src_port;
  uint16_t dest_port;
  uint32_t v_tag;
  uint32_t checksum;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_SCTP_H */
