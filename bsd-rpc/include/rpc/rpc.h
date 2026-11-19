#ifndef RPC_RPC_H
#define RPC_RPC_H

/**
 * @file rpc.h
 * @brief Top-level SunRPC header (<rpc/rpc.h>).
 */

/* clang-format off */
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#include <rpc/auth.h>
#include <rpc/clnt.h>
#include <rpc/pmap_clnt.h>
#include <rpc/pmap_prot.h>
#include <rpc/rpcb_prot.h>
#include <rpc/rpcent.h>
/* clang-format on */

#ifndef RPC_TLIERROR
#define RPC_TLIERROR ((enum clnt_stat)20)
#endif
#ifndef RPC_N2AXLATEFAILURE
#define RPC_N2AXLATEFAILURE ((enum clnt_stat)22)
#endif
#ifndef RPC_RPCBFAILURE
#define RPC_RPCBFAILURE RPC_PMAPFAILURE
#endif
#ifndef CLSET_FD_CLOSE
#define CLSET_FD_CLOSE 8
#endif
#ifndef CLSET_FD_NCLOSE
#define CLSET_FD_NCLOSE 9
#endif
#ifndef CLSET_VERS
#define CLSET_VERS 10
#endif
#ifndef xdr_pmaplist_ptr
#define xdr_pmaplist_ptr xdr_pmaplist
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct netconfig;
struct netbuf;

/** @brief Convert transport-specific address to universal address string. */
char *taddr2uaddr(const struct netconfig *nconf, const struct netbuf *nbuf);
/** @brief Convert universal address string to transport-specific address. */
struct netbuf *uaddr2taddr(const struct netconfig *nconf, const char *uaddr);
/** @brief Broadcast an RPC call to all network transports of the given type. */
enum clnt_stat rpc_broadcast(rpcprog_t prog, rpcvers_t vers, rpcproc_t proc,
                             xdrproc_t xargs, caddr_t argsp, xdrproc_t xresults,
                             caddr_t resultsp, resultproc_t eachresult,
                             const char *nettype);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_RPC_H */
