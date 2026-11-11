#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_rpc.c
 * @brief Implementation of SunRPC and NIS/YP client shims.
 */

/* clang-format off */
#include <rpc/rpc.h>
#include <rpcsvc/yp.h>
#include <rpcsvc/ypclnt.h>
#include <rpcsvc/mount.h>
#include <rpcsvc/rquota.h>
#include <rpcsvc/rstat.h>
#include <rpcsvc/rnusers.h>
#include <rpcsvc/rwall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__) || defined(__linux__) || defined(__CYGWIN__)

bool_t xdr_void(void) { return TRUE; }
bool_t xdr_int(XDR *xdrs, int *ip) {
  (void)xdrs;
  (void)ip;
  return TRUE;
}
bool_t xdr_u_int(XDR *xdrs, u_int *up) {
  (void)xdrs;
  (void)up;
  return TRUE;
}
bool_t xdr_long(XDR *xdrs, long *lp) {
  (void)xdrs;
  (void)lp;
  return TRUE;
}
bool_t xdr_u_long(XDR *xdrs, u_long *ulp) {
  (void)xdrs;
  (void)ulp;
  return TRUE;
}
bool_t xdr_short(XDR *xdrs, short *sp) {
  (void)xdrs;
  (void)sp;
  return TRUE;
}
bool_t xdr_u_short(XDR *xdrs, u_short *usp) {
  (void)xdrs;
  (void)usp;
  return TRUE;
}
bool_t xdr_int16_t(XDR *xdrs, int16_t *ip) {
  (void)xdrs;
  (void)ip;
  return TRUE;
}
bool_t xdr_uint16_t(XDR *xdrs, uint16_t *up) {
  (void)xdrs;
  (void)up;
  return TRUE;
}
bool_t xdr_int32_t(XDR *xdrs, int32_t *ip) {
  (void)xdrs;
  (void)ip;
  return TRUE;
}
bool_t xdr_uint32_t(XDR *xdrs, uint32_t *up) {
  (void)xdrs;
  (void)up;
  return TRUE;
}
bool_t xdr_int64_t(XDR *xdrs, int64_t *ip) {
  (void)xdrs;
  (void)ip;
  return TRUE;
}
bool_t xdr_uint64_t(XDR *xdrs, uint64_t *up) {
  (void)xdrs;
  (void)up;
  return TRUE;
}
bool_t xdr_bool(XDR *xdrs, bool_t *bp) {
  (void)xdrs;
  (void)bp;
  return TRUE;
}
bool_t xdr_enum(XDR *xdrs, enum_t *ep) {
  (void)xdrs;
  (void)ep;
  return TRUE;
}
bool_t xdr_array(XDR *xdrs, caddr_t *addrp, u_int *sizep, u_int maxsize,
                 u_int elsize, xdrproc_t elproc) {
  (void)xdrs;
  (void)addrp;
  (void)sizep;
  (void)maxsize;
  (void)elsize;
  (void)elproc;
  return TRUE;
}
bool_t xdr_bytes(XDR *xdrs, char **cpp, u_int *sizep, u_int maxsize) {
  (void)xdrs;
  (void)cpp;
  (void)sizep;
  (void)maxsize;
  return TRUE;
}
bool_t xdr_opaque(XDR *xdrs, caddr_t cp, u_int cnt) {
  (void)xdrs;
  (void)cp;
  (void)cnt;
  return TRUE;
}
bool_t xdr_string(XDR *xdrs, char **cpp, u_int maxsize) {
  (void)xdrs;
  (void)cpp;
  (void)maxsize;
  return TRUE;
}
bool_t xdr_union(XDR *xdrs, enum_t *dscmp, char *unp,
                 const struct xdr_discrim *choices, xdrproc_t dfault) {
  (void)xdrs;
  (void)dscmp;
  (void)unp;
  (void)choices;
  (void)dfault;
  return TRUE;
}
bool_t xdr_char(XDR *xdrs, char *cp) {
  (void)xdrs;
  (void)cp;
  return TRUE;
}
bool_t xdr_u_char(XDR *xdrs, u_char *cp) {
  (void)xdrs;
  (void)cp;
  return TRUE;
}
bool_t xdr_vector(XDR *xdrs, char *basep, u_int nelem, u_int elemsize,
                  xdrproc_t xdr_elem) {
  (void)xdrs;
  (void)basep;
  (void)nelem;
  (void)elemsize;
  (void)xdr_elem;
  return TRUE;
}
bool_t xdr_float(XDR *xdrs, float *fp) {
  (void)xdrs;
  (void)fp;
  return TRUE;
}
bool_t xdr_double(XDR *xdrs, double *dp) {
  (void)xdrs;
  (void)dp;
  return TRUE;
}
bool_t xdr_reference(XDR *xdrs, caddr_t *pp, u_int size, xdrproc_t proc) {
  (void)xdrs;
  (void)pp;
  (void)size;
  (void)proc;
  return TRUE;
}
bool_t xdr_pointer(XDR *xdrs, char **objpp, u_int objsize, xdrproc_t xdr_obj) {
  (void)xdrs;
  (void)objpp;
  (void)objsize;
  (void)xdr_obj;
  return TRUE;
}
bool_t xdr_wrapstring(XDR *xdrs, char **cpp) {
  (void)xdrs;
  (void)cpp;
  return TRUE;
}
void xdr_free(xdrproc_t proc, void *objp) {
  (void)proc;
  (void)objp;
}
void xdrmem_create(XDR *xdrs, char *addr, u_int size, enum xdr_op op) {
  if (xdrs != NULL) {
    memset(xdrs, 0, sizeof(*xdrs));
    xdrs->x_op = op;
    xdrs->x_base = addr;
    xdrs->x_handy = size;
  }
}
void xdrstdio_create(XDR *xdrs, FILE *file, enum xdr_op op) {
  if (xdrs != NULL) {
    memset(xdrs, 0, sizeof(*xdrs));
    xdrs->x_op = op;
    xdrs->x_private = (caddr_t)file;
  }
}

AUTH *authunix_create_default(void) { return (AUTH *)calloc(1, sizeof(AUTH)); }
AUTH *authnone_create(void) { return (AUTH *)calloc(1, sizeof(AUTH)); }
void auth_destroy(AUTH *auth) {
  if (auth != NULL) {
    free(auth);
  }
}

CLIENT *clnt_create(const char *hostname, rpcprog_t prog, rpcvers_t vers,
                    const char *proto) {
  (void)hostname;
  (void)prog;
  (void)vers;
  (void)proto;
  return NULL;
}
CLIENT *clnt_tp_create(const char *hostname, rpcprog_t prog, rpcvers_t vers,
                       const struct netconfig *nconf) {
  (void)hostname;
  (void)prog;
  (void)vers;
  (void)nconf;
  return NULL;
}
CLIENT *clntudp_create(struct sockaddr_in *raddr, u_long program,
                       u_long version, struct timeval wait, int *sockp) {
  (void)raddr;
  (void)program;
  (void)version;
  (void)wait;
  (void)sockp;
  return NULL;
}
CLIENT *clnttcp_create(struct sockaddr_in *raddr, u_long prog, u_long vers,
                       int *sockp, u_int sendsz, u_int recvsz) {
  (void)raddr;
  (void)prog;
  (void)vers;
  (void)sockp;
  (void)sendsz;
  (void)recvsz;
  return NULL;
}
void clnt_pcreateerror(const char *s) {
  if (s != NULL) {
    fprintf(stderr, "%s: RPC client creation failed\n", s);
  }
}
char *clnt_spcreateerror(const char *s) {
  (void)s;
  return "RPC client creation failed";
}
void clnt_perrno(enum clnt_stat num) { (void)num; }
void clnt_perror(CLIENT *rpch, const char *s) {
  (void)rpch;
  (void)s;
}
char *clnt_sperror(CLIENT *rpch, const char *s) {
  (void)rpch;
  (void)s;
  return "RPC error";
}
char *clnt_sperrno(enum clnt_stat stat) {
  (void)stat;
  return "RPC status error";
}
int callrpc(const char *host, int prognum, int versnum, int procnum,
            xdrproc_t inproc, void *in, xdrproc_t outproc, void *out) {
  (void)host;
  (void)prognum;
  (void)versnum;
  (void)procnum;
  (void)inproc;
  (void)in;
  (void)outproc;
  (void)out;
  return (int)RPC_PROGUNAVAIL;
}

bool_t xdr_pmap(XDR *xdrs, struct pmap *regs) {
  (void)xdrs;
  (void)regs;
  return TRUE;
}
bool_t xdr_pmaplist(XDR *xdrs, struct pmaplist **rp) {
  (void)xdrs;
  (void)rp;
  return TRUE;
}
bool_t pmap_set(u_long program, u_long version, int protocol, u_short port) {
  (void)program;
  (void)version;
  (void)protocol;
  (void)port;
  return FALSE;
}
bool_t pmap_unset(u_long program, u_long version) {
  (void)program;
  (void)version;
  return FALSE;
}
struct pmaplist *pmap_getmaps(struct sockaddr_in *address) {
  (void)address;
  return NULL;
}
u_short pmap_getport(struct sockaddr_in *address, u_long program,
                     u_long version, u_int protocol) {
  (void)address;
  (void)program;
  (void)version;
  (void)protocol;
  return 0;
}
enum clnt_stat clnt_broadcast(u_long prog, u_long vers, u_long proc,
                              xdrproc_t xargs, void *argsp, xdrproc_t xresults,
                              void *resultsp, resultproc_t eachresult) {
  (void)prog;
  (void)vers;
  (void)proc;
  (void)xargs;
  (void)argsp;
  (void)xresults;
  (void)resultsp;
  (void)eachresult;
  return RPC_CANTSEND;
}

bool_t xdr_rpcb(XDR *xdrs, RPCB *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
bool_t xdr_rpcblist_ptr(XDR *xdrs, rpcblist_ptr *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}

struct rpcent *getrpcbyname(const char *name) {
  (void)name;
  return NULL;
}
struct rpcent *getrpcbynumber(int number) {
  (void)number;
  return NULL;
}
struct rpcent *getrpcent(void) { return NULL; }
void setrpcent(int stayopen) { (void)stayopen; }
void endrpcent(void) {}

int yp_bind(const char *indomain) {
  (void)indomain;
  return YPERR_YPBIND;
}
void yp_unbind(const char *indomain) { (void)indomain; }
int yp_get_default_domain(char **outdomain) {
  static char dom[] = "localdomain";
  if (outdomain != NULL) {
    *outdomain = dom;
  }
  return 0;
}
int yp_match(const char *indomain, const char *inmap, const char *inkey,
             int inkeylen, char **outval, int *outvallen) {
  (void)indomain;
  (void)inmap;
  (void)inkey;
  (void)inkeylen;
  if (outval) {
    *outval = NULL;
  }
  if (outvallen) {
    *outvallen = 0;
  }
  return YPERR_YPBIND;
}
int yp_first(const char *indomain, const char *inmap, char **outkey,
             int *outkeylen, char **outval, int *outvallen) {
  (void)indomain;
  (void)inmap;
  if (outkey) {
    *outkey = NULL;
  }
  if (outkeylen) {
    *outkeylen = 0;
  }
  if (outval) {
    *outval = NULL;
  }
  if (outvallen) {
    *outvallen = 0;
  }
  return YPERR_YPBIND;
}
int yp_next(const char *indomain, const char *inmap, const char *inkey,
            int inkeylen, char **outkey, int *outkeylen, char **outval,
            int *outvallen) {
  (void)inkey;
  (void)inkeylen;
  return yp_first(indomain, inmap, outkey, outkeylen, outval, outvallen);
}
int yp_master(const char *indomain, const char *inmap, char **outname) {
  (void)indomain;
  (void)inmap;
  if (outname) {
    *outname = NULL;
  }
  return YPERR_YPBIND;
}
int yp_order(const char *indomain, const char *inmap, int *outorder) {
  (void)indomain;
  (void)inmap;
  if (outorder) {
    *outorder = 0;
  }
  return YPERR_YPBIND;
}
int yp_all(const char *indomain, const char *inmap,
           struct ypall_callback *incallback) {
  (void)indomain;
  (void)inmap;
  (void)incallback;
  return YPERR_YPBIND;
}
const char *yperr_string(int incode) {
  (void)incode;
  return "YP error";
}
int ypprot_err(unsigned int incode) {
  (void)incode;
  return YPERR_YPERR;
}
int yp_maplist(const char *indomain, struct ypmaplist **outmaplist) {
  (void)indomain;
  if (outmaplist) {
    *outmaplist = NULL;
  }
  return YPERR_YPBIND;
}

bool_t xdr_dirpath(XDR *xdrs, dirpath *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
bool_t xdr_mountlist(XDR *xdrs, mountlist *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
bool_t xdr_exports(XDR *xdrs, exports *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
bool_t xdr_getquota_args(XDR *xdrs, struct getquota_args *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
bool_t xdr_getquota_rslt(XDR *xdrs, struct getquota_rslt *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
bool_t xdr_statstime(XDR *xdrs, struct statstime *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
bool_t xdr_utmpidlearr(XDR *xdrs, struct utmpidlearr *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}

#endif

int bsd_rpc_dummy_for_coverage(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_rpc;

#endif
typedef int make_iso_compilers_happy_mega_bsd_rpc;
