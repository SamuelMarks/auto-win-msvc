#ifndef RPC_PMAP_CLNT_H
#define RPC_PMAP_CLNT_H

/**
 * @file pmap_clnt.h
 * @brief SunRPC portmapper client (<rpc/pmap_clnt.h>).
 */

/* clang-format off */
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/clnt.h>
#include <rpc/pmap_prot.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef bool_t (*resultproc_t)(caddr_t, struct sockaddr_in *);

bool_t pmap_set(u_long program, u_long version, int protocol, u_short port);
bool_t pmap_unset(u_long program, u_long version);
struct pmaplist *pmap_getmaps(struct sockaddr_in *address);
u_short pmap_getport(struct sockaddr_in *address, u_long program,
                     u_long version, u_int protocol);
enum clnt_stat clnt_broadcast(u_long prog, u_long vers, u_long proc,
                              xdrproc_t xargs, void *argsp, xdrproc_t xresults,
                              void *resultsp, resultproc_t eachresult);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_PMAP_CLNT_H */
