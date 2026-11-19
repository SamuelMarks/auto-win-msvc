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

#define YPBINDPROG 100007UL
#define YPBINDVERS 2UL
#define YPBINDPROC_DOMAIN 2UL
#define YPBIND_SUCC_VAL 1
#define YPBIND_FAIL_VAL 2

#define YP_FALSE 0
#define YP_TRUE 1
#define YP_NOMORE 2

#define YPMAXRECORD 1024
#define YPMAXDOMAIN 64
#define YPMAXMAP 64
#define YPMAXPEER 256

struct ypmaplist {
  char *map;
  char ypml_name[YPMAXMAP + 1];
  struct ypmaplist *next;
  struct ypmaplist *ypml_next;
};

struct ypresp_maplist {
  unsigned int status;
  struct ypmaplist *list;
};

struct ypbind_binding {
  struct in_addr ypbind_binding_addr;
  unsigned short ypbind_binding_port;
};

struct ypbind_resp {
  int ypbind_status;
  union {
    int ypbind_error;
    struct ypbind_binding ypbind_bindinfo;
  } ypbind_resp_u;
};

struct keydat_t {
  unsigned int keydat_len;
  char *keydat_val;
};

struct valdat_t {
  unsigned int valdat_len;
  char *valdat_val;
};

struct ypreq_key {
  char *domain;
  char *map;
  struct keydat_t key;
  struct keydat_t keydat;
};

struct ypresp_val {
  int status;
  int stat;
  struct valdat_t val;
  struct valdat_t valdat;
};

bool_t xdr_domainname(XDR *xdrs, char **objp);
bool_t xdr_ypbind_resp(XDR *xdrs, struct ypbind_resp *objp);
bool_t xdr_ypreq_key(XDR *xdrs, struct ypreq_key *objp);
bool_t xdr_ypresp_val(XDR *xdrs, struct ypresp_val *objp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_YP_PROT_H */
