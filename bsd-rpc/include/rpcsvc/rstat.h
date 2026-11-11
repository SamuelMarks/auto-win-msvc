#ifndef RPCSVC_RSTAT_H
#define RPCSVC_RSTAT_H

/**
 * @file rstat.h
 * @brief Remote statistics RPC protocol (<rpcsvc/rstat.h>).
 */

/* clang-format off */
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define RSTATPROG 100001UL
#define RSTATVERS_TIME 3UL
#define RSTATPROC_STATS 1UL

struct rstat_timeval {
  u_int tv_sec;
  u_int tv_usec;
};

struct statstime {
  int cp_time[4];
  int dk_xfer[4];
  u_int v_pgpgin;
  u_int v_pgpgout;
  u_int v_pswpin;
  u_int v_pswpout;
  u_int v_intr;
  int if_ipackets;
  int if_ierrors;
  int if_oerrors;
  int if_collisions;
  u_int v_swtch;
  int avenrun[3];
  struct rstat_timeval boottime;
  struct rstat_timeval curtime;
  int if_opackets;
};

bool_t xdr_statstime(XDR *xdrs, struct statstime *objp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_RSTAT_H */
