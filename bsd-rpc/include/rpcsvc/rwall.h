#ifndef RPCSVC_RWALL_H
#define RPCSVC_RWALL_H

/**
 * @file rwall.h
 * @brief Remote wall RPC protocol (<rpcsvc/rwall.h>).
 */

/* clang-format off */
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define WALLPROG 100008UL
#define WALLVERS 1UL
#define WALLPROC_WALL 2UL

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_RWALL_H */
