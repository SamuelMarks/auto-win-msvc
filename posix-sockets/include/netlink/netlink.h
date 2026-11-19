#ifndef NETLINK_NETLINK_H
#define NETLINK_NETLINK_H

/**
 * @file netlink.h
 * @brief FreeBSD Netlink protocol (<netlink/netlink.h>) definitions.
 */

/* clang-format off */
#include <sys/types.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AF_NETLINK
/** @brief Netlink socket address family. */
#define AF_NETLINK 38
#endif
#ifndef PF_NETLINK
/** @brief Netlink protocol family. */
#define PF_NETLINK AF_NETLINK
#endif

/** @brief Routing/device hook Netlink family. */
#define NETLINK_ROUTE 0
/** @brief Generic Netlink family. */
#define NETLINK_GENERIC 16

/** @brief Netlink socket address structure. */
struct sockaddr_nl {
  /** @brief Total structure length. */
  uint8_t nl_len;
  /** @brief AF_NETLINK family. */
  uint8_t nl_family;
  /** @brief Zero padding. */
  uint16_t nl_pad;
  /** @brief Port ID. */
  uint32_t nl_pid;
  /** @brief Multicast groups mask. */
  uint32_t nl_groups;
};

/** @brief Netlink message header. */
struct nlmsghdr {
  /** @brief Length of message including header. */
  uint32_t nlmsg_len;
  /** @brief Message content type. */
  uint16_t nlmsg_type;
  /** @brief Additional flags. */
  uint16_t nlmsg_flags;
  /** @brief Sequence number. */
  uint32_t nlmsg_seq;
  /** @brief Sending process port ID. */
  uint32_t nlmsg_pid;
};

/** @brief Netlink attribute header. */
struct nlattr {
  /** @brief Attribute length. */
  uint16_t nla_len;
  /** @brief Attribute type. */
  uint16_t nla_type;
};

/** @brief Request message flag. */
#define NLM_F_REQUEST 0x01
/** @brief Multipart message flag. */
#define NLM_F_MULTI 0x02
/** @brief Reply with ack flag. */
#define NLM_F_ACK 0x04
/** @brief Echo request flag. */
#define NLM_F_ECHO 0x08
/** @brief Return complete table flag. */
#define NLM_F_ROOT 0x100
/** @brief Return all matching entries flag. */
#define NLM_F_MATCH 0x200
/** @brief Dump request convenience mask. */
#define NLM_F_DUMP (NLM_F_ROOT | NLM_F_MATCH)

/** @brief Nothing message type. */
#define NLMSG_NOOP 0x1
/** @brief Error message type. */
#define NLMSG_ERROR 0x2
/** @brief End of multipart dump message type. */
#define NLMSG_DONE 0x3
/** @brief Data lost message type. */
#define NLMSG_OVERRUN 0x4

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETLINK_NETLINK_H */
