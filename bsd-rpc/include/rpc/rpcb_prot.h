#ifndef RPC_RPCB_PROT_H
#define RPC_RPCB_PROT_H

/**
 * @file rpcb_prot.h
 * @brief SunRPC rpcbind v3/v4 protocol (<rpc/rpcb_prot.h>).
 */

/* clang-format off */
#include <rpc/types.h>
#include <rpc/xdr.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define RPCBPROG 100000UL
#define RPCBVERS 3UL
#define RPCBVERS4 4UL
#define RPCBVERS_3 RPCBVERS
#define RPCBVERS_4 RPCBVERS4

#define RPCBPROC_NULL 0UL
#define RPCBPROC_SET 1UL
#define RPCBPROC_UNSET 2UL
#define RPCBPROC_GETADDR 3UL
#define RPCBPROC_DUMP 4UL
#define RPCBPROC_CALLIT 5UL
#define RPCBPROC_BCAST RPCBPROC_CALLIT
#define RPCBPROC_GETTIME 6UL
#define RPCBPROC_UADDR2TADDR 7UL
#define RPCBPROC_TADDR2UADDR 8UL
#define RPCBPROC_GETVERSADDR 9UL
#define RPCBPROC_INDIRECT 10UL
#define RPCBPROC_GETADDRLIST 11UL
#define RPCBPROC_GETSTAT 12UL

#define rpcb_highproc_2 RPCBPROC_CALLIT
#define rpcb_highproc_3 RPCBPROC_TADDR2UADDR
#define rpcb_highproc_4 RPCBPROC_GETSTAT
#define RPCBSTAT_HIGHPROC 13
#define RPCBVERS_STAT 3
#define RPCBVERS_4_STAT 2
#define RPCBVERS_3_STAT 1
#define RPCBVERS_2_STAT 0

#ifndef _PATH_RPCBINDSOCK
#define _PATH_RPCBINDSOCK "/var/run/rpcbind.sock"
#endif

struct rpcb {
  rpcprog_t r_prog;
  rpcvers_t r_vers;
  char *r_netid;
  char *r_addr;
  char *r_owner;
};
typedef struct rpcb RPCB;
typedef struct rpcb rpcb;

struct rpcblist {
  RPCB rpcb_map;
  struct rpcblist *rpcb_next;
};
typedef struct rpcblist RPCBLIST;
typedef struct rpcblist rpcblist;
typedef struct rpcblist *rpcblist_ptr;

struct rpcb_rmtcallargs {
  rpcprog_t prog;
  rpcvers_t vers;
  rpcproc_t proc;
  uint32_t arglen;
  caddr_t args_ptr;
  xdrproc_t xdr_args;
};
typedef struct rpcb_rmtcallargs rpcb_rmtcallargs;

struct rpcb_rmtcallres {
  char *addr_ptr;
  uint32_t resultslen;
  caddr_t results_ptr;
  xdrproc_t xdr_results;
};
typedef struct rpcb_rmtcallres rpcb_rmtcallres;

struct rpcb_entry {
  char *r_maddr;
  char *r_nc_netid;
  unsigned int r_nc_semantics;
  char *r_nc_protofmly;
  char *r_nc_proto;
};
typedef struct rpcb_entry rpcb_entry;

struct rpcb_entry_list {
  rpcb_entry rpcb_entry_map;
  struct rpcb_entry_list *rpcb_entry_next;
};
typedef struct rpcb_entry_list rpcb_entry_list;
typedef rpcb_entry_list *rpcb_entry_list_ptr;

struct rpcbs_addrlist {
  rpcprog_t prog;
  rpcvers_t vers;
  int success;
  int failure;
  char *netid;
  struct rpcbs_addrlist *next;
};
typedef struct rpcbs_addrlist rpcbs_addrlist;
typedef rpcbs_addrlist *rpcbs_addrlist_ptr;

struct rpcbs_rmtcalllist {
  rpcprog_t prog;
  rpcvers_t vers;
  rpcproc_t proc;
  int success;
  int failure;
  int indirect;
  char *netid;
  struct rpcbs_rmtcalllist *next;
};
typedef struct rpcbs_rmtcalllist rpcbs_rmtcalllist;
typedef rpcbs_rmtcalllist *rpcbs_rmtcalllist_ptr;

typedef int rpcbs_proc[RPCBSTAT_HIGHPROC];

struct rpcb_stat {
  rpcbs_proc info;
  int setinfo;
  int unsetinfo;
  rpcbs_addrlist_ptr addrinfo;
  rpcbs_rmtcalllist_ptr rmtinfo;
};
typedef struct rpcb_stat rpcb_stat;
typedef rpcb_stat rpcb_stat_byvers[RPCBVERS_STAT];

bool_t xdr_rpcb(XDR *xdrs, RPCB *objp);
bool_t xdr_rpcblist_ptr(XDR *xdrs, rpcblist_ptr *objp);
bool_t xdr_rpcb_rmtcallargs(XDR *xdrs, rpcb_rmtcallargs *objp);
bool_t xdr_rpcb_rmtcallres(XDR *xdrs, rpcb_rmtcallres *objp);
bool_t xdr_rpcb_entry_list_ptr(XDR *xdrs, rpcb_entry_list_ptr *objp);
bool_t xdr_rpcb_stat_byvers(XDR *xdrs, rpcb_stat_byvers objp);
bool_t xdr_netbuf(XDR *xdrs, struct netbuf *objp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_RPCB_PROT_H */
