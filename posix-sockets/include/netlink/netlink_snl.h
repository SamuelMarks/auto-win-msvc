#ifndef NETLINK_NETLINK_SNL_H
#define NETLINK_NETLINK_SNL_H

/**
 * @file netlink_snl.h
 * @brief FreeBSD Simple Netlink library (<netlink/netlink_snl.h>) polyfill.
 */

/* clang-format off */
#include <netlink/netlink.h>
#include <stdbool.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct snl_state {
  int fd;
  uint32_t seq;
  uint32_t peer_pid;
};

struct snl_writer {
  char *base;
  uint32_t offset;
  uint32_t size;
  struct nlmsghdr *hdr;
  struct snl_state *ss;
  bool error;
};

struct snl_errmsg_data {
  struct nlmsghdr *orig_hdr;
  int error;
  uint32_t error_offs;
  const char *error_str;
};

struct snl_attr_parser {
  uint16_t type;
  uint16_t off;
  void *cb;
  const void *arg;
};

struct snl_hdr_parser {
  size_t in_hdr_size;
  size_t out_size;
  size_t fp_size;
  size_t ap_size;
  const void *fp;
  const struct snl_attr_parser *ap;
};

static __inline bool snl_init(struct snl_state *ss, int netlink_family) {
  (void)netlink_family;
  if (ss != NULL) {
    ss->fd = -1;
    ss->seq = 1;
    ss->peer_pid = 0;
  }
  return false;
}

static __inline void snl_free(struct snl_state *ss) { (void)ss; }
static __inline void snl_clear_lb(struct snl_state *ss) { (void)ss; }

static __inline void snl_init_writer(struct snl_state *ss,
                                     struct snl_writer *nw) {
  if (nw != NULL) {
    nw->ss = ss;
    nw->error = true;
  }
}

static __inline struct nlmsghdr *snl_create_msg_request(struct snl_writer *nw,
                                                        int nlmsg_type) {
  (void)nw;
  (void)nlmsg_type;
  return NULL;
}

static __inline bool snl_finalize_msg(struct snl_writer *nw) {
  (void)nw;
  return false;
}

static __inline bool snl_send_message(struct snl_state *ss,
                                      struct nlmsghdr *hdr) {
  (void)ss;
  (void)hdr;
  return false;
}

static __inline struct nlmsghdr *
snl_read_reply_multi(struct snl_state *ss, uint32_t nlmsg_seq,
                     struct snl_errmsg_data *e) {
  (void)ss;
  (void)nlmsg_seq;
  (void)e;
  return NULL;
}

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
