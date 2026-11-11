#ifndef RPC_RPCENT_H
#define RPC_RPCENT_H

/**
 * @file rpcent.h
 * @brief SunRPC program number database (<rpc/rpcent.h>).
 */

#ifdef __cplusplus
extern "C" {
#endif

struct rpcent {
  char *r_name;
  char **r_aliases;
  int r_number;
};

struct rpcent *getrpcbyname(const char *name);
struct rpcent *getrpcbynumber(int number);
struct rpcent *getrpcent(void);
void setrpcent(int stayopen);
void endrpcent(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_RPCENT_H */
