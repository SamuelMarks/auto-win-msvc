#ifndef RPC_RPCENT_H
#define RPC_RPCENT_H

/**
 * @file rpcent.h
 * @brief SunRPC program number database (<rpc/rpcent.h>).
 */

/* clang-format off */
#if defined(__APPLE__)
#include <netdb.h>
#elif defined(__GLIBC__) && defined(__has_include)
#if __has_include(<rpc/netdb.h>)
#include <rpc/netdb.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__APPLE__) && !defined(_RPC_NETDB_H)
struct rpcent {
  char *r_name;
  char **r_aliases;
  int r_number;
};

struct rpcent *getrpcbyname(const char *rpc_name);
struct rpcent *getrpcbynumber(int number);
struct rpcent *getrpcent(void);
void setrpcent(int stayopen);
void endrpcent(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_RPCENT_H */
