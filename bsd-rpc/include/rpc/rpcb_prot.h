#ifndef RPC_RPCB_PROT_H
#define RPC_RPCB_PROT_H

/**
 * @file rpcb_prot.h
 * @brief SunRPC rpcbind protocol (<rpc/rpcb_prot.h>).
 */

/* clang-format off */
#include <rpc/xdr.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define RPCBPROG 100000UL
#define RPCBVERS 3UL
#define RPCBVERS4 4UL

struct rpcb {
  rpcprog_t r_prog;
  rpcvers_t r_vers;
  char *r_netid;
  char *r_addr;
  char *r_owner;
};
typedef struct rpcb RPCB;

struct rpcblist {
  RPCB rpcb_map;
  struct rpcblist *rpcb_next;
};
typedef struct rpcblist *rpcblist_ptr;

bool_t xdr_rpcb(XDR *xdrs, RPCB *objp);
bool_t xdr_rpcblist_ptr(XDR *xdrs, rpcblist_ptr *objp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_RPCB_PROT_H */
