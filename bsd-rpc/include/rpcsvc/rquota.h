#ifndef RPCSVC_RQUOTA_H
#define RPCSVC_RQUOTA_H

/**
 * @file rquota.h
 * @brief Remote quota RPC protocol (<rpcsvc/rquota.h>).
 */

/* clang-format off */
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define RQUOTAPROG 100011UL
#define RQUOTAVERS 1UL
#define RQUOTAPROC_GETQUOTA 1UL
#define RQUOTAPROC_GETACTIVEQUOTA 2UL

struct getquota_args {
  char *gqa_pathp;
  int gqa_uid;
};

struct rquota {
  int rq_bsize;
  bool_t rq_active;
  u_int rq_bhardlimit;
  u_int rq_bsoftlimit;
  u_int rq_curblocks;
  u_int rq_fhardlimit;
  u_int rq_fsoftlimit;
  u_int rq_curfiles;
  u_int rq_btimeleft;
  u_int rq_ftimeleft;
};

enum gqr_status { Q_OK = 1, Q_NOQUOTA = 2, Q_EPERM = 3 };

struct getquota_rslt {
  enum gqr_status status;
  union {
    struct rquota gqr_rquota;
  } getquota_rslt_u;
};

bool_t xdr_getquota_args(XDR *xdrs, struct getquota_args *objp);
bool_t xdr_getquota_rslt(XDR *xdrs, struct getquota_rslt *objp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_RQUOTA_H */
