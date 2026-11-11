#ifndef RPCSVC_RNUSERS_H
#define RPCSVC_RNUSERS_H

/**
 * @file rnusers.h
 * @brief Remote users RPC protocol (<rpcsvc/rnusers.h>).
 */

/* clang-format off */
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define RUSERSPROG 100002UL
#define RUSERSVERS_IDLE 2UL
#define RUSERSVERS_3 3UL
#define RUSERSPROC_NAMES 1UL
#define RUSERSPROC_ALLNAMES 2UL

struct ru_utmp {
  char ut_line[8];
  char ut_name[8];
  char ut_host[16];
  long ut_time;
};

struct utmpidle {
  struct ru_utmp ui_utmp;
  u_int ui_idle;
};

struct utmpidlearr {
  struct utmpidle **uia_arr;
  int uia_cnt;
};

bool_t xdr_utmpidlearr(XDR *xdrs, struct utmpidlearr *objp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_RNUSERS_H */
