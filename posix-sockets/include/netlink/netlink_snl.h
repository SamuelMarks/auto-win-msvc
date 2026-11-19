#ifndef NETLINK_NETLINK_SNL_H
#define NETLINK_NETLINK_SNL_H

/**
 * @file netlink_snl.h
 * @brief FreeBSD Simple Netlink library (<netlink/netlink_snl.h>) polyfill.
 */

/* clang-format off */
#include <netlink/netlink.h>
#include <stddef.h>
#if !defined(__cplusplus) && defined(__STDC_VERSION__) &&                      \
    (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#elif !defined(__cplusplus)
#ifndef __bool_true_false_are_defined
/** @brief Indicator that bool, true, and false macros are defined. */
#define __bool_true_false_are_defined 1
/** @brief C89 boolean fallback type. */
#define bool unsigned char
/** @brief Boolean true constant. */
#define true 1
/** @brief Boolean false constant. */
#define false 0
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Simple Netlink session state. */
struct snl_state {
  /** @brief Netlink socket descriptor. */
  int fd;
  /** @brief Next message sequence number. */
  uint32_t seq;
  /** @brief Peer port ID. */
  uint32_t peer_pid;
};

/** @brief Simple Netlink linear message writer. */
struct snl_writer {
  /** @brief Backing buffer base pointer. */
  char *base;
  /** @brief Current write offset. */
  uint32_t offset;
  /** @brief Total buffer capacity. */
  uint32_t size;
  /** @brief Active Netlink message header. */
  struct nlmsghdr *hdr;
  /** @brief Associated SNL session state. */
  struct snl_state *ss;
  /** @brief Sticky write error flag. */
  bool error;
};

/** @brief Netlink reply error details. */
struct snl_errmsg_data {
  /** @brief Original request header. */
  struct nlmsghdr *orig_hdr;
  /** @brief Errno code reported by kernel. */
  int error;
  /** @brief Byte offset of offending attribute. */
  uint32_t error_offs;
  /** @brief Optional extended error message string. */
  const char *error_str;
};

/** @brief Declarative Netlink attribute parser descriptor. */
struct snl_attr_parser {
  /** @brief Attribute type ID. */
  uint16_t type;
  /** @brief Target struct field byte offset. */
  uint16_t off;
  /** @brief Parser callback function pointer. */
  void *cb;
  /** @brief Optional callback argument. */
  const void *arg;
};

/** @brief Declarative Netlink header and attribute parser descriptor. */
struct snl_hdr_parser {
  /** @brief Expected input header size. */
  size_t in_hdr_size;
  /** @brief Target output structure size. */
  size_t out_size;
  /** @brief Field parser array length. */
  size_t fp_size;
  /** @brief Attribute parser array length. */
  size_t ap_size;
  /** @brief Field parser table pointer. */
  const void *fp;
  /** @brief Attribute parser table pointer. */
  const struct snl_attr_parser *ap;
};

/** @brief Initializes a Simple Netlink state structure (stub). */
static __inline bool snl_init(struct snl_state *ss, int netlink_family) {
  (void)netlink_family;
  if (ss != NULL) {
    ss->fd = -1;
    ss->seq = 1;
    ss->peer_pid = 0;
  }
  return false;
}

/** @brief Releases resources held by a Simple Netlink state structure. */
static __inline void snl_free(struct snl_state *ss) { (void)ss; }
/** @brief Resets scratch linear buffer in a Simple Netlink state structure. */
static __inline void snl_clear_lb(struct snl_state *ss) { (void)ss; }

/** @brief Initializes a Simple Netlink message writer. */
static __inline void snl_init_writer(struct snl_state *ss,
                                     struct snl_writer *nw) {
  if (nw != NULL) {
    nw->ss = ss;
    nw->error = true;
  }
}

/** @brief Begins constructing a Netlink request message. */
static __inline struct nlmsghdr *snl_create_msg_request(struct snl_writer *nw,
                                                        int nlmsg_type) {
  (void)nw;
  (void)nlmsg_type;
  return NULL;
}

/** @brief Finalizes a Netlink message in the writer buffer. */
static __inline bool snl_finalize_msg(struct snl_writer *nw) {
  (void)nw;
  return false;
}

/** @brief Sends a finalized Netlink message over the session socket. */
static __inline bool snl_send_message(struct snl_state *ss,
                                      struct nlmsghdr *hdr) {
  (void)ss;
  (void)hdr;
  return false;
}

/** @brief Reads next message from a multipart Netlink reply sequence. */
static __inline struct nlmsghdr *
snl_read_reply_multi(struct snl_state *ss, uint32_t nlmsg_seq,
                     struct snl_errmsg_data *e) {
  (void)ss;
  (void)nlmsg_seq;
  (void)e;
  return NULL;
}

/** @brief Parses a Netlink message into target structure using parser tables.
 */
static __inline bool snl_parse_nlmsg(struct snl_state *ss, struct nlmsghdr *hdr,
                                     const struct snl_hdr_parser *ps,
                                     void *target) {
  (void)ss;
  (void)hdr;
  (void)ps;
  (void)target;
  return false;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETLINK_NETLINK_SNL_H */
