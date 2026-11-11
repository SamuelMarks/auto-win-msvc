#ifndef RPCSVC_YP_PROT_H
#define RPCSVC_YP_PROT_H

/**
 * @file yp_prot.h
 * @brief NIS/YP protocol definitions (<rpcsvc/yp_prot.h>).
 */

/* clang-format off */
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define YPPROG 100004UL
#define YPVERS 2UL
#define YPPROC_NULL 0UL
#define YPPROC_DOMAIN 1UL
#define YPPROC_DOMAIN_NONACK 2UL
#define YPPROC_MATCH 3UL
#define YPPROC_FIRST 4UL
#define YPPROC_NEXT 5UL
#define YPPROC_XFR 6UL
#define YPPROC_CLEAR 7UL
#define YPPROC_ALL 8UL
#define YPPROC_MASTER 9UL
#define YPPROC_ORDER 10UL
#define YPPROC_MAPLIST 11UL

#define YPMAXRECORD 1024
#define YPMAXDOMAIN 64
#define YPMAXMAP 64
#define YPMAXPEER 256

struct ypmaplist {
  char ypml_name[YPMAXMAP + 1];
  struct ypmaplist *ypml_next;
};

struct ypresp_maplist {
  unsigned int status;
  struct ypmaplist *list;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_YP_PROT_H */
