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
#include <netconfig.h>
#include <rpc/nettype.h>
#include <rpc/rpc_com.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__) || defined(__linux__) || defined(__CYGWIN__)

static void *default_rpc_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

/** @brief Overrideable calloc hook for OOM testing in bsd-rpc. */
void *(*g_bsd_rpc_calloc)(size_t, size_t) = default_rpc_calloc;

/** @brief Resets g_bsd_rpc_calloc back to standard calloc. */
void bsd_rpc_reset_calloc_hook(void);
void bsd_rpc_reset_calloc_hook(void) { g_bsd_rpc_calloc = default_rpc_calloc; }

/** @brief XDR no-op void filter. */
bool_t xdr_void(void) { return TRUE; }
/** @brief XDR int filter. */
bool_t xdr_int(XDR *xdrs, int *ip) {
  (void)xdrs;
  (void)ip;
  return TRUE;
}
/** @brief XDR unsigned int filter. */
bool_t xdr_u_int(XDR *xdrs, u_int *up) {
  (void)xdrs;
  (void)up;
  return TRUE;
}
/** @brief XDR long filter. */
bool_t xdr_long(XDR *xdrs, long *lp) {
  (void)xdrs;
  (void)lp;
  return TRUE;
}
/** @brief XDR unsigned long filter. */
bool_t xdr_u_long(XDR *xdrs, u_long *ulp) {
  (void)xdrs;
  (void)ulp;
  return TRUE;
}
/** @brief XDR short filter. */
bool_t xdr_short(XDR *xdrs, short *sp) {
  (void)xdrs;
  (void)sp;
  return TRUE;
}
/** @brief XDR unsigned short filter. */
bool_t xdr_u_short(XDR *xdrs, u_short *usp) {
  (void)xdrs;
  (void)usp;
  return TRUE;
}
/** @brief XDR int16_t filter. */
bool_t xdr_int16_t(XDR *xdrs, int16_t *ip) {
  (void)xdrs;
  (void)ip;
  return TRUE;
}
/** @brief XDR uint16_t filter. */
bool_t xdr_uint16_t(XDR *xdrs, uint16_t *up) {
  (void)xdrs;
  (void)up;
  return TRUE;
}
/** @brief XDR int32_t filter. */
bool_t xdr_int32_t(XDR *xdrs, int32_t *ip) {
  (void)xdrs;
  (void)ip;
  return TRUE;
}
/** @brief XDR uint32_t filter. */
bool_t xdr_uint32_t(XDR *xdrs, uint32_t *up) {
  (void)xdrs;
  (void)up;
  return TRUE;
}
/** @brief XDR int64_t filter. */
bool_t xdr_int64_t(XDR *xdrs, int64_t *ip) {
  (void)xdrs;
  (void)ip;
  return TRUE;
}
/** @brief XDR uint64_t filter. */
bool_t xdr_uint64_t(XDR *xdrs, uint64_t *up) {
  (void)xdrs;
  (void)up;
  return TRUE;
}
/** @brief XDR boolean filter. */
bool_t xdr_bool(XDR *xdrs, bool_t *bp) {
  (void)xdrs;
  (void)bp;
  return TRUE;
}
/** @brief XDR enum filter. */
bool_t xdr_enum(XDR *xdrs, enum_t *ep) {
  (void)xdrs;
  (void)ep;
  return TRUE;
}
/** @brief XDR variable-length array filter. */
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
/** @brief XDR counted byte array filter. */
bool_t xdr_bytes(XDR *xdrs, char **cpp, u_int *sizep, u_int maxsize) {
  (void)xdrs;
  (void)cpp;
  (void)sizep;
  (void)maxsize;
  return TRUE;
}
/** @brief XDR fixed-size opaque data filter. */
bool_t xdr_opaque(XDR *xdrs, caddr_t cp, u_int cnt) {
  (void)xdrs;
  (void)cp;
  (void)cnt;
  return TRUE;
}
/** @brief XDR NUL-terminated string filter. */
bool_t xdr_string(XDR *xdrs, char **cpp, u_int maxsize) {
  (void)xdrs;
  (void)cpp;
  (void)maxsize;
  return TRUE;
}
/** @brief XDR discriminated union filter. */
bool_t xdr_union(XDR *xdrs, enum_t *dscmp, char *unp,
                 const struct xdr_discrim *choices, xdrproc_t dfault) {
  (void)xdrs;
  (void)dscmp;
  (void)unp;
  (void)choices;
  (void)dfault;
  return TRUE;
}
/** @brief XDR char filter. */
bool_t xdr_char(XDR *xdrs, char *cp) {
  (void)xdrs;
  (void)cp;
  return TRUE;
}
/** @brief XDR unsigned char filter. */
bool_t xdr_u_char(XDR *xdrs, u_char *cp) {
  (void)xdrs;
  (void)cp;
  return TRUE;
}
/** @brief XDR fixed-length vector filter. */
bool_t xdr_vector(XDR *xdrs, char *basep, u_int nelem, u_int elemsize,
                  xdrproc_t xdr_elem) {
  (void)xdrs;
  (void)basep;
  (void)nelem;
  (void)elemsize;
  (void)xdr_elem;
  return TRUE;
}
/** @brief XDR single-precision float filter. */
bool_t xdr_float(XDR *xdrs, float *fp) {
  (void)xdrs;
  (void)fp;
  return TRUE;
}
/** @brief XDR double-precision float filter. */
bool_t xdr_double(XDR *xdrs, double *dp) {
  (void)xdrs;
  (void)dp;
  return TRUE;
}
/** @brief XDR non-null pointer reference filter. */
bool_t xdr_reference(XDR *xdrs, caddr_t *pp, u_int size, xdrproc_t proc) {
  (void)xdrs;
  (void)pp;
  (void)size;
  (void)proc;
  return TRUE;
}
/** @brief XDR nullable pointer filter. */
bool_t xdr_pointer(XDR *xdrs, char **objpp, u_int objsize, xdrproc_t xdr_obj) {
  (void)xdrs;
  (void)objpp;
  (void)objsize;
  (void)xdr_obj;
  return TRUE;
}
/** @brief XDR unbounded string wrapper filter. */
bool_t xdr_wrapstring(XDR *xdrs, char **cpp) {
  (void)xdrs;
  (void)cpp;
  return TRUE;
}
/** @brief Free memory allocated by an XDR filter. */
void xdr_free(xdrproc_t proc, void *objp) {
  (void)proc;
  (void)objp;
}
/** @brief Initialize an in-memory XDR stream. */
void xdrmem_create(XDR *xdrs, char *addr, u_int size, enum xdr_op op) {
  if (xdrs != NULL) {
    memset(xdrs, 0, sizeof(*xdrs));
    xdrs->x_op = op;
    xdrs->x_base = addr;
    xdrs->x_handy = size;
  }
}
/** @brief Initialize a stdio FILE-backed XDR stream. */
void xdrstdio_create(XDR *xdrs, FILE *file, enum xdr_op op) {
  if (xdrs != NULL) {
    memset(xdrs, 0, sizeof(*xdrs));
    xdrs->x_op = op;
    xdrs->x_private = (caddr_t)file;
  }
}

/** @brief Create default UNIX credentials AUTH handle. */
AUTH *authunix_create_default(void) {
  return (AUTH *)g_bsd_rpc_calloc(1, sizeof(AUTH));
}
/** @brief Create null-authentication AUTH handle. */
AUTH *authnone_create(void) {
  return (AUTH *)g_bsd_rpc_calloc(1, sizeof(AUTH));
}
/** @brief Destroy an AUTH handle. */
void auth_destroy(AUTH *auth) {
  if (auth != NULL) {
    free(auth);
  }
}

/** @brief Create generic RPC client handle. */
CLIENT *clnt_create(const char *hostname, rpcprog_t prog, rpcvers_t vers,
                    const char *proto) {
  (void)hostname;
  (void)prog;
  (void)vers;
  (void)proto;
  return NULL;
}
/** @brief Create UDP RPC client handle. */
CLIENT *clntudp_create(struct sockaddr_in *raddr, u_long program,
                       u_long version, struct timeval wait, int *sockp) {
  (void)raddr;
  (void)program;
  (void)version;
  (void)wait;
  (void)sockp;
  return NULL;
}
/** @brief Create TCP RPC client handle. */
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
/** @brief Print RPC client creation error to stderr. */
void clnt_pcreateerror(const char *s) {
  if (s != NULL) {
    fprintf(stderr, "%s: RPC client creation failed\n", s);
  }
}
/** @brief Return string describing RPC client creation error. */
char *clnt_spcreateerror(const char *s) {
  static char msg[] = "RPC client creation failed";
  (void)s;
  return msg;
}
/** @brief Print RPC status error message to stderr. */
void clnt_perrno(enum clnt_stat num) { (void)num; }
/** @brief Print RPC call error message to stderr. */
void clnt_perror(CLIENT *rpch, const char *s) {
  (void)rpch;
  (void)s;
}
/** @brief Return string describing RPC call error. */
char *clnt_sperror(CLIENT *rpch, const char *s) {
  static char msg[] = "RPC error";
  (void)rpch;
  (void)s;
  return msg;
}
/** @brief Return string describing clnt_stat status code. */
char *clnt_sperrno(enum clnt_stat stat) {
  static char msg[] = "RPC status error";
  (void)stat;
  return msg;
}
/** @brief Invoke remote procedure call directly by host/prog/vers/proc. */
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

/** @brief XDR portmap entry filter. */
bool_t xdr_pmap(XDR *xdrs, struct pmap *regs) {
  (void)xdrs;
  (void)regs;
  return TRUE;
}
/** @brief XDR portmap linked list filter. */
bool_t xdr_pmaplist(XDR *xdrs, struct pmaplist **rp) {
  (void)xdrs;
  (void)rp;
  return TRUE;
}
/** @brief Register program mapping with portmapper. */
bool_t pmap_set(u_long program, u_long version, int protocol, u_short port) {
  (void)program;
  (void)version;
  (void)protocol;
  (void)port;
  return FALSE;
}
/** @brief Unregister program mapping from portmapper. */
bool_t pmap_unset(u_long program, u_long version) {
  (void)program;
  (void)version;
  return FALSE;
}
/** @brief Retrieve list of registered RPC programs from portmapper. */
struct pmaplist *pmap_getmaps(struct sockaddr_in *address) {
  (void)address;
  return NULL;
}
/** @brief Query port number for an RPC service from portmapper. */
u_short pmap_getport(struct sockaddr_in *address, u_long program,
                     u_long version, u_int protocol) {
  (void)address;
  (void)program;
  (void)version;
  (void)protocol;
  return 0;
}
/** @brief Broadcast RPC request via portmapper. */
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

/** @brief Look up RPC program entry by name. */
struct rpcent *getrpcbyname(const char *rpc_name) {
  (void)rpc_name;
  return NULL;
}
/** @brief Look up RPC program entry by program number. */
struct rpcent *getrpcbynumber(int number) {
  (void)number;
  return NULL;
}
/** @brief Read next entry from /etc/rpc database. */
struct rpcent *getrpcent(void) { return NULL; }
/** @brief Open or rewind /etc/rpc database. */
void setrpcent(int stayopen) { (void)stayopen; }
/** @brief Close /etc/rpc database. */
void endrpcent(void) {}

/** @brief Bind NIS client to domain. */
int yp_bind(const char *indomain) {
  (void)indomain;
  return YPERR_YPBIND;
}
/** @brief Unbind NIS client from domain. */
void yp_unbind(const char *indomain) { (void)indomain; }
/** @brief Retrieve default NIS domain name. */
int yp_get_default_domain(char **outdomain) {
  static char dom[] = "localdomain";
  if (outdomain != NULL) {
    *outdomain = dom;
  }
  return 0;
}
/** @brief Look up value matching key in NIS map. */
int yp_match(const char *indomain, const char *inmap, const char *inkey,
             int inkeylen, char **outval, int *outvallen) {
  (void)indomain;
  (void)inmap;
  (void)inkey;
  (void)inkeylen;
  if (outval != NULL) {
    *outval = NULL;
  }
  if (outvallen != NULL) {
    *outvallen = 0;
  }
  return YPERR_YPBIND;
}
/** @brief Retrieve first key-value entry from NIS map. */
int yp_first(const char *indomain, const char *inmap, char **outkey,
             int *outkeylen, char **outval, int *outvallen) {
  (void)indomain;
  (void)inmap;
  if (outkey != NULL) {
    *outkey = NULL;
  }
  if (outkeylen != NULL) {
    *outkeylen = 0;
  }
  if (outval != NULL) {
    *outval = NULL;
  }
  if (outvallen != NULL) {
    *outvallen = 0;
  }
  return YPERR_YPBIND;
}
/** @brief Retrieve next key-value entry from NIS map. */
int yp_next(const char *indomain, const char *inmap, const char *inkey,
            int inkeylen, char **outkey, int *outkeylen, char **outval,
            int *outvallen) {
  (void)inkey;
  (void)inkeylen;
  return yp_first(indomain, inmap, outkey, outkeylen, outval, outvallen);
}
/** @brief Retrieve master server hostname for NIS map. */
int yp_master(const char *indomain, const char *inmap, char **outname) {
  (void)indomain;
  (void)inmap;
  if (outname != NULL) {
    *outname = NULL;
  }
  return YPERR_YPBIND;
}
/** @brief Retrieve order/version number for NIS map. */
int yp_order(const char *indomain, const char *inmap, int *outorder) {
  (void)indomain;
  (void)inmap;
  if (outorder != NULL) {
    *outorder = 0;
  }
  return YPERR_YPBIND;
}
/** @brief Enumerate all key-value pairs in NIS map via callback. */
int yp_all(const char *indomain, const char *inmap,
           struct ypall_callback *incallback) {
  (void)indomain;
  (void)inmap;
  (void)incallback;
  return YPERR_YPBIND;
}
/** @brief Convert YP error code to human-readable string. */
const char *yperr_string(int incode) {
  (void)incode;
  return "YP error";
}
/** @brief Convert YP protocol status code to YPERR_* code. */
int ypprot_err(unsigned int incode) {
  (void)incode;
  return YPERR_YPERR;
}

/** @brief Global SunRPC client creation error state. */
struct rpc_createerr rpc_createerr;

#endif /* non-Apple base SunRPC */

#if !defined(__FreeBSD__)
/** @brief XDR rpcb v3/v4 mapping entry filter. */
bool_t xdr_rpcb(XDR *xdrs, RPCB *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR rpcb mapping list pointer filter. */
bool_t xdr_rpcblist_ptr(XDR *xdrs, rpcblist_ptr *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR rpcb remote call arguments filter. */
bool_t xdr_rpcb_rmtcallargs(XDR *xdrs, rpcb_rmtcallargs *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR rpcb remote call result filter. */
bool_t xdr_rpcb_rmtcallres(XDR *xdrs, rpcb_rmtcallres *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR rpcb entry list pointer filter. */
bool_t xdr_rpcb_entry_list_ptr(XDR *xdrs, rpcb_entry_list_ptr *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR rpcb statistics array filter. */
bool_t xdr_rpcb_stat_byvers(XDR *xdrs, rpcb_stat_byvers objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR TI-RPC netbuf filter. */
bool_t xdr_netbuf(XDR *xdrs, struct netbuf *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
#if !defined(__APPLE__)
/** @brief XDR NIS domainname filter. */
bool_t xdr_domainname(XDR *xdrs, char **objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR ypbind response filter. */
bool_t xdr_ypbind_resp(XDR *xdrs, struct ypbind_resp *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR ypreq_key filter. */
bool_t xdr_ypreq_key(XDR *xdrs, struct ypreq_key *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR ypresp_val filter. */
bool_t xdr_ypresp_val(XDR *xdrs, struct ypresp_val *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
#endif
/** @brief Retrieve list of NIS maps in domain. */
int yp_maplist(const char *indomain, struct ypmaplist **outmaplist) {
  (void)indomain;
  if (outmaplist != NULL) {
    *outmaplist = NULL;
  }
  return YPERR_YPBIND;
}
/** @brief XDR NFS mount dirpath filter. */
bool_t xdr_dirpath(XDR *xdrs, dirpath *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR NFS mountlist filter. */
bool_t xdr_mountlist(XDR *xdrs, mountlist *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR NFS exports list filter. */
bool_t xdr_exports(XDR *xdrs, exports *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR extended quota arguments filter. */
bool_t xdr_ext_getquota_args(XDR *xdrs, struct ext_getquota_args *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR quota arguments filter. */
bool_t xdr_getquota_args(XDR *xdrs, struct getquota_args *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR quota result filter. */
bool_t xdr_getquota_rslt(XDR *xdrs, struct getquota_rslt *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR rstat statstime filter. */
bool_t xdr_statstime(XDR *xdrs, struct statstime *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief Query remote host kernel statistics via rstatd. */
int rstat(const char *host, struct statstime *statp) {
  (void)host;
  (void)statp;
  return -1;
}
/** @brief XDR rnusers utmpidlearr filter. */
bool_t xdr_utmpidlearr(XDR *xdrs, struct utmpidlearr *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief XDR rnusers utmp_array filter. */
bool_t xdr_utmp_array(XDR *xdrs, struct utmp_array *objp) {
  (void)xdrs;
  (void)objp;
  return TRUE;
}
/** @brief Open /etc/netconfig iteration handle. */
void *setnetconfig(void) { return NULL; }
/** @brief Read next netconfig entry. */
struct netconfig *getnetconfig(void *h) {
  (void)h;
  return NULL;
}
/** @brief Close /etc/netconfig iteration handle. */
int endnetconfig(void *h) {
  (void)h;
  return 0;
}
/** @brief Look up netconfig entry by netid. */
struct netconfig *getnetconfigent(const char *n) {
  (void)n;
  return NULL;
}
/** @brief Free netconfig entry returned by getnetconfigent. */
void freenetconfigent(struct netconfig *n) { (void)n; }
/** @brief Print netconfig error message to stderr. */
void nc_perror(const char *s) { (void)s; }
/** @brief Return string describing last netconfig error. */
char *nc_sperror(void) {
  static char msg[] = "netconfig unavailable";
  return msg;
}
/** @brief Open nettype configuration handle. */
void *__rpc_setconf(const char *n) {
  (void)n;
  return NULL;
}
/** @brief Close nettype configuration handle. */
void __rpc_endconf(void *v) { (void)v; }
/** @brief Read next matching netconfig entry for nettype handle. */
struct netconfig *__rpc_getconf(void *v) {
  (void)v;
  return NULL;
}
/** @brief Retrieve preferred IP netconfig entry. */
struct netconfig *__rpc_getconfip(const char *n) {
  (void)n;
  return NULL;
}
/** @brief Create socket descriptor from netconfig entry. */
int __rpc_nconf2fd(const struct netconfig *n) {
  (void)n;
  return -1;
}
/** @brief Convert netconfig entry to socket parameters. */
int __rpc_nconf2sockinfo(const struct netconfig *n, struct __rpc_sockinfo *s) {
  (void)n;
  (void)s;
  return 0;
}
/** @brief Convert universal address string to netbuf for address family. */
struct netbuf *__rpc_uaddr2taddr_af(int af, const char *u) {
  (void)af;
  (void)u;
  return NULL;
}
/** @brief Convert netbuf to universal address string for address family. */
char *__rpc_taddr2uaddr_af(int af, const struct netbuf *n) {
  (void)af;
  (void)n;
  return NULL;
}
/** @brief Broadcast RPC call with explicit backoff timeouts. */
enum clnt_stat rpc_broadcast_exp(rpcprog_t prog, rpcvers_t vers, rpcproc_t proc,
                                 xdrproc_t xargs, caddr_t argsp,
                                 xdrproc_t xresults, caddr_t resultsp,
                                 resultproc_t eachresult, int inittime,
                                 int waittime, const char *nettype) {
  (void)prog;
  (void)vers;
  (void)proc;
  (void)xargs;
  (void)argsp;
  (void)xresults;
  (void)resultsp;
  (void)eachresult;
  (void)inittime;
  (void)waittime;
  (void)nettype;
  return RPC_CANTSEND;
}
/** @brief Create RPC client for specified transport netconfig. */
CLIENT *clnt_tp_create(const char *h, rpcprog_t p, rpcvers_t v,
                       const struct netconfig *n) {
  (void)h;
  (void)p;
  (void)v;
  (void)n;
  return NULL;
}
/** @brief Create TLI/socket RPC client handle. */
CLIENT *clnt_tli_create(int fd, const struct netconfig *n, struct netbuf *b,
                        rpcprog_t p, rpcvers_t v, u_int s, u_int r) {
  (void)fd;
  (void)n;
  (void)b;
  (void)p;
  (void)v;
  (void)s;
  (void)r;
  return NULL;
}
/** @brief Create virtual-circuit (stream) RPC client handle. */
CLIENT *clnt_vc_create(int fd, const struct netbuf *ra, rpcprog_t p,
                       rpcvers_t v, u_int s, u_int r) {
  (void)fd;
  (void)ra;
  (void)p;
  (void)v;
  (void)s;
  (void)r;
  return NULL;
}
/** @brief Create datagram RPC client handle. */
CLIENT *clnt_dg_create(int fd, const struct netbuf *sa, rpcprog_t p,
                       rpcvers_t v, u_int s, u_int r) {
  (void)fd;
  (void)sa;
  (void)p;
  (void)v;
  (void)s;
  (void)r;
  return NULL;
}
/** @brief Create in-process raw memory RPC client handle. */
CLIENT *clnt_raw_create(rpcprog_t p, rpcvers_t v) {
  (void)p;
  (void)v;
  return NULL;
}
/** @brief Unregister RPC service mapping from rpcbind. */
bool_t rpcb_unset(rpcprog_t p, rpcvers_t v, const struct netconfig *n) {
  (void)p;
  (void)v;
  (void)n;
  return FALSE;
}
/** @brief Look up transport address for RPC service via rpcbind. */
struct netbuf *rpcb_getaddr(rpcprog_t p, rpcvers_t v, const struct netconfig *n,
                            struct sockaddr *a, const char *h) {
  (void)p;
  (void)v;
  (void)n;
  (void)a;
  (void)h;
  return NULL;
}
#endif

/** @brief Convert transport-specific netbuf to universal address string. */
char *taddr2uaddr(const struct netconfig *nconf, const struct netbuf *nbuf) {
  (void)nconf;
  (void)nbuf;
  return NULL;
}
/** @brief Convert universal address string to transport-specific netbuf. */
struct netbuf *uaddr2taddr(const struct netconfig *nconf, const char *uaddr) {
  (void)nconf;
  (void)uaddr;
  return NULL;
}
/** @brief Broadcast RPC call across matching transports. */
enum clnt_stat rpc_broadcast(rpcprog_t prog, rpcvers_t vers, rpcproc_t proc,
                             xdrproc_t xargs, caddr_t argsp, xdrproc_t xresults,
                             caddr_t resultsp, resultproc_t eachresult,
                             const char *nettype) {
  return rpc_broadcast_exp(prog, vers, proc, xargs, argsp, xresults, resultsp,
                           eachresult, 0, 0, nettype);
}

/** @brief Coverage probe helper returning 0. */
int bsd_rpc_dummy_for_coverage(void);
int bsd_rpc_dummy_for_coverage(void) { return 0; }

/** @brief Dummy typedef preventing empty translation unit warnings. */
typedef int make_iso_compilers_happy_tu_bsd_rpc;

#endif
/** @brief Dummy typedef preventing empty mega translation unit warnings. */
typedef int make_iso_compilers_happy_mega_bsd_rpc;
