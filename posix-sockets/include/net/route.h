#ifndef NET_ROUTE_H
#define NET_ROUTE_H

/**
 * @file route.h
 * @brief BSD kernel routing table (<net/route.h>) definitions.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/socket.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define RTF_UP 0x1
#define RTF_GATEWAY 0x2
#define RTF_HOST 0x4
#define RTF_REJECT 0x8
#define RTF_DYNAMIC 0x10
#define RTF_MODIFIED 0x20
#define RTF_DONE 0x40
#define RTF_STATIC 0x800
#define RTF_BLACKHOLE 0x1000

#define RTM_VERSION 5
#define RTM_ADD 0x1
#define RTM_DELETE 0x2
#define RTM_CHANGE 0x3
#define RTM_GET 0x4
#define RTM_LOSING 0x5
#define RTM_REDIRECT 0x6
#define RTM_MISS 0x7
#define RTM_IFINFO 0xe
#define RTM_NEWADDR 0xc
#define RTM_DELADDR 0xd

#define RTA_DST 0x1
#define RTA_GATEWAY 0x2
#define RTA_NETMASK 0x4
#define RTA_GENMASK 0x8
#define RTA_IFP 0x10
#define RTA_IFA 0x20
#define RTA_AUTHOR 0x40
#define RTA_BRD 0x80

#define RTAX_DST 0
#define RTAX_GATEWAY 1
#define RTAX_NETMASK 2
#define RTAX_GENMASK 3
#define RTAX_IFP 4
#define RTAX_IFA 5
#define RTAX_AUTHOR 6
#define RTAX_BRD 7
#define RTAX_MAX 8

struct rt_metrics {
  unsigned long rmx_locks;
  unsigned long rmx_mtu;
  unsigned long rmx_hopcount;
  unsigned long rmx_expire;
  unsigned long rmx_recvpipe;
  unsigned long rmx_sendpipe;
  unsigned long rmx_ssthresh;
  unsigned long rmx_rtt;
  unsigned long rmx_rttvar;
  unsigned long rmx_pksent;
  unsigned long rmx_weight;
  unsigned long rmx_nhidx;
  unsigned long rmx_filler[2];
};

struct rt_msghdr {
  unsigned short rtm_msglen;
  unsigned char rtm_version;
  unsigned char rtm_type;
  unsigned short rtm_index;
  int rtm_flags;
  int rtm_addrs;
  pid_t rtm_pid;
  int rtm_seq;
  int rtm_errno;
  int rtm_fmask;
  unsigned long rtm_inits;
  struct rt_metrics rtm_rmx;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NET_ROUTE_H */
