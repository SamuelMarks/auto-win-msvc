#ifndef NETLINK_NETLINK_H
#define NETLINK_NETLINK_H

/**
 * @file netlink.h
 * @brief FreeBSD Netlink protocol (<netlink/netlink.h>) definitions.
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

#ifndef AF_NETLINK
#define AF_NETLINK 38
#endif
#ifndef PF_NETLINK
#define PF_NETLINK AF_NETLINK
#endif

#define NETLINK_ROUTE 0
#define NETLINK_GENERIC 16

struct sockaddr_nl {
  uint8_t nl_len;
  uint8_t nl_family;
  uint16_t nl_pad;
  uint32_t nl_pid;
  uint32_t nl_groups;
};

struct nlmsghdr {
  uint32_t nlmsg_len;
  uint16_t nlmsg_type;
  uint16_t nlmsg_flags;
  uint32_t nlmsg_seq;
  uint32_t nlmsg_pid;
};

struct nlattr {
  uint16_t nla_len;
  uint16_t nla_type;
};

#define NLM_F_REQUEST 0x01
#define NLM_F_MULTI 0x02
#define NLM_F_ACK 0x04
#define NLM_F_ECHO 0x08
#define NLM_F_ROOT 0x100
#define NLM_F_MATCH 0x200
#define NLM_F_DUMP (NLM_F_ROOT | NLM_F_MATCH)

#define NLMSG_NOOP 0x1
#define NLMSG_ERROR 0x2
#define NLMSG_DONE 0x3
#define NLMSG_OVERRUN 0x4

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETLINK_NETLINK_H */
