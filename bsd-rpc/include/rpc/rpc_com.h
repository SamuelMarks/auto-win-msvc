#ifndef RPC_COM_H
#define RPC_COM_H

/**
 * @file rpc_com.h
 * @brief SunRPC/TI-RPC internal helper declarations (<rpc/rpc_com.h>).
 */

/* clang-format off */
#include <netconfig.h>
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RPC_MAXDATASIZE
#define RPC_MAXDATASIZE 9000
#endif

struct __rpc_sockinfo {
  int si_af;
  int si_proto;
  int si_socktype;
  int si_alen;
};

/** @brief Create a socket file descriptor matching a netconfig entry. */
int __rpc_nconf2fd(const struct netconfig *nconf);
/** @brief Populate socket family/type/protocol info from a netconfig entry. */
int __rpc_nconf2sockinfo(const struct netconfig *nconf,
                         struct __rpc_sockinfo *sip);
/** @brief Convert universal address string to netbuf for address family af. */
struct netbuf *__rpc_uaddr2taddr_af(int af, const char *uaddr);
/** @brief Convert netbuf to universal address string for address family af. */
char *__rpc_taddr2uaddr_af(int af, const struct netbuf *nbuf);
/** @brief Broadcast RPC call with explicit initial and maximum wait timeouts.
 */
enum clnt_stat rpc_broadcast_exp(rpcprog_t prog, rpcvers_t vers, rpcproc_t proc,
                                 xdrproc_t xargs, caddr_t argsp,
                                 xdrproc_t xresults, caddr_t resultsp,
                                 resultproc_t eachresult, int inittime,
                                 int waittime, const char *nettype);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_COM_H */
