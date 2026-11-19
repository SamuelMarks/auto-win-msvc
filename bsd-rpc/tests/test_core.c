/* clang-format off */
#include "greatest.h"
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

extern int bsd_rpc_dummy_for_coverage(void);

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__) || defined(__linux__) || defined(__CYGWIN__)
extern void *(*g_bsd_rpc_calloc)(size_t, size_t);
extern void bsd_rpc_reset_calloc_hook(void);

static void *mock_rpc_calloc_oom(size_t nmemb, size_t size) {
  (void)nmemb;
  (void)size;
  return NULL;
}

TEST test_bsd_rpc_base_sunrpc_and_yp(void) {
  XDR xdrs;
  char membuf[32];
  int iv = 0;
  u_int uiv = 0;
  long lv = 0;
  u_long ulv = 0;
  short sv = 0;
  u_short usv = 0;
  int16_t i16 = 0;
  uint16_t u16 = 0;
  int32_t i32 = 0;
  uint32_t u32 = 0;
  int64_t i64 = 0;
  uint64_t u64 = 0;
  bool_t bv = 0;
  enum_t ev = 0;
  char cv = 0;
  u_char ucv = 0;
  float fv = 0.0f;
  double dv = 0.0;
  char *strp = membuf;
  caddr_t addrp = membuf;
  AUTH *auth;
  struct sockaddr_in sin_addr;
  struct timeval tv;
  int sock = -1;
  struct pmap pm;
  struct pmaplist *pml = NULL;
  char *dom = NULL;
  char *ykey = NULL;
  char *yval = NULL;
  char *ymaster = NULL;
  int yklen = -1;
  int yvlen = -1;
  int yord = -1;
  struct ypall_callback ycb;

  memset(&sin_addr, 0, sizeof(sin_addr));
  memset(&tv, 0, sizeof(tv));
  memset(&pm, 0, sizeof(pm));
  memset(&ycb, 0, sizeof(ycb));

  /* XDR primitives and stream creators */
  ASSERT_EQ(TRUE, xdr_void());
  ASSERT_EQ(TRUE, xdr_int(&xdrs, &iv));
  ASSERT_EQ(TRUE, xdr_u_int(&xdrs, &uiv));
  ASSERT_EQ(TRUE, xdr_long(&xdrs, &lv));
  ASSERT_EQ(TRUE, xdr_u_long(&xdrs, &ulv));
  ASSERT_EQ(TRUE, xdr_short(&xdrs, &sv));
  ASSERT_EQ(TRUE, xdr_u_short(&xdrs, &usv));
  ASSERT_EQ(TRUE, xdr_int16_t(&xdrs, &i16));
  ASSERT_EQ(TRUE, xdr_uint16_t(&xdrs, &u16));
  ASSERT_EQ(TRUE, xdr_int32_t(&xdrs, &i32));
  ASSERT_EQ(TRUE, xdr_uint32_t(&xdrs, &u32));
  ASSERT_EQ(TRUE, xdr_int64_t(&xdrs, &i64));
  ASSERT_EQ(TRUE, xdr_uint64_t(&xdrs, &u64));
  ASSERT_EQ(TRUE, xdr_bool(&xdrs, &bv));
  ASSERT_EQ(TRUE, xdr_enum(&xdrs, &ev));
  ASSERT_EQ(TRUE, xdr_array(&xdrs, &addrp, &uiv, 8, 4, (xdrproc_t)0));
  ASSERT_EQ(TRUE, xdr_bytes(&xdrs, &strp, &uiv, 8));
  ASSERT_EQ(TRUE, xdr_opaque(&xdrs, addrp, 4));
  ASSERT_EQ(TRUE, xdr_string(&xdrs, &strp, 8));
  ASSERT_EQ(TRUE, xdr_union(&xdrs, &ev, membuf, NULL, (xdrproc_t)0));
  ASSERT_EQ(TRUE, xdr_char(&xdrs, &cv));
  ASSERT_EQ(TRUE, xdr_u_char(&xdrs, &ucv));
  ASSERT_EQ(TRUE, xdr_vector(&xdrs, membuf, 2, 1, (xdrproc_t)0));
  ASSERT_EQ(TRUE, xdr_float(&xdrs, &fv));
  ASSERT_EQ(TRUE, xdr_double(&xdrs, &dv));
  ASSERT_EQ(TRUE, xdr_reference(&xdrs, &addrp, 4, (xdrproc_t)0));
  ASSERT_EQ(TRUE, xdr_pointer(&xdrs, &strp, 4, (xdrproc_t)0));
  ASSERT_EQ(TRUE, xdr_wrapstring(&xdrs, &strp));
  xdr_free((xdrproc_t)0, membuf);

  xdrmem_create(NULL, membuf, sizeof(membuf), XDR_ENCODE);
  xdrmem_create(&xdrs, membuf, sizeof(membuf), XDR_ENCODE);
  ASSERT_EQ(XDR_ENCODE, xdrs.x_op);

  xdrstdio_create(NULL, stderr, XDR_DECODE);
  xdrstdio_create(&xdrs, stderr, XDR_DECODE);
  ASSERT_EQ(XDR_DECODE, xdrs.x_op);

  /* AUTH lifecycle + OOM */
  auth_destroy(NULL);
  auth = authunix_create_default();
  ASSERT(auth != NULL);
  auth_destroy(auth);

  auth = authnone_create();
  ASSERT(auth != NULL);
  auth_destroy(auth);

  g_bsd_rpc_calloc = mock_rpc_calloc_oom;
  ASSERT(authunix_create_default() == NULL);
  ASSERT(authnone_create() == NULL);
  bsd_rpc_reset_calloc_hook();

  /* Client, portmap, and rpcent shims */
  ASSERT(clnt_create("localhost", 100000UL, 2UL, "udp") == NULL);
  ASSERT(clntudp_create(&sin_addr, 100000UL, 2UL, tv, &sock) == NULL);
  ASSERT(clnttcp_create(&sin_addr, 100000UL, 2UL, &sock, 0, 0) == NULL);
  clnt_pcreateerror(NULL);
  clnt_pcreateerror("test_bsd_rpc");
  ASSERT(clnt_spcreateerror("test") != NULL);
  clnt_perrno(RPC_SUCCESS);
  clnt_perror(NULL, "test");
  ASSERT(clnt_sperror(NULL, "test") != NULL);
  ASSERT(clnt_sperrno(RPC_SUCCESS) != NULL);
  ASSERT_EQ((int)RPC_PROGUNAVAIL, callrpc("localhost", 1, 1, 1, (xdrproc_t)0,
                                          NULL, (xdrproc_t)0, NULL));

  ASSERT_EQ(TRUE, xdr_pmap(&xdrs, &pm));
  ASSERT_EQ(TRUE, xdr_pmaplist(&xdrs, &pml));
  ASSERT_EQ(FALSE, pmap_set(100000UL, 2UL, 17, 111));
  ASSERT_EQ(FALSE, pmap_unset(100000UL, 2UL));
  ASSERT(pmap_getmaps(&sin_addr) == NULL);
  ASSERT_EQ(0, pmap_getport(&sin_addr, 100000UL, 2UL, 17));
  ASSERT_EQ(RPC_CANTSEND, clnt_broadcast(1UL, 1UL, 1UL, (xdrproc_t)0, NULL,
                                         (xdrproc_t)0, NULL, (resultproc_t)0));

  ASSERT(getrpcbyname("rpcbind") == NULL);
  ASSERT(getrpcbynumber(100000) == NULL);
  ASSERT(getrpcent() == NULL);
  setrpcent(1);
  endrpcent();

  /* NIS / YP client shims: both NULL and non-NULL output pointers */
  ASSERT_EQ(YPERR_YPBIND, yp_bind("dom"));
  yp_unbind("dom");
  ASSERT_EQ(0, yp_get_default_domain(NULL));
  ASSERT_EQ(0, yp_get_default_domain(&dom));
  ASSERT_STR_EQ("localdomain", dom);

  ASSERT_EQ(YPERR_YPBIND, yp_match("d", "m", "k", 1, NULL, NULL));
  ASSERT_EQ(YPERR_YPBIND, yp_match("d", "m", "k", 1, &yval, &yvlen));
  ASSERT(yval == NULL);
  ASSERT_EQ(0, yvlen);

  ASSERT_EQ(YPERR_YPBIND, yp_first("d", "m", NULL, NULL, NULL, NULL));
  ASSERT_EQ(YPERR_YPBIND, yp_first("d", "m", &ykey, &yklen, &yval, &yvlen));
  ASSERT_EQ(YPERR_YPBIND,
            yp_next("d", "m", "k", 1, &ykey, &yklen, &yval, &yvlen));

  ASSERT_EQ(YPERR_YPBIND, yp_master("d", "m", NULL));
  ASSERT_EQ(YPERR_YPBIND, yp_master("d", "m", &ymaster));
  ASSERT(ymaster == NULL);

  ASSERT_EQ(YPERR_YPBIND, yp_order("d", "m", NULL));
  ASSERT_EQ(YPERR_YPBIND, yp_order("d", "m", &yord));
  ASSERT_EQ(0, yord);

  ASSERT_EQ(YPERR_YPBIND, yp_all("d", "m", &ycb));
  ASSERT(yperr_string(YPERR_YPBIND) != NULL);
  ASSERT_EQ(YPERR_YPERR, ypprot_err(0));

  PASS();
}
#endif

TEST test_bsd_rpc_tirpc_and_rpcsvc(void) {
  XDR xdrs;
  RPCB rpcb_item;
  rpcblist_ptr rbl = NULL;
  rpcb_rmtcallargs rmt_args;
  rpcb_rmtcallres rmt_res;
  rpcb_entry_list_ptr rel = NULL;
  rpcb_stat_byvers rstat_vers;
  struct netbuf nbuf;
  struct ypmaplist *yml = (struct ypmaplist *)(size_t)1;
  dirpath dp = NULL;
  mountlist ml = NULL;
  exports ex = NULL;
  struct ext_getquota_args eq_args;
  struct getquota_args q_args;
  struct getquota_rslt q_res;
  struct statstime st_time;
  struct utmpidlearr uidle;
  struct utmp_array uarr;
  struct __rpc_sockinfo sinfo;

  memset(&xdrs, 0, sizeof(xdrs));
  memset(&rpcb_item, 0, sizeof(rpcb_item));
  memset(&rmt_args, 0, sizeof(rmt_args));
  memset(&rmt_res, 0, sizeof(rmt_res));
  memset(&rstat_vers, 0, sizeof(rstat_vers));
  memset(&nbuf, 0, sizeof(nbuf));
  memset(&eq_args, 0, sizeof(eq_args));
  memset(&q_args, 0, sizeof(q_args));
  memset(&q_res, 0, sizeof(q_res));
  memset(&st_time, 0, sizeof(st_time));
  memset(&uidle, 0, sizeof(uidle));
  memset(&uarr, 0, sizeof(uarr));
  memset(&sinfo, 0, sizeof(sinfo));

#if !defined(__FreeBSD__)
  ASSERT_EQ(TRUE, xdr_rpcb(&xdrs, &rpcb_item));
  ASSERT_EQ(TRUE, xdr_rpcblist_ptr(&xdrs, &rbl));
  ASSERT_EQ(TRUE, xdr_rpcb_rmtcallargs(&xdrs, &rmt_args));
  ASSERT_EQ(TRUE, xdr_rpcb_rmtcallres(&xdrs, &rmt_res));
  ASSERT_EQ(TRUE, xdr_rpcb_entry_list_ptr(&xdrs, &rel));
  ASSERT_EQ(TRUE, xdr_rpcb_stat_byvers(&xdrs, rstat_vers));
  ASSERT_EQ(TRUE, xdr_netbuf(&xdrs, &nbuf));

#if !defined(__APPLE__)
  {
    char *dname = NULL;
    struct ypbind_resp ypb;
    struct ypreq_key yprk;
    struct ypresp_val yprv;
    memset(&ypb, 0, sizeof(ypb));
    memset(&yprk, 0, sizeof(yprk));
    memset(&yprv, 0, sizeof(yprv));
    ASSERT_EQ(TRUE, xdr_domainname(&xdrs, &dname));
    ASSERT_EQ(TRUE, xdr_ypbind_resp(&xdrs, &ypb));
    ASSERT_EQ(TRUE, xdr_ypreq_key(&xdrs, &yprk));
    ASSERT_EQ(TRUE, xdr_ypresp_val(&xdrs, &yprv));
  }
#endif

  ASSERT_EQ(YPERR_YPBIND, yp_maplist("dom", NULL));
  ASSERT_EQ(YPERR_YPBIND, yp_maplist("dom", &yml));
  ASSERT(yml == NULL);

  ASSERT_EQ(TRUE, xdr_dirpath(&xdrs, &dp));
  ASSERT_EQ(TRUE, xdr_mountlist(&xdrs, &ml));
  ASSERT_EQ(TRUE, xdr_exports(&xdrs, &ex));
  ASSERT_EQ(TRUE, xdr_ext_getquota_args(&xdrs, &eq_args));
  ASSERT_EQ(TRUE, xdr_getquota_args(&xdrs, &q_args));
  ASSERT_EQ(TRUE, xdr_getquota_rslt(&xdrs, &q_res));
  ASSERT_EQ(TRUE, xdr_statstime(&xdrs, &st_time));
  ASSERT_EQ(-1, rstat("localhost", &st_time));
  ASSERT_EQ(TRUE, xdr_utmpidlearr(&xdrs, &uidle));
  ASSERT_EQ(TRUE, xdr_utmp_array(&xdrs, &uarr));

  ASSERT(setnetconfig() == NULL);
  ASSERT(getnetconfig(NULL) == NULL);
  ASSERT_EQ(0, endnetconfig(NULL));
  ASSERT(getnetconfigent("udp") == NULL);
  freenetconfigent(NULL);
  nc_perror("nc");
  ASSERT(nc_sperror() != NULL);

  ASSERT(__rpc_setconf("udp") == NULL);
  __rpc_endconf(NULL);
  ASSERT(__rpc_getconf(NULL) == NULL);
  ASSERT(__rpc_getconfip("udp") == NULL);
  ASSERT_EQ(-1, __rpc_nconf2fd(NULL));
  ASSERT_EQ(0, __rpc_nconf2sockinfo(NULL, &sinfo));
  ASSERT(__rpc_uaddr2taddr_af(AF_INET, "127.0.0.1.0.111") == NULL);
  ASSERT(__rpc_taddr2uaddr_af(AF_INET, &nbuf) == NULL);
  ASSERT_EQ(RPC_CANTSEND,
            rpc_broadcast_exp(1UL, 1UL, 1UL, (xdrproc_t)0, NULL, (xdrproc_t)0,
                              NULL, (resultproc_t)0, 0, 0, "udp"));

  ASSERT(clnt_tp_create("localhost", 1UL, 1UL, NULL) == NULL);
  ASSERT(clnt_tli_create(-1, NULL, &nbuf, 1UL, 1UL, 0, 0) == NULL);
  ASSERT(clnt_vc_create(-1, &nbuf, 1UL, 1UL, 0, 0) == NULL);
  ASSERT(clnt_dg_create(-1, &nbuf, 1UL, 1UL, 0, 0) == NULL);
  ASSERT(clnt_raw_create(1UL, 1UL) == NULL);
  ASSERT_EQ(FALSE, rpcb_unset(1UL, 1UL, NULL));
  ASSERT(rpcb_getaddr(1UL, 1UL, NULL, NULL, "localhost") == NULL);
#endif

  ASSERT(taddr2uaddr(NULL, &nbuf) == NULL);
  ASSERT(uaddr2taddr(NULL, "127.0.0.1.0.111") == NULL);
  ASSERT_EQ(RPC_CANTSEND,
            rpc_broadcast(1UL, 1UL, 1UL, (xdrproc_t)0, NULL, (xdrproc_t)0, NULL,
                          (resultproc_t)0, "udp"));
  ASSERT_EQ(0, bsd_rpc_dummy_for_coverage());

  PASS();
}

SUITE(suite_bsd_rpc_core) {
#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__) || defined(__linux__) || defined(__CYGWIN__)
  RUN_TEST(test_bsd_rpc_base_sunrpc_and_yp);
#endif
  RUN_TEST(test_bsd_rpc_tirpc_and_rpcsvc);
}
