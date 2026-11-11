#ifndef RPC_CLNT_H
#define RPC_CLNT_H

/**
 * @file clnt.h
 * @brief SunRPC client interface (<rpc/clnt.h>) polyfill.
 */

/* clang-format off */
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/auth.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

enum clnt_stat {
  RPC_SUCCESS = 0,
  RPC_CANTENCODEARGS = 1,
  RPC_CANTDECODERES = 2,
  RPC_CANTSEND = 3,
  RPC_CANTRECV = 4,
  RPC_TIMEDOUT = 5,
  RPC_VERSMISMATCH = 6,
  RPC_AUTHERROR = 7,
  RPC_PROGUNAVAIL = 8,
  RPC_PROGVERSMISMATCH = 9,
  RPC_PROCUNAVAIL = 10,
  RPC_CANTDECODEARGS = 11,
  RPC_SYSTEMERROR = 12,
  RPC_UNKNOWNHOST = 13,
  RPC_PMAPFAILURE = 14,
  RPC_PROGNOTREGISTERED = 15,
  RPC_FAILED = 16,
  RPC_UNKNOWNPROTO = 17
};

#define RPC_ANYSOCK -1

struct rpc_err {
  enum clnt_stat re_status;
  int re_errno;
};

typedef struct __rpc_client CLIENT;

struct clnt_ops {
  enum clnt_stat (*cl_call)(CLIENT *, rpcproc_t, xdrproc_t, void *, xdrproc_t,
                            void *, struct timeval);
  void (*cl_abort)(CLIENT *);
  void (*cl_geterr)(CLIENT *, struct rpc_err *);
  bool_t (*cl_freeres)(CLIENT *, xdrproc_t, void *);
  void (*cl_destroy)(CLIENT *);
  bool_t (*cl_control)(CLIENT *, u_int, void *);
};

struct __rpc_client {
  AUTH *cl_auth;
  const struct clnt_ops *cl_ops;
  void *cl_private;
  char *cl_netid;
  char *cl_tp;
};

#define CLSET_TIMEOUT 1
#define CLGET_TIMEOUT 2
#define CLSET_RETRY_TIMEOUT 4
#define CLGET_RETRY_TIMEOUT 5

#define NULLPROC ((rpcproc_t)0)

CLIENT *clnt_create(const char *hostname, rpcprog_t prog, rpcvers_t vers,
                    const char *proto);
CLIENT *clnt_tp_create(const char *hostname, rpcprog_t prog, rpcvers_t vers,
                       const struct netconfig *nconf);
CLIENT *clntudp_create(struct sockaddr_in *raddr, u_long program,
                       u_long version, struct timeval wait, int *sockp);
CLIENT *clnttcp_create(struct sockaddr_in *raddr, u_long prog, u_long vers,
                       int *sockp, u_int sendsz, u_int recvsz);
void clnt_pcreateerror(const char *s);
char *clnt_spcreateerror(const char *s);
void clnt_perrno(enum clnt_stat num);
void clnt_perror(CLIENT *rpch, const char *s);
char *clnt_sperror(CLIENT *rpch, const char *s);
char *clnt_sperrno(enum clnt_stat stat);
int callrpc(const char *host, int prognum, int versnum, int procnum,
            xdrproc_t inproc, void *in, xdrproc_t outproc, void *out);

#define clnt_call(rh, proc, xargs, argsp, xres, resp, secs)                    \
  ((*(rh)->cl_ops->cl_call)(rh, proc, xargs, argsp, xres, resp, secs))
#define clnt_freeres(rh, xres, resp)                                           \
  ((*(rh)->cl_ops->cl_freeres)(rh, xres, resp))
#define clnt_destroy(rh) ((*(rh)->cl_ops->cl_destroy)(rh))
#define clnt_control(cl, rq, in) ((*(cl)->cl_ops->cl_control)(cl, rq, in))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_CLNT_H */
