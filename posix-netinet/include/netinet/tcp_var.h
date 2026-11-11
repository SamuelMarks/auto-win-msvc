#ifndef NETINET_TCP_VAR_H
#define NETINET_TCP_VAR_H

/**
 * @file tcp_var.h
 * @brief FreeBSD kernel TCP variables and statistics (<netinet/tcp_var.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <netinet/in_pcb.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct xtcpcb {
  size_t xt_len;
  struct xinpcb xt_inp;
  int t_state;
  unsigned int t_flags;
  int32_t t_sndzerowin;
  uint32_t t_rcvtime;
  uint32_t t_starttime;
  uint32_t t_rtttime;
  uint32_t snd_max;
  uint32_t snd_nxt;
  uint32_t snd_up;
  uint32_t snd_wnd;
  uint32_t snd_cwnd;
  uint32_t snd_ssthresh;
  uint32_t rcv_wnd;
  uint32_t rcv_nxt;
  uint32_t rcv_up;
  uint32_t t_maxseg;
};

struct tcpstat {
  uint64_t tcps_connattempt;
  uint64_t tcps_accepts;
  uint64_t tcps_connects;
  uint64_t tcps_drops;
  uint64_t tcps_sndtotal;
  uint64_t tcps_rcvtotal;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_TCP_VAR_H */
