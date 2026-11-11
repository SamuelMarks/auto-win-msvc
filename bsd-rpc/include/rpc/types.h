#ifndef RPC_TYPES_H
#define RPC_TYPES_H

/**
 * @file types.h
 * @brief SunRPC fundamental types (<rpc/types.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef int bool_t;
typedef int enum_t;
typedef uint32_t rpcprog_t;
typedef uint32_t rpcvers_t;
typedef uint32_t rpcproc_t;
typedef uint32_t rpcprot_t;
typedef uint32_t rpcport_t;
typedef int32_t rpc_inline_t;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define mem_alloc(bsize) calloc(1, (bsize))
#define mem_free(ptr, bsize) free((ptr))

struct netconfig {
  char *nc_netid;
  unsigned long nc_semantics;
  unsigned long nc_flag;
  char *nc_protofmly;
  char *nc_proto;
  char *nc_device;
  unsigned long nc_nlookups;
  char **nc_lookups;
};

struct netbuf {
  unsigned int maxlen;
  unsigned int len;
  void *buf;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_TYPES_H */
