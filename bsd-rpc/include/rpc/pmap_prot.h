#ifndef RPC_PMAP_PROT_H
#define RPC_PMAP_PROT_H

/**
 * @file pmap_prot.h
 * @brief SunRPC portmapper protocol (<rpc/pmap_prot.h>).
 */

/* clang-format off */
#include <rpc/xdr.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define PMAPPORT 111
#define PMAPPROG 100000UL
#define PMAPVERS 2UL
#define PMAPPROC_NULL 0UL
#define PMAPPROC_SET 1UL
#define PMAPPROC_UNSET 2UL
#define PMAPPROC_GETPORT 3UL
#define PMAPPROC_DUMP 4UL
#define PMAPPROC_CALLIT 5UL

struct pmap {
  unsigned long pm_prog;
  unsigned long pm_vers;
  unsigned long pm_prot;
  unsigned long pm_port;
};

struct pmaplist {
  struct pmap pml_map;
  struct pmaplist *pml_next;
};

bool_t xdr_pmap(XDR *xdrs, struct pmap *regs);
bool_t xdr_pmaplist(XDR *xdrs, struct pmaplist **rp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_PMAP_PROT_H */
