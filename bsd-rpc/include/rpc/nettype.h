#ifndef RPC_NETTYPE_H
#define RPC_NETTYPE_H

/**
 * @file nettype.h
 * @brief TI-RPC network type helpers (<rpc/nettype.h>) polyfill.
 */

/* clang-format off */
#include <netconfig.h>
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define _RPC_NONE 0
#define _RPC_NETPATH 1
#define _RPC_VISIBLE 2
#define _RPC_CIRCUIT_V 3
#define _RPC_DATAGRAM_V 4
#define _RPC_CIRCUIT_N 5
#define _RPC_DATAGRAM_N 6
#define _RPC_TCP 7
#define _RPC_UDP 8

/** @brief Open network type configuration iteration handle. */
void *__rpc_setconf(const char *nettype);
/** @brief Close network type configuration iteration handle. */
void __rpc_endconf(void *vhandle);
/** @brief Retrieve next matching netconfig entry for nettype handle. */
struct netconfig *__rpc_getconf(void *vhandle);
/** @brief Retrieve preferred IPv4/IPv6 netconfig entry for nettype. */
struct netconfig *__rpc_getconfip(const char *nettype);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_NETTYPE_H */
