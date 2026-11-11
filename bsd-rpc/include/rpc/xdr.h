#ifndef RPC_XDR_H
#define RPC_XDR_H

/**
 * @file xdr.h
 * @brief SunRPC External Data Representation (<rpc/xdr.h>) polyfill.
 */

/* clang-format off */
#include <rpc/types.h>
#include <stdio.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

enum xdr_op { XDR_ENCODE = 0, XDR_DECODE = 1, XDR_FREE = 2 };

typedef struct __rpc_xdr XDR;
typedef bool_t (*xdrproc_t)(XDR *, ...);

struct xdr_ops {
  bool_t (*x_getlong)(XDR *, long *);
  bool_t (*x_putlong)(XDR *, const long *);
  bool_t (*x_getbytes)(XDR *, caddr_t, u_int);
  bool_t (*x_putbytes)(XDR *, const char *, u_int);
  u_int (*x_getpostn)(XDR *);
  bool_t (*x_setpostn)(XDR *, u_int);
  int32_t *(*x_inline)(XDR *, u_int);
  void (*x_destroy)(XDR *);
};

struct __rpc_xdr {
  enum xdr_op x_op;
  const struct xdr_ops *x_ops;
  caddr_t x_public;
  caddr_t x_private;
  caddr_t x_base;
  u_int x_handy;
};

struct xdr_discrim {
  int value;
  xdrproc_t proc;
};

bool_t xdr_void(void);
bool_t xdr_int(XDR *xdrs, int *ip);
bool_t xdr_u_int(XDR *xdrs, u_int *up);
bool_t xdr_long(XDR *xdrs, long *lp);
bool_t xdr_u_long(XDR *xdrs, u_long *ulp);
bool_t xdr_short(XDR *xdrs, short *sp);
bool_t xdr_u_short(XDR *xdrs, u_short *usp);
bool_t xdr_int16_t(XDR *xdrs, int16_t *ip);
bool_t xdr_uint16_t(XDR *xdrs, uint16_t *up);
bool_t xdr_int32_t(XDR *xdrs, int32_t *ip);
bool_t xdr_uint32_t(XDR *xdrs, uint32_t *up);
bool_t xdr_int64_t(XDR *xdrs, int64_t *ip);
bool_t xdr_uint64_t(XDR *xdrs, uint64_t *up);
bool_t xdr_bool(XDR *xdrs, bool_t *bp);
bool_t xdr_enum(XDR *xdrs, enum_t *ep);
bool_t xdr_array(XDR *xdrs, caddr_t *addrp, u_int *sizep, u_int maxsize,
                 u_int elsize, xdrproc_t elproc);
bool_t xdr_bytes(XDR *xdrs, char **cpp, u_int *sizep, u_int maxsize);
bool_t xdr_opaque(XDR *xdrs, caddr_t cp, u_int cnt);
bool_t xdr_string(XDR *xdrs, char **cpp, u_int maxsize);
bool_t xdr_union(XDR *xdrs, enum_t *dscmp, char *unp,
                 const struct xdr_discrim *choices, xdrproc_t dfault);
bool_t xdr_char(XDR *xdrs, char *cp);
bool_t xdr_u_char(XDR *xdrs, u_char *cp);
bool_t xdr_vector(XDR *xdrs, char *basep, u_int nelem, u_int elemsize,
                  xdrproc_t xdr_elem);
bool_t xdr_float(XDR *xdrs, float *fp);
bool_t xdr_double(XDR *xdrs, double *dp);
bool_t xdr_reference(XDR *xdrs, caddr_t *pp, u_int size, xdrproc_t proc);
bool_t xdr_pointer(XDR *xdrs, char **objpp, u_int objsize, xdrproc_t xdr_obj);
bool_t xdr_wrapstring(XDR *xdrs, char **cpp);
void xdr_free(xdrproc_t proc, void *objp);
void xdrmem_create(XDR *xdrs, char *addr, u_int size, enum xdr_op op);
void xdrstdio_create(XDR *xdrs, FILE *file, enum xdr_op op);

#define xdr_destroy(xdrs)                                                      \
  ((void)((xdrs)->x_ops && (xdrs)->x_ops->x_destroy                            \
              ? ((*(xdrs)->x_ops->x_destroy)(xdrs), 0)                         \
              : 0))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_XDR_H */
